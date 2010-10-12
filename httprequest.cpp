#include <QBuffer>
#include <QHash>
#include <QRegExp>
#include <QStringList>
#include "httprequest.h"
#include "output.h"

HttpRequest::HttpRequest(QObject* parent, QIODevice* device)
    : QObject(parent), m_contentLength(0), m_validRequest(true),
      m_headersFinished(false), m_device(device)

{
    connect(device, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
    connect(this, SIGNAL(ready()), parent, SLOT(respondToRequest()));
    processReadyRead();
}
bool HttpRequest::isValid() const
{
    return m_validRequest;
}

QIODevice* HttpRequest::device() const
{
    return m_device;
}

bool HttpRequest::hasHeader(QByteArray key) const
{
    return m_headers.contains(key.toLower());
}

QByteArray HttpRequest::header(QByteArray key) const
{
    return m_headers.value(key.toLower());
}

QByteArray HttpRequest::uri() const
{
    return m_uri;
}

QByteArray HttpRequest::method() const
{
    return m_method;
}

QByteArray HttpRequest::protocol() const
{
    return m_protocol;
}

bool HttpRequest::acceptsEncoding(QByteArray type)
{
    return ( hasHeader("Accept-Encoding") &&
             header("Accept-Encoding").contains(type));
}

void HttpRequest::processReadyRead()
{
    // on construction m_validRequest == true and m_headersFinished == false
    QByteArray line;
    // if we are still parsing headers
    QRegExp request_rx("(GET|POST|PUT|HEAD|DELETE|OPTIONS|TRACE|CONNECT) (\\S+) (\\S+)");
    QRegExp header_rx("([^:]+):(.+)");
    while ((!m_headersFinished) && (m_validRequest) && (m_device->canReadLine())){
        line = m_device->readLine().trimmed();

        if (m_method == ""){
            // first line..
            if (request_rx.exactMatch(line)){
                Output::debug("request: "+line);
                m_method = request_rx.cap(1).toAscii().trimmed();
                m_uri = request_rx.cap(2).toAscii().trimmed();
                m_protocol = request_rx.cap(3).toAscii().trimmed();
                QRegExp protocol_rx("HTTP/1.[01]");
                if (!protocol_rx.exactMatch(m_protocol))
                    m_validRequest = false;
            }else{
                // invalid first line. terminate loop
                m_validRequest = false;
            }
        }else{
            // header line
            if (line.length() == 0){
                // last header line. normal termination of while loop
                m_headersFinished = true;
                Output::debug("valid request ready");
                emit ready();
            }else if (header_rx.exactMatch(line)){
                // read the header
                QString key = header_rx.cap(1).trimmed();
                QString value = header_rx.cap(2).trimmed();
                m_headers.insert(key.toAscii(), value.toAscii());
            }else{
                // header is invalid.. termate loop
                m_validRequest = false;
            }
        }
    }

    // this needs to move to some point after we have authorized
    // but its ok here. we just accept all the data and look at it if we need.
    // will be a welcome improvement when we can reject a client before they send
    // the request body


    if (!m_validRequest){
        Output::debug("invalid http request");
    }else if (m_headersFinished){
        if (hasHeader("content-length")){
            m_contentLength = header("content-length").toLongLong();
        }

        //  Requirements for HTTP/1.1 clients:
        //  If a client will wait for a 100 (Continue) response before
        //  sending the request body, it MUST send an Expect request-header
        //  field (section 14.20) with the "100-continue" expectation.


        // this should not emit readyread until the 100 continue has been sent
        if ((m_contentLength > 0) && (m_device->bytesAvailable() > 0)){
            // there is content for someone to read, waiting in the socket
            emit readyRead();
        }
    }
}

bool HttpRequest::accept()
{
    if ((hasHeader("expect") &&
        (header("expect") == "100-continue"))){
        m_device->write("HTTP/1.1 100 Continue\r\n\r\n");
    }
}
