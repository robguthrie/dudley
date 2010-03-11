#include "dudleylog.h"
#include <QDateTime>
#include <QFile>
#include <QStringList>
#include <iostream>
/*
  how is this used?
  call the constructor upon the logfile
  the DudleyLog constuctor will then open the file, read it to the end
  line by line..
    read the line from the log
    extract the details from the line
        operation (added | deleted | modified | renamed)
        filename,
        mtime
        sha1

    create a DudleyFileInfo about the file
    add the DudleyFileInfo object to the m_files qhash



    if the file exists
        create a QFileInfo from the referenced file
        if filename is known in m_files qhash
            if their signitures match nothing has changed
            if their signitures are different then the file has been modified
        else ( filename is not known in m_files)

  and build a qhash indexed by filename, and maybe a qhash indexed by md5

  */
DudleyLog::DudleyLog(QString logFile)
{
    QFile file(logFile);
    if (file.open(QFile::ReadWrite)) {
        std::cout << "Reading Logfile: " << qPrintable(logFile) << std::endl;
        char buf[1024];
        QString *str;
        while (file.readLine(buf, sizeof(buf)) != -1){
            // we read a line!
            str = new QString(buf);
            QStringList parts = str->split(',');
            QDateTime dateTime = QDateTime::fromString(parts[0].trimmed(), Qt::ISODate);
            QString operation = parts[1].trimmed();
            QString filePath = parts[2].trimmed();
            // ok so do we already have this filename?
            QHash<QString, DudleyFileInfo*>::const_iterator i = m_files.find(filePath);
            if (i != m_files.end() && i.key() == filePath) {

                // compare mtime, then sha1, standard sig test
                std::cout << "found reference to file already in hash: " << qPrintable(filePath) << std::endl;
            }else{
                std::cout << "inserting file deets into hash: " << qPrintable(filePath) << std::endl;
                m_files.insert(filePath, new DudleyFileInfo());
                // insert it into the hash with an add operation?
                // maybe after checking for sha1 clash
            }
        }
    }else{
        std::cout << "Could not read Logfile: " << qPrintable(logFile) << std::endl;
    }
}
