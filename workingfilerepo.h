#ifndef WORKINGFILEREPO_H
#define WORKINGFILEREPO_H

#include <QFile>
#include <QFileInfo>
#include "filerepostatelogger.h"
#include "filerepostate.h"
#include "filerepo.h"

class WorkingFileRepo : public FileRepo
{
public:
    WorkingFileRepo(QObject *parent, QString path, QString name);
    QString type();
    bool canReadData() const;
    void updateState();
    bool hasFile(FileInfo fileInfo) const;
    QIODevice* getFile(FileInfo* fileInfo) const;

private:
    QStringList filesOnDisk();
    FileInfo* newFileInfo(QString filePath);
    QString readFingerPrint(QString filePath);
    void findAllFiles(QString path, QStringList *found_files);
    QString relativeFilePath(QString filePath);
};




#endif // WORKINGFILEREPO_H
