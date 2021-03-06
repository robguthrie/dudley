#include "httpresponse.h"
#include "output.h"
#include "fileinfo.h"
#include "repo.h"

HttpResponse::HttpResponse(QObject* parent)
: HttpMessage(parent)
{
    m_state = ReadFirstLine;
    m_responseCode = "200 OK";
}

void HttpResponse::setResponseCode(QByteArray code){
    m_responseCode = code.trimmed();
}

QByteArray HttpResponse::responseCode() const
{
    return m_responseCode;
}

// this function is used like this
// r->setResponse("404 Not Found", "my dogs ran away");
// or just like this
// r->setResponse("here are my puppies: <img src='dogs.jpg' />");
// the latter will use the default response code of 200 OK
void HttpResponse::setResponse(QByteArray code_or_content, QByteArray content){
    setHeader("Content-Type", "text/html; charset=utf-8");
    setHeader("Date", QDateTime::currentDateTime().toString(dateFormat));
    if (content.isEmpty()){
        setContent(code_or_content);
    }else{
        m_responseCode = code_or_content;
        setContent(content);
    }
    setState(Finished);
}


QByteArray HttpResponse::headers() const
{
    QByteArray text;
    // first line, usually : HTTP/1.1 200 OK
    text = m_protocol+" "+m_responseCode+"\r\n";
    text += HttpMessage::headers();
    return text;
}

QString HttpResponse::inspect(bool show_headers) const
{
    QString s = "Http Response: code="+m_responseCode+"\n";
    s.append(HttpMessage::inspect(show_headers));
    return s;
}
