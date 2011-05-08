#ifndef FILEREPO_H
#define FILEREPO_H

#include <QObject>
#include <QString>
#include <QIODevice>
#include <QSettings>

#include "state.h"
#include "statediff.h"
#include "statelogger.h"
class Synchronizer;
class Repo : public QObject
{
    Q_OBJECT
public:
    Repo(QObject *parent = 0, QString path = "nopath");
    virtual QString path() const;
    virtual QString logPath();
    virtual bool isReady() const;
    virtual bool initialize();
    StateDiff detectChanges() const;
    State* state() const;
    virtual StateLogger* logger();
    virtual QMap<QString, QVariant> settings();
    virtual QString type() const = 0;
    virtual bool canReadData() const = 0;
    virtual bool fileExists(FileInfo file_info) const = 0;
    virtual FileInfo readFileInfo(QString filePath) const = 0;
    virtual FileInfo readFileInfoCheap(QString filePath) const = 0;
    virtual QStringList readFilePaths() const = 0;
    virtual QString readFingerPrint(QString filePath) const = 0;
    //    virtual bool hasFile(FileInfo fileInfo) const = 0;
    // the returned file should be open
    virtual QIODevice* getFile(FileInfo fileInfo) = 0;
    virtual QIODevice* putFile(QString file_path) = 0;
    virtual void putFileComplete(QIODevice* device, QString file_path) = 0;
    void performChanges(StateDiff *state_diff, Synchronizer* synchronizer);
public slots:
    virtual void putFileFailed(QIODevice *device = 0) = 0;

protected:
//    virtual bool addFile(QString file_path, qint64 size, QDateTime modified_at, QString fingerprint) = 0;
//    virtual bool modfiyFile(QString file_path, qint64 size, QDateTime modified_at, QString fingerprint) = 0;
//    virtual bool renameFile(QString file_path, QString new_file_path) = 0;
//    virtual bool removeFile(QString file_path) = 0;

    StateLogger *m_logger;
    QString m_path;
    QString m_log_path;
};

#endif // FILEREPO_H
