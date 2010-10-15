#include "httpmessage.h"
#include <QRegExp>
#include "output.h"
HttpMessage::HttpMessage(QObject* parent)
    :QObject(parent)
{
    m_headersFinished = false;
    m_isFormData = false;
    m_complete = false;
}

void HttpMessage::setHeadersFinished()
{
    m_headersFinished = true;
    // read the content-disposition header
    if (m_headers.contains("content-disposition")){
        QList<QByteArray> tokens = m_headers.value("content-disposition").split(';');
        QRegExp kvrx("([^=])=(.*)"); // match key=value type bits
        foreach(QByteArray token, tokens){
            if (kvrx.exactMatch(token.trimmed())){
                QString key = kvrx.cap(1);
                QString value = kvrx.cap(2);
                if (key == "name"){
                    m_name = value.toAscii();
                }else if (key == "filename"){
                    m_fileName = value.toAscii();
                }else{
                    Output::debug("unrecognised content-disposition keyvalue pair: "+key+"="+value);
                }
            }
        }
    }
}
bool HttpMessage::isEmpty()
{
    return (m_headers.size() == 0);
}

bool HttpMessage::headersFinsihed()
{
    return m_headersFinished;
}

void HttpMessage::setHeader(QByteArray key, QByteArray value)
{
    m_headers.insert(key.toLower(), value);
}

QByteArray HttpMessage::header(QByteArray key)
{
    return m_headers.value(key.toLower());
}

void HttpMessage::printHeaders()
{
    QMap<QByteArray, QByteArray>::const_iterator i = m_headers.begin();
    for(; i != m_headers.end(); ++i)
    {
        Output::debug(i.key()+":"+i.value());
    }
}

void HttpMessage::setComplete()
{
    m_complete = true;
}

bool HttpMessage::complete()
{
    return m_complete;
}
