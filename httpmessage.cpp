#include "httpmessage.h"
#include <QRegExp>
#include <QVariant>

#include "output.h"

QString HttpMessage::dateFormat = QString("ddd, dd MMM yyyy hh:mm:ss G'M'T");

HttpMessage::HttpMessage(QObject* parent)
    :QObject(parent)
{
    m_protocol = "HTTP/1.1";
    m_contentBytesTransferred = 0;
    m_contentDevice = 0;
    m_headersFinished = false;
    m_hasMultipleParts = false;
    m_valid = true;
    m_complete = false;
}

HttpMessage::~HttpMessage()
{
    foreach(HttpMessage* message, m_childMessages){
        message->deleteLater();
    }
    // also delete the content device
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

void HttpMessage::setComplete()
{
    m_complete = true;
    emit complete(m_contentBytesTransferred);
    emit finished();
}

QByteArray HttpMessage::protocol() const
{
    return m_protocol;
}

bool HttpMessage::hasHeader(QByteArray key) const
{
    return m_headers.contains(key.toLower());
}

QVariant HttpMessage::header(QByteArray key) const
{
    return m_headers.value(key.toLower());
}

QByteArray HttpMessage::headers() const
{
    QByteArray text;
    QMap<QByteArray, QVariant>::const_iterator i;
    for(i = m_headers.begin(); i != m_headers.end(); ++i){
        text += i.key()+": "+i.value().toByteArray()+"\r\n";
    }
    text += "\r\n";
    return text;
}

void HttpMessage::setContent(QByteArray body)
{
    QByteArray* nb = new QByteArray(body);
    QIODevice* d = (QIODevice*) new QBuffer(nb);
    d->open(QIODevice::ReadOnly);
    setContentDevice(d);
    setHeader("Content-Length", body.size());
}

qint64 HttpMessage::contentLength() const
{
    return m_contentLength;
}

qint64 HttpMessage::contentBytesTransferred() const
{
    return m_contentBytesTransferred;
}

QIODevice* HttpMessage::contentDevice() const
{
    return m_contentDevice;
}

void HttpMessage::setContentDevice(QIODevice *file)
{
    m_contentDevice = file;
}

void HttpMessage::parseLine(QByteArray line)
{
    if (!m_headersFinished){
        QRegExp header_rx("([^:]+):(.+)\r\n");
        if (line == "\r\n"){
            // empty line. normal end of headers
            m_headersFinished = true;
            emit headersFinished();
        }else if (header_rx.exactMatch(line)){
            // read the header
            setHeader(header_rx.cap(1).toAscii(), header_rx.cap(2));
        }else{
            // header is invalid.. termate loop
            setInvalid();
        }
    }else{
        m_contentBytesTransferred += line.size();

        if (m_hasMultipleParts){
            // read lines at a time
            parseMultiPartContentLine(line);
        }else{
            m_contentDevice->write(line);
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

// protected
void HttpMessage::setHeader(QByteArray key, QVariant value)
{
    key = key.trimmed().toLower();
    m_headers.insert(key, value);

    // read the content-disposition header (probably only found in a multipart message
    if (key == "content-disposition"){
        QList<QByteArray> tokens = value.toByteArray().split(';');
        QRegExp kvrx("([^=]+)=[\"']?([^\"']+)[\"']?"); // match key="value" type bits
        foreach(QByteArray token, tokens){
            if (kvrx.exactMatch(token.trimmed())){
                QString key = kvrx.cap(1);
                QString value = kvrx.cap(2);
                if (key == "name"){
                    m_formFieldName = value.toAscii();
                }else if (key == "filename"){
                    m_formFieldFileName = value.toAscii();
                }else{
                    g_log->debug("unrecognised content-disposition keyvalue pair: "+key+"="+value);
                }
            }
        }
    }else if (key == "content-type"){
        QRegExp form_data_rx("multipart/form-data; boundary=(.*)");
        form_data_rx.setCaseSensitivity(Qt::CaseInsensitive);
        if (form_data_rx.exactMatch(value.toByteArray())){
            m_hasMultipleParts = true;
            m_formDataBoundry = form_data_rx.cap(1).toUtf8().trimmed();
        }
    }else if (key == "content-length"){
        m_contentLength = value.toLongLong();
    }
}

// protected
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
        m_currentMessage->parseLine(line);
    }else{
        g_log->debug("no m_current message.. very strange:"+line);
        setInvalid();
    }
}

bool HttpMessage::isMultiPart() const
{
    return m_hasMultipleParts;
}

QByteArray HttpMessage::formFieldName() const
{
    return m_formFieldName;
}

QByteArray HttpMessage::formFieldFileName() const
{
    return m_formFieldFileName;
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
    QMap<QByteArray, QVariant>::const_iterator i;
    for(i = m_headers.begin(); i != m_headers.end(); ++i){
        g_log->debug(i.key()+":"+i.value().toByteArray());
    }
}
