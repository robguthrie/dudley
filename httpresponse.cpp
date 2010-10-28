#include "httpresponse.h"
#include "output.h"
#include "fileinfo.h"
#include "repo.h"

HttpResponse::HttpResponse(QObject* parent, QIODevice* device)
: HttpMessage(parent), m_device(device)
{
    m_ready = false;
    m_responseCode = "200 OK";
    setHeader("Content-Type", "text/html; charset=utf-8");
    setHeader("Keep-Alive", "timeout=300");
    setHeader("Date", QDateTime::currentDateTime().toString(dateFormat));
}

bool HttpResponse::isReady() const
{
    return m_ready;
}

void HttpResponse::setReady()
{
    m_ready = true;
    emit ready();
}

void HttpResponse::setResponse(QByteArray code, QByteArray body){
    m_responseCode = code.trimmed();
    setContent(body);
    if (!body.isEmpty()){
        setContent(code+": "+body);
        setReady();
    }
}

void HttpResponse::setResponseFile(Repo* repo, FileInfo* file_info)
{
    m_fileInfo = file_info;
    QIODevice *file = repo->getFile(file_info);
    setLastModified(file_info->lastModified());
    setHeader("Content-Type", file_info->mimeType());
    setHeader("Content-Length", file_info->size());
    setContentDevice(file);
}


void HttpResponse::setLastModified(QDateTime d)
{
    setHeader("Last-Modified", d.toString(dateFormat));
}

FileInfo* HttpResponse::fileInfo()
{
    return m_fileInfo;
}

QByteArray HttpResponse::headers() const
{
    QByteArray text;
    // first line, usually : HTTP/1.1 200 OK
    text = m_protocol+" "+m_responseCode+"\r\n";
    text += HttpMessage::headers();
    return text;
}
