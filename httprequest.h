#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <QByteArray>
#include <QObject>
#include <QIODevice>
#include <QHash>
#include "httpmessage.h"

class HttpRequest : public HttpMessage {
Q_OBJECT
public:
    HttpRequest(QObject* parent, QIODevice* sockets);
    QByteArray uri() const;
    QByteArray protocol() const;
    QByteArray method() const;
    void       accept();
    QIODevice *device() const;

public slots:
    void processReadyRead();
private:
    void parseRequestLine();

    QByteArray m_method;
    QByteArray m_uri;
    QByteArray m_protocol;
    QIODevice* m_device;
};

#endif /* !_HTTP_REQUEST_ */
