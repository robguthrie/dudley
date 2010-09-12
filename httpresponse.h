#ifndef HTTPRESPONSE_H
#define HTTPRESPONSE_H
#include <QByteArray>
#include <QIODevice>
class HttpResponse
{
public:
    HttpResponse();
    void setContentType(QString contentType);
    void setContentLength(quint64 size);
    QByteArray header();

private:
    QByteArray m_contentType;
    QByteArray m_contentLength;
};

#endif // HTTPRESPONSE_H
