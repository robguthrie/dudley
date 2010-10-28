#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_
#include <QByteArray>
#include <QObject>
#include "httpmessage.h"

class HttpRequest : public HttpMessage {
Q_OBJECT
public:
    HttpRequest(QObject* parent, QIODevice* device);
    QByteArray uri() const;
    QByteArray method() const;

public slots:
    void processReadyRead();

private:
    void parseRequestLine(QByteArray line);
    QByteArray m_method;
    QByteArray m_uri;
    QIODevice* m_device;
};

#endif /* !_HTTP_REQUEST_ */
