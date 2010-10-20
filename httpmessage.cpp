#include "httpmessage.h"
#include <QRegExp>
#include "output.h"
HttpMessage::HttpMessage(QObject* parent)
    :QObject(parent)
{
    m_contentLength = 0;
    m_contentBytesReceived = 0;
    m_headersFinished = false;
    m_hasMultipleParts = false;
    m_complete = false;
    m_valid = true;
    m_data.open(QBuffer::ReadWrite | QBuffer::Append);
}

HttpMessage::~HttpMessage()
{
    foreach(HttpMessage* message, m_messages){
        message->deleteLater();
    }
}

qint64 HttpMessage::contentLength() const
{
    return m_contentLength;
}

qint64 HttpMessage::contentBytesReceived() const
{
    return m_contentBytesReceived;
}

QBuffer* HttpMessage::contentDevice()
{
    return &m_data;
}

QByteArray HttpMessage::formFieldName() const
{
    return m_formFieldName;
}

QByteArray HttpMessage::formFieldFileName() const
{
    return m_formFieldFileName;
}

void HttpMessage::setHeadersFinished()
{
    // read the content-disposition header (probably only found in a multipart message
    if (m_headers.contains("content-disposition")){
        QList<QByteArray> tokens = m_headers.value("content-disposition").split(';');
        QRegExp kvrx("([^=]+)=[\"']?([^\"']+)[\"']?"); // match key="value" type bits
        foreach(QByteArray token, tokens){
            if (kvrx.exactMatch(token.trimmed())){
                QString key = kvrx.cap(1);
                QString value = kvrx.cap(2);
                if (key == "name"){
                    Output::debug("matched name:"+value);
                    m_formFieldName = value.toAscii();
                }else if (key == "filename"){
                    Output::debug("matched filename:"+value);
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
            m_hasMultipleParts = true;
            m_formDataBoundry = form_data_rx.cap(1).toUtf8().trimmed();
        }
    }

    if (hasHeader("content-length")){
        m_contentLength = header("content-length").toLongLong();
    }

    m_headersFinished = true;
    printHeaders();

    emit headersReady();

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
    m_complete = true;
    emit complete(m_contentBytesReceived);
    emit finished();
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

bool HttpMessage::headersFinsihed() const
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
    QRegExp header_rx("([^:]+):(.+)\r\n");
    while ((m_valid) && (!m_headersFinished) && (device->canReadLine())){
        line = device->readLine();
        if (line == "\r\n"){
            // empty line. normal end of headers
            setHeadersFinished();
        }else if (header_rx.exactMatch(line)){
            // read the header
            setHeader(header_rx.cap(1).toAscii(), header_rx.cap(2).toAscii());
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
        if (m_hasMultipleParts){
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
        if (line ==  "--"+m_formDataBoundry+"\r\n"){
            // this is the end of the content for the current part, or the frist cp
            if (!m_currentMessage){
                Output::debug("first boundry line");
            }else{
                Output::debug("middle data boundry line");
                // the the current message to finsihed and prep a new one
                m_currentMessage->setComplete();
                m_messages << m_currentMessage;
            }
            // prepare new content part for the following data
            m_currentMessage = new HttpMessage();
        }else if (line == "--"+m_formDataBoundry+"--\r\n"){
            Output::debug("final boundry line");
            m_messages << m_currentMessage;
            m_currentMessage->setComplete();
            m_currentMessage = 0;
            this->setComplete();
        }else if (m_currentMessage){
            if (m_currentMessage->headersFinsihed()){
                m_contentBytesReceived += line.size();
                // the headers are finished already. this is data
                if (qint64 bw = m_currentMessage->m_data.write(line)){
//                    Output::debug("wrote "+QString::number(bw)+" bytes to m_data (the upload buffer)");
//                    Output::debug("m_data size:"+QString::number(m_currentMessage->m_data.size()));
                }else{
                    Output::debug("failed to write "+QString::number(bw)+" bytes to m_data (the upload buffer)");
                    setInvalid();
                }
            }else if (header_rx.exactMatch(line)){
                m_currentMessage->setHeader(header_rx.cap(1).toAscii(), header_rx.cap(2).toAscii());
            }else if (line == "\r\n"){
                // this empty line indicates the end of headers
                Output::debug("end of headers (of a multipart child message)");
                m_currentMessage->setHeadersFinished();
                if (!m_currentMessage->formFieldFileName().isEmpty()){
                    m_pendingFileUploadMessages << m_currentMessage;
                    emit fileUploadStarted();
                }
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

bool HttpMessage::hasPendingFileUploads() const
{
    return (m_pendingFileUploadMessages.size() > 0);
}

HttpMessage*  HttpMessage::getNextFileUploadMessage()
{
    Output::debug("returning next upload message.. buffer size: "+QString::number(m_pendingFileUploadMessages.first()->contentDevice()->size()));
    return m_pendingFileUploadMessages.takeFirst();
}

void HttpMessage::printHeaders()
{
    QMap<QByteArray, QByteArray>::const_iterator i = m_headers.begin();
    for(; i != m_headers.end(); ++i){
        Output::debug(i.key()+":"+i.value());
    }

    if (m_hasMultipleParts){
        Output::debug("m_formDataBoundry: "+m_formDataBoundry);
        Output::debug("m_formFieldName: "+m_formFieldName);
        Output::debug("m_formFieldFileName: "+m_formFieldFileName);
    }
    if (m_complete) Output::debug("complete message");
    if (!m_valid) Output::debug("invalid message");
}

bool HttpMessage::isMultiPart() const
{
    return m_hasMultipleParts;
}
