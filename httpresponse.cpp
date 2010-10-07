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
    delete m_outStream;
    m_socket->deleteLater();
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
    Output::debug("sending header:\n"+b);
    m_outStream->writeRawData(b, b.length());
    m_headerSent = true;
}

QByteArray HttpResponse::header()
{
    time_t currentTime = time(0);

    QByteArray text;
    text += QByteArray("HTTP/1.1 "+m_responseCode+"\r\n");
    text += QByteArray("Date: ") + QByteArray(asctime(gmtime(&currentTime)))
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
    // this look horrible..
    // copy body passed in to new memory on the heap
    // before using the pointer for the buffer
    Output::debug(QString("HttpResponse::setBody setting body bytearray size: %1").arg(body.size()));
    QByteArray* nb = new QByteArray(body);
    Output::debug(QString("HttpResponse::setBody final body bytearray size: %1").arg(nb->size()));
    QIODevice* d = (QIODevice*) new QBuffer(nb);
    d->open(QIODevice::ReadOnly);
    Output::debug(QString("HttpResponse::setBody buffer bytes available: %1").arg(d->bytesAvailable()));
    setBodyIODevice(d);
    setContentLength(body.size());
}

void HttpResponse::send()
{
    // if we have a m_bodyIODevice then
    if (!m_headerSent) sendHeader();
    if (m_bodyIODevice != 0){
        if (m_bodyIODevice->bytesAvailable()){
            Output::debug(QString("HttpResponse::send() bytes available in m_bodyIODevice: %1").arg(m_bodyIODevice->bytesAvailable()));
            if (m_socket->isWritable()){
                QByteArray bytes = m_bodyIODevice->readAll();
                m_bodyBytesSent += m_socket->write(bytes);
                Output::debug(QString("HttpResponse::send() m_bodyIODevice->readAll() got %1 bytes").arg(bytes.length()));
                Output::debug(QString("HttpResponse::send() %1 bytes written total").arg(m_bodyBytesSent));
                if (m_bodyBytesSent >= m_contentLength){
                    Output::debug("sent all "+QByteArray::number(m_contentLength)+" bytes, closing connection");
                    m_socket->waitForBytesWritten();
                    m_socket->close();
                }
            }else{
                Output::debug("socket is not writable anymmore - fuck knows why");
            }
        }
    }else{
        Output::error("m_bodyIODevice is null pointer");
        m_socket->close();
    }
}
