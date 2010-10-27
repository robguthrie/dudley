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
class HttpRequestContext;
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
    void respondToRequest();
    void responseFinished();
    void requestFinished();
    void processFileUploadStarted();
    void processFileUploadFinished();

private:

    RepoModel* m_repoModel;
    FileTransferManager* transferManager;
    QHash<QTcpSocket*, HttpController*> m_controllers;
    int m_socketsOpened;
    int m_socketsClosed;
    int m_requestsStarted;
    int m_requestsFinished;
    int m_responsesStarted;
    int m_responsesFinished;
};


#endif // SERVER_H




