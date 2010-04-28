#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <iostream>
#include <QDir>

#include "filerepostate.h"
#include "fileinfo.h"
#include "filerepostatelogger.h"

FileRepoState::FileRepoState(FileRepoStateLogger* logger)
{
    m_logger = logger;
    reload();
}

FileRepoStateLogger* FileRepoState::logger()
{
    return m_logger;
}

void FileRepoState::reload()
{
    m_files.clear();
    m_fingerprints.clear();
    m_logger->loadState(this);
}

bool FileRepoState::containsFilePath(QString file_path)
{
    return m_files.contains(file_path);
}

FileInfo* FileRepoState::fileInfoByFilePath(QString file_path)
{
    return m_files.value(file_path);
}

FileInfo* FileRepoState::fileInfoByFingerPrint(QString sha1)
{
    return m_fingerprints.value(sha1);
}

void FileRepoState::addFile(FileInfo* file_info)
{
    addFile(file_info->filePath(), file_info->lastModified(),
            file_info->size(), file_info->fingerPrint());
}

void FileRepoState::addFile(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1)
{
    m_logger->logAddFile(filePath, modifiedAt, sizeInBytes, sha1);
    addFileSilent(filePath, modifiedAt, sizeInBytes, sha1);
}

void FileRepoState::addFileSilent(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1)
{
    if (!m_files.contains(filePath)){
        // update existing without touching disk
        FileInfo *fileInfo = new FileInfo(filePath, modifiedAt, sizeInBytes, sha1);
        m_files.insert(filePath, fileInfo);
        m_fingerprints.insert(sha1, fileInfo);
    }else{
        // insert new
        std::cout << "adding file in error. its already indexed";
    }
}

void FileRepoState::modifyFile(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1)
{
    m_logger->logModifyFile(filePath, modifiedAt, sizeInBytes, sha1);
    modifyFileSilent(filePath, modifiedAt, sizeInBytes, sha1);
}

void FileRepoState::modifyFileSilent(QString filePath, QDateTime modifiedAt, qint64 sizeInBytes, QString sha1)
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
        std::cout << "trying to modify state for fileinfo which does not exist";
    }
}

bool FileRepoState::removeFile(QString filePath)
{

    if (removeFileSilent(filePath)){
        m_logger->logRemoveFile(filePath);
        return true;
    }else{
        return false;
    }
}

bool FileRepoState::removeFileSilent(QString filePath)
{
    FileInfo *fileInfo = m_files.value(filePath);
    if (m_files.remove(filePath)){
        m_fingerprints.remove(fileInfo->fingerPrint());
        return true;
    }else return false;
}

bool FileRepoState::renameFile(QString filePath, QString newFilePath)
{

    if (renameFileSilent(filePath, newFilePath)){
        m_logger->logRenameFile(filePath, newFilePath);
        return true;
    }else return false;
}

bool FileRepoState::renameFileSilent(QString filePath, QString newFilePath)
{
    if (m_files.contains(filePath)){
        FileInfo *fileInfo = m_files.value(filePath);
        m_files.remove(filePath);
        fileInfo->rename(newFilePath);
        m_files.insert(newFilePath, fileInfo);
        return true;
    }else return false;
}

// an known file is is thought to be on disk as far as a collection is concerned
QHash<QString, FileInfo*> FileRepoState::knownFiles(QStringList found_files)
{
    QString file_path;
    QHash<QString, FileInfo*> known_found_files;

    foreach(file_path, (found_files)){
        if (m_files.contains(file_path)){
            known_found_files.insert(file_path, m_files.value(file_path));
        }
    }
    return known_found_files;
}

QHash<QString, FileInfo*> FileRepoState::missingFiles(QStringList found_files)
{
    QHash<QString, FileInfo*> missing_files = m_files;
    QString file_path;
    foreach(file_path, found_files){
        missing_files.remove(file_path);
    }
    return missing_files;
}

// a missing file is in the collection but not on the disk
QStringList FileRepoState::missingFilePaths(QStringList found_files)
{
    return missingFiles(found_files).keys();
}

// an known file is is thought to be on disk as far as a collection is concerned
QStringList FileRepoState::knownFilePaths(QStringList found_files)
{
    QString file_path;
    QStringList known_found_files;

    foreach(file_path, found_files){
        if (m_files.contains(file_path)){
            known_found_files << file_path;
        }
    }
    return known_found_files;
}

// an unknown file is on the disk but not in the collection
QStringList FileRepoState::unknownFilePaths(QStringList found_files)
{
    QStringList unknown_file_paths;
    QString file_path;
    foreach(file_path, (found_files)){
        if (!m_files.contains(file_path)){
            unknown_file_paths << file_path;
        }
    }
    return unknown_file_paths;
}
