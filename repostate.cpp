#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <iostream>
#include <QDir>

#include "repostate.h"
#include "fileinfo.h"
#include "repostatelogger.h"
#include "output.h"

RepoState::RepoState(QObject* parent)
    :QObject(parent), m_logger(0), m_logChanges(false)
{}

RepoState::RepoState(QObject* parent, QString logs_dir)
    :QObject(parent), m_logChanges(false)
{
    m_logger = new RepoStateLogger(logs_dir);
    if (m_logger->isReady()){
        reload();
    }else{
        // warn of unready logger..
        g_log->warning("FileRepoState cant open history. need to initialize logger");
    }
}

RepoState::~RepoState()
{
    if (m_logger != 0) delete m_logger;
}

void RepoState::reload()
{
    m_files.clear();
    m_fingerprints.clear();
    m_logChanges = false;
    m_logger->playAllLogs(this); // the logger calls the file manipulation functions upon this state
    m_logChanges = true;
}

//QStringList FileRepoState::difference(FileRepoState *state)
//{
//    //prehaps insted of returning a qStringList we just return bool for different
//    // and keep pending changes in the logger?
//    // given a filerepostate create a log changes that would be required
//    // to make self identical to the given state
//
//    // what files from state are missing in self
//    // what files from state have different fingerprints to those in self
//    // what files from self are not present in state?
//}

RepoStateLogger* RepoState::logger()
{
    return m_logger;
}

void RepoState::importLog(QString name, QString body)
{
//    m_logChanges = false;
    m_logger->writeLogFile(name, body);
//    m_logger->playLogFile(name, this);
//    m_logChanges = true;
}

void RepoState::setLogger(RepoStateLogger* logger)
{
    m_logger = logger;
}

// should support a commit message or label eventually
bool RepoState::commitChanges()
{
    return m_logger->commitChanges();
}

bool RepoState::containsFileInfo(FileInfo* file_info)
{
    return containsFingerPrint(file_info->fingerPrint());
}

bool RepoState::containsFilePath(QString file_path)
{
    return m_files.contains(file_path);
}

bool RepoState::containsFingerPrint(QString finger_print)
{
    return m_fingerprints.contains(finger_print);
}

FileInfo* RepoState::fileInfoByFilePath(QString file_path)
{
    return m_files.value(file_path);
}

FileInfo* RepoState::fileInfoByFingerPrint(QString sha1)
{
    return m_fingerprints.value(sha1);
}

void RepoState::addFile(FileInfo* file_info)
{
    addFile(file_info->filePath(), file_info->size(),
            file_info->lastModified(), file_info->fingerPrint());
}

void RepoState::addFile(QString filePath, qint64 sizeInBytes, QDateTime modifiedAt, QString sha1)
{
    if (!m_files.contains(filePath)){
        // update existing without touching disk
        FileInfo *fileInfo = new FileInfo(this, filePath, sizeInBytes, modifiedAt, sha1);
        m_files.insert(filePath, fileInfo);
        m_fingerprints.insert(sha1, fileInfo);
        if (m_logChanges){
            m_logger->logAddFile(filePath, sizeInBytes, modifiedAt, sha1);
        }
    }else{
        // insert new
        g_log->error("adding file "+filePath+". it's already indexed");
    }
}

void RepoState::modifyFile(QString filePath, qint64 sizeInBytes, QDateTime modifiedAt, QString sha1)
{
    if (m_files.contains(filePath)){
        // update existing without touching disk
        // likely (inevitable even) that the sha1 has changed.. so need to pull the old sha1 and insert the new
        FileInfo *fileInfo = m_files.value(filePath);
        QString old_sha1 = fileInfo->fingerPrint();
        m_fingerprints.remove(old_sha1);
        fileInfo->update(sizeInBytes, modifiedAt, sha1);
        m_fingerprints.insert(sha1, fileInfo);
        if (m_logChanges){
            m_logger->logModifyFile(filePath, sizeInBytes, modifiedAt, sha1);
        }
    }else{
        g_log->error("modify state for fileinfo which does not exist: "+filePath+", "+sha1);
    }
}

bool RepoState::removeFile(QString filePath)
{
    FileInfo *fileInfo = m_files.value(filePath);
    if (m_files.remove(filePath)){
        m_fingerprints.remove(fileInfo->fingerPrint());
        if (m_logChanges) m_logger->logRemoveFile(filePath);
        return true;
    }else return false;
}

bool RepoState::renameFile(QString filePath, QString newFilePath)
{
    if (m_files.contains(filePath)){
        FileInfo *fileInfo = m_files.value(filePath);
        m_files.remove(filePath);
        fileInfo->rename(newFilePath);
        m_files.insert(newFilePath, fileInfo);
        if (m_logChanges) m_logger->logRenameFile(filePath, newFilePath);
        return true;
    }else return false;
}

// an known file is is thought to be on disk as far as a collection is concerned
QHash<QString, FileInfo*> RepoState::knownFiles(QStringList found_files)
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

QHash<QString, FileInfo*> RepoState::missingFiles(QStringList found_files)
{
    QHash<QString, FileInfo*> missing_files = m_files;
    QString file_path;
    foreach(file_path, found_files){
        missing_files.remove(file_path);
    }
    return missing_files;
}

// a missing file is in the collection but not on the disk
QStringList RepoState::missingFilePaths(QStringList found_files)
{
    return missingFiles(found_files).keys();
}

// an known file is is thought to be on disk as far as a collection is concerned
QStringList RepoState::knownFilePaths(QStringList found_files)
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
QStringList RepoState::unknownFilePaths(QStringList found_files)
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

QStringList RepoState::subDirs(QString path)
{
    QStringList keys = m_files.keys();
    QStringList sub_dirs;
    foreach(QString key, keys){
        // match any key beginning with path followed by /subdirname/morestuff
        QString rx_str;
        if (path.length() == 0){
            rx_str = QString("([^/?*:;{}\\\\]+)/(.*)");
        }else{
            rx_str = QString("%1/([^/?*:;{}\\\\]+)/(.*)").arg(QRegExp::escape(path));
        }
        QRegExp subdirs_rx = QRegExp(rx_str);
        if (subdirs_rx.exactMatch(key)){
            sub_dirs << subdirs_rx.cap(1);
        }
    }
    sub_dirs.removeDuplicates();
    return sub_dirs;
}

QList<FileInfo*> RepoState::filesInDir(QString path)
{
//    g_log->debug("filesInDir path:"+path);
    QHash<QString, FileInfo*>::const_iterator i;
    QList<FileInfo*> matches;
//    g_log->debug(QString("size of m_files: %1").arg(m_files.size()));
    for(i = m_files.begin(); i != m_files.end(); ++i){
        QRegExp files_in_dir_rx = QRegExp(QString("^%1/([^/?*\\\\]+)").arg(QRegExp::escape(path)));
        if (files_in_dir_rx.exactMatch(i.key())){
            matches << i.value();
        }
    }

    return matches;
}

//QHash<QString, FileInfo*> FileRepoState::findFiles(QString query)
//{
//    QHash<QString, FileInfo*>::const_iterator i;
//    QHash<QString, FileInfo*> matches;
//    for (i = m_files.begin(); i != m_files.end(); ++i){
//        if (i.key().startsWith(query)){
//            matches.insert(i.key(), i.value());
//        }
//    }
//    return matches;
//}
