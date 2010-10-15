#ifndef _HTTP_REQUEST_H_
#define _HTTP_REQUEST_H_

#include <QByteArray>
#include <QObject>
#include <QIODevice>
#include <QHash>
#include "httpmessage.h"

class HttpRequest : public QObject {
Q_OBJECT
public:
    HttpRequest(QObject* parent, QIODevice* sockets);
    bool isValid() const;
    QByteArray uri() const;
    QByteArray protocol() const;
    QByteArray method() const;
    void       accept();
    bool       hasHeader(QByteArray key) const;
    QByteArray header(QByteArray key) const;
    bool       acceptsEncoding(QByteArray type);
    QIODevice *device() const;

signals:
    void headersReady();
    void contentReady(HttpMessage*);
    void finished();

public slots:
    void processReadyRead();
private:
    void parseRequestLine();
    void parseHeaders();
    void printHeaders();
    void parseContent();
    void parseMultiPartContent();
    QByteArray m_method;
    QByteArray m_uri;
    QByteArray m_protocol;
    qint64 m_contentLength;
    bool m_validRequest;
    bool m_headersFinished;
    QMap<QByteArray, QByteArray> m_headers;
    QIODevice* m_device;
    QByteArray m_formDataBoundry;
    qint64 m_contentBytesReceived;
    QList<HttpMessage*> m_contentParts;
    HttpMessage* m_currentContentPart;
};

#endif /* !_HTTP_REQUEST_ */

