#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <QByteArray>
#include <QObject>
#include <QIODevice>
#include <QHash>

class HttpRequest : public QObject {
Q_OBJECT
public:
    HttpRequest(QObject* parent, QIODevice* sockets);
    bool isValid() const;
    QByteArray uri() const;
    QByteArray protocol() const;
    QByteArray method() const;
    bool       accept();
    bool       hasHeader(QByteArray key) const;
    QByteArray header(QByteArray key) const;
    bool       acceptsEncoding(QByteArray type);
    QIODevice *device() const;
signals:
    void ready(); // this fires when we have parsed the http request
    void readyRead(); // this fires when there is content to read
public slots:
    void processReadyRead();
private:
    QByteArray m_method;
    QByteArray m_uri;
    QByteArray m_protocol;
    qint64 m_contentLength;
    bool m_validRequest;
    bool m_headersFinished;
    QHash<QByteArray, QByteArray> m_headers;
    QIODevice* m_device;
};

#endif /* !_HTTP_REQUEST_ */

