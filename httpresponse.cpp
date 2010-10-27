#include "httpresponse.h"
#include "httprequest.h"
#include "output.h"

QString HttpResponse::dateFormat = QString("ddd, dd MMM yyyy hh:mm:ss G'M'T");

HttpResponse::HttpResponse(QObject* parent)
: QObject(parent)
{
    m_responseCode = "200 OK";
    m_contentType = "text/html; charset=utf-8";
    m_headerSent = false;
    m_failed = false;
    m_finished = false;
    m_contentDevice = 0;
    m_contentLength = 0;
    m_bodyBytesSent = 0;
    m_protocol = "HTTP/1.1";
    m_maxAge = 30; // things get stale quick
}

QByteArray HttpResponse::setProtocol(QByteArray protocol)
{
    m_protcol = protocol;
}

QByteArray HttpResponse::protocol() const
{
    return m_protocol;
}

HttpResponse::~HttpResponse()
{
    if (m_contentDevice){
        m_contentDevice->close();
        m_contentDevice->deleteLater();
    }
}

bool HttpResponse::failed()
{
    return m_failed;
}

void HttpResponse::setResponseCode(QByteArray code, QByteArray error_message){
    m_responseCode = code.trimmed();
    if (error_message.length() != 0){
        setBody(code+": "+error_message);
    }
}
void HttpResponse::setContentType(QString contentType)
{
    m_contentType = QByteArray(contentType.toAscii()).trimmed();
}

void HttpResponse::setContentLength(quint64 size)
{
    m_contentLength = size;
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

QByteArray HttpResponse::header()
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

QIODevice* HttpResponse::contentDevice()
{
    return m_contentDevice;
}

// set the QIODevice from which to read (it shoud be open already)
void HttpResponse::setContentDevice(QIODevice *file)
{
    m_contentDevice = file;
// i dont think i need this if the filetransfer can manage it
//    if (!m_contentDevice->isOpen()){
//        g_log->error("m_bodyIODevice is not open");
//    }
//    if (!m_contentDevice->isReadable())
//        g_log->error("m_bodyIODevice is not readable");
//    connect(file, SIGNAL(readyRead()), this, SLOT(send()));
}

void HttpResponse::setBody(QByteArray body)
{

//    if (m_request->acceptsEncoding("deflate")){
//        QByteArray comp_data = qCompress(body, 9);
//        uint32_t comp_size_no = htonl(nb.size());
//        nb.prepend(comp_size_no, 4);
//    }else{
//
//    }
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
