#ifndef FILEREPO_H
#define FILEREPO_H

#include <QObject>
#include <QString>
#include <QIODevice>
#include <QSettings>

#include "state.h"

class Repo : public QObject
{
    Q_OBJECT
public:
    Repo(QObject *parent = 0, QString path = "nopath", QString name = "noname");
    virtual QString name() const;
    virtual QString path() const;
    virtual QString logPath();
    virtual bool isReady() const;
    virtual bool initialise();
    void detectChanges();
    virtual StateLogger* logger();
    virtual QMap<QString, QVariant> settings();
    virtual QString type() const = 0;
    virtual bool canReadData() const = 0;
    virtual bool fileExists(FileInfo file_info) const = 0;
    virtual FileInfo readFileInfo(QString filePath) = 0;
    virtual FileInfo readFileInfoCheap(QString filePath) = 0;
    virtual QStringList readFilePaths() = 0;
    virtual QString readFingerPrint(QString filePath) = 0;
    //    virtual bool hasFile(FileInfo fileInfo) const = 0;
    // the returned file should be open
    virtual QIODevice* getFile(FileInfo fileInfo) = 0;
    virtual QIODevice* putFile(QString file_path) = 0;
    virtual void putFileComplete(QIODevice* device, QString file_path) = 0;

public slots:
    virtual void putFileFailed(QIODevice *device = 0) = 0;

protected:
    StateLogger *m_logger;
    QString m_path;
    QString m_name;
    QString m_log_path;
};

#endif // FILEREPO_H
