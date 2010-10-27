#include "filetransfer.h"
#include "output.h"
FileTransfer::FileTransfer(QObject *parent, QString source_name,
                           QIODevice* source_file, QString dest_name, QIODevice* dest_file,
                           FileInfo* file_info) :
    QObject(parent), m_sourceName(source_name), m_sourceFile(source_file),
    m_destName(dest_name), m_destFile(dest_file), m_fileInfo(file_info)
{
    m_bytesWritten = 0;
    m_complete = false;

    if (everythingIsOk()) {
        connect(m_sourceFile, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
        connect(m_sourceFile, SIGNAL(aboutToClose()), this, SLOT(processAboutToClose()));
        connect(m_destFile, SIGNAL(aboutToClose()), this, SLOT(processAboutToClose()));
    }
}

// check that everything is okkk
bool FileTransfer::everythingIsOk()
{
    if (!m_sourceFile)
        m_errors << "source file is null";

    if (!m_destFile)
        m_errors << "dest file is null";

    if (!m_sourceFile->isReadable())
        m_errors << "source file is not readable";

    if (!m_sourceFile->isOpen())
        m_errors << "source file is not open";

    if (!m_destFile->isOpen())
        m_errors << "dest file is not open";

    if (!m_destFile->isWritable())
        m_errors << "dest file is not writable";

    if (m_fileInfo == 0)
        m_errors << "fileinfo is null";
    else if (m_fileInfo->size() == 0)
        m_errors << "file size is 0 - we dont copy these (unknown size is -1)";

    // might be minimum requirements for fileinfo completeness ..
    if (m_errors.isEmpty()){
        return true;
    }else{
        printErrors();
        if (!m_finished){
            m_finished = true;
            emit finished();
        }
        return false;
    }
}

QStringList FileTransfer::errors() const
{
    return m_errors;
}

void FileTransfer::processReadyRead()
{
//
//    g_log->debug("after seeking to destFile pos");
//    g_log->debug("sourceFile pos:"+QString::number(m_sourceFile->pos()));
//    g_log->debug("sourceFile bytesAvailable:"+QString::number(m_sourceFile->bytesAvailable()));
//    g_log->debug("sourceFile size:"+QString::number(m_sourceFile->size()));
//    g_log->debug("fileinfo expected size:"+QString::number(m_fileInfo->size()));
//    g_log->debug("bytes copied:"+QString::number(m_bytesWritten));
    // seek to size of destFile?

    if (!m_sourceFile->isSequential()){
        m_sourceFile->seek(m_bytesWritten);
    }
    if (everythingIsOk() && !m_complete && m_sourceFile->bytesAvailable()){
        qint64 bw = m_destFile->write(m_sourceFile->read(256*1024));
        if (bw >= 0){
            m_bytesWritten += bw;
//            g_log->debug("wrote "+QString::number(bw)+"to the temp file device");
        }else{
            // there was a problem
            g_log->debug("Problem writing bytes from source to dest");
            m_errors << "Problem writing bytes from source to dest";
        }
    }

    // if the size is known
    if (m_bytesWritten == m_fileInfo->size()){
        g_log->debug("Wrote all the data! file transfer finished: m_bytesWritten = "+QString::number(m_bytesWritten));
        m_complete = true;
        m_finished = true;
        m_destFile->close();
        m_sourceFile->close();
        emit finished();
    }
}

void FileTransfer::processSourceComplete(qint64 contentBytesReceived)
{
    // the httpserver has read the complete upload.. phew
    m_fileInfo->setSize(contentBytesReceived);
    processReadyRead();
}

// if the files close.. this attempt at a transfer is over
void FileTransfer::processAboutToClose()
{
    if (!m_complete) /*check*/ everythingIsOk(); // this reads very badly
}


void FileTransfer::printErrors() const
{
    foreach(QString error, m_errors){
        g_log->error("FileTransfer:"+error);
    }
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
   return m_fileInfo->fileName()+" "+humanSize(m_bytesWritten)+" of "+humanSize(m_fileInfo->size())+" "+status()+"<br />";
}
QString FileTransfer::sourceName() const
{
    return m_sourceName;
}

QString FileTransfer::destName() const
{
    return m_destName;
}

QIODevice* FileTransfer::sourceFile() const
{
    return m_sourceFile;
}

QIODevice* FileTransfer::destFile() const
{
    return m_destFile;
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

FileInfo*   FileTransfer::fileInfo() const
{
    return m_fileInfo;
}

