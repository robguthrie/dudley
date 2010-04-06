#ifndef LOGGER_H
#define LOGGER_H
#include <QString>
#include <QFile>
#include <QTextStream>
#include "fileinfo.h"
#include "fileinfocollection.h"

class Logger
{
private:
    bool m_verbose;
    FileInfoCollection *m_collection;
    QString m_logFileDir;
    QStringList m_logLines;
    void readLogLine(QString line);
public:
    Logger(QString logFileDir, bool verbose = false);

    void playLogs(FileInfoCollection* collection);
    void readLogFile(QString logFilePath, FileInfoCollection* collection);
    void writeLogFile();
    void printLogFile();
    QString log(QString operation, FileInfo* fi, QString newFilePath = "");
    void stageWorkingDirectory();
    void stageFileInfoCollection();
};

#endif // LOGGER_H
