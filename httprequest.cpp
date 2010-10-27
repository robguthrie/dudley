#include <QIODevice>
#include <QRegExp>
#include <QStringList>

#include "httprequest.h"
#include "output.h"

HttpRequest::HttpRequest(QObject* parent)
    : HttpMessage(parent)

{
    m_currentMessage = 0;
    m_method = "";
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

//void HttpRequest::accept()
//{
//    if (hasHeader("expect")){
//        g_log->debug("got some kind of expect header");
//        if (header("expect").toLower() == "100-continue") {
//            g_log->debug("sending 100 continue ok!");
//            m_device->write("HTTP/1.1 100 Continue\r\n\r\n");
//        }
//    }
//}

void HttpRequest::processReadyRead()
{
    // on construction m_validRequest == true and m_headersFinished == false
    QIODevice* device = (QIODevice*) sender();
    QByteArray line;
    while(m_valid && !isComplete() && device->canReadLine()){
        line = device->readLine();
        if (m_method == ""){
            parseRequestLine(line);
        }else{
            readData(line);
        }
    }

    // what if the rest of the data does not end in a line?
    if (m_valid && headersFinished() && !isComplete()){
        if (method() == "GET"){
            setComplete();
        }

        if (method() == "POST" &&
            !device->canReadLine() &&
            !isMultiPart() &&
            hasHeader("content-length") &&
            (device->bytesAvailable() == m_contentLength - m_contentBytesTransferred))
        {
            readData(device->readAll());
            setComplete();
        }
    }
}

void HttpRequest::parseRequestLine(QByteArray line)
{
    if ((m_valid) && (m_method == "") && (!m_headersReceived)){
        // first line is the request string
        QRegExp request_rx("(GET|POST|PUT|HEAD|DELETE|OPTIONS|TRACE|CONNECT) (\\S+) (\\S+)");
        line = line.trimmed();
        if (request_rx.exactMatch(line)){
            m_method = request_rx.cap(1).toAscii().trimmed();
            m_uri = request_rx.cap(2).toAscii().trimmed();
            m_protocol = request_rx.cap(3).toAscii().trimmed();
            g_log->debug("http request: "+line);
            QRegExp protocol_rx("HTTP/1.[01]");
            if (!protocol_rx.exactMatch(m_protocol))
                setInvalid();
        }else{
            // invalid first line. terminate loop
            g_log->error("invalid first line in http request: "+line);
            setInvalid();
        }
    }
}
