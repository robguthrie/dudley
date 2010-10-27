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

bool HttpMessage::isValid() const
{
    return m_valid;
}

bool HttpMessage::isEmpty() const
{
    return (m_headers.size() == 0);
}

bool HttpMessage::isComplete() const
{
    return m_complete;
}

QIODevice* HttpMessage::contentDevice()
{
    return &m_data;
}

QList<HttpMessage*> HttpMessage::childMessages() const
{
    return m_messages;
}

QByteArray HttpMessage::formFieldName() const
{
    return m_formFieldName;
}

QByteArray HttpMessage::formFieldFileName() const
{
    return m_formFieldFileName;
}

bool HttpMessage::headersFinished() const
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

void HttpMessage::setComplete()
{
    m_complete = true;
    emit complete(m_contentBytesReceived);
    emit finished();
}

void HttpMessage::setInvalid()
{
    m_valid = false;
    emit finished();
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
                    g_log->debug("matched name:"+value);
                    m_formFieldName = value.toAscii();
                }else if (key == "filename"){
                    g_log->debug("matched filename:"+value);
                    m_formFieldFileName = value.toAscii();
                }else{
                    g_log->debug("unrecognised content-disposition keyvalue pair: "+key+"="+value);
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

    if (!m_formFieldFileName.isEmpty()){
        emit isFileUpload();
    }

    emit headersReady();
    printHeaders();
}

void HttpMessage::parseData(QByteArray data)
{
    if (!m_headersFinished){
        parseHeaderLine(data);
    }else{
        m_contentBytesReceived += data.size();

        if (m_hasMultipleParts){
            // read lines at a time
            parseMultiPartContentLine(data);
        }else{
            m_data.write(data);
        }

        if (hasHeader("content-length")){
            if (m_contentBytesReceived == m_contentLength){
                setComplete();
            }
            if (m_contentBytesReceived > m_contentLength){
                g_log->error("more content arrived than was expected");
                setInvalid();
            }
        }
    }
}

void HttpMessage::parseHeaderLine(QByteArray line)
{
    QRegExp header_rx("([^:]+):(.+)\r\n");
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

void HttpMessage::parseMultiPartContentLine(QByteArray line)
{
    if (line ==  "--"+m_formDataBoundry+"\r\n"){
        // this is the end of the content for the current part, or the frist cp
        if (!m_currentMessage){
            g_log->debug("first boundry line");
        }else{
            g_log->debug("middle data boundry line");
            // the the current message to finsihed and prep a new one
            m_currentMessage->setComplete();
            m_messages << m_currentMessage;
        }
        // prepare new content part for the following data
        m_currentMessage = new HttpMessage();
        connect(m_currentMessage, SIGNAL(isFileUpload()), this, SLOT(processIsFileUpload()));
    }else if (line == "--"+m_formDataBoundry+"--\r\n"){
        g_log->debug("final boundry line");
        m_messages << m_currentMessage;
        m_currentMessage->setComplete();
        m_currentMessage = 0;
        this->setComplete();
    }else if (m_currentMessage){
        m_currentMessage->parseData(line);
    }else{
        g_log->debug("no m_current message.. very strange:"+line);
        setInvalid();
    }
}

void HttpMessage::processIsFileUpload()
{
    // we get this from child messages
    m_pendingFileUploadMessages << (HttpMessage*) sender();
    emit fileUploadStarted();
}

bool HttpMessage::hasPendingFileUploads() const
{
    return (m_pendingFileUploadMessages.size() > 0);
}

HttpMessage*  HttpMessage::getNextFileUploadMessage()
{
    return m_pendingFileUploadMessages.takeFirst();
}

void HttpMessage::printHeaders()
{
    QMap<QByteArray, QByteArray>::const_iterator i = m_headers.begin();
    for(; i != m_headers.end(); ++i){
        g_log->debug(i.key()+":"+i.value());
    }

    if (m_hasMultipleParts){
        g_log->debug("m_formDataBoundry: "+m_formDataBoundry);
        g_log->debug("m_formFieldName: "+m_formFieldName);
        g_log->debug("m_formFieldFileName: "+m_formFieldFileName);
    }
    if (m_complete) g_log->debug("complete message");
    if (!m_valid) g_log->debug("invalid message");
}

bool HttpMessage::isMultiPart() const
{
    return m_hasMultipleParts;
}
