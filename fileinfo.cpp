#include "fileinfo.h"
#include <QStringList>
#include <iostream>
#include <QCryptographicHash>
#include <QFile>
#include <QIODevice>
#include <QDebug>
#include "mimetypefinder.h"

MimeTypeFinder FileInfo::mimeTypeFinder = MimeTypeFinder();
FileInfo::FileInfo() {}

FileInfo::FileInfo(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1)
{
    m_filePath = filePath;
    update(modifiedAt, sizeInBytes, sha1);
}

FileInfo::FileInfo(const FileInfo &f)
{
    m_filePath = f.m_filePath;
    m_modifiedAt = f.m_modifiedAt;
    m_sizeInBytes = f.m_sizeInBytes;
    m_sha1 = f.m_sha1;
}

// we call this when updating collection from log file
void FileInfo::update(QDateTime modifiedAt, qint64 sizeInBytes, QString sha1)
{
    m_modifiedAt = modifiedAt;
    m_sizeInBytes = sizeInBytes;
    m_sha1 = sha1;
}

void FileInfo::rename(QString newFilePath)
{
    m_filePath = newFilePath;
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

QDateTime FileInfo::lastModified() const
{
    return m_modifiedAt;
}

QString FileInfo::fingerPrint() const
{
    return m_sha1;
}

QString FileInfo::filePath() const
{
    return m_filePath;
}

QString FileInfo::fileName() const
{
    QFileInfo fi(m_filePath);
    return fi.fileName();
}

QString FileInfo::humanSize() const
{
    float num = (float) m_sizeInBytes;
    QStringList list;
    list << "KB" << "MB" << "GB" << "TB";

    QStringListIterator i(list);
    QString unit("bytes");

    while(num >= 1024.0 && i.hasNext())
     {
        unit = i.next();
        num /= 1024.0;
    }
    return QString().setNum(num,'f',2)+" "+unit;
}

qint64 FileInfo::size() const
{
    return m_sizeInBytes;
}


QString FileInfo::toString() const
{
    QStringList list;
    list << m_sha1 << QString::number(m_sizeInBytes) << m_filePath;
    return list.join(",");
}

QString FileInfo::mimeType() const
{
    return mimeTypeFinder.lookup(QFileInfo(filePath()).suffix().toLower());
}
