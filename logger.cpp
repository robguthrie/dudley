#include <QFile>
#include <QDir>
#include <iostream>
#include "logger.h"
#include "fileinfocollection.h"
#include <QDebug>
Logger::Logger(QString logFileDir, bool verbose)
{
    m_verbose = verbose;
    m_logFileDir = logFileDir;
}

void Logger::playLogs(FileInfoCollection* collection)
{
    // foreach .log file in the directory play it into the collection
    QDir dir(m_logFileDir);
    dir.setFilter(QDir::Files | QDir::NoDotAndDotDot | QDir::Readable);
    dir.setSorting(QDir::Name);
    QStringList list = dir.entryList();
    for (int i = 0; i < list.size(); ++i) {
        QString filename = list.at(i);
        if (filename.endsWith(".log")) {
            readLogFile(m_logFileDir+"/"+filename, collection);
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

// flush the m_logLines to the file
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

QString Logger::log(QString operation, FileInfo* fi)
{
    // if the eventname is update or add
    // datetime, operation, filepath
    // datetime, add_file, filepath, mtime(ISO), size, sha1
    // datetime, remove_file, filepath
    // datetime, move_file, filepath, newfilepath

    QStringList tokens;
    QString escapedFilePath = fi->filePath().replace(",", "DUDLEYCOMMA");

//    tokens.append(currentTime.toString(Qt::ISODate));
    tokens.append(operation);
    if (operation == "modify_file" || operation == "add_file"){
        tokens.append(escapedFilePath);
        tokens.append(fi->lastModified().toString(Qt::ISODate));
        tokens.append(QString::number(fi->size()));
        tokens.append(fi->fingerPrint());
    }else if ( operation == "delete_file") {
        tokens.append(escapedFilePath);
    }else if ( operation == "rename_file") {
        tokens.append(fi->oldFilePath().replace(",", "DUDLEYCOMMA"));
        tokens.append(escapedFilePath);
    }else{
        std::cout << "invalid log operation: " << qPrintable(operation) << std::endl;
    }
    QString line = tokens.join(",");
    m_logLines = m_logLines << line;
    if (m_verbose)
        std::cout << qPrintable(line) << std::endl;
    else
        std::cout << "not saying "<< qPrintable(line) << std::endl;

    return line;
}
