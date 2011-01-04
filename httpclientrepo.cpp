#include "httpclientrepo.h"
#include <QDesktopServices>
#include <QTextStream>
#include "output.h"
#include "state.h"
#include "statelogger.h"
#include <QBuffer>

HttpClientRepo::HttpClientRepo(QObject *parent, QString path, QString name)
    :Repo(parent, path, name)
{
    g_log->debug("constructed HttpClientRepo on path: "+path);
    m_pendingLogDownloads = 0;
    m_log_path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    m_log_path.append(QString("/logs/%1/%2").arg(this->type(), this->name()));
    g_log->debug("logs data location:"+m_log_path);
    m_logger = new StateLogger(this, m_log_path);

    // path is something like http://localhost:54573/music
    // where the node is serving the repo "music" at http://localhost:54573
    QRegExp valid_repo_url("^(http://[^/]+)/(\\w+)/?$");
    if (valid_repo_url.exactMatch(path)){
        m_host_url = valid_repo_url.cap(1);
        m_host_repo_name = valid_repo_url.cap(2);
    }else{
        g_log->error("invalid repo url: "+path);
    }

    // setup a recurring timer to ping the server and update the m_canReadData bool
    // read the pong queue and check that we have had a pong (ping reply) to our last ping?

    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
}

QString HttpClientRepo::type() const
{
    return QString("HttpClient");
}

bool HttpClientRepo::canReadData() const
{
    // check that m_lastPingTime is within 10 Minutes
    return true;
}

void HttpClientRepo::ping()
{
    // set last ping time
    m_lastPingTime = QDateTime::currentDateTime();
    this->get(urlFor(QString("ping")));
}

void HttpClientRepo::updateState(bool commit_changes)
{
    m_commitChanges = commit_changes;
    this->get(urlFor(QString("history"), m_host_repo_name));
}

bool HttpClientRepo::fileExists(FileInfo* file_info) const
{
    return m_state->containsFileInfo(file_info);
}

QIODevice* HttpClientRepo::getFile(FileInfo* fileInfo)
{
    return this->get(fileUrl(fileInfo));
}

// do an http get and return the QNetworkReply (which is a QIODevice i believe)
QIODevice* HttpClientRepo::get(QUrl url)
{
    g_log->debug("HttpClientRepo get:"+url.toString());
    QString url_str = url.toString();
    QNetworkReply* reply = m_manager->get(QNetworkRequest(url));
    if (!reply->isOpen()) g_log->debug("HttpClientRepo::get("+url_str+") reply is not open");
    if (reply->error()) g_log->debug("HttpClientRepo::get("+url_str+") reply error: "+QString::number(reply->error())+" "+reply->errorString());
    if (!reply->isReadable()) g_log->debug("HttpClientRepo::get("+url_str+") reply is not readable");
    if (reply->isFinished()) g_log->debug("HttpClientRepo::get("+url_str+") reply is finished already!");
    return reply;
}

QIODevice* HttpClientRepo::putFile(QString file_name)
{
    return new QBuffer();
}

void HttpClientRepo::putFileComplete(QIODevice *device, QString file_path)
{
    QUrl url = urlFor("upload", file_path);
    QNetworkRequest r(url);
    m_manager->post(r, device);
    g_log->debug("HttpClientRepo::putFileFinished beginning upload of:"+file_path);
}

void HttpClientRepo::putFileFailed(QIODevice *device)
{
    if (!device) device = (QBuffer*) sender();
    device->deleteLater();
}

QString HttpClientRepo::temporaryFilePath(FileInfo* fileInfo)
{
    QString path = QDesktopServices::storageLocation(QDesktopServices::CacheLocation);
    return path+"/"+fileInfo->fingerPrint()+fileInfo->fileName();
}

void HttpClientRepo::requestFinished(QNetworkReply* reply)
{
    if (reply->error()){

        g_log->error("requestFinised reply error: "+QString::number(reply->error())+" "+reply->errorString());
        QList<QByteArray> headers = reply->rawHeaderList();
        foreach(QByteArray h, headers){
            g_log->error(QTextStream(h).readAll());
        }
        return;
    }else{
        g_log->debug("requestFinised successfully: "+reply->url().toString());
    }

    // handle replies of commits, commitlists and files and pings
    //check what the requet was for.. then process it
    QString request_url = reply->url().toString();
    QRegExp rx(QRegExp::escape(m_host_url)+"/(file|history|commit|ping)/?(\\w+)?/?(.*)");

    // need to check the response code.. 200?
    if (rx.exactMatch(request_url)){
        QStringList tokens = rx.capturedTexts();
        QString action;
        QString repo_name;
        QString file_path;
        if (tokens.size() > 1) action = tokens.at(1);
        if (tokens.size() > 2) repo_name = tokens.at(2);
        if (tokens.size() > 3) file_path = tokens.at(3);
        if (file_path.startsWith("/")) file_path.remove(0, 1);
        QByteArray body = reply->readAll();
        QString bodystr = QTextStream(body).readAll();
        if ((action == "ping") && (body == "pong")){
            // add the pong and its time to a limited length queue
            m_lastPingTime = QDateTime::currentDateTime();
        }else if (action == "file"){
            g_log->debug(QString("HttpClientRepo::requestFinished() file %1 size: %2").arg(file_path, QString::number(reply->size())));
            // files will be written as bytes are received.. so file should be written by now
        }else if (action == "history"){
            QStringList commit_list = bodystr.trimmed().split("\n");
            // http requests could be returned might arrive out of order
            // and log files must be played in order
            // so we wait until all the commits have been downloaded until we
            // reload the state
            g_log->debug(bodystr);
            m_pendingLogDownloads = 0;
            foreach(QString commit_name, commit_list){
                if (!m_logger->hasLogFile(commit_name)){
                    m_pendingLogDownloads++;
                    this->get(urlFor("commit", m_host_repo_name, commit_name));
                }
            }
        }else if (action == "commit"){
            // this is a commit log..
            m_logger->writeLogFile(file_path, body);
            m_pendingLogDownloads--;
            if (m_pendingLogDownloads == 0){
                // we can reload the state now..
                m_logger->reload();
            }
        }else if (action == "upload"){
            g_log->debug("upload request finished");
        }else{
            g_log->error("action not recognised: "+action);
        }
    }else{
        g_log->error("could not parse request url: "+request_url);
    }
}

QUrl HttpClientRepo::fileUrl(FileInfo* fileInfo){
    return urlFor(QString("file"), m_host_repo_name, fileInfo->fingerPrint(), fileInfo->filePath());
}

QUrl HttpClientRepo::urlFor(QString a, QString b, QString c, QString d)
{
    QStringList tokens;
    tokens << m_host_url;
    if (a.length() > 0) tokens << a;
    if (b.length() > 0) tokens << b;
    if (c.length() > 0) tokens << c;
    if (d.length() > 0) tokens << d;
    // might need to add authentication in here
    g_log->debug("request url:"+tokens.join("/"));
    return QUrl(tokens.join("/"));
}

QString HttpClientRepo::relativeFilePath(QString filePath){
    if (filePath.startsWith(m_path+'/')){
        return filePath.remove(0, m_path.length()+1);
    }else{
        return filePath;
    }
}
