#ifndef HTTPCLIENTFILEREPO_H
#define HTTPCLIENTFILEREPO_H

#include "filerepostate.h"
#include "filerepo.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
class HttpClientFileRepo : public FileRepo
{
    Q_OBJECT
public:
    HttpClientFileRepo(QObject *parent, QString path, QString name);
    QString type();
    bool canReadData() const;
    void updateState();
//    bool hasFile(FileInfo fileInfo) const;
    QIODevice* getFile(FileInfo* fileInfo);

private slots:
    void requestFinished(QNetworkReply* reply);
//    void alertReadyRead();
//    void alertDownloadProgress(qint64 bytesReceived, qint64 bytesTotal);

private:
    void ping();
    QIODevice* get(QUrl url);
    QUrl fileUrl(FileInfo* fileInfo);
    QUrl urlFor(QString a, QString b = "", QString c = "", QString d = "");
    QString relativeFilePath(QString filePath);
    QNetworkAccessManager* m_manager;
    QDateTime m_lastPingTime;
};

#endif // HTTPCLIENTFILEREPO_H
