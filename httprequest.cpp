#include <QIODevice>
#include <QRegExp>
#include <QStringList>

#include "httprequest.h"
#include "output.h"

HttpRequest::HttpRequest(QObject* parent)
    : HttpMessage(parent)
{
}

QByteArray HttpRequest::uri() const
{
    return m_uri;
}

QByteArray HttpRequest::method() const
{
    return m_method;
}

void HttpRequest::parseData(QIODevice* device)
{
    // read the request line
    if (m_state == BeforeStart && device->canReadLine()){
        QByteArray line = device->readLine();
        QRegExp request_rx("(GET|POST|PUT|HEAD|DELETE|OPTIONS|TRACE|CONNECT) (\\S+) (\\S+)\r\n");
        if (request_rx.exactMatch(line)){
            m_method = request_rx.cap(1).toAscii().trimmed();
            m_uri = request_rx.cap(2).toAscii().trimmed();
            m_protocol = request_rx.cap(3).toAscii().trimmed();
            QRegExp protocol_rx("HTTP/1.[01]");
            if (!protocol_rx.exactMatch(m_protocol)){
                setState(Invalid);
            }else{
                setState(ReadingHeaders);
            }
        }else{
            // invalid first line. terminate loop
            setState(Invalid, "invalid first line in http request: "+line);
        }
    }
    HttpMessage::parseData(device);
}
