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
    LocalDiskRepo(QObject *parent, QString path);
    QString type() const;
    bool canReadData() const;
    bool fileExists(FileInfo fileInfo) const;
    QIODevice* getFile(FileInfo fileInfo);
    QIODevice* putFile(QString file_name);
    void putFileComplete(QIODevice* device, QString file_name);
    QStringList readFilePaths() const;
    FileInfo readFileInfo(QString filePath) const;
    FileInfo readFileInfoCheap(QString filePath) const;
    QString readFingerPrint(QString filePath) const;

public slots:
    void putFileFailed(QIODevice* device = 0);

private:
    QString readFingerPrint(QFile* d) const;
    void findAllFiles(QString path, QStringList* found_files) const;
    QString absoluteFilePath(QString file_path) const;
    QString relativeFilePath(QString file_path) const;
    QString temporaryFilePath(QString file_path) const;
};

#endif // WORKINGFILEREPO_H
