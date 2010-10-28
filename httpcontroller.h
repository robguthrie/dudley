#ifndef HTTPCONTROLLER_H
#define HTTPCONTROLLER_H

#include <QObject>
#include <QHash>
#include <httpserver.h>
#include <httprequest.h>
#include <httpresponse.h>
#include <repo.h>
#include <repomodel.h>
#include <filetransfermanager.h>

class HttpController : public QObject
{
    Q_OBJECT
public:
    explicit HttpController(HttpServer *parent, QTcpSocket* socket);
    ~HttpController();
    bool responseIsReady() const;
    void sendResponse();
signals:
    void requestFinished(QTcpSocket* socket);
    void responseReady(QTcpSocket* socket);
    void responseFinished(QTcpSocket* socket);

private slots:
    void respondToRequest();
    void processFileUploadStarted();
    void processFileUploadFinished();
    void processRequestFinished();
    void processResponseReady();
    void processResponseFinished();


private:
    void routeRequestToAction();
    void actionRootRequest();
    void actionFaviconRequest();
    void actionUploadRequest();
    void actionHistoryRequest();
    void actionCommitRequest();
    void actionBrowseRequest();
    void actionFileRequestByFileName();
    void actionFileRequestByFingerprint();

    void setFileResponse(Repo* repo, FileInfo* file_info);

    QString browseUploadForm(QString path);
    QString browseFileIndex(QString repo_name, QList<FileInfo*> fileInfos);
    QString browseBreadCrumb(QStringList dirs) const;
    QString linkToBrowse(QStringList tokens) const;
    QString linkToFile(QString repo_name, FileInfo* f);
    QString browseDirIndex(QStringList path_dirs, QStringList sub_dirs);
    QString cleanPath(QString path);

    QHash<QByteArray, QByteArray> m_params;
    QList<FileTransfer*>        m_transfers;
    RepoModel*                  m_repoModel;
    FileTransferManager*        m_transferManager;
    HttpRequest*                m_request;
    HttpResponse*               m_response;
    QTcpSocket*                 m_socket;
    HttpServer*                 m_server;

};

#endif // HTTPCONTROLLER_H
