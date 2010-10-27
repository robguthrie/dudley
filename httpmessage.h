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
    void parseData(QByteArray line);
    bool headersFinished() const;
    void printHeaders();
    bool isEmpty() const;
    bool isValid() const;
    bool isComplete() const;
    bool isMultiPart() const;
    bool hasHeader(QByteArray key) const;
    bool hasPendingFileUploads() const;
    HttpMessage*  getNextFileUploadMessage();
    qint64 contentLength() const;
    qint64 contentBytesReceived() const;
    QByteArray header(QByteArray key) const;
    QByteArray formFieldName() const;
    QByteArray formFieldFileName() const;
    QIODevice* contentDevice();
    QList<HttpMessage*> childMessages() const;

signals:
    void headersReady(); // when the headers have been parsed
    void complete(qint64 contentBytesReceived); // when the whole thing has been parsed (complete message finished)
    void finished(); // when its finished (may be invalid and finished)
    void fileUploadStarted(); // when we start getting an upload in a multipart format
    void fileUploadComplete(); // when a child message (we must be multipart) has completely uploaded
    void isFileUpload();

public slots:
    void processIsFileUpload();

protected:
    QBuffer m_data;
    void parseHeaderLine(QByteArray line);
    void parseContentLine(QByteArray line);
    void parseMultiPartContentLine(QByteArray line);

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

    bool m_hasMultipleParts; // true when this is a multipart message (not true for the child messages themselves)
    QByteArray m_formFieldName;
    QByteArray m_formFieldFileName;
    QByteArray m_formDataBoundry; // hopefully contains the boundry when multipart
    QList<HttpMessage*> m_messages; // where we keep child messages (from multipart)
    HttpMessage* m_currentMessage;
    QList<HttpMessage*> m_pendingFileUploadMessages;
};

#endif // HTTPREQUESTCONTENT_H
