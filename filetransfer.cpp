#include "filetransfer.h"
#include "output.h"
FileTransfer::FileTransfer(QObject *parent, QObject* source_parent,
                           QIODevice* source_file, QObject* dest_parent, QIODevice* dest_file,
                           FileInfo* file_info) :
    QObject(parent), m_sourceParent(source_parent), m_sourceFile(source_file),
    m_destParent(dest_parent), m_destFile(dest_file), m_fileInfo(file_info)
{
    m_bytesWritten = 0;
    m_complete = false;

    if (!m_sourceParent) m_errors << "source parent is null";
    if (!m_sourceFile)   m_errors << "source file is null";
    if (!m_destParent)   m_errors << "dest parent is null";
    if (!m_destFile)     m_errors << "dest file is null";

    if (!(m_sourceFile->isOpen() && m_sourceFile->isReadable())){
        m_errors << "source file is not open/readable";
    }

    if (!(m_destFile->isOpen() && m_destFile->isWritable())){
        m_errors << "dest file is not open/writable";
    }

    if (m_fileInfo->size() == 0){
        m_errors << "file size is 0 - we dont copy these (unknown size is -1)";
    }

    // check that fileInfo has a fileName or sha1 and size
    if (m_errors.isEmpty()) {
        connect(m_sourceFile, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
        connect(m_sourceFile, SIGNAL(aboutToClose()), this, SLOT(processAboutToClose()));
        connect(m_destFile, SIGNAL(aboutToClose()), this, SLOT(processAboutToClose()));
        if (m_fileInfo->size() == -1){
            connect(m_sourceParent, SIGNAL(complete(qint64)), this, SLOT(processSourceComplete(qint64)));
        }
    }else{
        printErrors();
        emit finished();
    }
}

QStringList FileTransfer::errors() const
{
    return m_errors;
}

void FileTransfer::processReadyRead()
{
    m_sourceFile->seek(m_destFile->pos());
    Output::debug("after seeking to destFile pos");
    Output::debug("sourceFile pos:"+QString::number(m_sourceFile->pos()));
    Output::debug("sourceFile bytesAvailable:"+QString::number(m_sourceFile->bytesAvailable()));
    Output::debug("sourceFile size:"+QString::number(m_sourceFile->size()));
    Output::debug("fileinfo expected size:"+QString::number(m_fileInfo->size()));
    Output::debug("bytes copied:"+QString::number(m_bytesWritten));
    // seek to size of destFile?

    if ((!m_complete) && (m_sourceFile->bytesAvailable())){
        qint64 bw = m_destFile->write(m_sourceFile->readAll());
        if (bw >= 0){
            m_bytesWritten += bw;
            Output::debug("wrote "+QString::number(bw)+"to the temp file device");
        }else{
            // there was a problem
            Output::debug("Problem writing bytes from source to dest");
            m_errors << "Problem writing bytes from source to dest";
        }
    }

    // if the size is known
    if (m_bytesWritten == m_fileInfo->size()){
        Output::debug("Wrote all the data! file transfer finished");
        m_complete = true;
        m_destFile->close();
        emit finished();
    }
}

void FileTransfer::processSourceComplete(qint64 contentBytesReceived)
{
    // the httpserver has read the complete upload.. phew
    m_fileInfo->setSize(contentBytesReceived);
    processReadyRead();
}

void FileTransfer::processAboutToClose()
{
    emit finished();
}

void FileTransfer::printErrors() const
{
    foreach(QString error, m_errors){
        Output::error("FileTransfer:"+error);
    }
}
QObject* FileTransfer::sourceParent() const
{
    return m_sourceParent;
}

QObject* FileTransfer::destParent() const
{
    return m_destParent;
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

FileInfo*   FileTransfer::fileInfo() const
{
    return m_fileInfo;
}

