#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include "fileinfo.h"
#include "httprequest.h"
#include "httpcontroller.h"

class RepoModel;
class FileTransferManager;

class HttpServer : public QTcpServer
{
    Q_OBJECT
public:
    HttpServer(QObject *parent, RepoModel* repoTableModel, FileTransferManager* ftm);
    RepoModel* repoModel() const;
    FileTransferManager* transferManager() const;
    QUrl url() const;


public slots:
    void printStatus(QString a = "");
    void acceptConnection();
    void processRequestFinished();
    void processReadyRead();
    void processDisconnected();
    void processError();
    void processResponseFinished();
    void processResponseWritten();

private:
    QString bestIpAddress() const;
    RepoModel* m_repoModel;
    FileTransferManager* m_transferManager;
    // we must reply to requests in the order that we got them
    QHash<QTcpSocket*, QList<HttpController*> > m_controllers;
    QSet<QTcpSocket*> m_openRequests;
};


#endif // SERVER_H




