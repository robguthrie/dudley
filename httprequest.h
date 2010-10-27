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
    QByteArray protocol() const;
    QByteArray method() const;
    //    void       accept();

public slots:
    void processReadyRead();

private:
    void parseRequestLine(QByteArray line);
    QByteArray m_method;
    QByteArray m_uri;
    QByteArray m_protocol;
};

#endif /* !_HTTP_REQUEST_ */
