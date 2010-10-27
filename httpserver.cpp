#include <QTcpSocket>
#include <QCoreApplication>
#include <QDataStream>
#include <QStringList>
#include <QDateTime>
#include <QUrl>
#include <output.h>
#include "httpserver.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "repo.h"
#include "repomodel.h"
#include "repostatelogger.h"
#include "filetransfer.h"
#include "filetransfermanager.h"
#include "fileinfo.h"
#include "httpcontroller.h"

HttpServer::HttpServer(QObject *parent, RepoModel* model, FileTransferManager* ftm)
    :QTcpServer(parent), m_repoModel(model), transferManager(ftm)
{
    // need a function to be called on newConnection
    // it will read the connection
    // if it gets a GET request we will return a bit of fake html
    // later we will return files and indexes
    //    g_log->debug("constructing a tcp server");
    m_socketsOpened = 0;
    m_socketsClosed = 0;
    m_requestsStarted = 0;
    m_requestsFinished = 0;
    m_responsesStarted = 0;
    m_responsesFinished = 0;
    connect(this, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
}

void HttpServer::printStatus(QString a)
{
//    g_log->info("Server Connection Status: "+a);
//    g_log->info("m_socketsOpened = "+QString::number(m_socketsOpened));
//    g_log->info("m_socketsClosed = "+QString::number(m_socketsClosed));
//    g_log->info("m_requestsStarted = "+QString::number(m_requestsStarted));
//    g_log->info("m_requestsFinished = "+QString::number(m_requestsFinished));
//    g_log->info("m_responsesStarted = "+QString::number(m_responsesStarted));
//    g_log->info("m_responsesFinished = "+QString::number(m_responsesFinished));
//    g_log->info("m_requests.size() = "+QString::number(m_requests.size()));
}

void HttpServer::acceptConnection()
{
//    printStatus("accept connection");
    if (this->hasPendingConnections()){
        m_socketsOpened++;

        QTcpSocket* socket = this->nextPendingConnection();
        connect(socket, SIGNAL(disconnected()), this, SLOT(processDisconnected()));
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(processError()));
        connect(socket, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
    }
}

void HttpServer::processDisconnected()
{
    m_socketsClosed++;
    QTcpSocket* socket = (QTcpSocket*) sender();
    if (m_controllers.contains(socket)){
        m_controllers.remove(socket);
        // if this happens.. then the request did not finish properly
        g_log->debug("socket disconnected and removed from m_handledSockets");
    }
    socket->deleteLater();
}

void HttpServer::processError()
{
    QTcpSocket* socket = (QTcpSocket*) sender();
    g_log->debug("Server::processError: "+socket->errorString());
}

void HttpServer::processReadyRead()
{
    QTcpSocket* socket = (QTcpSocket*) sender();
    // we only worry about new connections.. existing ones will manage fine
    if (!m_controllers.contains(socket)){
        m_requestsStarted++;
        // the http request will call respondToRequest, finished etc when it is ready
        HttpController* controller = new HttpController(this, socket);
        m_controllers.insert(socket, controller);
        printStatus("new controller");
    }else{
//        printStatus("ready read but this socket is being responded to it seems");
    }
}

void HttpServer::requestFinished()
{
    HttpRequest* request = (HttpRequest*) sender();
    int n = m_controllers.remove((QTcpSocket*)request->device());
    m_requestsFinished++;
    printStatus("request finished:"+QString::number(n));
}

// just dumped this in here
void HttpServer::sendResponse()
{
    // get the sender of the signal (the response object)
    // get the context object via  m_requests(response->request) object
    // send headers if they have not already been sent
    if (m_finished){
        g_log->error("send called on finished response");
        return;
    }

    if (m_failed){
        g_log->error("send called on failed response");
        return;
    }

    if (!m_headerSent){
        // send header
        int headerSize = m_destDevice->write(header());
        if (headerSize != -1){
            m_headerSent = true;
        }else{
            g_log->error("failed to write header to socket");
            m_failed = true;
        }
    }

    if (m_contentDevice == 0){
        m_failed = true;
        g_log->error("HttpResponse::send() no device to read content from");
    }

    if (!m_contentDevice->isReadable()){
        m_failed = true;
        g_log->error("m_bodyIODevice is not readable anymore");
    }

    if (!m_destDevice->isWritable()){
        m_failed = true;
        g_log->error("m_socket is not writable anymore - finished");
    }
    // send the content if there is any to send
    // move this function into the server and create a filetransfer to represent the body ..
    // we will have access to the context for this to work nicely once in the server
    if ((!m_failed) && (m_bodyBytesSent < m_contentLength)){
        if (m_contentDevice->bytesAvailable()){
            QByteArray bytes = m_contentDevice->readAll();
            int bytes_written = m_destDevice->write(bytes);
            if (bytes_written == -1){
                m_failed = true;
                g_log->error("HttpResponse::send() "+QString(m_request->uri())+"failed to write "+QString::number(bytes.length()));
            }else{
                m_bodyBytesSent += bytes_written;
            }
        }
        // wait to send more data? avoid finished
        if ((!m_failed) && (m_bodyBytesSent < m_contentLength)) return;
    }

    if (m_bodyBytesSent > m_contentLength){
        m_failed = true;
        g_log->error("resonse sent "+QByteArray::number(m_bodyBytesSent - m_contentLength)+" bytes EXTRA!");
    }

    // ok how did it go?
    if ((!m_failed) && (m_bodyBytesSent == m_contentLength)){
            g_log->debug("response sent successfully. "+QByteArray::number(m_contentLength)+" bytes");
            m_finished = true;
    }
    // if we get here.. it must be finished;

    emit finished();
}

void HttpServer::responseFinished()
{

    HttpResponse* response = (HttpResponse*) sender();
    if (response->failed()){
        g_log->debug("Server::responseFinished() with failure");
    }

    //  close if its http 1.0
    if (response->protocol() == "HTTP/1.0"){
        response->destDevice()->close();
    }

    if (m_requestContexts.contains(response->request())){
        HttpRequestContext* ftc = m_requestContexts.value(response->request());
        m_requestContexts.remove(ftc->m_request);
        delete ftc;
    }

    m_responsesFinished++;
    response->deleteLater();
    printStatus("response finished");
}

RepoModel* HttpServer::repoModel() const
{
    return m_repoModel;
}
