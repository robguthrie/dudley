#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <QByteArray>
#include <QObject>
#include <QIODevice>
#include <QHash>
#include "httpmessage.h"
#include <QVariant>
class HttpRequest : public HttpMessage {
Q_OBJECT
public:
    HttpRequest(QObject* parent, QIODevice* sockets);
    QByteArray uri() const;
    QByteArray protocol() const;
    QByteArray method() const;
    void       accept();
    QIODevice *device() const;
    QHash<QString, QVariant> params();
    void printParams() const;

public slots:
    void parseParams();
    void processReadyRead();

private:
    void parseRequestLine();
    QHash<QString, QVariant> m_params;
    QByteArray m_method;
    QByteArray m_uri;
    QByteArray m_protocol;
    QIODevice* m_device; // the socket we talk on
};

#endif /* !_HTTP_REQUEST_ */
