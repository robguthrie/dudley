#include "httpresponse.h"
#include "httprequest.h"
#include "output.h"

HttpResponse::HttpResponse(QObject* parent)
: HttpMessage(parent)
{
    m_responseCode = "200 OK";
    m_contentDevice = 0;
    m_maxAge = 30; // things get stale quick
}

HttpResponse::~HttpResponse()
{
    if (m_contentDevice){
        m_contentDevice->close();
        m_contentDevice->deleteLater();
    }
}

void HttpResponse::setResponseCode(QByteArray code, QByteArray error_message){
    m_responseCode = code.trimmed();
    if (error_message.length() != 0){
        setBody(code+": "+error_message);
    }
}

void HttpResponse::setLastModified(QDateTime d)
{
    m_lastModified = d;
}

void HttpResponse::setCacheNeverExpires()
{
    m_maxAge = 60*60*24*365*3; // expires in 3 years ~ never
}

void HttpResponse::setMaxAge(qint64 m)
{
    m_maxAge = m;
}

QByteArray HttpResponse::headers()
{
    QMap<QByteArray, QByteArray> h; // holds the headers
    h["Content-Length"] = QByteArray::number(m_contentLength);
    h["Keep-Alive"] = QByteArray("timeout=300");
    h["Content-Type"] = m_contentType;
    h["Date"] = QDateTime::currentDateTime().toString(dateFormat).toAscii();
    if(!m_lastModified.isNull())
        h["Last-Modified"] = m_lastModified.toString(dateFormat).toAscii();
    h["Cache-Control"] = "max-age="+QByteArray::number(m_maxAge);
    QByteArray text;
    // first line, usually : HTTP/1.1 200 OK
    text += m_request->protocol()+" "+m_responseCode+"\r\n";
    QMap<QByteArray, QByteArray>::iterator i;
    for(i = h.begin(); i != h.end(); ++i){
        text += i.key().trimmed()+": "+i.value().trimmed()+"\r\n";
    }
    text += "\r\n";
    return text;
}


void HttpResponse::setContent(QByteArray body)
{
    QByteArray* nb = new QByteArray(body);
    QIODevice* d = (QIODevice*) new QBuffer(nb);
    d->open(QIODevice::ReadOnly);
    setContentDevice(d);
    setContentLength(body.size());
}

void HttpResponse::send(QByteArray code, QByteArray body){
    this->setResponseCode(code);
    this->setBody(body);
    this->send();
}

void HttpResponse::send(QByteArray body){
    this->setBody(body);
    this->send();
}

void HttpResponse::send()
{
    emit ready();
}

// just dumped this in here
void HttpResponse::sendResponse(QIODevice* device)
{
    // get the sender of the signal (the response object)
    // get the context object via  m_requests(response->request) object
    // send headers if they have not already been sent
    if (m_finished){
        g_log->error("send called on finished response");
        return;
    }

    if (m_failed){
        g_log->error("send called on failed response");
        return;
    }

    if (!m_headersSent){
        // send header
        int headerSize = device->write(header());
        if (headerSize != -1){
            m_headersSent = true;
        }else{
            g_log->error("failed to write header to socket");
            m_failed = true;
        }
    }

    if (m_contentDevice == 0){
        m_failed = true;
        g_log->error("HttpResponse::send() no device to read content from");
    }

    if (!m_contentDevice->isReadable()){
        m_failed = true;
        g_log->error("m_bodyIODevice is not readable anymore");
    }

    if (!device->isWritable()){
        m_failed = true;
        g_log->error("m_socket is not writable anymore - finished");
    }

    // send the content if there is any to send
    // move this function into the server and create a filetransfer to represent the body ..
    // we will have access to the context for this to work nicely once in the server
    if ((!m_failed) && (m_contentBytesSent < m_contentLength)){
        if (m_contentDevice->bytesAvailable()){
            QByteArray bytes = m_contentDevice->readAll();
            int bytes_written = device->write(bytes);
            if (bytes_written == -1){
                m_failed = true;
                g_log->error("HttpResponse::send() "+QString(m_request->uri())+"failed to write "+QString::number(bytes.length()));
            }else{
                m_contentBytesSent += bytes_written;
            }
        }
        // wait to send more data? avoid finished
        if ((!m_failed) && (m_contentBytesSent < m_contentLength)) return;
    }

    if (m_contentBytesSent > m_contentLength){
        m_failed = true;
        g_log->error("resonse sent "+QByteArray::number(m_contentBytesSent - m_contentLength)+" bytes EXTRA!");
    }

    // ok how did it go?
    if ((!m_failed) && (m_contentBytesSent == m_contentLength)){
            g_log->debug("response sent successfully. "+QByteArray::number(m_contentLength)+" bytes");
            m_finished = true;
    }
    // if we get here.. it must be finished;

    emit finished();
}
