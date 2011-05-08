#ifndef HTTPCONTROLLER_H
#define HTTPCONTROLLER_H

#include <QObject>
#include <QHash>
#include <httprequest.h>
#include <httpresponse.h>
#include <repo.h>
#include <repomodel.h>
#include <filetransfermanager.h>

/*
 controller states enum?

 start
 reading_request - until request finished
                     request_headers_finished
                     request_upload_started
                     request_finished
 response ready
 response sent

 */
class HttpServer;
class HttpController : public QObject
{
    Q_OBJECT

public:

    enum States{
        BeforeStart,
        ReadingRequest,
        RoutingRequest,
        RoutedRequest,
        ConnectingUploadContentDevice,
        RequestFinished,
        ActioningRequest,
        ResponseFinished,
        ResponseWritten
    };

    explicit HttpController(HttpServer *parent, QTcpSocket* socket);
    void sendResponse();
    States state() const;
    void start();
    void actionName() const;
    HttpRequest* request() const;
    HttpResponse* response() const;
    QTcpSocket* socket() const;

    QByteArray statusReport(bool show_headers = false) const;

signals:
    void requestFinished();
    void responseFinished();
    void responseWritten();

private slots:
    void processReadyRead();
    void routeRequest();
    void processUploadStarted();
    void processRequestFinished();
    void processResponseFinished();

private:
    void setupRoutes();
    void setState(States state);
    void actionRoot();
    void actionFavicon();
    void actionBrowserUpload();
//    void actionHistory();
//    void actionCommit();
    void actionBrowse();
    void actionFileByFileName();
    void actionFileByFingerprint();

    void setResponseContentFromRepo(Repo* repo, FileInfo file_info);

    QString browseUploadForm(QString path);
    QString browseFileIndex(QString repo_name, QList<FileInfo*> fileInfos);
    QString browseBreadCrumb(QStringList dirs) const;
    QString linkToBrowse(QStringList tokens) const;
    QString linkToFile(QString repo_name, FileInfo* f);
    QString browseDirIndex(QStringList path_dirs, QStringList sub_dirs);
    QString cleanPath(QString path);

    QHash<QByteArray, QByteArray> m_params;
    QList<FileTransfer*>        m_fileUploads;
    RepoModel*                  m_repoModel;
    FileTransferManager*        m_transferManager;
    HttpRequest*                m_request;
    HttpResponse*               m_response;
    QTcpSocket*                 m_socket;
    HttpServer*                 m_server;
    States                      m_state;
    QString                     m_route_key;

    QMap<QString, QRegExp>      m_routes;
    QHash<QString, QString>     m_route_args;

    Repo*                       m_responseRepo;
    FileInfo                    m_responseFileInfo;
};

#endif // HTTPCONTROLLER_H
