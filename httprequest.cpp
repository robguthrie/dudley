#include <QBuffer>
#include <QHash>
#include <QRegExp>
#include <QStringList>
#include "httprequest.h"
#include "output.h"

HttpRequest::HttpRequest(QObject* parent, QIODevice* device)
    : QObject(parent), m_contentLength(0), m_validRequest(true),
      m_headersFinished(false), m_device(device), m_currentContentPart(0)

{
    connect(device, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
    connect(this, SIGNAL(headersReady()), parent, SLOT(respondToRequest()));
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

void HttpRequest::accept()
{
    if ((hasHeader("expect") && (header("expect").toLower() == "100-continue"))){
        Output::debug("sending 100 continue");
//        m_device->write("HTTP/1.1 100 Continue\r\n\r\n");
    }
    // need to verify the correct times to send this
    m_device->write("HTTP/1.1 100 Continue\r\n\r\n");
}

void HttpRequest::processReadyRead()
{
    // on construction m_validRequest == true and m_headersFinished == false
    parseRequestLine();
    parseHeaders();
    parseContent();

    if (!m_validRequest){
        Output::error("invalid http request");
        emit finished();
    }
    // is the request finished?
}
void HttpRequest::parseRequestLine()
{
    if ((m_validRequest) && (m_method == "") && (m_device->canReadLine())){
        // first line is the request string
        QRegExp request_rx("(GET|POST|PUT|HEAD|DELETE|OPTIONS|TRACE|CONNECT) (\\S+) (\\S+)");
        QByteArray line = m_device->readLine().trimmed();
        if (request_rx.exactMatch(line)){
            Output::debug("http request: "+line);
            m_method = request_rx.cap(1).toAscii().trimmed();
            m_uri = request_rx.cap(2).toAscii().trimmed();
            m_protocol = request_rx.cap(3).toAscii().trimmed();
            QRegExp protocol_rx("HTTP/1.[01]");
            if (!protocol_rx.exactMatch(m_protocol))
                m_validRequest = false;
        }else{
            // invalid first line. terminate loop
            Output::error("invalid first line in http request: "+line);
            m_validRequest = false;
        }
    }
}

void HttpRequest::printHeaders()
{
    QMap<QByteArray, QByteArray>::const_iterator i = m_headers.begin();
    for(; i != m_headers.end(); ++i)
    {
        Output::debug(i.key()+":"+i.value());
    }
}

void HttpRequest::parseHeaders()
{
    // read 0 or more header lines..
    QByteArray line;
    QRegExp header_rx("([^:]+):(.+)");
    while ((m_validRequest) && (m_method != "") && (!m_headersFinished) && (m_device->canReadLine())){
        line = m_device->readLine().trimmed();
        if (line.length() == 0){
            // empty line. normal end of headers
            Output::debug("request headers finished:");
            printHeaders();
            if (hasHeader("content-length")){
                m_contentLength = header("content-length").toLongLong();
            }
            if (hasHeader("content-type")){
                QRegExp form_data_rx("multipart/form-data; boundary=(.*)");
                form_data_rx.setCaseSensitivity(Qt::CaseInsensitive);
                if (form_data_rx.exactMatch(header("content-type"))){
                    m_formDataBoundry = form_data_rx.cap(1).toAscii();
                    Output::debug("found out the form data boundry: "+m_formDataBoundry);
                }else{
                    Output::debug("unrecognised POST content-type: "+header("content-type"));
                }
            }
            m_headersFinished = true;
            emit headersReady();
        }else if (header_rx.exactMatch(line)){
            // read the header
            QString key = header_rx.cap(1).trimmed();
            QString value = header_rx.cap(2).trimmed();
            m_headers.insert(key.toAscii().toLower(), value.toAscii());
        }else{
            // header is invalid.. termate loop
            m_validRequest = false;
            emit finished();
        }
    }
}

void HttpRequest::parseContent()
{
    if ((m_validRequest) && (m_method != "") && (m_headersFinished) && (m_device->bytesAvailable())){
        if (!hasHeader("content-length")){
            Output::debug("request has body but no content-length, alas.");
        }

        // if the request is HTTP/1.1 then we wont have content until we accept
        if (m_formDataBoundry == ""){
            // just signal that there is content to read in the device?
            if (hasHeader("content-length")){
                Output::debug(" more content, not multipart, but there is a content length.");
                if (m_contentLength < (m_contentBytesReceived + m_device->bytesAvailable())){
                    qint64 n_extra_bytes = (m_contentBytesReceived + m_device->bytesAvailable()) - m_contentLength;
                    Output::error("more data than expected has arrived in the request, by "+QString::number(n_extra_bytes)+" bytes");
                }
            }else{
                Output::debug("data available from request device.. but it's not multipart and there is no content length ");
            }
        }else{
            parseMultiPartContent();
        }
    }
}

void HttpRequest::parseMultiPartContent(){
    // request is multipart format.
    QByteArray line;
    QRegExp header_rx("([^:]+):(.+)");
    while (m_device->canReadLine()){
        line = m_device->readLine();
        m_contentBytesReceived += line.size();
        if (line.contains(m_formDataBoundry)){
            if (m_currentContentPart){
                // this is the end of the content for the current part
                m_currentContentPart->setComplete();
                m_contentParts << m_currentContentPart;
                Output::debug("completed currentContentPart:");
                m_currentContentPart->printHeaders();
                emit contentReady(m_currentContentPart);
            }
            if (line.endsWith("--")){
                // last boundry line of the request
                Output::debug("got last boundry line");
                emit finished();
            }else{
                // prepare new content part for the following data
                m_currentContentPart = new HttpMessage();
            }
        }else if (m_currentContentPart){
            if (m_currentContentPart->headersFinsihed()){
                // if the headers are finished, and this is not boundry. then its data
                // we chop because readLine appends a \0 terminator
                line.chop(1);
                m_currentContentPart->m_data += line;
            }else{
                // this is either a header or a blank line to indicate end of headers
                if (header_rx.exactMatch(line)){
                    m_currentContentPart->setHeader(header_rx.cap(1).toAscii(), header_rx.cap(2).toAscii());
                }else{
                    // assuming its a blank line - headers finished or end of content
                    m_currentContentPart->setHeadersFinished();
                    // now just to test our theory
                    if (line != "\r\n"){
                        Output::debug("theory debunked! closed the headers and the line was not the right kind of blank");
                        if (line.trimmed().length() == 0){
                            Output::debug("however it is some kind of blank..");
                        }else{
                            Output::debug("its not blank at all...");
                        }
                    }
                }
            }
        }else{
            Output::debug("no m_currentContentPart .. this is the 'should not happen condition'");
        }
    }
}


