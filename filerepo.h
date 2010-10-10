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
    virtual QString type() const = 0;
    virtual QString name() const;
    virtual QString path() const;
    virtual QString logPath();
    virtual bool isReady() const;
    virtual bool initialize();
    virtual bool canReadData() const = 0;
    virtual void updateState() = 0;
    virtual FileRepoState* state();
    virtual QMap<QString, QVariant> settings();
    virtual bool hasFileInfoByFilePath(QString file_name) const;
    virtual bool hasFileInfoByFingerPrint(QString finger_print) const;
    virtual FileInfo* fileInfoByFilePath(QString file_name) const;
    virtual FileInfo* fileInfoByFingerPrint(QString finger_print) const;
//    virtual bool hasFile(FileInfo fileInfo) const = 0;
    // the returned file should be open
    virtual QIODevice* getFile(FileInfo* fileInfo) = 0;
//    virtual void putFile(FileInfo* fileInfo, QIODevice* file) = 0;

protected:
    FileRepoState *m_state;
    QString m_path;
    QString m_name;
    QString m_log_path;
};

#endif // FILEREPO_H
