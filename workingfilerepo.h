#ifndef WORKINGFILEREPO_H
#define WORKINGFILEREPO_H

#include <QFile>
#include <QFileInfo>
#include "filerepostatelogger.h"
#include "filerepostate.h"
#include "filerepo.h"

class WorkingFileRepo : public FileRepo
{
//    Q_OBJECT
public:
    /*
    FileRepo();

    // update the given state object to represent the current repo state
    virtual bool isReady();
    virtual void updateState();
    virtual void hasFile(FileInfo fileInfo) const;
    virtual void addFile(QIODevice *sourceFile, FileInfo fileInfo);
    virtual bool deleteFile(FileInfo fileInfo);
    virtual bool renameFile(FileInfo fileInfo, QString newFileName);
    virtual QIODevice readFile(FileInfo fileInfo);
    virtual QString type() = 0;
    virtual QString path() = 0;
    */
    WorkingFileRepo(QObject *parent, QString path, QString name);
    bool isReady();
    void updateState();
    FileRepoState* state();
    bool hasFile(FileInfo fileInfo) const;
    QIODevice* getFile(FileInfo* fileInfo) const;
//    virtual void addFile(QIODevice *sourceFile, FileInfo fileInfo) = 0;
//    virtual bool deleteFile(FileInfo fileInfo) = 0;
//    virtual bool renameFile(FileInfo fileInfo, QString newFileName) = 0;
    //    virtual QIODevice readFile(FileInfo fileInfo);

//    bool hasFileInfoByFingerPrint(QString finger_print) const;
//    bool hasFileInfoByFilePath(QString file_name) const;
//    FileInfo* fileInfoByFilePath(QString file_name) const;
//    FileInfo* fileInfoByFingerPrint(QString finger_print) const;
    QString type();
    QString path();

    bool initialize();

private:
    bool canOpenWorkingDirectory() const;
    QStringList filesOnDisk();
    FileInfo* newFileInfo(QString filePath);
    QString readFingerPrint(QString filePath);
    void findAllFiles(QString path, QStringList *found_files);

    QString relativeFilePath(QString filePath);
    QString m_path;
    QString config_path;

};




#endif // WORKINGFILEREPO_H
