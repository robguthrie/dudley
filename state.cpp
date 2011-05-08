#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <iostream>
#include <QDir>

#include "state.h"
#include "statediff.h"
#include "fileinfo.h"
#include "statelogger.h"
#include "output.h"


//QStringList State::filesInDir(QString path) const
//{
//    QHash<QString, FileInfo>::const_iterator i;
//    QStringList matches;
//    for(i = m_files.begin(); i != m_files.end(); ++i){
//        QRegExp files_in_dir_rx = QRegExp(QString("^%1/([^/?*\\\\]+)").arg(QRegExp::escape(path)));
//        if (files_in_dir_rx.exactMatch(i.key())){
//            matches << i.key();
//        }
//    }
//    return matches;
//}

QList<FileInfo> State::filesInDir(QString path) const
{
    QHash<QString, FileInfo>::const_iterator i;
    QList<FileInfo> matches;
    for(i = m_files.begin(); i != m_files.end(); ++i){
        QRegExp files_in_dir_rx = QRegExp(QString("^%1/([^/?*\\\\]+)").arg(QRegExp::escape(path)));
        if (files_in_dir_rx.exactMatch(i.key())){
            matches << i.value();
        }
    }
    return matches;
}

/* return a list of paths which represent sub directories of path */
/* looks really inefficient and should be used sparingly if at all */
QStringList State::subDirectories(QString path) const
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

bool State::knownFilePath(QString path) const
{
    return m_files.contains(path);
}

bool State::knownFingerPrint(QString finger_print) const
{
    return m_fingerprints.contains(finger_print);
}

/* return the subset of file_paths which are present in the state */
QStringList State::knownFilePaths(QStringList file_paths) const
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
QStringList State::unknownFilePaths(QStringList file_paths) const
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
QStringList State::missingFilePaths(QStringList found_files) const
{
    QStringList missing_files = m_files.keys();
    QString file_path;
    foreach(file_path, found_files){
        missing_files.removeAll(file_path);
    }
    return missing_files;
}

FileInfo State::fileInfoByFilePath(QString path) const
{
    return m_files.value(path);
}

FileInfo State::fileInfoByFingerPrint(QString sha1) const
{
    return m_fingerprints.value(sha1);
}

void State::clear()
{
    m_files.clear();
    m_fingerprints.clear();
}

bool State::canPerformChanges(StateDiff &sd) const
{
    QList<StateOp> *state_ops = sd.stateOps();
    StateOp op;
    foreach(op, *state_ops){
        QString file_path = op.filePath();
        QString action = op.action();

        if (action == "AddFile"){
            if (!canAddFile(file_path)){
                qDebug() << "cannot add file : "
                         << file_path
                         << " we must already have it";
                return false;
            }
        }else if (action == "ModifyFile"){
            if (!canModifyFile(file_path)){
                qDebug() << "cannot modify file : "
                         << file_path
                         << " we must already have it";
                return false;
            }
        }else if (action == "RenameFile"){
            if (!canRenameFile(file_path, op.newFilePath())){
                qDebug() << "cannot rename file. either "
                         << file_path
                         << " does not exist, or there is already a file called "
                         << op.newFilePath();
                return false;
            }
        }else if (action == "RemoveFile"){
            if (!canRemoveFile(file_path)){
                qDebug() << "cannot remove file: "
                         << file_path
                         << "does it exist in the state?";
                return false;
            }
        }
    }
    return true;
}

bool State::canAddFile(QString file_path) const
{
    return !knownFilePath(file_path);
}

bool State::canModifyFile(QString file_path) const
{
    return knownFilePath(file_path);
}

bool State::canRenameFile(QString file_path, QString new_file_path) const
{
    return (knownFilePath(file_path)) && (!knownFilePath(new_file_path));
}

bool State::canRemoveFile(QString file_path) const
{
    return knownFilePath(file_path);
}

void State::performChanges(StateDiff &sd)
{
    QList<StateOp> *state_ops = sd.stateOps();
    StateOp op;
    foreach(op, *state_ops){
        QString action = op.action();
        if (action == "AddFile"){
            addFile(op.filePath(), op.size(), op.modifiedAt(), op.fingerPrint());
        }else if (action == "ModifyFile"){
            modifyFile(op.filePath(), op.size(), op.modifiedAt(), op.fingerPrint());
        }else if (action == "RenameFile"){
            renameFile(op.filePath(), op.newFilePath());
        }else if (action == "RemoveFile"){
            removeFile(op.filePath());
        }
    }
}

void State::addFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1)
{
    FileInfo file_info(file_path, size, modified_at, sha1);
    m_files.insert(file_path, file_info);
    m_fingerprints.insert(sha1, file_info);
}

void State::modifyFile(QString file_path, qint64 size, QDateTime modified_at, QString sha1)
{
    FileInfo file_info = m_files.value(file_path);
    QString old_sha1 = file_info.fingerPrint();
    m_fingerprints.remove(old_sha1);
    m_files.remove(file_path);
    file_info.update(size, modified_at, sha1);
    m_fingerprints.insert(sha1, file_info);
    m_files.insert(file_path, file_info);
}

void State::removeFile(QString file_path)
{
    FileInfo fileInfo = m_files.value(file_path);
    m_files.remove(file_path);
    m_fingerprints.remove(fileInfo.fingerPrint());
}

void State::renameFile(QString file_path, QString new_file_path)
{
    FileInfo file_info = m_files.value(file_path);
    m_files.remove(file_path);
    m_fingerprints.remove(file_info.fingerPrint());
    file_info.rename(new_file_path);
    m_files.insert(new_file_path, file_info);
    m_fingerprints.insert(file_info.fingerPrint(), file_info);
}
