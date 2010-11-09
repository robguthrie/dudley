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
    HttpResponse(QObject* parent);

    bool isReady();
    QByteArray headers() const;
    QByteArray responseCode() const;
    void setResponseCode(QByteArray code);
    void setResponse(QByteArray code_or_body, QByteArray body = "");

private:
    QByteArray   m_responseCode;
};

#endif // HTTPRESPONSE_H
