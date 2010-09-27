#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <QByteArray>
#include <QIODevice>
class HttpResponse
{
public:
    HttpResponse();
    void setErrorMessage(QString message);
    void setResponseCode(QByteArray code);
    void setContentType(QString contentType);
    void setContentLength(quint64 size);
    QByteArray header();

private:
    QByteArray m_responseCode;
    QByteArray m_contentType;
    QByteArray m_contentLength;
    QByteArray m_errorMessage;
};

#endif // HTTPRESPONSE_H
