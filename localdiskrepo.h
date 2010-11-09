#ifndef WORKINGFILEREPO_H
#define WORKINGFILEREPO_H

#include <QFile>
#include <QFileInfo>
#include "repostatelogger.h"
#include "repostate.h"
#include "repo.h"

class LocalDiskRepo : public Repo
{
    Q_OBJECT
public:
    LocalDiskRepo(QObject *parent, QString path, QString name);
    QString type() const;
    bool canReadData() const;
    void updateState(bool commit_changes = true);
    bool hasFile(FileInfo* fileInfo) const;
    QIODevice* getFile(FileInfo* fileInfo);
    QIODevice* putFile(QString file_name);
    void putFileComplete(QIODevice* device, QString file_name);

public slots:
    void putFileFailed(QIODevice* device = 0);

private:
    QStringList filesOnDisk();
    FileInfo* newFileInfo(QString filePath);
    QString readFingerPrint(QString filePath);
    QString readFingerPrint(QFile* d);
    void findAllFiles(QString path, QStringList *found_files);
    QString absoluteFilePath(QString file_path);
    QString relativeFilePath(QString file_path);
    QString temporaryFilePath(QString file_path);
};




#endif // WORKINGFILEREPO_H
