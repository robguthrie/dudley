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
    void addLogLine(QStringList tokens);
    void readLogFile(QString logFilePath, FileInfoCollection* collection);
    void logAddFile(FileInfo* fi);
    void logModifyFile(FileInfo* fi);
    void logDeleteFile(QString file_path);
    void logRenameFile(QString file_path, QString new_file_path);
    void findAllFiles(QString path, QStringList *found_files);
public:
    Logger(QString collection_path,  bool verbose = false);
    bool initialize();
    bool isInitialized();
    void playLogs();
    void writeLogFile();
    void printLogFile();
    void stageWorkingDirectory();
    void stageFileInfoCollection();
    QStringList* filesOnDisk();
    FileInfoCollection* collection();
};

#endif // LOGGER_H
