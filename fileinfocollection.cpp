#include "fileinfocollection.h"
#include <QDateTime>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include <iostream>
#include <QDir>
#include <QDateTime>
#include <QDebug>

FileInfoCollection::FileInfoCollection(QString path)
{
    m_path = path;
}

/* open a logfile and read it. line by line. thus startign the process of
   updating the in-memory representation of the collection */
void FileInfoCollection::readLogFile(QString logFilePath)
{
    QFile file(logFilePath);
    if (file.open(QFile::ReadOnly)) {
        std::cout << "Reading Logfile: " << qPrintable(logFilePath) << std::endl;
        char buf[1024];
        while (file.readLine(buf, sizeof(buf)) != -1){
            readLogLine(QString(buf));
        }
    }else{
        std::cout << "Could not read Logfile: " << qPrintable(logFilePath) << std::endl;
    }
}

/* given a line in logfile format, follow the instruction and update the (in memory) collection */
void FileInfoCollection::readLogLine(QString line)
{
    // datetime, operation, filepath
    // datetime, add_file, filepath, mtime(ISO), size, sha1
    // datetime, remove_file, filepath
    // datetime, move_file, filepath, newfilepath

    QStringList parts = line.split(',');
    // we ignore the line timestamp.. it will be used later..
    QString operation(parts[1].trimmed());
    QString filePath = parts[2].trimmed();

    if (operation == "add_file" || operation == "modify_file"){
        QDateTime modifiedAt = QDateTime::fromString(parts[3].trimmed(), Qt::ISODate);
        qint64 sizeInBytes = parts[4].trimmed().toULongLong();
        QString sha1 = parts[5].trimmed();

        if (m_files.contains(filePath)){
            // update existing
            FileInfo *fileInfo = m_files.value(filePath);
            fileInfo->update(modifiedAt, sizeInBytes, sha1); // update without touching disk
        }else{
            // insert new
            FileInfo *fileInfo = new FileInfo(filePath, m_path, modifiedAt, sizeInBytes, sha1);
            m_files.insert(filePath, fileInfo);
        }
    }else if (operation == "delete_file") {
        m_files.remove(filePath);
    }else if (operation == "rename_file"){
        if (m_files.contains(filePath)){
            QString newFilePath = parts[3].trimmed();
            FileInfo *fileInfo = m_files.value(filePath);
            fileInfo->rename(newFilePath);
            m_files.remove(filePath);
            m_files.insert(newFilePath, fileInfo);
        }
    }
}

QStringList FileInfoCollection::scanForChanges(QString path)
{
    QDir dir(path);
    dir.setFilter(QDir::AllEntries | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Size | QDir::Reversed);
    QFileInfoList list = dir.entryInfoList();
    QHash<QString, FileInfo*> unknown_files;
    QStringList logLines;
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo qfi = list.at(i);
        QString filePath = relativeFilePath(qfi.absoluteFilePath());

        /* if fileInfo refers to a directory, need to recurse into the directory,
           passing the address of the QFileInfoList to the recurring fuction */
        if (!qfi.isDir()) {
            /* check if we have the file in the list
              if we do.. then compare and see if its different */
            if (m_files.contains(filePath)){
                FileInfo *fi = m_files.value(filePath);
                if (fi->lastModified() != qfi.lastModified()){
                    // file has changed on disk
                    // update the fileInfo obj and write a log line
                    fi->update(&qfi);
                    logLines << logLine("modify_file", fi);
                } else {
                    // file has not changed
                    // maybe a line of debug here
                }
            } else {
                // filename is not known
                //add it to the collection of unknown filenames
                // files which are not known need new fileinfos created
                FileInfo *fi = new FileInfo(filePath, m_path, &qfi);
                logLines << logLine("add_file", fi);
                unknown_files.insert(filePath, fi);
                // we wanna check for file renames with a sha1 check..
                // then the unmatched are called new files
            }
        }else{
              //std::cout << qPrintable(fileInfo.filePath()) << std::endl;
              scanForChanges(qfi.absoluteFilePath());
        }
    }
    return logLines;
}

QString FileInfoCollection::logLine(QString operation, FileInfo* fi)
{
    // if the eventname is update or add
    // datetime, operation, filepath
    // datetime, add_file, filepath, mtime(ISO), size, sha1
    // datetime, remove_file, filepath
    // datetime, move_file, filepath, newfilepath
    QDateTime currentTime = QDateTime::currentDateTime();
    QStringList tokens;
    tokens.append(currentTime.toString(Qt::ISODate));
    tokens.append(operation);
    if (operation == "modify_file" || operation == "add_file"){
        tokens.append(fi->filePath());
        tokens.append(fi->lastModified().toString(Qt::ISODate));
        tokens.append(QString::number(fi->size()));
        tokens.append(fi->fingerPrint());
    }else if ( operation == "remove_file") {
        tokens.append(fi->filePath());
    }else if ( operation == "rename_file") {
        tokens.append(fi->oldFilePath());
        tokens.append(fi->filePath());
    }
    return tokens.join(",");
}

QString FileInfoCollection::relativeFilePath(QString path)
{
    // remove the m_path prefixing the string
    if (path.startsWith(m_path))
        path = path.remove(0, m_path.size()+1);

    return path;
}
