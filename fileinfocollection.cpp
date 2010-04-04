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

bool FileInfoCollection::rename(FileInfo *fileInfo, QString newFilePath)
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


//void FileInfoCollection::scanForChanges(QString path, Logger *logger, bool read_sha1s, bool write_log, bool print_log)
//{
//    QStringList found_files;
//    QList<FileInfo*> unknown_files;
//    QStringList logLines;
//    std::cout << "scanning " << qPrintable(path) << " (" << m_files.size() << " files on last scan)" << std::endl;
//    recursiveFind(path, logger, &unknown_files, &found_files, read_sha1s);
//
//    // detect missing files
//    // remove keys where they are in found_files
//    // the rest are missing files
//    //
//    QList<QString> missingFilePaths = m_files.keys();
//    QString key;
//
//    foreach(key, found_files)
//        missingFilePaths.removeAll(key);
//
//
//    // we wanna check for file renames with a sha1 check..
//    // then the unmatched are called new files
//    FileInfo *unknown_fi;
//    foreach(key, missingFilePaths){
//        FileInfo *missing_fi = m_files.value(key);
//        foreach(unknown_fi, unknown_files){
//            if (unknown_fi->fingerPrint() == missing_fi->fingerPrint()){
//                // this unknown(looks new) file is actually a missing file renamed
//                // we can tell because they have the same fingerprint
//                missingFilePaths.removeAll(missing_fi->filePath());
//                this->rename(missing_fi, unknown_fi->filePath());
//                unknown_files.removeAll(unknown_fi);
//                logger->log("rename_file", missing_fi);
//            }
//        }
//    }
//
//    // deleted sweep
//    foreach(key, missingFilePaths){
//        logger->log("delete_file", fi);
//        this->remove(key);
//    }
//
//    // finally decare the remaining unknown files to be new files
//    foreach(unknown_fi, unknown_files){
//        m_files.insert(unknown_fi->filePath(), unknown_fi);
//        if (read_sha1s){
//            m_fingerprints.insert(unknown_fi->fingerPrint(), unknown_fi);
//        }
//        logger->log("add_file", unknown_fi);
//    }
//
//    if (write_log) logger->writeLogFile();
//    if (print_log) logger->printLogFile();
//
//}
//
//void FileInfoCollection::recursiveFind(QString path, Logger *logger, QList<FileInfo*> *unknown_files, QStringList *found_files, bool read_sha1s)
//{
//    QDir dir(path);
//    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
//    dir.setSorting(QDir::Size | QDir::Reversed);
//    QFileInfoList list = dir.entryInfoList();
//
//    for (int i = 0; i < list.size(); ++i) {
//        QFileInfo qfi = list.at(i);
//        QString filePath = relativeFilePath(qfi.absoluteFilePath());
//
//        /* if fileInfo refers to a directory, need to recurse into the directory,
//           passing the address of the QFileInfoList to the recurring fuction */
//        if (!qfi.isDir()) {
//            /* check if we have the file in the list
//              if we do.. then compare and see if its different */
//            (*found_files) << filePath;
//            if (m_files.contains(filePath)){
//                FileInfo *fi = m_files.value(filePath);
//                if (fi->modified(&qfi)){
//                    // file has changed on disk
//                    // update the fileInfo obj and write a log line
//                    QString old_sha1 = fi->fingerPrint();
//                    fi->update(&qfi, read_sha1s);
//                    if (read_sha1s){
//                        m_fingerprints.remove(old_sha1);
//                        m_fingerprints.insert(fi->fingerPrint(), fi);
//                    }
//
//                    logger->log("modify_file", fi);
//                } else {
//                    // file has not changed
//                    // maybe a line of debug here
//                }
//            } else {
//                // filename is not known
//                //add it to the collection of unknown filenames
//                // files which are not known need new fileinfos created
//                if (read_sha1s)
//                    std::cout << "reading sha1 for " << qPrintable(filePath) << std::endl;
//                FileInfo *fi = new FileInfo(filePath, m_path, &qfi, read_sha1s);
//                (*unknown_files) << fi;
//            }
//        }else{
//            recursiveFind(qfi.absoluteFilePath(), logger, unknown_files, found_files, read_sha1s);
//        }
//    }
//
//}




// beable to produce a scan summary... files checked, modified, missing, added
