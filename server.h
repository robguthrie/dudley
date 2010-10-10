#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include "fileinfo.h"
#include "httprequest.h"
class RepoTableModel;
class HttpResponse;
class FileRepo;

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(RepoTableModel* repoTableModel, QObject *parent = 0);

    void routeRequestToAction(HttpRequest* request, HttpResponse* response);
    void actionFileRequest(HttpResponse* response, FileRepo* repo, QString fingerprint, QString file_name);
    void actionHistoryRequest(HttpResponse* response, FileRepo* repo);
    void actionCommitRequest(HttpResponse* response, FileRepo* repo, QString commit_name);
    void actionBrowseRequest(HttpResponse* response, FileRepo* repo, QString dir_name);

    QString browseFileIndex(QString repo_name, QList<FileInfo*> fileInfos);
    QString browseBreadCrumb(QStringList dirs) const;
    QString linkToBrowse(QStringList tokens) const;
    QString linkToFile(QString repo_name, FileInfo* f);
    QString browseDirIndex(QStringList path_dirs, QStringList sub_dirs);

public slots:
    void printStatus(QString a = "");
    void acceptConnection();
    void processReadyRead();
    void processDisconnected();
    void processAboutToClose();
    void processError();
    void respondToRequest();
    void responseFinished();
private:
    RepoTableModel* repoTableModel;
    QSet<QIODevice*> m_handledSockets;
    QSet<HttpRequest*> m_activeRequests;
    QSet<HttpResponse*> m_activeResponses;
    int m_socketsOpened;
    int m_socketsClosed;
    int m_requestsStarted;
    int m_requestsFinished;
    int m_responsesStarted;
    int m_responsesFinished;
};

#endif // SERVER_H




