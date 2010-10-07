#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <QByteArray>
#include <QIODevice>
#include <QTcpSocket>
#include <QFile>

class HttpResponse : public QObject
{
    Q_OBJECT
public:
    HttpResponse(QObject* parent, QTcpSocket* socket);
    ~HttpResponse();
    void setResponseCode(QByteArray code, QByteArray error_message = "");
    void setContentType(QString contentType);
    void setContentLength(quint64 size);
    void setBodyIODevice(QIODevice* file);
    void setBody(QByteArray body);

public slots:
    void send();

private:
    void         sendHeader();
    QByteArray   header();
    QTcpSocket  *m_socket;
    QDataStream *m_outStream;
    QByteArray   m_responseCode;
    QByteArray   m_contentType;
    qint64       m_contentLength;
    qint64       m_bodyBytesSent;
    QByteArray   m_errorMessage;
    QIODevice   *m_bodyIODevice;
    QByteArray   m_body;
    bool         m_headerSent;
};

#endif // HTTPRESPONSE_H
