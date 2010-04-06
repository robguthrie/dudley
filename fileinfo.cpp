#include "fileinfo.h"
#include <QCryptographicHash>
#include <QFile>
#include <QIODevice>
#include <QDebug>

FileInfo::FileInfo(QString filePath, QString collectionPath, bool read_sha1)
{
    QFileInfo *qfi = new QFileInfo(collectionPath+"/"+filePath);
    m_collectionPath = collectionPath;
    m_filePath = filePath;
    m_modifiedAt = qfi->lastModified();
    m_sizeInBytes = qfi->size();
    m_sha1 = "";
    if (read_sha1){
        updateFingerPrint();
    }
}

FileInfo::FileInfo(QString filePath, QString collectionPath, QFileInfo* qfi, bool read_sha1)
{
    // read the status from the qFileInfo object into our object..
    m_collectionPath = collectionPath;
    m_filePath = filePath;
    update(qfi, read_sha1);
}

FileInfo::FileInfo(QString filePath, QString collectionPath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1)
{
    m_filePath = filePath;
    m_collectionPath = collectionPath;
    update(modifiedAt, sizeInBytes, sha1);
}

// we call this when updating collection from log file
void FileInfo::update(QDateTime modifiedAt, qint64 sizeInBytes, QString sha1)
{
    m_modifiedAt = modifiedAt;
    m_sizeInBytes = sizeInBytes;
    m_sha1 = sha1;
}

// harmless way to see if we are out of date
bool FileInfo::modified(QFileInfo *qfi)
{

    // should check that filePath is the same
    if (m_modifiedAt.toString(Qt::ISODate) != qfi->lastModified().toString(Qt::ISODate)){
        return true;
    }else{
        return false;
    }
}

bool FileInfo::isIdenticalContent(FileInfo *fi)
{
    if (this->size() == fi->size()){
        if (this->fingerPrint() == fi->fingerPrint()){
            return true;
        }
    }
    return false;
}

//// we call this to update in memory collection from disk
//void FileInfo::update(QFileInfo* qfi, bool read_sha1)
//{
//    // should check that filePath is the same
//    m_modifiedAt = qfi->lastModified();
//    m_sizeInBytes = qfi->size();
//    if (read_sha1){
//        updateFingerPrint();
//    }else{
//        m_sha1 = "";
//    }
//}

QDateTime FileInfo::lastModified()
{
    return m_modifiedAt;
}

QString FileInfo::fingerPrint()
{
    if (m_sha1 == "") updateFingerPrint();
    return m_sha1;
}

void FileInfo::updateFingerPrint()
{
    m_sha1 = readFingerPrint();
}

QString FileInfo::readFingerPrint()
{
    QFile file(m_collectionPath+"/"+m_filePath);
    if (file.open(QIODevice::ReadOnly)){
        //QCryptographicHash::hash(file.readAll(), QCryptographicHash::Sha1).toHex();
        return QCryptographicHash::hash(file.readAll(), QCryptographicHash::Sha1).toHex();
    }else{
        return "failed to open file for fingerprint";
    }
}

QString FileInfo::filePath()
{
    return m_filePath;
}

QString FileInfo::oldFilePath()
{
    return m_oldFilePath;
}

qint64 FileInfo::size()
{
    return m_sizeInBytes;
}

void FileInfo::rename(QString newFilePath)
{
    m_oldFilePath = m_filePath;
    m_filePath = newFilePath;
}
