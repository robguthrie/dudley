#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_
#include <QByteArray>
#include <QObject>
#include "httpmessage.h"

class HttpRequest : public HttpMessage {
Q_OBJECT
public:
    HttpRequest(QObject* parent);
    QByteArray uri() const;
    QByteArray method() const;
    QByteArray requestLine() const;
    void parseLine(QByteArray line);
    QString inspect(bool show_headers) const;
private:
    QByteArray m_method;
    QByteArray m_uri;
    QByteArray m_requestLine;
};

#endif /* !_HTTP_REQUEST_ */
