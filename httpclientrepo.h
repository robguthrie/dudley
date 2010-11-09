#ifndef HTTPCLIENTFILEREPO_H
#define HTTPCLIENTFILEREPO_H

#include "repostate.h"
#include "repo.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
class HttpClientRepo : public Repo
{
    Q_OBJECT
public:
    HttpClientRepo(QObject *parent, QString path, QString name);
    QString type() const;
    bool canReadData() const;
    void updateState(bool commit_changes = true);
    bool hasFile(FileInfo* file_info) const;
    QIODevice* getFile(FileInfo* fileInfo);
    QIODevice* putFile(QString file_path);
    void       putFileComplete(QIODevice *device, QString file_path);

public slots:
    void       putFileFailed(QIODevice *device = 0);

private slots:
    void requestFinished(QNetworkReply* reply);

private:
    void ping();
    QIODevice* get(QUrl url);
    QString temporaryFilePath(FileInfo *fileInfo);
    QUrl fileUrl(FileInfo* fileInfo);
    QUrl urlFor(QString a, QString b = "", QString c = "", QString d = "");
    QString relativeFilePath(QString filePath);
    QNetworkAccessManager* m_manager;
    QDateTime m_lastPingTime;
    bool m_commitChanges;
    QString m_host_url;
    QString m_host_repo_name;
    int m_pendingLogDownloads;
};

#endif // HTTPCLIENTFILEREPO_H
