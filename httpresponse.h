#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <QByteArray>
#include <QIODevice>
#include <QTcpSocket>
#include <QFile>
#include <QDateTime>
class HttpRequest;

class HttpResponse : public QObject
{
    Q_OBJECT
public:
    HttpResponse(QObject* parent, HttpRequest* request, QIODevice* destDevice);
    ~HttpResponse();

    void setResponseCode(QByteArray code, QByteArray error_message = "");
    void setContentType(QString contentType);
    void setLastModified(QDateTime d);
    void setContentLength(quint64 size);
    void setContentDevice(QIODevice* file);
    void setCacheNeverExpires();
    void setMaxAge(qint64 m);
    void setBody(QByteArray body);
    bool failed();
    QByteArray protocol() const;
    HttpRequest* request();
    QIODevice* destDevice();
    QIODevice* contentDevice();
signals:
    void finished();

public slots:
    void send();
    void send(QByteArray code, QByteArray body);
    void send(QByteArray body);

private:
    HttpRequest* m_request;
    QIODevice   *m_destDevice;
    QIODevice   *m_contentDevice;

    QByteArray   header();
    QDateTime    m_lastModified;
    QByteArray   m_responseCode;
    QByteArray   m_contentType;
    qint64       m_contentLength;
    qint64       m_bodyBytesSent;

    bool         m_headerSent;
    bool         m_failed;
    bool         m_finished;


    QByteArray   m_protocol;
    qint64       m_maxAge;
    static QString dateFormat;
};

#endif // HTTPRESPONSE_H
