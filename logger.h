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
    QString m_logFileDir;
    QStringList m_logLines;
    void readLogLine(QString line);
public:
    Logger(QString logFileDir, bool verbose = false);
    void playLogs(FileInfoCollection* collection);
    void readLogFile(QString logFilePath, FileInfoCollection* collection);
    void writeLogFile();
    QString log(QString operation, FileInfo* fi);
};

#endif // LOGGER_H
