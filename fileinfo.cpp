#include "fileinfo.h"
#include <QStringList>
#include <iostream>
#include <QCryptographicHash>
#include <QFile>
#include <QIODevice>
#include <QDebug>
#include "mimetypefinder.h"

MimeTypeFinder FileInfo::mimeTypeFinder = MimeTypeFinder();

FileInfo::FileInfo()
{
}

FileInfo::FileInfo(QString filePath, qint64 size)
{
    m_filePath = filePath;
    m_size = size;
}

FileInfo::FileInfo(QString filePath, qint64 size, QDateTime modifiedAt, QString sha1)
{
    m_filePath = filePath;
    update(size, modifiedAt, sha1);
}

// we call this when updating collection from log file
void FileInfo::update(qint64 size, QDateTime modifiedAt, QString sha1)
{
    m_modifiedAt = modifiedAt;
    m_size = size;
    m_sha1 = sha1;
}

void FileInfo::rename(QString newFilePath)
{
    m_filePath = newFilePath;
}

// are is the file (namelessly) identical to another
bool FileInfo::isIdenticalTo(FileInfo fi) const
{
   return (m_size == fi.size()) && (m_sha1 == fi.fingerPrint());
}

// try to cheaply detect change in a file
// we just compare mtime and size....
bool FileInfo::sameModifiedAtAndSize(QDateTime lastModified, qint64 size) const
{
    return (m_modifiedAt.toString(Qt::ISODate) == lastModified.toString(Qt::ISODate)) &&
           (m_size == size);
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
    return m_size;
}

void FileInfo::setSize(qint64 size)
{
    m_size = size;
}

QString FileInfo::toString() const
{
    QStringList list;
    list << m_sha1 << QString::number(m_size) << m_filePath;
    return list.join(",");
}

QString FileInfo::mimeType() const
{
    return mimeTypeFinder.lookup(QFileInfo(filePath()).suffix().toLower());
}
