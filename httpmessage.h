#ifndef HTTPREQUESTCONTENT_H
#define HTTPREQUESTCONTENT_H

#include <QObject>
#include <QByteArray>
#include <QVariant>
#include <QBuffer>
#include <QMap>
#include "readoncebuffer.h"

class HttpMessage : public QObject
{
    Q_OBJECT

public:
    enum State {
        Invalid,
        ReadFirstLine,
        ReadingHeaders,
        WaitingForContentDevice,
        ReadingContent,
        Finished
    };
    Q_ENUMS(State);
    HttpMessage(QObject* parent = 0, HttpMessage* parent_message = 0);
    ~HttpMessage();
    State               state() const;
    void                setState(State state, QByteArray message = "");
    QByteArray          protocol() const;
    bool                hasHeader(QByteArray key) const;
    QVariant            header(QByteArray key) const;
    void                setHeader(QByteArray key, QVariant value);
    virtual QByteArray  headers() const;

    virtual void        parseLine(QByteArray line);
    void                setContent(QByteArray body);
    qint64              contentLength() const;
    qint64              headerLength() const;
    qint64              contentBytesTransferred() const;
    QIODevice*          contentDevice() const;
    void                setContentDevice(QIODevice* device);

    bool                isMultiPart() const;
    HttpMessage*        parentMessage() const;
    bool                hasPendingFileUploads() const;
    HttpMessage*        getNextFileUploadMessage();
    QByteArray          formFieldName() const;
    QByteArray          formFieldFileName() const;
    QList<HttpMessage*> childMessages() const;
    virtual QString     inspect(bool show_headers = false) const;
signals:
    void                headersFinished(); // when all the header block has arrived
    void                complete(qint64 contentBytesReceived); // when the whole thing has arrived and is valid
    void                contentBytesWritten(qint64 bw); // when bytes have been written to the contentDevice
    void                finished(); // when its finished (may be invalid and finished)
    void                fileUploadStarted(); // when we start getting an upload in a multipart format
    void                isFileUpload(); // when a child message discovered it contains a file

protected slots:
    void                processIsFileUpload();

protected:
    State               m_state;

    qint64              m_contentLength; // the content length as described by the header of this part
    qint64              m_contentBytesTransferred; // number of content bytes received
    qint64              m_headerLength;  // number of header bytes we have counted
    QByteArray          m_protocol;
    QIODevice*          m_contentDevice;
    bool                m_ownContentDevice;

    void                createOwnContentDevice();
    void                destroyOwnContentDevice();
    qint64              writeContentBytes(QByteArray data);
    void                setInvalid(QByteArray error = "");
    void                setComplete();

    bool                m_isMultiPart; // true when this is a multipart message (not true for the child messages themselves)
    QByteArray          m_formFieldName;
    QByteArray          m_formFieldFileName;
    QByteArray          m_formDataBoundary; // hopefully contains the boundry when multipart
    QList<HttpMessage*> m_childMessages; // where we keep child messages (from multipart)
    QList<HttpMessage*> m_pendingFileUploadMessages; //  those child messages which are fileuploads
    HttpMessage*        m_parentMessage;
    static QString      dateFormat;

private:
    QMap<QByteArray, QVariant> m_headers;
    HttpMessage*        m_currentMessage;
    bool                m_maybeLastLine;
    QByteArray          m_boundry;
    QByteArray          m_f_boundry;
    QByteArray          m_line;
};

#endif // HTTPREQUESTCONTENT_H
