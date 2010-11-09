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
    void parseData(QIODevice *device);

private:
    void parseRequestLine(QByteArray line);
    QByteArray m_method;
    QByteArray m_uri;
};

#endif /* !_HTTP_REQUEST_ */
