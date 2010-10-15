#ifndef HTTPREQUESTCONTENT_H
#define HTTPREQUESTCONTENT_H

#include <QObject>
#include <QByteArray>
#include <QMap>

class HttpMessage : public QObject
{
    Q_OBJECT
public:
    HttpMessage(QObject* parent = 0);
    void setHeadersFinished();
    bool headersFinsihed();
    void printHeaders();
    bool isEmpty();
    void setHeader(QByteArray key, QByteArray value);
    QByteArray header(QByteArray key);
    QByteArray m_data;
    void setComplete();
    bool complete();

private:
    qint64 m_contentLength; // the content length as described by the header of this part
    bool m_headersFinished;
    QMap<QByteArray, QByteArray> m_headers;

    // m_name and m_fileName are captured from the content-disposition header
    QByteArray m_name;
    QByteArray m_fileName;
    bool m_isFormData; // this is true if c-d header has "form-data" in it
    bool m_complete;
};

#endif // HTTPREQUESTCONTENT_H
