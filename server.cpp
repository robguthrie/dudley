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
    while (socket->canReadLine()){
        QStringList tokens = QString(socket->readLine()).split(QRegExp("[ \r\n][ \r\n]*"));
        Output::debug(tokens.join("\n"));

        /*
            indexes and file retrival!
            ok so a request should be formed like
            http://nodename.dinotech.co.nz
                or
            http://elmrow.dudsbabyduds.org:56789/pics/2009-09-09/IMG456.JPG
            http://elmrow.dudsbabyduds.org:56789/pics/.dudley/logs
            http://elmrow.dudsbabyduds.org:56789/pics/.dudley/20091230235959999.log

            so first token in the string is reponame
            some commands:
            history - return
            nodes


        */
        if (tokens[0] == "GET") {
            QTextStream os(socket);
            os.setAutoDetectUnicode(true);
            os << "HTTP/1.0 200 Ok\r\n"
                "Content-Type: text/html; charset=\"utf-8\"\r\n"
                // dudley version
                // format options?
                "\r\n"
                "<h1>Dudley Node</h1>\n" "\n";

        }
        socket->close();
    }
}
