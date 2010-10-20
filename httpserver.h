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
class HttpFileTransferContext;
class HttpServer : public QTcpServer
{
    Q_OBJECT
public:
    HttpServer(QObject *parent, RepoModel* repoTableModel, FileTransferManager* ftm);

public slots:
    void printStatus(QString a = "");
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
    void routeRequestToAction(HttpRequest* request, HttpResponse* response);
    void actionFaviconRequest(HttpResponse* response);
    void actionUploadRequest(HttpRequest* request, HttpResponse* response, QString repo_name, QString path = "");
    void actionHistoryRequest(HttpResponse* response, QString repo_name);
    void actionCommitRequest(HttpResponse* response, QString repo_name, QString commit_name);
    void actionBrowseRequest(HttpResponse* response, QString repo_name, QString dir_name);
    void actionFileRequestByFileName(HttpResponse* response, QString repo_name, QString file_path);
    void actionFileRequestByFingerprint(HttpResponse* response, QString repo_name, QString fingerprint);
    void setFileResponse(HttpResponse* response, Repo* repo, FileInfo* file_info);

    QString browseUploadForm(QString path);
    QString browseFileIndex(QString repo_name, QList<FileInfo*> fileInfos);
    QString browseBreadCrumb(QStringList dirs) const;
    QString linkToBrowse(QStringList tokens) const;
    QString linkToFile(QString repo_name, FileInfo* f);
    QString browseDirIndex(QStringList path_dirs, QStringList sub_dirs);
    QString cleanPath(QString path);

    RepoModel* repoModel;
    FileTransferManager* fileTransferManager;
    QSet<QIODevice*> m_handledSockets;
    QSet<HttpRequest*> m_activeRequests;
    QSet<HttpResponse*> m_activeResponses;
    QHash<HttpRequest*, HttpFileTransferContext*> m_fileTransferContexts;
    int m_socketsOpened;
    int m_socketsClosed;
    int m_requestsStarted;
    int m_requestsFinished;
    int m_responsesStarted;
    int m_responsesFinished;
};


#endif // SERVER_H




