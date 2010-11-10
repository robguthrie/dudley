#include <QIODevice>
#include <QRegExp>
#include <QStringList>

#include "httprequest.h"
#include "output.h"

HttpRequest::HttpRequest(QObject* parent)
    : HttpMessage(parent)
{
    m_state = ReadFirstLine;
}

QByteArray HttpRequest::uri() const
{
    return m_uri;
}

QByteArray HttpRequest::method() const
{
    return m_method;
}

QByteArray HttpRequest::requestLine() const
{
    return m_requestLine;
}

void HttpRequest::parseLine(QByteArray line)
{
    // read the request line
    if (m_state == ReadFirstLine){
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
            setState(Invalid, "invalid first line in http request: "+m_requestLine);
        }
    }else{
        HttpMessage::parseLine(line);
    }
}

QString HttpRequest::inspect(bool show_headers) const
{
    QString s = "HttpRequest: method="+m_method+" uri="+m_uri+" protocol="+m_protocol+"\n";
    s.append(HttpMessage::inspect(show_headers));
    return s;
}
