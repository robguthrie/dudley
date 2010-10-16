#include <QBuffer>
#include <QHash>
#include <QRegExp>
#include <QStringList>
#include "httprequest.h"
#include "output.h"

HttpRequest::HttpRequest(QObject* parent, QIODevice* device)
    : HttpMessage(parent), m_device(device)

{
    m_currentMessage = 0;
    m_method = "";
    Output::debug("hello");
    connect(this, SIGNAL(headersReady()), parent, SLOT(respondToRequest()));
    connect(this, SIGNAL(finished()), parent, SLOT(requestFinished()));
    connect(device, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
    processReadyRead();
}

QIODevice* HttpRequest::device() const
{
    return m_device;
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

    if (!m_headersFinished){
        Output::debug("here i am");
        if (m_method == "") parseRequestLine();
        Output::debug("here i am:"+m_method);
        if (m_method != "") parseHeaders(m_device);
    }
    if (m_headersFinished){
        if (m_method == "POST"){
            parseContent(m_device);
        }else{
            setComplete();
        }
    }

}

void HttpRequest::parseRequestLine()
{
    if ((m_valid) && (m_method == "") && (!m_headersFinished) && (m_device->canReadLine())){
        // first line is the request string
        QRegExp request_rx("(GET|POST|PUT|HEAD|DELETE|OPTIONS|TRACE|CONNECT) (\\S+) (\\S+)");
        QByteArray line = m_device->readLine().trimmed();
        if (request_rx.exactMatch(line)){
            m_method = request_rx.cap(1).toAscii().trimmed();
            m_uri = request_rx.cap(2).toAscii().trimmed();
            m_protocol = request_rx.cap(3).toAscii().trimmed();
            Output::debug("http request: "+line);
            QRegExp protocol_rx("HTTP/1.[01]");
            if (!protocol_rx.exactMatch(m_protocol))
                setInvalid();
        }else{
            // invalid first line. terminate loop
            Output::error("invalid first line in http request: "+line);
            setInvalid();
        }
    }
}
