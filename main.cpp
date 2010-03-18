#include <QtCore/QCoreApplication>
#include <QDir>
#include <QDateTime>
#include <iostream>
#include <QDebug>
#include <QSettings>
#include "fileinfocollection.h"

/* dudley
   binaryfile revision control

   dudley allows you to save a history of your binary files
   dudley uses a logfile to track changes to a directory


   a dudley working directory has a .dudley directory

   dudley must be run within the dudley working directory
   files
    ./dudley/history.log
    ./dudley/settings.ini
        qSettings? based

   commands
    ./dudley init
        create a dudley working directory with the current working directory

    ./dudley status
        print a list of changes that have occured since the the logfile shows.

    ./dudley commit
        update the logfile to the current status

    ./dudley backup
        given some more work.. this represents the update of a dudley archive
        to backup to s3
            we need to calculate which files s3 has..
            and which we need to send.

*/
int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QString collection_path = QDir::currentPath();
    QString config_path = collection_path+"/.dudley/";
    QString log_path = config_path + "history.log";
    QString command = argv[1];
    QStringList logLines;

    if (QFile::exists(config_path)){
        // QSettings settings(config_path+"settings.ini", QSettings::IniFormat)

        FileInfoCollection collection(collection_path);
        collection.readLogFile(log_path);
        logLines = collection.scanForChanges(collection_path);
        if (command == "commit"){
            std::cout << "gonna save em to the log";
            QFile file(log_path);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text | QFile::Append)){
                QTextStream out(&file);
                out << logLines.join("\n") << endl;

                file.close();
            }else{
                std::cout << "could not open history.log for writing" << std::endl;
            }

        }else{
            std::cout << qPrintable(logLines.join("\n")) << std::endl;
        }
    }else{
        if (command == "init"){
            QDir dir;
            dir.mkdir(config_path);
            std::cout << "initialized dudley upon " << qPrintable(config_path) << std::endl;
        }else{
            std::cout << "no config directory found here. init first or cd to correct directory" << std::endl;
        }
    }

    return 0;
}

