#ifndef WORKINGFILEREPO_H
#define WORKINGFILEREPO_H

#include <QFile>
#include <QFileInfo>
#include "repostatelogger.h"
#include "repostate.h"
#include "repo.h"

class LocalDiskRepo : public Repo
{
public:
    LocalDiskRepo(QObject *parent, QString path, QString name);
    QString type() const;
    bool canReadData() const;
    void updateState(bool commit_changes = true);
    QIODevice* getFile(FileInfo* fileInfo);
    bool hasFile(FileInfo* fileInfo) const;
    bool putFile(FileInfo* fileInfo, QIODevice *file);
private:
    QStringList filesOnDisk();
    FileInfo* newFileInfo(QString filePath);
    QString readFingerPrint(QString filePath);
    QString readFingerPrint(QFile* d);
    void findAllFiles(QString path, QStringList *found_files);
    QString relativeFilePath(QString filePath);
    QString temporaryFilePath(FileInfo* f);
    void putFileFinished(FileInfo* file_info, QIODevice* device);
};




#endif // WORKINGFILEREPO_H
