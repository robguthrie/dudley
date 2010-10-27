#include "fileinfo.h"
#include <QStringList>
#include <iostream>
#include <QCryptographicHash>
#include <QFile>
#include <QIODevice>
#include <QDebug>
#include "mimetypefinder.h"

MimeTypeFinder FileInfo::mimeTypeFinder = MimeTypeFinder();
FileInfo::FileInfo(QObject* parent): QObject(parent) {}

FileInfo::FileInfo(QObject* parent, QString filePath, qint64 sizeInBytes)
    :QObject(parent)
{
    m_filePath = filePath;
    m_sizeInBytes = sizeInBytes;
}

FileInfo::FileInfo(QObject* parent, QString filePath, qint64 sizeInBytes, QDateTime modifiedAt, QString sha1)
    :QObject(parent)
{
    m_filePath = filePath;
    update(sizeInBytes, modifiedAt, sha1);
}

FileInfo::FileInfo(const FileInfo &f)
    :QObject(f.parent())
{
    m_filePath = f.m_filePath;
    m_modifiedAt = f.m_modifiedAt;
    m_sizeInBytes = f.m_sizeInBytes;
    m_sha1 = f.m_sha1;
}

// we call this when updating collection from log file
void FileInfo::update(qint64 sizeInBytes, QDateTime modifiedAt, QString sha1)
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

void FileInfo::setLastModified(QDateTime date_time)
{
    m_modifiedAt = date_time;
}

QString FileInfo::fingerPrint() const
{
    return m_sha1;
}

void FileInfo::setFingerPrint(QString fingerprint)
{
    m_sha1 = fingerprint;
}

QString FileInfo::filePath() const
{
    return m_filePath;
}

void FileInfo::setFilePath(QString file_path)
{
    m_filePath = file_path;
}

QString FileInfo::fileName() const
{
    QFileInfo fi(m_filePath);
    return fi.fileName();
}

qint64 FileInfo::size() const
{
    return m_sizeInBytes;
}

void FileInfo::setSize(qint64 size)
{
    m_sizeInBytes = size;
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
