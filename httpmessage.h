#ifndef HTTPREQUESTCONTENT_H
#define HTTPREQUESTCONTENT_H

#include <QObject>
#include <QByteArray>
#include <QMap>
class QIODevice;

class HttpMessage : public QObject
{
    Q_OBJECT
public:
    HttpMessage(QObject* parent = 0);

    bool headersFinsihed();
    void printHeaders();
    bool isEmpty() const;
    bool isValid() const;
    bool isComplete() const;
    bool hasHeader(QByteArray key) const;
    QByteArray header(QByteArray key) const;
    QByteArray m_data;

signals:
    void headersReady(); // when the headers have been parsed
    void ready(); // when the whole thing has been parsed (complete message finished)
    void finished(); // when its finished (may be invalid and finished)

protected:
    void parseHeaders(QIODevice* device);
    void parseContent(QIODevice* device);
    void parseMultiPartContent(QIODevice* device);

    void setInvalid();
    bool m_valid;

    void setComplete();
    bool m_complete;

    qint64 m_contentLength; // the content length as described by the header of this part
    qint64 m_contentBytesReceived;

    void setHeader(QByteArray key, QByteArray value);
    void setHeadersFinished();
    QMap<QByteArray, QByteArray> m_headers;
    bool m_headersFinished;

    bool m_multiPart; // true when this is a multipart formdata message
    QByteArray m_formFieldName;
    QByteArray m_formFieldFileName;
    QByteArray m_formDataBoundry; // hopefully contains the boundry when multipart
    QList<HttpMessage*> m_messages; // where we keep child messages (from multipart)
    HttpMessage* m_currentMessage;
};

#endif // HTTPREQUESTCONTENT_H
