#ifndef WORKINGFILEREPO_H
#define WORKINGFILEREPO_H

#include <QFile>
#include <QFileInfo>
#include "statelogger.h"
#include "state.h"
#include "repo.h"

class LocalDiskRepo : public Repo
{
    Q_OBJECT
public:
    LocalDiskRepo(QObject *parent, QString path, QString name);
    QString type() const;
    bool canReadData() const;
    bool fileExists(FileInfo fileInfo) const;
    QIODevice* getFile(FileInfo fileInfo);
    QIODevice* putFile(QString file_name);
    void putFileComplete(QIODevice* device, QString file_name);
    QStringList readFilePaths();
    FileInfo readFileInfo(QString filePath);
    FileInfo readFileInfoCheap(QString filePath);
    QString readFingerPrint(QString filePath);

public slots:
    void putFileFailed(QIODevice* device = 0);

private:
    QString readFingerPrint(QFile* d);
    void findAllFiles(QString path, QStringList* found_files);
    QString absoluteFilePath(QString file_path);
    QString relativeFilePath(QString file_path);
    QString temporaryFilePath(QString file_path);
};

#endif // WORKINGFILEREPO_H
