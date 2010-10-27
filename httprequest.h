#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_
#include <QTcpSocket>
#include <QByteArray>
#include <QObject>
#include <QIODevice>
#include <QHash>
#include "httpmessage.h"
#include <QVariant>
class HttpResponse;
class FileInfo;
class FileTransfer;
class HttpRequest : public HttpMessage {
Q_OBJECT
public:
    HttpRequest(QObject* parent, QIODevice* socket);
    QByteArray uri() const;
    QByteArray protocol() const;
    QByteArray method() const;
    void       accept();
    QIODevice *device() const;
    QHash<QString, QVariant> params();
    void printParams() const;

public slots:
    void parseParams();
    void processReadyRead();

private:
    void parseRequestLine(QByteArray line);
    QHash<QString, QVariant> m_params;
    QByteArray m_method;
    QByteArray m_uri;
    QByteArray m_protocol;
    QIODevice* m_device; // the socket we talk on
};

class HttpRequestContext{
public:
    HttpRequestContext(HttpRequest* request, HttpResponse* response, QString repo_name, QString path);
    QString m_repoName;
    QString m_path;
    HttpRequest* m_request;
    HttpResponse* m_response;
    FileInfo* m_fileInfo;
    QList<FileTransfer*> m_transfers;
    void setFileInfo(FileInfo* file_info);
};
#endif /* !_HTTP_REQUEST_ */
