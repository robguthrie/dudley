#ifndef FILEREPO_H
#define FILEREPO_H

#include "repostate.h"
#include <QObject>
#include <QString>
#include <QIODevice>
#include <QSettings>

class Repo : public QObject
{
    Q_OBJECT
public:
    Repo(QObject *parent = 0, QString path = "nopath", QString name = "noname");
    virtual QString type() const = 0;
    virtual QString name() const;
    virtual QString path() const;
    virtual QString logPath();
    virtual bool isReady() const;
    virtual bool initialize();
    virtual bool canReadData() const = 0;
    virtual bool hasFile(FileInfo* file_info) const = 0;
    virtual void updateState(bool commit_changes = true) = 0;
    virtual RepoState* state();
    virtual QMap<QString, QVariant> settings();

    virtual FileInfo* fileInfoByFilePath(QString file_name) const;
    virtual FileInfo* fileInfoByFingerPrint(QString finger_print) const;
//    virtual bool hasFile(FileInfo fileInfo) const = 0;
    // the returned file should be open
    virtual QIODevice* getFile(FileInfo* fileInfo) = 0;

    virtual QIODevice* putFile(QString file_path) = 0;
    virtual void putFileComplete(QIODevice* device, QString file_path) = 0;

public slots:
    virtual void putFileFailed(QIODevice *device = 0) = 0;

protected:
    RepoState *m_state;
    QString m_path;
    QString m_name;
    QString m_log_path;
};

#endif // FILEREPO_H
