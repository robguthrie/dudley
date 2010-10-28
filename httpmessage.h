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


    bool                isValid() const;
    bool                isComplete() const;

    QByteArray          protocol() const;
    bool                hasHeader(QByteArray key) const;
    QVariant            header(QByteArray key) const;
    void                setHeader(QByteArray key, QVariant value);
    virtual QByteArray  headers() const;
    void                printHeaders();

    void                parseLine(QByteArray line);
    void                setContent(QByteArray body);
    qint64              contentLength() const;
    qint64              contentBytesTransferred() const;
    QIODevice*          contentDevice() const;
    void                setContentDevice(QIODevice* device);

    bool                isMultiPart() const;
    bool                hasPendingFileUploads() const;
    HttpMessage*        getNextFileUploadMessage();
    QByteArray          formFieldName() const;
    QByteArray          formFieldFileName() const;

signals:
    void                headersFinished(); // when all the header block has arrived
    void                complete(qint64 contentBytesReceived); // when the whole thing has arrived and is valid
    void                finished(); // when its finished (may be invalid and finished)
    void                fileUploadStarted(); // when we start getting an upload in a multipart format
    void                fileUploadComplete(); // when a child message (we must be multipart) has completely uploaded
    void                isFileUpload();

protected slots:
    void                processIsFileUpload();

protected:
    qint64              m_contentLength; // the content length as described by the header of this part
    qint64              m_contentBytesTransferred;
    QByteArray          m_protocol;
    QIODevice*          m_contentDevice;

    void                parseMultiPartContentLine(QByteArray line);

    void                setInvalid();
    bool                m_valid;

    void                setComplete();
    bool                m_complete;


    bool                m_headersFinished;

    bool                m_hasMultipleParts; // true when this is a multipart message (not true for the child messages themselves)
    QByteArray          m_formFieldName;
    QByteArray          m_formFieldFileName;
    QByteArray          m_formDataBoundry; // hopefully contains the boundry when multipart
    QList<HttpMessage*> m_childMessages; // where we keep child messages (from multipart)
    QList<HttpMessage*> m_pendingFileUploadMessages; //  those child messages which are fileuploads

    static QString      dateFormat;

private:
    QMap<QByteArray, QVariant> m_headers;
    HttpMessage*        m_currentMessage;
};

#endif // HTTPREQUESTCONTENT_H
