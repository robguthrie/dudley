#include "server.h"
#include <QTcpSocket>
#include <QStringList>
#include <QDateTime>
#include <output.h>

Server::Server(QObject *parent)
    :QTcpServer(parent)
{
    // need a function to be called on newConnection
    // it will read the connection
    // if it gets a GET request we will return a bit of fake html
    // later we will return files and indexes
    Output::debug("constructing a tcp server");
    connect(this, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
}

void Server::acceptConnection()
{
    Output::debug("accepting a connection");
    if (this->hasPendingConnections()){
        QTcpSocket* socket = this->nextPendingConnection();
        connect(socket, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
        connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    }
}

void Server::processReadyRead()
{
    Output::debug("processReadyRead called");
    QTcpSocket* socket = (QTcpSocket*)sender(); // sender returns the pointer to the SIGNAL emitter
    if (socket->canReadLine()){
        Output::debug("can read LINE!!!");
        QStringList tokens = QString(socket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));
        if (tokens[0] == "GET") {
            QTextStream os(socket);
//            os.setAutoDetectUnicode(true);
            os << "HTTP/1.0 200 Ok\r\n"
                "Content-Type: text/html; charset=\"utf-8\"\r\n"
                "\r\n"
                "<h1>Nothing to see here!!! but really!!!!!! WOW</h1>\n" "\n";
            Output::debug("wrote to textstream for socket");
            socket->close();
        }
    }
}
