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
    // we need to respond to requests in the order that they were received
    // when a request is being sent, it's socket is in m_handledSockets
    // when a request is finished we remove the socket from m_handledSockets
    // controllers responses are sent back on the socket in the order they appear in the qlist
    if (!m_handledSockets.contains(socket)){
        m_requestsStarted++;
        m_handledSockets << socket;

        if (!m_controllers.contains(socket)){
            // first request on this socket
            m_controllers.insert(socket, QList<HttpController*>());
        }

        HttpController* controller = new HttpController(this, socket);
        m_controllers[socket].append(controller);
        printStatus("new controller");
    }else{
//        printStatus("ready read but this socket is being responded to it seems");
    }
}

void HttpServer::requestFinished()
{
    HttpRequest* request = (HttpRequest*) sender();
    m_handledSockets.remove((QTcpSocket*)request->device());
    m_requestsFinished++;
}

void HttpServer::processResponseReady(QTcpSocket* socket)
{
    // this makes sure that we send responses back in the correct order.
    // send each ready response (in order) on the socket until none left or not ready
    if (m_controllers.contains(socket)){
        if (m_controllers[socket].first()->responseIsReady()){

            // remove the controller and fire it off
        }
    }
}

void HttpServer::responseFinished()
{

    HttpResponse* response = (HttpResponse*) sender();
    if (response->complete()){
        g_log->debug("Server::responseFinished() incomplete finish");
    }

    //  close if its http 1.0
    if (response->protocol() == "HTTP/1.0"){
        response->destDevice()->close();
    }

    // delete the controller

    m_responsesFinished++;
    response->deleteLater();
    printStatus("response finished");
}

RepoModel* HttpServer::repoModel() const
{
    return m_repoModel;
}
