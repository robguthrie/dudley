#include "synchronizer.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include "repo.h"
#include "statelogger.h"
#include "parser.h"
#include "serializer.h"

Synchronizer::Synchronizer(QObject *parent, Repo *repo, QUrl tracker_url, QUrl self_url) :
    QObject(parent)
{
    m_repo = repo;
    m_trackerUrl = tracker_url;
    m_selfUrl = self_url;
    m_historyUpdateInterval = 600;
    m_numPushes = 0;
    m_numPulls = 0;
    m_maxPushes = 2;
    m_maxPulls = 2;
    m_networkManager = new QNetworkAccessManager(this);
    connect(m_networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(requestFinished(QNetworkReply*)));
    m_timer = new QTimer(this);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(loop()));
}

QUrl Synchronizer::trackerUrl() const
{
    return m_trackerUrl;
}

Repo* Synchronizer::repo() const
{
    return m_repo;
}

bool Synchronizer::isReady() const
{
    if (m_repo && m_repo->isReady() && m_trackerUrl.isValid()){
        return true;
    }else{
        return false;
    }
}

bool Synchronizer::isActive() const
{
    return m_timer->isActive();
}

void Synchronizer::start()
{
    m_timer->start(500);
}

void Synchronizer::stop()
{
    qDebug() << "stopping";
    m_timer->stop();
}

void Synchronizer::loop()
{
    // keep the history up to date
    // if the history is more than x minutes old pull it
    if (m_historyReceivedAt.secsTo(QDateTime::currentDateTime()) > m_historyUpdateInterval){
        requestHistory();
    }

    // if we are not already using all allocated connections, pull/push queued statediffs
    while (m_numPulls < (m_maxPulls+1)){
       if (!m_stateDiffsToPull.isEmpty()){
           QString name = m_stateDiffsToPull.takeFirst();
           m_numPulls += 1;
           get(m_trackerUrl, "state_diffs/"+name);
       }else break;
    }

    while (m_numPushes < (m_maxPushes+1)){
       if (!m_stateDiffsToPush.isEmpty()){
           QString name = m_stateDiffsToPush.takeFirst();
           bool ok;
           StateDiff diff = m_repo->logger()->loadStateDiff(name, &ok);
           if (ok){
               m_numPushes += 1;
               post(m_trackerUrl, "state_diffs", diff.toJSON());
           }else{
               qCritical() << "failed to loadStateDiff for push";
           }
       }else break;
    }

    // if numPulls and numPushes == 0 and toPull and toPush == 0
    // so we must have pulled and pushed all statediffs
    // so the problem of applying state
    // incomming state diffs should be ordered alphabetically by name
    // we then want a minimal list of operations from that
    // then presumably we have synchronized metadata
    // now we want to compare the most recent state to the repo
    // and see which files we need to download before we can recreate the state locally
}

QNetworkReply* Synchronizer::registerSelf()
{
   QVariantMap peer;
   peer["url"] = m_selfUrl;
   QVariantMap data;
   data["peer"] = peer;
   QJson::Serializer serializer;
   return post(m_trackerUrl, "peers", serializer.serialize(data));
}


void Synchronizer::selfRegistered(QByteArray body)
{
    QJson::Parser parser;
    bool ok;
    QVariantMap data = parser.parse(body, &ok).toMap();
    if(ok){
        m_selfUrlRegisteredAt = QDateTime::currentDateTime();
        m_selfUrl = QUrl(data["peer"].toMap()["url"].toString());
    }
}

QNetworkReply* Synchronizer::requestPeers()
{
    return get(m_trackerUrl, "peers");
}

void Synchronizer::peersReceived(QByteArray body)
{
    m_peersReceivedAt = QDateTime::currentDateTime();
    qDebug() << body;
    QJson::Parser parser;
    bool ok;
    QVariantList peers = parser.parse(body, &ok).toList();
    if (ok){
        m_peerUrlStrings.clear();
        foreach(QVariant peer, peers){
            QString peer_url = peer.toMap()["peer"].toMap()["url"].toString();
            if (peer_url != m_selfUrl.toString()){
                m_peerUrlStrings << peer_url;
            }
        }
    }
}

QStringList Synchronizer::peerUrlStrings() const
{
    return m_peerUrlStrings;
}

QNetworkReply* Synchronizer::requestHistory()
{
    m_historyRequestedAt = QDateTime::currentDateTime();
    return get(m_trackerUrl, "history");
}

void Synchronizer::historyReceived(QByteArray body)
{
    qDebug() << "parsing history index "<< body;

    QJson::Parser parser;
    bool ok;
    QStringList list = parser.parse(body, &ok).toStringList();
    if (!ok) {
        qFatal("An error occurred during logfile parsing");
        exit(1);
    }
    qDebug() << list;
    m_remoteStateDiffNames = list;
    m_localStateDiffNames = m_repo->logger()->logNames();
    m_historyReceivedAt = QDateTime::currentDateTime();

    m_stateDiffsToPull.clear();
    m_stateDiffsToPush.clear();

    foreach(QString name, m_remoteStateDiffNames){
        if (!m_localStateDiffNames.contains(name)){
            m_stateDiffsToPull << name;
        }
    }

    foreach(QString name, m_localStateDiffNames){
        if(!m_remoteStateDiffNames.contains(name)){
            m_stateDiffsToPush << name;
        }
    }

    if (isActive()){
        loop();
    }
}

QStringList Synchronizer::stateDiffsToPush() const
{
    return m_stateDiffsToPush;
}

QStringList Synchronizer::stateDiffsToPull() const
{
    return m_stateDiffsToPull;
}

void Synchronizer::stateDiffPulled(QByteArray body)
{
    StateDiff sd;
    if(sd.fromJSON(body)){
        m_newStateDiffs.insert(sd.name(), sd);
    }else{
        qCritical() << "failed to read statediff JSON";
    }
}

QNetworkReply* Synchronizer::post(QUrl base_url, QString action, QByteArray body)
{
    qDebug() << "post started: " << action;
    QNetworkReply* reply = m_networkManager->post(request(base_url, action), body);
    debugNetworkReply(reply);
    return reply;
}

// do an http get and return the QNetworkReply (which is a QIODevice i believe)
QNetworkReply* Synchronizer::get(QUrl base_url, QString action)
{
    qDebug() << "get request started:" << action;
    QNetworkReply* reply = m_networkManager->get(request(base_url, action));
    debugNetworkReply(reply);
    return reply;
}

QNetworkRequest Synchronizer::request(QUrl base_url, QString action)
{
    QUrl url(base_url.toString()+"/"+action);
    QNetworkRequest req(url);
    req.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    req.setRawHeader("Accept", "application/json");
    return req;
}

void Synchronizer::requestFinished(QNetworkReply* reply)
{
    bool success = true;
    if (reply->error()){
        success = false;
        qCritical() << "Synchroniser::requestFinised error: "
                    << reply->error()
                    << reply->errorString()
                    << reply->url().toString();
    }else{
        m_lastReplyAt = QDateTime::currentDateTime();
    }

    QStringList escaped_urls;
    escaped_urls << QRegExp::escape(m_trackerUrl.toString());
    foreach(QString peer_url, m_peerUrlStrings){
        escaped_urls << QRegExp::escape(peer_url);
    }
    foreach(QString url, escaped_urls){
        qDebug() << "hello:" << url;
    }
    QString base_urls_rx = QString("(") + escaped_urls.join("|") + ")";
    QRegExp rx(base_urls_rx+"/(peers|history|state_diffs)/?(.*)");

    QString action, file_path;
    QString request_url = reply->request().url().toString();
    if (rx.exactMatch(request_url)){
        QStringList tokens = rx.capturedTexts();
        if (tokens.size() > 2) action = tokens.at(2);
        if (tokens.size() > 3) file_path = tokens.at(3);
        if (file_path.startsWith("/")) file_path.remove(0, 1);

        //    if (action == "file"){
        //        // file should be written by now
        //        qDebug() << "requestFinished() file/size" << file_path << "/" << reply->size();
        //    }else
        if ((action == "history") && success){
            historyReceived(reply->readAll());
        }else if (action == "state_diffs"){
            if (reply->operation() == QNetworkAccessManager::PostOperation){
                // this is the end of a push operation
                m_numPushes = m_numPushes - 1;
            }else{
                // this is a statediff, so save it
                m_numPulls = m_numPulls - 1;
                if (success){
                    stateDiffPulled(reply->readAll());
                }
            }
        }else if (action == "peers"){
            if (success) {
                if (reply->operation() == QNetworkAccessManager::PostOperation){
                    selfRegistered(reply->readAll());
                }else{
                    peersReceived(reply->readAll());
                }
            }
        }else{
            qCritical() << "action not recognised: " << action;
        }
    }else{
        qCritical() << "could not parse request url: " << request_url;
    }

    if (!success){
        qDebug() << "failed response:" << request_url;
    }
}

void Synchronizer::debugNetworkReply(QNetworkReply* reply)
{
    qDebug() << "Debugging network reply for url: "
             << reply->url().toString();

    QList<QByteArray> headers = reply->rawHeaderList();
    foreach(QByteArray h, headers){
        qDebug() << QTextStream(h).readAll();
    }

    if (!reply->isOpen()){
        qDebug() << "Synchroniser networkReply is not open";
    }

    if (reply->error()){
        qDebug() << "Synchroniser::get() reply error: "
                 << reply->error()
                 << reply->errorString();
    }

    if (!reply->isReadable()){
        qDebug() << "Synchroniser::get() reply is not readable";
    }

    if (reply->isFinished()){
        qDebug() << "Synchroniser::get() reply is finished already!";
    }
}
