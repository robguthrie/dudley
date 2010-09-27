#include "httpresponse.h"

HttpResponse::HttpResponse()
: m_responseCode("200 OK"),m_contentType("text/html; charset=utf-8") { }

void HttpResponse::setErrorMessage(QString message)
{
    m_errorMessage = QByteArray(message.toUtf8());
}

void HttpResponse::setResponseCode(QByteArray code){
    m_responseCode = code;
}

void HttpResponse::setContentType(QString contentType)
{
    m_contentType = QByteArray(contentType.toUtf8());
}

void HttpResponse::setContentLength(quint64 size)
{
    m_contentLength = QByteArray::number(size);
}

QByteArray HttpResponse::header()
{
    time_t currentTime = time(0);

    QByteArray text;
    text += QByteArray("HTTP/1.1 "+m_responseCode+" \r\n");
    text += QByteArray("Date: ") + QByteArray(asctime(gmtime(&currentTime))) + QByteArray("")
    + QByteArray("Content-Type: " + m_contentType + " \r\n")
    + QByteArray("Content-Length: " + m_contentLength  + "\r\n");

    text+= QByteArray("\r\n");
    return text;
}
