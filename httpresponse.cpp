#include "httpresponse.h"
#include "output.h"

HttpResponse::HttpResponse(QObject* parent, QTcpSocket* socket)
: QObject(parent), m_responseCode("200 OK"), m_errorMessage(""),
  m_contentType("text/html; charset=utf-8"), m_socket(socket),
  m_headerSent(false)
{
    m_outStream = new QDataStream(m_socket);
    m_bodyIODevice = 0;
    m_bodyBytesSent = 0;
}

HttpResponse::~HttpResponse()
{

    if (m_socket->isOpen()) m_socket->close();
    m_socket->deleteLater();
    if (m_bodyIODevice){
        m_bodyIODevice->close();
        m_bodyIODevice->deleteLater();
    }
    delete m_outStream;
}

void HttpResponse::setResponseCode(QByteArray code, QByteArray error_message){
    m_responseCode = code;
    if (error_message.length() == 0) error_message = code;
    m_contentLength = error_message.size();
    m_body = error_message;
}

void HttpResponse::setContentType(QString contentType)
{
    m_contentType = QByteArray(contentType.toUtf8());
}

void HttpResponse::setContentLength(quint64 size)
{
    m_contentLength = size;
}

void HttpResponse::sendHeader()
{
    QByteArray b = header();
    m_outStream->writeRawData(b, b.length());
    m_headerSent = true;
}

QByteArray HttpResponse::header()
{
    time_t currentTime = time(0);

    QByteArray text;
    text += QByteArray("HTTP/1.1 "+m_responseCode+"\r\n");
    text += QByteArray("Date: ") + QByteArray(asctime(gmtime(&currentTime)))
    + QByteArray("Keep-Alive: timeout=50")
    + QByteArray("Content-Type: " + m_contentType + "\r\n")
    + QByteArray("Content-Length: " + QByteArray::number(m_contentLength)  + "\r\n");
    text+= QByteArray("\r\n");
    return text;
}

// set the QIODevice from which to read (it shoud be open already)
void HttpResponse::setBodyIODevice(QIODevice *file)
{
    m_bodyIODevice = file;
    if (!m_bodyIODevice->isOpen())
        Output::error("m_bodyIODevice is not open");

    if (!m_bodyIODevice->isReadable())
        Output::error("m_bodyIODevice is not readable");

    connect(file, SIGNAL(readyRead()), this, SLOT(send()));
}

void HttpResponse::setBody(QByteArray body)
{
    QByteArray* nb = new QByteArray(body);
    QIODevice* d = (QIODevice*) new QBuffer(nb);
    d->open(QIODevice::ReadOnly);
    setBodyIODevice(d);
    setContentLength(body.size());
}

void HttpResponse::send()
{
    // if we have a m_bodyIODevice then
    if (!m_headerSent) sendHeader();
    if (m_bodyIODevice != 0){
        if (m_bodyIODevice->bytesAvailable()){
            if ((m_socket->isWritable()) && (m_bodyIODevice->isReadable())){
                QByteArray bytes = m_bodyIODevice->readAll();
                int bytes_written = m_socket->write(bytes);
                if (bytes_written == -1){
                    Output::debug("HttpResponse::send() failed to write "+QString::number(bytes.length()));
                    m_socket->close();
                }
                m_bodyBytesSent += bytes_written;
//                Output::debug(QString("HttpResponse::send() %1 bytes available. %2 bytes written ").arg(QString::number(bytes.length()), QString::number(bytes_written)));
                if (m_bodyBytesSent >= m_contentLength){
                    Output::debug("sent all "+QByteArray::number(m_contentLength)+" bytes, closing connection");
                    m_socket->waitForBytesWritten();
                    m_socket->close();
                }
            }else{
                if (!m_socket->isWritable()){
                    Output::debug("socket is not writable anymore - fuck knows why - closing");
                }
                if (!m_bodyIODevice->isReadable()){
                    Output::debug("source file is not readable anymore - fuck knows why - closing");
                }
                m_socket->close();
            }
        }
    }else{
        Output::error("m_bodyIODevice is null pointer");
        m_socket->close();
    }
}
