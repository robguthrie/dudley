#ifndef FILEINFOCOLLECTION_H
#define FILEINFOCOLLECTION_H

#include <QString>
#include <QHash>
#include "fileinfo.h"
class FileInfoCollection
{

private:
    QHash<QString, FileInfo*> m_files;
    QString m_path;
    void readLogLine(QString line);
    QString logLine(QString operation, FileInfo* fi);

public:
    FileInfoCollection(QString path);
    void readLogFile(QString logFilePath);
    QStringList scanForChanges(QString path);
    QString relativeFilePath(QString path);

};

#endif // FILEINFOCOLLECTION_H
