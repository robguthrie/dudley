#ifndef HTTPREQUESTCONTENT_H
#define HTTPREQUESTCONTENT_H

#include <QObject>
#include <QByteArray>
#include <QBuffer>
#include <QMap>
#include "readoncebuffer.h"
class QIODevice;

class HttpMessage : public QObject
{
    Q_OBJECT
public:
    HttpMessage(QObject* parent = 0);
    ~HttpMessage();

    void        readData(QByteArray line);

    bool        isValid() const;
    bool        isComplete() const;
    QByteArray  protocol() const;

    qint64      contentLength() const;
    qint64      contentBytesReceived() const;
    qint64      contentBytesSent() const;
    QIODevice*  contentDevice() const;
    void        setContent(QByteArray body);

    bool        hasHeader(QByteArray key) const;
    QByteArray  header(QByteArray key) const;

    // headersReceived or headersSent
    bool        headersReady() const;
    void        printHeaders();

    bool        isMultiPart() const;

    bool                hasPendingFileUploads() const;
    HttpMessage*        getNextFileUploadMessage();
    QByteArray          formFieldName() const;
    QByteArray          formFieldFileName() const;
    QList<HttpMessage*> childMessages() const;

signals:
    void headersReady(); // when the headers have been parsed and are ready to be read
    void complete(qint64 contentBytesReceived); // when the whole thing has been parsed (complete message finished)
    void finished(); // when its finished (may be invalid and finished)
    void fileUploadStarted(); // when we start getting an upload in a multipart format
    void fileUploadComplete(); // when a child message (we must be multipart) has completely uploaded
    void isFileUpload();

public slots:
    void processIsFileUpload();

protected:
    qint64      m_contentLength; // the content length as described by the header of this part
    qint64      m_contentBytesTransferred;
    qint64      m_contentBytesSent;
    QByteArray  m_contentType;
    QByteArray  m_protocol;
    QIODevice*  m_contentDevice;

    void parseHeaderLine(QByteArray line);
    void parseContentLine(QByteArray line);
    void parseMultiPartContentLine(QByteArray line);

    void setInvalid();
    bool m_valid;

    void setComplete();
    bool m_complete;

    void parseHeaders();
    bool m_headersParsed;

    bool m_hasMultipleParts; // true when this is a multipart message (not true for the child messages themselves)
    QByteArray m_formFieldName;
    QByteArray m_formFieldFileName;
    QByteArray m_formDataBoundry; // hopefully contains the boundry when multipart
    QList<HttpMessage*> m_childMessages; // where we keep child messages (from multipart)
    QList<HttpMessage*> m_pendingFileUploadMessages; //  those child messages which are fileuploads

    static QString dateFormat;

private:
    void setHeader(QByteArray key, QByteArray value);
    QMap<QByteArray, QByteArray> m_headers;
    HttpMessage* m_currentMessage;
};

#endif // HTTPREQUESTCONTENT_H
