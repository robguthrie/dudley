#include "fileinfocollection.h"
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <iostream>
#include <QDir>
#include <QDateTime>
#include <QDebug>
#include "logger.h"

FileInfoCollection::FileInfoCollection()
{
}

FileInfoCollection::FileInfoCollection(QString path)
{
    m_path = path;
}

bool FileInfoCollection::exists()
{
    return QFile::exists(m_path);
}

QString FileInfoCollection::path()
{
    return m_path;
}

void FileInfoCollection::update(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1)
{
    if (m_files.contains(filePath)){
        // update existing without touching disk
        // likely (inevitable even) that the sha1 has changed.. so need to pull the old sha1 and insert the new
        FileInfo *fileInfo = m_files.value(filePath);
        QString old_sha1 = fileInfo->fingerPrint();
        m_fingerprints.remove(old_sha1);
        fileInfo->update(modifiedAt, sizeInBytes, sha1);
        m_fingerprints.insert(sha1, fileInfo);
    }else{
        // insert new
        FileInfo *fileInfo = new FileInfo(filePath, m_path, modifiedAt, sizeInBytes, sha1);
        m_files.insert(filePath, fileInfo);
        m_fingerprints.insert(sha1, fileInfo);
    }
}

bool FileInfoCollection::remove(QString filePath)
{
    FileInfo *fileInfo;
    if (fileInfo = m_files.value(filePath)){
        m_fingerprints.remove(fileInfo->fingerPrint());
        m_files.remove(filePath);
        return true;
    }else{
        return false;
    }
}

bool FileInfoCollection::rename(QString filePath, QString newFilePath)
{
    if (m_files.contains(filePath)){
        FileInfo *fileInfo = m_files.value(filePath);
        m_files.remove(filePath);
        fileInfo->rename(newFilePath);
        m_files.insert(newFilePath, fileInfo);
        return true;
    }else{
        return false;
    }
}

void FileInfoCollection::rename(FileInfo *fileInfo, QString newFilePath)
{
    m_files.remove(fileInfo->filePath());
    fileInfo->rename(newFilePath);
    m_files.insert(newFilePath, fileInfo);
}


QString FileInfoCollection::relativeFilePath(QString path)
{
    // remove the m_path prefixing the string
    if (path.startsWith(m_path))
        path = path.remove(0, m_path.size()+1);
    return path;
}

bool FileInfoCollection::containsFilePath(QString file_path)
{
    return m_files.contains(file_path);
}

FileInfo* FileInfoCollection::getFileInfo(QString file_path)
{
    return m_files.value(file_path);
}

// an known file is is thought to be on disk as far as a collection is concerned
QHash<QString, FileInfo*> FileInfoCollection::knownFiles(QStringList *found_files)
{
    QString file_path;
    QHash<QString, FileInfo*> known_found_files;

    foreach(file_path, (*found_files)){
        if (m_files.contains(file_path)){
            known_found_files.insert(file_path, m_files.value(file_path));
        }
    }
    return known_found_files;
}

QHash<QString, FileInfo*> FileInfoCollection::missingFiles(QStringList *found_files)
{
    QHash<QString, FileInfo*> missing_files = m_files;
    QString file_path;
    foreach(file_path, (*found_files)){
        missing_files.remove(file_path);
    }
    return missing_files;
}


// an known file is is thought to be on disk as far as a collection is concerned
QStringList FileInfoCollection::knownFilePaths(QStringList *found_files)
{
    QString file_path;
    QStringList known_found_files;

    foreach(file_path, (*found_files)){
        if (m_files.contains(file_path)){
            known_found_files << file_path;
        }
    }
    return known_found_files;
}

// an unknown file is on the disk but not in the collection
QStringList FileInfoCollection::unknownFilePaths(QStringList *found_files)
{
    QStringList unknown_file_paths;
    QString file_path;
    foreach(file_path, (*found_files)){
        if (!m_files.contains(file_path)){
            unknown_file_paths << file_path;
        }
    }
    return unknown_file_paths;
}

// a missing file is in the collection but not on the disk
QStringList FileInfoCollection::missingFilePaths(QStringList *found_files)
{
    return missingFiles(found_files).keys();
}

