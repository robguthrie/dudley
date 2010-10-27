#ifndef HTTPCONTROLLER_H
#define HTTPCONTROLLER_H

#include <QObject>
#include <QHash>
#include <httpserver.h>
#include <httprequest.h>
#include <httpresponse.h>

class HttpController : public QObject
{
    Q_OBJECT
public:
    explicit HttpController(HttpServer *parent, HttpRequest* request, HttpResponse* response, QTcpSocket* socket);

signals:

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

private:
    QHash<QByteArray, QVariant> m_params;
    QList<FileTransfer*>        m_transfers;
    RepoModel*                  m_repoModel;
    HttpRequest*                m_request;
    HttpResponse*               m_response;
    QTcpSocket*                 m_socket;
};

#endif // HTTPCONTROLLER_H