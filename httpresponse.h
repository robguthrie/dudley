#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <QByteArray>
#include <QIODevice>
#include <QTcpSocket>
#include <QFile>
#include <QDateTime>
#include <httpmessage.h>

class Repo;
class FileInfo;

class HttpResponse : public HttpMessage
{
    Q_OBJECT
public:
    HttpResponse(QObject* parent, QIODevice* device);

    bool isReady() const;
    QByteArray headers() const;
    void setResponse(QByteArray code, QByteArray body = "");
    void setResponseFile(Repo* repo, FileInfo* file_info);
    void setLastModified(QDateTime d);
    FileInfo* fileInfo();

signals:
    void ready();  // when we want to be eligible to be sent
    void failed();

public slots:
    void send();
    void send(QByteArray code, QByteArray body);
    void send(QByteArray body);

private:
    void         setReady();
    bool         m_ready; // response is ready to be sent
    bool         m_failed; // sending the response has failed
    QByteArray   m_responseCode;
    QDateTime    m_lastModified;
    qint64       m_maxAge;
    QIODevice*   m_device;
    FileInfo*    m_fileInfo;
};

#endif // HTTPRESPONSE_H
