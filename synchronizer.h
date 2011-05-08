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
    Synchronizer(QObject *parent, Repo *repo, QUrl tracker_url, QUrl self_url);
    QUrl trackerUrl() const;
    bool isReady() const;
    bool isActive() const;
    void start();
    void stop();
    QStringList stateDiffsToPush() const;
    QStringList stateDiffsToPull() const;
    QNetworkReply* requestHistory();
    QNetworkReply* registerSelf();
    QNetworkReply* requestPeers();
    QStringList peerUrlStrings() const;

    Repo* repo() const;

signals:

public slots:
    void loop();

private slots:
    void requestFinished(QNetworkReply *reply);

private:
    void selfRegistered(QByteArray body);
    void peersReceived(QByteArray body);
    void historyReceived(QByteArray body);
    void stateDiffPulled(QByteArray body);
    QNetworkReply* post(QUrl base_url, QString action, QByteArray body);
    QNetworkReply* get(QUrl base_url, QString action);
    QNetworkRequest request(QUrl base_url, QString action);
    void debugNetworkReply(QNetworkReply* reply);

    QIODevice* get() const;
    Repo* m_repo;
    QUrl m_trackerUrl;
    QUrl m_selfUrl;

    QDateTime m_lastReplyAt;
    QDateTime m_historyRequestedAt;
    QDateTime m_historyReceivedAt;
    QDateTime m_peersReceivedAt;
    QDateTime m_selfUrlRegisteredAt;

    QStringList m_peerUrlStrings;
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
