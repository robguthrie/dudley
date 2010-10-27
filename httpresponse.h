#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <QByteArray>
#include <QIODevice>
#include <QTcpSocket>
#include <QFile>
#include <QDateTime>
#include <httpmessage.h>

class HttpResponse : public HttpMessage
{
    Q_OBJECT
public:
    HttpResponse(QObject* parent);
    ~HttpResponse();

    void setResponseCode(QByteArray code, QByteArray error_message = "");
    void setLastModified(QDateTime d);
    void setCacheNeverExpires();
    void setMaxAge(qint64 m);

public slots:
    void send();
    void send(QByteArray code, QByteArray body);
    void send(QByteArray body);

private:
    QByteArray   header();
    QDateTime    m_lastModified;
    QByteArray   m_responseCode;

    bool         m_headersSent;

    QByteArray   m_protocol;
    qint64       m_maxAge;

};

#endif // HTTPRESPONSE_H
