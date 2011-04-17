#include <QTcpSocket>
#include <QCoreApplication>
#include <QDataStream>
#include <QStringList>
#include <QDateTime>
#include <QUrl>
#include <output.h>
#include "httpserver.h"
#include "httpcontroller.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "repo.h"
#include "repomodel.h"
#include "statelogger.h"
#include "filetransfer.h"
#include "filetransfermanager.h"
#include "fileinfo.h"
#include <QTimer>

HttpServer::HttpServer(QObject *parent, RepoModel* model, FileTransferManager* ftm)
    :QTcpServer(parent), m_repoModel(model), m_transferManager(ftm)
{
    connect(this, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
}

void HttpServer::printStatus(QString a)
{
    g_log->info("server status: "+a);
    g_log->info("Open Requests: "+QString::number(m_openRequests.size()));
    foreach(QList<HttpController*> list, m_controllers){
        foreach(HttpController* controller, list){
            g_log->info(controller->statusReport());
        }
    }
}

void HttpServer::acceptConnection()
{
    if (this->hasPendingConnections()){
        QTcpSocket* socket = this->nextPendingConnection();
        connect(socket, SIGNAL(disconnected()), this, SLOT(processDisconnected()));
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(processError()));
        connect(socket, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
    }
}

void HttpServer::processDisconnected()
{
    QTcpSocket* socket = (QTcpSocket*) sender();
    foreach(HttpController* controller, m_controllers[socket]){
        controller->deleteLater();
    }
    m_controllers.remove(socket);
    m_openRequests.remove(socket);
    socket->deleteLater();
}

void HttpServer::processError()
{
    QTcpSocket* socket = (QTcpSocket*) sender();
    qDebug("Server::processError: "+socket->errorString());
}

void HttpServer::processReadyRead()
{
    QTcpSocket* socket = (QTcpSocket*) sender();
    // we need to respond to requests in the order that they were received
    // when a request is being sent, it's socket is in m_handledSockets
    // when a request is finished we remove the socket from m_handledSockets
    // controllers responses are sent back on the socket in the order they appear in the qlist
    if (!m_openRequests.contains(socket)){

        m_openRequests << socket;

        if (!m_controllers.contains(socket)){
            // first request on this socket
            m_controllers.insert(socket, QList<HttpController*>());
        }

        HttpController* controller = new HttpController(this, socket);
        m_controllers[socket].append(controller);
        controller->start();
    }
}

void HttpServer::processRequestFinished()
{
    qDebug("processRequestFinished");
    printStatus();
    HttpController* controller = (HttpController*) sender();
    m_openRequests.remove(controller->socket());
}

void HttpServer::processResponseFinished()
{
    // this makes sure that we send responses back in the correct order.
    // send each ready response (in order) on the socket until none left or not ready
    HttpController* c = (HttpController*) sender();
    QList<HttpController*> clist = m_controllers[c->socket()];
    foreach(HttpController* controller, clist){
        if (controller->state() == HttpController::ResponseFinished){
            controller->sendResponse();
        }else{
            break;
        }
    }
}

void HttpServer::processResponseWritten()
{
    qDebug("processResponseWritten");
    HttpController* controller = (HttpController*) sender();
    m_controllers[controller->socket()].removeAll(controller); // its really not removeAll
    controller->deleteLater();
    if (controller->request()->protocol() == "HTTP/1.0"){
        controller->socket()->close();
    }
}

RepoModel* HttpServer::repoModel() const
{
    return m_repoModel;
}

FileTransferManager* HttpServer::transferManager() const
{
    return m_transferManager;
}
