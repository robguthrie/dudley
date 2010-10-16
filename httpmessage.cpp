#include "httpmessage.h"
#include <QRegExp>
#include "output.h"
HttpMessage::HttpMessage(QObject* parent)
    :QObject(parent)
{
    m_headersFinished = false;
    m_multiPart = false;
    m_complete = false;
    m_valid = true;
}

void HttpMessage::setHeadersFinished()
{
    // read the content-disposition header (probably only found in a multipart message
    if (m_headers.contains("content-disposition")){
        QList<QByteArray> tokens = m_headers.value("content-disposition").split(';');
        QRegExp kvrx("([^=])=(.*)"); // match key=value type bits
        foreach(QByteArray token, tokens){
            if (kvrx.exactMatch(token.trimmed())){
                QString key = kvrx.cap(1);
                QString value = kvrx.cap(2);
                if (key == "name"){
                    m_formFieldName = value.toAscii();
                }else if (key == "filename"){
                    m_formFieldFileName = value.toAscii();
                }else{
                    Output::debug("unrecognised content-disposition keyvalue pair: "+key+"="+value);
                }
            }
        }
    }

    if (m_headers.contains("content-type")){
        QRegExp form_data_rx("multipart/form-data; boundary=(.*)");
        form_data_rx.setCaseSensitivity(Qt::CaseInsensitive);
        if (form_data_rx.exactMatch(header("content-type"))){
            m_multiPart = true;
            m_formDataBoundry = form_data_rx.cap(1).toUtf8().trimmed();
            Output::debug("found out the form data boundry: "+m_formDataBoundry);
        }else{
            Output::debug("unrecognised message content-type: "+header("content-type"));
        }
    }

    if (hasHeader("content-length")){
        m_contentLength = header("content-length").toLongLong();
    }

    m_headersFinished = true;
    emit headersReady();
    Output::debug("message headers finished:");
    printHeaders();
}

bool HttpMessage::isValid() const
{
    return m_valid;
}

bool HttpMessage::isEmpty() const
{
    return (m_headers.size() == 0);
}

void HttpMessage::setComplete()
{
    emit ready();
    emit finished();
    m_complete = true;
}

bool HttpMessage::isComplete() const
{
    return m_complete;
}

void HttpMessage::setInvalid()
{
    m_valid = false;
    emit finished();
}

bool HttpMessage::headersFinsihed()
{
    return m_headersFinished;
}

void HttpMessage::setHeader(QByteArray key, QByteArray value)
{
    m_headers.insert(key.trimmed().toLower(), value.trimmed());
}
bool HttpMessage::hasHeader(QByteArray key) const
{
    return m_headers.contains(key.toLower());
}

QByteArray HttpMessage::header(QByteArray key) const
{
    return m_headers.value(key.toLower());
}

void HttpMessage::parseHeaders(QIODevice* device)
{
    // read 0 or more header lines..
    QByteArray line;
    QRegExp header_rx("([^:]+):(.+)");
    while ((m_valid) && (!m_headersFinished) && (device->canReadLine())){
        line = device->readLine().trimmed();
        if (line.length() == 0){
            // empty line. normal end of headers
            setHeadersFinished();
        }else if (header_rx.exactMatch(line)){
            // read the header
            setHeader(header_rx.cap(1).toAscii(),
                      header_rx.cap(2).toAscii());
        }else{
            // header is invalid.. termate loop
            setInvalid();
        }
    }
}

void HttpMessage::parseContent(QIODevice* device)
{
    if ((m_valid) && (m_headersFinished) && (device->bytesAvailable())){
        // if the request is HTTP/1.1 then we wont have content until we accept
        if (m_multiPart){
            parseMultiPartContent(device);
        }else{
            // just signal that there is content to read in the device?
            if (hasHeader("content-length")){
                Output::debug(" more content, not multipart, but there is a content length.");
                if (m_contentLength < (m_contentBytesReceived + device->bytesAvailable())){
                    qint64 n_extra_bytes = (m_contentBytesReceived + device->bytesAvailable()) - m_contentLength;
                    Output::error("more data than expected has arrived in the request, by "+QString::number(n_extra_bytes)+" bytes");
                }
            }else{
                Output::debug("data available from request device.. but it's not multipart and there is no content length ");
            }
        }
    }
    
}

void HttpMessage::parseMultiPartContent(QIODevice* device){
    // request is multipart format.
    QByteArray line;
    QRegExp header_rx("([^:]+):(.+)\r\n");

    while (device->canReadLine()){
        line = device->readLine();
        m_contentBytesReceived += line.size();
        if (line ==  "--"+m_formDataBoundry+"\r\n"){
            // this is the end of the content for the current part, or the frist cp
            if (m_currentMessage){
                Output::debug("middle data boundry line:"+line);
                m_currentMessage->setComplete();
                m_messages << m_currentMessage;
            }else{
                Output::debug("first boundry line:"+line);
            }
            // prepare new content part for the following data
            m_currentMessage = new HttpMessage();
        }else if (line == ("--"+m_formDataBoundry+"--\r\n")){
            Output::debug("final boundry:"+line);
            // last boundry line of the request
            this->setComplete();
            m_currentMessage = 0;
            Output::debug("found last boundry line!");
        }else if (m_currentMessage){
            if (m_currentMessage->headersFinsihed()){
                // if the headers are finished, this is either a boundry or data
                Output::debug("data line:"+line);
                m_currentMessage->m_data += line;
            }else if (header_rx.exactMatch(line)){
                // this is either a header or a blank line to indicate end of headers
                Output::debug("matched header line: "+line);
                m_currentMessage->setHeader(header_rx.cap(1).toAscii(), header_rx.cap(2).toAscii());
            }else if (line == "\r\n"){
                // its supposed to a blank line - headers finished or end of content
                Output::debug("end of headers line. (blank)");
                m_currentMessage->setHeadersFinished();
            }else{
                Output::debug("invalid httpmessage; unexpect line: "+line);
                setInvalid();
            }
        }else{
            Output::debug("no m_current message.. very strange:"+line);
            setInvalid();
        }
    }
}

void HttpMessage::printHeaders()
{
    QMap<QByteArray, QByteArray>::const_iterator i = m_headers.begin();
    for(; i != m_headers.end(); ++i){
        Output::debug(i.key()+":"+i.value());
    }

    if (m_multiPart){
        Output::debug("m_formDataBoundry: "+m_formDataBoundry);
        Output::debug("m_formFieldName: "+m_formFieldName);
        Output::debug("m_formFieldFileName: "+m_formFieldFileName);
    }
    if (m_complete) Output::debug("complete message");
    if (!m_valid) Output::debug("invalid message");
}
