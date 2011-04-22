#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <iostream>
#include <QDir>

#include "state.h"
#include "fileinfo.h"
#include "statelogger.h"
#include "output.h"

State::State(QObject* parent)
    :QObject(parent)
{}

State::~State()
{
}

QStringList State::filesInDir(QString path)
{
    QHash<QString, FileInfo>::const_iterator i;
    QStringList matches;
    for(i = m_files.begin(); i != m_files.end(); ++i){
        QRegExp files_in_dir_rx = QRegExp(QString("^%1/([^/?*\\\\]+)").arg(QRegExp::escape(path)));
        if (files_in_dir_rx.exactMatch(i.key())){
            matches << i.key();
        }
    }
    return matches;
}

/* return a list of paths which represent sub directories of path */
/* looks really inefficient and should be used sparingly if at all */
QStringList State::subDirectories(QString path)
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

bool State::knownFilePath(QString path)
{
    return m_files.contains(path);
}

bool State::knownFingerPrint(QString finger_print)
{
    return m_fingerprints.contains(finger_print);
}

/* return the subset of file_paths which are present in the state */
QStringList State::knownFilePaths(QStringList file_paths)
{
    QString file_path;
    QStringList known_file_paths;

    foreach(file_path, file_paths){
        if (knownFilePath(file_path)){
            known_file_paths << file_path;
        }
    }
    return known_file_paths;
}

/* return the subset of file_paths which are not present in the state */
QStringList State::unknownFilePaths(QStringList file_paths)
{
    QString file_path;
    QStringList unknown_file_paths;
    foreach(file_path, file_paths){
        if (!knownFilePath(file_path)){
            unknown_file_paths << file_path;
        }
    }
    return unknown_file_paths;
}

/* return the file_paths in state not mentioned in found_files */
QStringList State::missingFilePaths(QStringList found_files)
{
    QStringList missing_files = m_files.keys();
    QString file_path;
    foreach(file_path, found_files){
        missing_files.removeAll(file_path);
    }
    return missing_files;
}

FileInfo State::fileInfoByFilePath(QString path)
{
    return m_files.value(path);
}

FileInfo State::fileInfoByFingerPrint(QString sha1)
{
    return m_fingerprints.value(sha1);
}

void State::clear()
{
    m_files.clear();
    m_fingerprints.clear();
}

void State::addFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1)
{
    if (!m_files.contains(file_path)){
        // update existing without touching disk
        FileInfo file_info(file_path, size, modified_at, sha1);
        m_files.insert(file_path, file_info);
        m_fingerprints.insert(sha1, file_info);
    }else{
        // insert new
        qCritical() << "adding file " << file_path << ". it's already indexed";
    }
}

void State::modifyFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1)
{
    if (m_files.contains(file_path)){
        qDebug() << "called modify file with: " << file_path << sha1;
        // update existing without touching disk
        // likely (inevitable even) that the sha1 has changed.. so need to pull the old sha1 and insert the new
        FileInfo file_info = m_files.value(file_path);
        QString old_sha1 = file_info.fingerPrint();
        m_fingerprints.remove(old_sha1);
        m_files.remove(file_path);
        file_info.update(size, modified_at, sha1);
        m_fingerprints.insert(sha1, file_info);
        m_files.insert(file_path, file_info);
    }else{
        qCritical() << "modify state for fileinfo which does not exist: " << file_path << ", " << sha1;
    }
}

bool State::removeFile(QString file_path)
{
    FileInfo fileInfo = m_files.value(file_path);
    if (m_files.remove(file_path)){
        m_fingerprints.remove(fileInfo.fingerPrint());
        return true;
    }else{
        return false;
    }
}

bool State::renameFile(QString file_path, QString new_file_path)
{
    if (m_files.contains(file_path)){
        FileInfo file_info = m_files.value(file_path);
        m_files.remove(file_path);
        m_fingerprints.remove(file_info.fingerPrint());
        file_info.rename(new_file_path);
        m_files.insert(new_file_path, file_info);
        m_fingerprints.insert(file_info.fingerPrint(), file_info);
        return true;
    }else{
        return false;
    }
}
