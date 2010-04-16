#include "fileinfo.h"
#include <iostream>
#include <QCryptographicHash>
#include <QFile>
#include <QIODevice>
#include <QDebug>

FileInfo::FileInfo(QString file_path, QString collection_path)
{
    QFileInfo *qfi = new QFileInfo(collection_path+"/"+file_path);
    m_collectionPath = collection_path;
    m_filePath = file_path;
    m_modifiedAt = qfi->lastModified();
    m_sizeInBytes = qfi->size();
    m_sha1 = "";
}

FileInfo::FileInfo(QString filePath, QString collectionPath, QFileInfo* qfi)
{
    // read the status from the qFileInfo object into our object..
    m_collectionPath = collectionPath;
    m_filePath = filePath;
    m_modifiedAt = qfi->lastModified();
    m_sizeInBytes = qfi->size();
    m_sha1 = "";
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
        std::cout << "reading fingerprint of " << qPrintable(m_filePath) << std::endl;
        return QCryptographicHash::hash(file.readAll(), QCryptographicHash::Sha1).toHex();
    }else{
        return "failed to open file for fingerprint";
    }
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
