#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include "fileinfo.h"
#include "httprequest.h"

class RepoModel;
class HttpResponse;
class Repo;
class FileTransfer;
class FileTransferManager;
class HttpController;

class HttpServer : public QTcpServer
{
    Q_OBJECT
public:
    HttpServer(QObject *parent, RepoModel* repoTableModel, FileTransferManager* ftm);
    RepoModel* repoModel() const;
    void printStatus(QString a = "");

public slots:
    void acceptConnection();
    void processReadyRead();
    void processDisconnected();
    void processAboutToClose();
    void processError();
    void requestFinished();
    void responseFinished();
    void processResponseReady(QTcpSocket* socket);

private:

    RepoModel* m_repoModel;
    FileTransferManager* transferManager;
    // we must reply to requests in the order that we got them
    QHash<QTcpSocket*, QList<HttpController*> > m_controllers;
    QSet<QTcpSocket*> m_handledSockets;
    int m_socketsOpened;
    int m_socketsClosed;
    int m_requestsStarted;
    int m_requestsFinished;
    int m_responsesStarted;
    int m_responsesFinished;
};


#endif // SERVER_H




