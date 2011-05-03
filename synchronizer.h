#ifndef SYNCHRONISER_H
#define SYNCHRONISER_H

#include <QObject>
#include <QUrl>
#include <QDateTime>
#include <QStringList>
#include <QNetworkAccessManager>
#include <QTimer>
#include "statediff.h"

class Repo;

class Synchronizer : public QObject
{
    Q_OBJECT
public:
    //explicit Synchronizer(QObject *parent = 0);
    Synchronizer(QObject *parent, Repo *repo, QString tracker_url);
    bool isReady() const;
    bool isActive() const;
    void start();
    void stop();
    QStringList stateDiffsToPush() const;
    QStringList stateDiffsToPull() const;
    QNetworkReply* requestHistory();

signals:

public slots:
    void loop();

private slots:
    void requestFinished(QNetworkReply *reply);

private:
    void historyReceived(QByteArray body);
    void stateDiffPulled(QByteArray body);
    QNetworkReply* post(QString action, QByteArray body);
    QNetworkReply* get(QString action);
    QNetworkRequest request(QString action);
    void debugNetworkReply(QNetworkReply* reply);

    QIODevice* get() const;
    Repo* m_repo;
    QString m_trackerUrl;
    QDateTime m_lastReplyAt;
    QDateTime m_historyRequestedAt;
    QDateTime m_historyReceivedAt;

    QStringList m_remoteStateDiffNames;
    QStringList m_localStateDiffNames;
    QStringList m_stateDiffsToPull;
    QStringList m_stateDiffsToPush;
    QHash<QString, StateDiff> m_newStateDiffs;
    QNetworkAccessManager *m_networkManager;
    QTimer *m_timer;
    int m_historyUpdateInterval;
    int m_numPushes;
    int m_numPulls;
    int m_maxPushes;
    int m_maxPulls;
};

#endif // SYNCHRONISER_H
