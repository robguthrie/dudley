#include "httpmessage.h"
#include <QRegExp>
#include <QVariant>
#include <QCoreApplication>

#include "output.h"

QString HttpMessage::dateFormat = QString("ddd, dd MMM yyyy hh:mm:ss G'M'T");

HttpMessage::HttpMessage(QObject* parent, HttpMessage* parent_message)
    :QObject(parent)
{
    m_state = BeforeStart;
    m_parentMessage = parent_message;

    if (parent_message){
        // this is for multipart child messages who dont have first lines
        g_log->debug("created child message");
        m_state = ReadingHeaders;
        connect(this, SIGNAL(isFileUpload()), m_parentMessage, SLOT(processIsFileUpload()));
    }
    m_protocol = "HTTP/1.1";
    m_contentBytesTransferred = 0;
    m_isMultiPart = false;
    m_currentMessage = 0;
    m_contentLength = 0;
    m_headerLength = 0;
    m_maybeLastLine = false;
    m_contentDevice = 0;
    m_ownContentDevice = false;
    m_boundry = "";
    m_f_boundry = "";
    m_formDataBoundary = "";
}

HttpMessage::~HttpMessage()
{
    destroyOwnContentDevice();
}


void HttpMessage::setContent(QByteArray body)
{
    createOwnContentDevice();
    m_contentDevice->write(body);
    setHeader("Content-Length", body.size());
}

void HttpMessage::createOwnContentDevice()
{
    if (!m_contentDevice){
        m_ownContentDevice = true;
        QIODevice* d = (QIODevice*) new QBuffer();
        if (!d->open(QIODevice::ReadWrite | QIODevice::Append)){
             g_log->debug("coul dnot open buffer for RW");
        }
        setContentDevice(d);
    }else{
        g_log->error("a contentDevice already exists!");
    }
}

void HttpMessage::destroyOwnContentDevice()
{
    if (m_ownContentDevice && m_contentDevice)
    {
        m_ownContentDevice = false;
        m_contentDevice->close();
        m_contentDevice->deleteLater();
        m_contentDevice = 0;
    }
}

void HttpMessage::setContentDevice(QIODevice *file)
{
    if (!m_contentDevice){
        m_contentDevice = file;
        if (file->isOpen() && file->isWritable()){
            if (m_state == WaitingForContentDevice){
                // an upload should set waitingforcontentdevice state
                setState(ReadingContent);
            }
        }else{
            g_log->error("content device is not open/writable");
        }
    }else{
        g_log->error("already a content device on message.. and we are setting another");
    }
}

void HttpMessage::processIsFileUpload()
{
    // child messages messages call this slot
    m_pendingFileUploadMessages << (HttpMessage*) sender();
    emit fileUploadStarted();
}

HttpMessage::State HttpMessage::state() const
{
    return m_state;
}

void HttpMessage::setState(State state, QByteArray message)
{
    QString state_name = QLatin1String(ENUM_NAME(HttpMessage, State, state));
    g_log->debug("setting message state:"+state_name);

    if (!message.isEmpty()){
        g_log->info(message);
    }
    m_state = state;
    switch (state){

    case Invalid:
        emit finished();
        break;

    case ReadingHeaders:
        break;

    case Finished:
        emit complete(m_contentLength);
        emit finished();
        break;

    default:
    break;
    }

}

bool HttpMessage::hasHeader(QByteArray key) const
{
    return m_headers.contains(key.toLower());
}

QVariant HttpMessage::header(QByteArray key) const
{
    return m_headers.value(key.toLower());
}

void HttpMessage::setHeader(QByteArray key, QVariant value)
{
    key = key.trimmed().toLower();
    m_headers.insert(key, value);
    if (key == "content-disposition"){
        QList<QByteArray> tokens = value.toByteArray().trimmed().split(';');
        QRegExp kvrx("([^=]+)=[\"']?([^\"']+)[\"']?"); // match key="value" type bits
        foreach(QByteArray token, tokens){
            if (kvrx.exactMatch(token.trimmed())){
                QString key = kvrx.cap(1);
                QString value = kvrx.cap(2);
                if (key.toLower() == "name"){
                    m_formFieldName = value.toAscii();
                }else if (key.toLower() == "filename"){
                    m_formFieldFileName = value.toAscii();
                }else{
                    g_log->debug("unrecognised content-disposition keyvalue pair: "+key+"="+value);
                }
            }
        }
    }else if (key == "content-type"){
        QRegExp form_data_rx("multipart/form-data; boundary=(.*)");
//        form_data_rx.setCaseSensitivity(Qt::CaseInsensitive);
        if (form_data_rx.exactMatch(value.toString())){
            m_isMultiPart = true;
            m_formDataBoundary = form_data_rx.cap(1).toAscii();
            m_boundry = "--"+m_formDataBoundary+"\r\n";
            m_f_boundry = "--"+m_formDataBoundary+"--\r\n";
            g_log->debug("multipart message boundry: "+m_formDataBoundary);
        }
    }else if (key == "content-length"){
        m_contentLength = value.toLongLong();
    }
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

void HttpMessage::parseData(QIODevice* device)
{
    QByteArray line;
    while (m_state == ReadingHeaders && device->canReadLine()){
        QRegExp header_rx("([^:]+):(.+)\r\n");
        line = device->readLine();
        m_headerLength += line.length();
        if (m_parentMessage){
            m_parentMessage->m_contentBytesTransferred += line.length();
        }
        if (line == "\r\n"){
            // empty line. normal end of headers
            emit headersFinished();

            // need to determine next state
            if (m_isMultiPart){
                g_log->debug("multipart message");
                setState(ReadingContent);
            }else if (m_parentMessage){
                setState(WaitingForContentDevice);
                if (!m_formFieldFileName.isEmpty()){
                    g_log->debug("multipart child fileupload");
                    emit isFileUpload();
                }else{
                    // this should be form field values
                    g_log->debug("multipart child message");
                    createOwnContentDevice();
                }
            }else if (m_contentLength != 0){
                g_log->debug("have a content length and dont know why. discarding");
                setState(Finished);
            }else{
                //g_log->debug("no content length, not multipart, no data expected, finsihed");
                setState(Finished);
            }
            break;
        }else if (header_rx.exactMatch(line)){
            // read the header
            setHeader(header_rx.cap(1).toAscii(), header_rx.cap(2).trimmed());
        }else{
            // header is invalid.. termate loop
            setState(Invalid, "invalid header detected: "+line);
        }
    }

    if (m_state == WaitingForContentDevice){
        g_log->warning("message waiting for content device");
    }

    if (m_state != ReadingContent)
        return;

    if (m_isMultiPart){
        while(m_state == ReadingContent && device->canReadLine()){
            if (device->peek(m_boundry.length()) == m_boundry){
                device->readLine();
                // this is the end of the content for the current part, or the frist cp
                m_contentBytesTransferred += m_boundry.length();
                g_log->debug("boundry line"); // throw away line
                if (m_currentMessage){
                    m_childMessages << m_currentMessage;
                    m_currentMessage->setState(Finished);
                }
                // prepare new content part for the following data
                m_currentMessage = new HttpMessage(this, this);
            }else if (device->peek(m_f_boundry.length()) == m_f_boundry){
                device->readLine(); // throw away line
                m_contentBytesTransferred += m_f_boundry.length();
                g_log->debug("final boundry line"); // throw away line
                m_childMessages << m_currentMessage;
                m_currentMessage->setState(Finished);
                m_currentMessage = 0;
                setState(Finished);
            }else if (m_currentMessage){
                m_currentMessage->parseData(device);
            }else{
                // this is treated as data for shitty clients that cant parse multipart
                g_log->warning("multipart message contains fallback content. dropping");
                device->readLine();
            }
        }
    }else if (m_parentMessage){
        // only read one line at a time here... the parent messsage scans
        // the lines first to check for boundrys
        if (device->bytesAvailable()){
            line = device->readLine();
            QString data(line);
            if (data.contains(m_parentMessage->m_formDataBoundary)){
                g_log->debug("found a formDataBoundry within the child!!!");
            }

            m_parentMessage->m_contentBytesTransferred += line.length();
            if (m_maybeLastLine){
                // if this is true.. then what we thought might be the last line
                // was just a crlf in the middle of the data
                // so we need to put the crlf back into the data
                m_maybeLastLine = false;
                line.prepend("\r\n");
            }

            // any line could be your last
            if (line.endsWith("\r\n")){
                m_maybeLastLine = true;
                line.chop(2);
            }
            writeContentBytes(line);
        }
    }else{
        while(m_state == ReadingContent && device->bytesAvailable()){
            qint64 max_size = (m_contentLength - m_contentBytesTransferred);
            writeContentBytes(device->read(max_size));
            if (m_contentBytesTransferred == m_contentLength){
                setState(Finished);
            }else if (m_contentBytesTransferred > m_contentLength){
                setState(Invalid, "read more bytes than we were supposed to.. very bad");
            }
        }
    }
}

qint64 HttpMessage::writeContentBytes(QByteArray data)
{

    qint64 bw = m_contentDevice->write(data);
    if (data.length() != bw){
        g_log->error("HttpMessage::writeContentBytes bytes written not equal to data lenght");
    }
    m_contentBytesTransferred += bw;
    emit contentBytesWritten(bw);
    return bw;
}

bool HttpMessage::isMultiPart() const
{
    return m_isMultiPart;
}

HttpMessage* HttpMessage::parentMessage() const
{
    return m_parentMessage;
}

QByteArray HttpMessage::formFieldName() const
{
    return m_formFieldName;
}

QByteArray HttpMessage::formFieldFileName() const
{
    return m_formFieldFileName;
}

bool HttpMessage::hasPendingFileUploads() const
{
    return (m_pendingFileUploadMessages.size() > 0);
}

HttpMessage*  HttpMessage::getNextFileUploadMessage()
{
    return m_pendingFileUploadMessages.takeFirst();
}

qint64 HttpMessage::contentLength() const
{
    return m_contentLength;
}

qint64 HttpMessage::headerLength() const
{
    return m_headerLength;
}
qint64 HttpMessage::contentBytesTransferred() const
{
    return m_contentBytesTransferred;
}

QIODevice* HttpMessage::contentDevice() const
{
    return m_contentDevice;
}

QByteArray HttpMessage::protocol() const
{
    return m_protocol;
}

QList<HttpMessage*> HttpMessage::childMessages() const
{
    return m_childMessages;
}
