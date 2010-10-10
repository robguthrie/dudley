#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <QByteArray>
#include <QIODevice>
#include <QTcpSocket>
#include <QFile>
class HttpRequest;

class HttpResponse : public QObject
{
    Q_OBJECT
public:
    HttpResponse(QObject* parent, HttpRequest* request, QIODevice* destDevice);
    ~HttpResponse();
    void setResponseCode(QByteArray code, QByteArray error_message = "");
    void setContentType(QString contentType);
    void setContentLength(quint64 size);
    void setContentDevice(QIODevice* file);
    void setBody(QByteArray body);
    bool failed();
    QByteArray protocol() const;
    QIODevice* destDevice();
    QIODevice* contentDevice();
signals:
    void finished();

public slots:
    void send();

private:

    QMap<QByteArray, QByteArray> m_headers;
    QByteArray   header();
    QByteArray   m_responseCode;
    QByteArray   m_contentType;
    QByteArray   m_errorMessage;
    qint64       m_contentLength;
    qint64       m_bodyBytesSent;

    QIODevice   *m_contentDevice;
    QIODevice   *m_destDevice;

    bool         m_headerSent;
    bool         m_failed;
    bool         m_finished;

    HttpRequest* m_request;
    QByteArray   m_protocol;
};

#endif // HTTPRESPONSE_H
