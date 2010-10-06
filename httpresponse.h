#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <QByteArray>
#include <QIODevice>
#include <QTcpSocket>

class HttpResponse : public QObject
{
    Q_OBJECT
public:
    HttpResponse(QObject* parent, QTcpSocket* socket);
    void setErrorMessage(QString message);
    void setResponseCode(QByteArray code);
    void setContentType(QString contentType);
    void setContentLength(quint64 size);
    void send();
    QByteArray header();
    QByteArray body;

public slots:
    void bodyBytesAvailable();

private:
    QTcpSocket *m_socket;
    QByteArray m_responseCode;
    QByteArray m_contentType;
    QByteArray m_contentLength;
    QByteArray m_errorMessage;
};

#endif // HTTPRESPONSE_H
