#include <QFile>
#include <QDir>
#include <iostream>
#include "logger.h"
#include "fileinfocollection.h"
#include <QDebug>

// logger is a file collection history log
Logger::Logger(QString collection_path,  bool verbose)
{
    m_verbose = verbose;
    m_logFileDir = collection_path+"/.dudley/logs";
    m_collection = new FileInfoCollection(collection_path);
}

bool Logger::initialize()
{
    QDir dir;
    return dir.mkpath(m_logFileDir);
}

bool Logger::isInitialized()
{
    QDir dir;
    return dir.exists(m_logFileDir);
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
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        if (m_verbose)
            std::cout << "Reading Logfile: " << qPrintable(logFilePath) << std::endl;

        QTextStream in(&file);
        while (!in.atEnd()){

            //  operation, filepath
            //  add_file, filepath, mtime(ISO), size, sha1
            //  remove_file, filepath
            //  move_file, filepath, newfilepath
            QString line = in.readLine();
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
        std::cout << qPrintable(m_logLines.join("\n")) << endl;
    }
}

void Logger::logAddFile(FileInfo* fi)
{
    QStringList tokens;
    tokens << "add_file"
           << fi->filePath()
           << fi->lastModified().toString(Qt::ISODate)
           << QString::number(fi->size())
           << fi->fingerPrint();
    addLogLine(tokens);
}

void Logger::logModifyFile(FileInfo* fi)
{
    QStringList tokens;
    tokens << "modify_file"
           << fi->filePath()
           << fi->lastModified().toString(Qt::ISODate)
           << QString::number(fi->size())
           << fi->fingerPrint();
    addLogLine(tokens);
}

void Logger::logDeleteFile(QString file_path)
{
    QStringList tokens;
    tokens << "delete_file"
           << file_path;
    addLogLine(tokens);
}

void Logger::logRenameFile(QString file_path, QString new_file_path)
{
    QStringList tokens;
    tokens << "rename_file"
           << file_path
           << new_file_path;
    addLogLine(tokens);
}

void Logger::addLogLine(QStringList tokens)
{
   tokens.replaceInStrings(",", "DUDLEYCOMMA");
   m_logLines << tokens.join(",");
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
    QString collection_path = m_collection->path();

    QStringList* found_files = new QStringList;
    if (m_verbose) std::cout << "reading filetree" << std::endl;
    findAllFiles(collection_path, found_files);


    QString file_path;

    // a missing file is in the collection but not on the disk
    // an unknown file is on the disk but not in the collection
    if (m_verbose) std::cout << "checking for files which have gone missing" << std::endl;
    QStringList missing_file_paths = m_collection->missingFilePaths(*found_files);
    if (m_verbose) std::cout << "checking for files which are unrecognised" << std::endl;
    QStringList unknown_found_file_paths = m_collection->unknownFilePaths(*found_files);
    if (m_verbose) std::cout << "checking for known files" << std::endl;
    QStringList known_found_file_paths = m_collection->knownFilePaths(*found_files);


    if (m_verbose) std::cout << "scanning known files for modifications" << std::endl;
    foreach(file_path, known_found_file_paths){
        // check if that file has been modified
        FileInfo *stored_fi = m_collection->getFileInfo(file_path);
        FileInfo *current_fi = new FileInfo(file_path, collection_path);
        if (stored_fi->lastModified().toString(Qt::ISODate) != current_fi->lastModified().toString(Qt::ISODate)){
            // file has changed on disk but filename is the same.
            // stage the change
            logModifyFile(current_fi);
        }
    }

    if (m_verbose) std::cout << "scanning unknown files" << std::endl;
    foreach(file_path, unknown_found_file_paths){
        FileInfo *unknown_fi = new FileInfo(file_path, collection_path);
        QString missing_file_path;
        foreach(missing_file_path, missing_file_paths){
            FileInfo *missing_fi = m_collection->getFileInfo(missing_file_path);
            if (missing_fi->isIdenticalTo(unknown_fi)){
                // this unknown file is actually a missing file renamed
                missing_file_paths.removeAll(missing_file_path);
                unknown_found_file_paths.removeAll(file_path);
                logRenameFile(missing_file_path, file_path);
            }
        }
    }

    // finally decare the remaining unknown files to be new files
    if (m_verbose) std::cout << "scanning new files" << std::endl;
    foreach(file_path, unknown_found_file_paths){
        FileInfo *unknown_fi = new FileInfo(file_path, collection_path);
        logAddFile(unknown_fi);
    }

    // deleted sweep
    foreach(file_path, missing_file_paths)
        logDeleteFile(file_path);
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

QStringList* Logger::filesOnDisk()
{
    QStringList *found_files;
    findAllFiles(m_path, found_files);
    return found_files;
}

void Logger::findAllFiles(QString path, QStringList *found_files)
{
    // first grab the list of files
    QDir dir(path);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    dir.setSorting(QDir::Name);
    QStringList fileNames = dir.entryList();
    QString fileName;
    foreach(fileName, fileNames){
        found_files->append(m_collection->relativeFilePath(path+'/'+fileName));
    }

    // then recurse down into every directory

    dir.setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList dirs = dir.entryList();
    for (int i = 0; i < dirs.size(); ++i) {
        findAllFiles(path+"/"+dirs[i], found_files);
    }
}

FileInfoCollection* Logger::collection()
{
    return m_collection;
}
