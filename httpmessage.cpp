#include "httpmessage.h"
#include <QRegExp>
#include "output.h"

QString HttpMessage::dateFormat = QString("ddd, dd MMM yyyy hh:mm:ss G'M'T");

HttpMessage::HttpMessage(QObject* parent)
    :QObject(parent)
{
    m_contentType = "text/html; charset=utf-8";
    m_contentLength = 0;
    m_contentBytesTransferred = 0;
    m_contentBytesSent = 0;
    m_headersReady = false;
    m_headersSent = false;
    m_hasMultipleParts = false;
    m_complete = false;
    m_valid = true;
    m_protocol = "HTTP/1.1";
    m_data.open(QBuffer::ReadWrite | QBuffer::Append);
}

HttpMessage::~HttpMessage()
{
    foreach(HttpMessage* message, m_childMessages){
        message->deleteLater();
    }
}

QByteArray HttpMessage::protocol() const
{
    return m_protocol;
}

QByteArray HttpMessage::setProtocol(QByteArray protocol)
{
    m_protcol = protocol;
}

bool HttpMessage::hasHeader(QByteArray key) const
{
    return m_headers.contains(key.toLower());
}

//private
void HttpMessage::setHeader(QByteArray key, QByteArray value)
{
    m_headers.insert(key.trimmed().toLower(), value.trimmed());
}

void HttpMessage::setContentType(QByteArray contentType)
{
    m_contentType = contentType;
}

qint64 HttpMessage::contentLength() const
{
    return m_contentLength;
}

void HttpMessage::setContentLength(quint64 size)
{
    m_contentLength = size;
}

qint64 HttpMessage::contentBytesTransferred() const
{
    return m_contentBytesTransferred;
}

bool HttpMessage::isValid() const
{
    return m_valid;
}

void HttpMessage::setInvalid()
{
    m_valid = false;
    emit finished();
}

bool HttpMessage::isComplete() const
{
    return m_complete;
}

QIODevice* HttpMessage::contentDevice()
{
    return m_contentDevice;
}

void HttpMessage::setContentDevice(QIODevice *file)
{
    m_contentDevice = file;
}

QList<HttpMessage*> HttpMessage::childMessages() const
{
    return m_childMessages;
}

QByteArray HttpMessage::formFieldName() const
{
    return m_formFieldName;
}

QByteArray HttpMessage::formFieldFileName() const
{
    return m_formFieldFileName;
}

void HttpMessage::setComplete()
{
    m_complete = true;
    emit complete(m_contentBytesTransferred);
    emit finished();
}

void HttpMessage::parseHeaders()
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

    m_headersReady = true;

    if (!m_formFieldFileName.isEmpty()){
        emit isFileUpload();
    }
    emit headersReady();
}

void HttpMessage::readData(QByteArray data)
{
    if (!m_headersReceived){
        parseHeaderLine(data);
    }else{
        m_contentBytesTransferred += data.size();

        if (m_hasMultipleParts){
            // read lines at a time
            parseMultiPartContentLine(data);
        }else{
            m_data.write(data);
        }

        if (hasHeader("content-length")){
            if (m_contentBytesTransferred == m_contentLength){
                setComplete();
            }
            if (m_contentBytesTransferred > m_contentLength){
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
        parseHeaders();
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
            m_childMessages << m_currentMessage;
        }
        // prepare new content part for the following data
        m_currentMessage = new HttpMessage();
        connect(m_currentMessage, SIGNAL(isFileUpload()), this, SLOT(processIsFileUpload()));
    }else if (line == "--"+m_formDataBoundry+"--\r\n"){
        g_log->debug("final boundry line");
        m_childMessages << m_currentMessage;
        m_currentMessage->setComplete();
        m_currentMessage = 0;
        this->setComplete();
    }else if (m_currentMessage){
        m_currentMessage->readData(line);
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
