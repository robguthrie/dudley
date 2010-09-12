#ifndef FILEREPO_H
#define FILEREPO_H

#include "filerepostate.h"
#include <QObject>
#include <QString>
#include <QIODevice>
class FileRepo : public QObject
{
    Q_OBJECT
public:

    FileRepo(QObject *parent = 0, QString name = "noname");

    // update the given state object to represent the current repo state
    virtual bool isReady() = 0;
    virtual void updateState() = 0;
    virtual FileRepoState* state();

//    virtual void addFile(QIODevice *sourceFile, FileInfo fileInfo) = 0;
//    virtual bool deleteFile(FileInfo fileInfo) = 0;
//    virtual bool renameFile(FileInfo fileInfo, QString newFileName) = 0;
    //    virtual QIODevice readFile(FileInfo fileInfo);
    virtual QString type() = 0;
    virtual QString path() = 0;
    virtual QString name();

    virtual bool hasFile(FileInfo fileInfo) const = 0;
    virtual bool hasFileInfoByFilePath(QString file_name) const;
    virtual bool hasFileInfoByFingerPrint(QString finger_print) const;
    virtual FileInfo* fileInfoByFilePath(QString file_name) const;
    virtual FileInfo* fileInfoByFingerPrint(QString finger_print) const;
    virtual QIODevice* getFile(FileInfo* fileInfo) const = 0;
protected:
    FileRepoState *m_state;
    QString m_name;
};

#endif // FILEREPO_H
