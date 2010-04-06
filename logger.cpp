#include <QFile>
#include <QDir>
#include <iostream>
#include "logger.h"
#include "fileinfocollection.h"
#include <QDebug>

// logger is a file collection history log
Logger::Logger(QString logFileDir, QString collection_path,  bool verbose)
{
    m_verbose = verbose;
    m_logFileDir = logFileDir;
    m_collection = new FileInfoCollection(collection_path);
}

// given a collection object.. play the list of file operations stored
// across each log file representing the history, (they each represent a commit)
// so that the collection object represents the last
// known state of the directory we track
void Logger::playLogs()
{
    // foreach .log file in the directory play it into the collection
    QDir dir(m_logFileDir);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
    dir.setSorting(QDir::Name);
    QStringList list = dir.entryList();
    for (int i = 0; i < list.size(); ++i) {
        QString filename = list.at(i);
        if (filename.endsWith(".log")) {
            readLogFile(m_logFileDir+"/"+filename, m_collection);
        }
    }
}

/* open a logfile and read it. line by line. thus startign the process of
   updating the in-memory representation of the collection */
void Logger::readLogFile(QString logFilePath, FileInfoCollection* collection)
{
    QString line;
    QFile file(logFilePath);
    if (file.open(QFile::ReadOnly)) {
        if (m_verbose)
            std::cout << "Reading Logfile: " << qPrintable(logFilePath) << std::endl;
        char buf[1024];

        while (file.readLine(buf, sizeof(buf)) != -1){

            //  operation, filepath
            //  add_file, filepath, mtime(ISO), size, sha1
            //  remove_file, filepath
            //  move_file, filepath, newfilepath
            line = QString(buf);
            QStringList parts = line.split(',');
            // we ignore the line timestamp.. it will be used later..
            QString operation(parts[0].trimmed());
            QString filePath = parts[1].trimmed().replace("DUDLEYCOMMA", ",");

            if (operation == "add_file" || operation == "modify_file"){
                QDateTime modifiedAt = QDateTime::fromString(parts[2].trimmed(), Qt::ISODate);
                qint64 sizeInBytes = parts[3].trimmed().toULongLong();
                QString sha1 = parts[4].trimmed();
                collection->update(filePath, modifiedAt, sizeInBytes, sha1);

            }else if (operation == "delete_file") {
                collection->remove(filePath);
            }else if (operation == "rename_file"){
                QString newFilePath = parts[2].trimmed().replace("DUDLEYCOMMA", ",");
                collection->rename(filePath, newFilePath);
            }else if (operation == "commit"){
                // ignore for now
            }
        }
        file.close();
    }else{
        std::cout << "Could not read Logfile: " << qPrintable(logFilePath) << std::endl;
    }
}

// flush the staged operations to a commit log file
// should probably do the operation, fileInfo* to log line here..
void Logger::writeLogFile()
{
    if (m_logLines.size() > 0){
        if (!QFile::exists(m_logFileDir)){
            QDir dir;
            dir.mkdir(m_logFileDir);
        }
        QFile file(m_logFileDir+"/"+QDateTime::currentDateTime().toString(Qt::ISODate)+".log");
        if (file.open(QIODevice::WriteOnly | QIODevice::Text | QFile::Append)){
            QTextStream out(&file);
            out << m_logLines.join("\n") << endl;
            std::cout << qPrintable(QString::number(m_logLines.size())) << " lines appended to logfile" << std::endl;
            file.close();
            m_logLines.empty();
        }else{
            std::cout << "could not open history.log for writing" << std::endl;
        }
    }
}

// show the operations staged to be written to a history commit file.
void Logger::printLogFile()
{
    if (m_logLines.size() > 0){
        std::cout << m_logLines.join("\n") << endl;
    }
}

// proposeAmmendments(collection)
// given a fileinfo_collection propose ammendments to history,
// required to make member fileinfocollection represent the new_collection's state


// add an operation to the staging area
// move bulk of this code to the flush function
QString Logger::log(QString operation, FileInfo* fi, QString newFilePath)
{

    // if the eventname is update or add
    // datetime, operation, filepath
    // datetime, add_file, filepath, mtime(ISO), size, sha1
    // datetime, remove_file, filepath
    // datetime, move_file, filepath, newfilepath

    QStringList tokens;

//    tokens.append(currentTime.toString(Qt::ISODate));
    tokens.append(operation);

    if (operation == "modify_file" || operation == "add_file"){
        tokens.append(fi->filePath());
        tokens.append(fi->lastModified().toString(Qt::ISODate));
        tokens.append(QString::number(fi->size()));
        tokens.append(fi->fingerPrint());
    }else if ( operation == "delete_file") {
        tokens.append(fi->filePath());
    }else if ( operation == "rename_file") {
        tokens.append(fi->filePath());
        tokens.append(newFilePath);
    }else{
        std::cout << "unrecognised log operation: " << qPrintable(operation) << std::endl;
    }
    tokens.replaceInStrings(",", "DUDLEYCOMMA");
    QString line = tokens.join(",");
    m_logLines = m_logLines << line;
    if (m_verbose)
        std::cout << qPrintable(line) << std::endl;
    return line;
}


/*
  rethink
  recursively get a qlist of qfileinfos for the whole working tree?
  can a qstringlist of paths do?
  benchmark ?
  - this is the ondisk state.

  we compare this to our fileinfocolleciton
  first, determine missing files
  then split found files (ie: the whole list) into known and unknown files
  then split known into modified and unchanged
  - at this stage we wanna read sha1's for modified and unknown files
  then split unknown into renamed (removing them from missing) and new
  then turn remaining missing files into deleted
  and stage these changes in the logger
  */

void Logger::stageWorkingDirectory()
{
    QStringList found_files;
    findAllFiles(m_collection->path(), &found_files);

    // first determine missing files.
    QList<QString> missingFilePaths = m_files.keys();
    QString filePath;
    foreach(filePath, found_files) missingFilePaths.removeAll(filePath);

    // now for all the found_files ..split into known and unknown files
    QString known_files;
    QString unknown_files;
    foreach(filePath, found_files){
        if (m_files.contains(filePath)){
            known_files << filePath;

            // check if that file has been modified
            FileInfo *stored_fi = m_files.value(filePath);
            FileInfo *current_fi = new FileInfo(filePath, m_path, qfi); // reads the sha1 by default
            if (stored_fi->lastModified() != current_fi->lastModified()){
                // file has changed on disk
                // dont want to update our collection.. just stage it in the logger
                logger->log("modify_file", current_fi);
            }
        }else{
            unknown_files << filePath;
            FileInfo *unknown_fi = new FileInfo(filePath, m_path);
            QString missingFilePath;
            foreach(missingFilePath, missingFilePaths){
                FileInfo *missing_fi = m_files.value(missingFilePath);
                if (missing_fi->isIdenticalContent(unknown_fi)){
                    // this unknown(looks new) file is actually a missing file renamed
                    missingFilePaths.removeAll(missingFilePath);
                    unknown_files.removeAll(filePath);
                    logger->log("rename_file", missing_fi, filePath);
                }
            }
        }
    }

    // deleted sweep
    foreach(key, missingFilePaths){
        logger->log("delete_file", fi);
    }

    // finally decare the remaining unknown files to be new files
    foreach(unknown_fi, unknown_files){
        logger->log("add_file", unknown_fi);
    }

}

/*  we compare this to our fileinfocolleciton
  first, determine missing files
  then split found files (ie: the whole list) into known and unknown files
  then split known into modified and unchanged
  - at this stage we wanna read sha1's for modified and unknown files
  then split unknown into renamed (removing them from missing) and new
  then turn remaining missing files into deleted
  and stage these changes in the logger
  */
void Logger::stageFileInfoCollection()
{

}

void Logger::findAllFiles(QString path, QStringList *found_files)
{
    // first grab the list of files
    QDir dir(path);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    QStringList fileNames = dir.entryList();
    QString fileName;
    foreach(fileName, fileNames) found_files << relativeFilePath(path+'/'+fileName);

    for (int i = 0; i < list.size(); ++i) {
        recursiveListFiles(path+"/"+dirs[0], found_files);
    }

    // then recurse down into every directory
    QDir dir(path);
    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    QStringList dirs = dir.entryList();

    for (int i = 0; i < list.size(); ++i) {
        recursiveListFiles(path+"/"+dirs[0], found_files);
    }
}
