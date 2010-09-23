#ifndef FILEREPO_H
#define FILEREPO_H

#include "filerepostate.h"
#include <QObject>
#include <QString>
#include <QIODevice>
#include <QSettings>

class FileRepo : public QObject
{
    Q_OBJECT
public:
    FileRepo(QObject *parent = 0, QString path = "nopath", QString name = "noname");
    virtual QString type() = 0;
    virtual void updateState() = 0;
    virtual bool canReadData() const = 0;
    virtual QString path();
    virtual QString log_path();
    virtual QString name();
    // update the given state object to represent the current repo state
    virtual bool isReady() const;
    virtual bool initialize();
    virtual FileRepoState* state();
    virtual QMap<QString, QVariant> settings();
    virtual bool hasFileInfoByFilePath(QString file_name) const;
    virtual bool hasFileInfoByFingerPrint(QString finger_print) const;
    virtual FileInfo* fileInfoByFilePath(QString file_name) const;
    virtual FileInfo* fileInfoByFingerPrint(QString finger_print) const;
    virtual bool hasFile(FileInfo fileInfo) const = 0;
    virtual QIODevice* getFile(FileInfo* fileInfo) const = 0;

protected:
    FileRepoState *m_state;
    QString m_path;
    QString m_name;
    QString m_log_path;
};

//    virtual void addFile(QIODevice *sourceFile, FileInfo fileInfo) = 0;
//    virtual bool deleteFile(FileInfo fileInfo) = 0;
//    virtual bool renameFile(FileInfo fileInfo, QString newFileName) = 0;
    //    virtual QIODevice readFile(FileInfo fileInfo);

#endif // FILEREPO_H
