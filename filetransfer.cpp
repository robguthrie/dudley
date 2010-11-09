#include "filetransfer.h"
#include "repo.h"
#include "output.h"

FileTransfer::FileTransfer(QObject *parent, QString file_path, qint64 file_size)
    : QObject(parent)
{
    m_sourceName = "undefined";
    m_sourceDevice = 0;
    m_destName = "undefined";
    m_destDevice = 0;
    m_filePath = "undefined";
    m_bytesWritten = 0;
    m_destRepo = 0;
    m_complete = false;
    m_filePath = file_path;
    m_fileSize = file_size;
    m_finished = false;
}

bool FileTransfer::start()
{
    if (everythingIsOk()) {
        if (m_sourceDevice){
            connect(m_sourceDevice, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
            processReadyRead();
        }
        return true;
    }else{
        return false;
    }
}

void FileTransfer::setFinished()
{
    if (!m_finished){
        if(m_sourceDevice){
            disconnect(m_sourceDevice, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
        }

        if(m_destRepo){
            if (m_complete){
                m_destRepo->putFileComplete(m_destDevice, m_filePath);
            }else{
                m_destRepo->putFileFailed(m_destDevice);
            }
        }

        if (m_complete){
            emit complete(m_fileSize);
        }

        m_finished = true;
        emit finished();
    }
}

// check that everything is okkk
bool FileTransfer::everythingIsOk()
{
    // beware the trecherous field of braces and blocks
    if (m_sourceDevice){
        if (!m_sourceDevice->isReadable()) m_errors << "source file is not readable";
        if (!m_sourceDevice->isOpen())     m_errors << "source file is not open";
    }

    if (m_destDevice){
        if (!m_destDevice->isOpen())       m_errors << "dest file is not open";
        if (!m_destDevice->isWritable())   m_errors << "dest file is not writable";
    }else{
        m_errors << "dest file is null";
    }

    if (m_fileSize == 0)
        m_errors << "file size is 0 - we dont copy these (unknown size is -1)";

    // might be minimum requirements for fileinfo completeness ..
    if (m_errors.isEmpty()){
        return true;
    }else{
        printErrors();
        setFinished();
        return false;
    }
}

void FileTransfer::processReadyRead()
{
    if (m_sourceDevice){
        if (!m_sourceDevice->isSequential()){
            m_sourceDevice->seek(m_bytesWritten);
        }
        while (everythingIsOk() && !m_complete && m_sourceDevice->bytesAvailable()
    //        && (m_destDevice->bytesToWrite() == 0)){
            ){
            // we will expect to see bw bytes written before reading more from the source
            qint64 bw = m_destDevice->write(m_sourceDevice->read(256*1024));
            if (bw >= 0){
                m_bytesWritten += bw;
            }else{
                // there was a problem
                g_log->debug("Problem writing bytes from source to dest");
                m_errors << "Problem writing bytes from source to dest";
            }
        }
    }
    checkIfFinished();
}
void FileTransfer::processBytesWritten(qint64 bw)
{
    m_bytesWritten += bw;
    checkIfFinished();
}

void FileTransfer::checkIfFinished()
{
    if (m_bytesWritten == m_fileSize){
        g_log->debug("file transfer finished: m_bytesWritten = "+QString::number(m_bytesWritten));
        m_complete = true;
        setFinished();
    }
}

QStringList FileTransfer::errors() const
{
    return m_errors;
}

void FileTransfer::printErrors() const
{
    g_log->error("ERRRROR! FileTransfer:"+errors().join(", "));
}

QString FileTransfer::status() const
{
    if (m_complete)
        return "complete";
    else if (m_finished)
        return "failed";
    else
        return "in progress";
}

QString FileTransfer::statusLine() const
{
    return m_filePath+" "+humanSize(m_bytesWritten)+"("+QString::number(m_bytesWritten)+") of "+humanSize(m_fileSize)+"("+QString::number(m_fileSize)+")  "+status()+"<br />";
}


void FileTransfer::setSource(QString name, QIODevice* source_device)
{
    m_sourceDevice = source_device;
    m_sourceName = name;
}

void FileTransfer::setDest(QString name, QIODevice* dest_device)
{
    m_destDevice = dest_device;
    m_destName = name;
}

void FileTransfer::setDestRepo(Repo* repo)
{
    m_destRepo = repo;
}

void FileTransfer::setFileSize(qint64 size)
{
    m_fileSize = size;
    checkIfFinished();
}

QString FileTransfer::sourceName() const
{
    return m_sourceName;
}

QString FileTransfer::destName() const
{
    return m_destName;
}

QString FileTransfer::filePath() const
{
    return m_filePath;
}

QIODevice* FileTransfer::sourceDevice() const
{
    return m_sourceDevice;
}

QIODevice* FileTransfer::destDevice() const
{
    return m_destDevice;
}

qint64 FileTransfer::bytesWritten() const
{
    return m_bytesWritten;
}

bool FileTransfer::isComplete() const
{
    return m_complete;
}

bool FileTransfer::isFinished() const
{
    return m_finished;
}
