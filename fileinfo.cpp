#include "fileinfo.h"
#include <iostream>
#include <QCryptographicHash>
#include <QFile>
#include <QIODevice>
#include <QDebug>

FileInfo::FileInfo(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1)
{
    m_filePath = filePath;
    update(modifiedAt, sizeInBytes, sha1);
}


// we call this when updating collection from log file
void FileInfo::update(QDateTime modifiedAt, qint64 sizeInBytes, QString sha1)
{
    m_modifiedAt = modifiedAt;
    m_sizeInBytes = sizeInBytes;
    m_sha1 = sha1;
}

// are is the file (namelessly) identical to another
bool FileInfo::isIdenticalTo(FileInfo *fi)
{
    if (this->size() == fi->size()){
        if (this->fingerPrint() == fi->fingerPrint()){
            return true;
        }else{
            return false;
        }
    }
    return false;
}

// try to cheaply detect change in a file
// we just compare mtime and size....
bool FileInfo::seemsIdenticalTo(QFileInfo q){
    return (this->lastModified().toString(Qt::ISODate) == q.lastModified().toString(Qt::ISODate)) &&
           (this->size() == q.size());
}

QDateTime FileInfo::lastModified()
{
    return m_modifiedAt;
}

QString FileInfo::fingerPrint()
{
    return m_sha1;
}

QString FileInfo::filePath()
{
    return m_filePath;
}

qint64 FileInfo::size()
{
    return m_sizeInBytes;
}

void FileInfo::rename(QString newFilePath)
{
    m_filePath = newFilePath;
}
