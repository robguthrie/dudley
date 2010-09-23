#include "httpclientfilerepo.h"
#include <QDesktopServices>
#include "output.h"

HttpClientFileRepo::HttpClientFileRepo(QObject *parent, QString path, QString name)
    :FileRepo(parent, path, name)
{
    Output::debug("constructed HttpClientFileRepo on path: "+path);
    m_log_path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    m_log_path.append(QString("/logs/%1/%2/").arg(this->type(), this->name()));
    Output::debug("logs data location:"+m_log_path);
    m_state = new FileRepoState(this, m_log_path);

    QNetworkAccessManager *m_manager = new QNetworkAccessManager(this);
    connect(&networkManager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(handleNetworkData(QNetworkReply*)));
}

bool HttpClientFileRepo::canReadData() const
{
    // test reading the repo..
    // do a repo ping
    return false;
}

QString HttpClientFileRepo::type()
{
    return QString("HttpClientFileRepo");
}

void HttpClientFileRepo::updateState()
{
    // request state updates from host and rebuild our state
    // get a list of all the commits on thehost
    // find all missing commit names
    // download each file and write it to our logger
    // reload the logger
    QNetworkRequest request(historyUrl());
    QNetworkReply* reply = m_manager.get(request);
    QString body(reply->readAll());
    QStringList commit_list = body.split("\n");
    QStringList new_commits;
    foreach(QString commit_name, commit_list){
        if (!m_state->hasCommit(commit_name)){
            // we need this one
            new_commits << commit_name;

        }
    }

}
void HttpClientFileRepo::getCommit(QString commit_name)
{
    QNetworkRequest request(historyUrl(commit_name));
    QNetworkReply* reply = m_manager.get(request);
    // should really connect a finished signal to this?
    // How about: download metadata (commits and lists) in the same way as the downloadmanager
    // adding to a queue..
    // and do the reading (into a qtextstream) as it happens.
    // need to read the data as utf8 into a string..
    QByteArray body = reply->readAll();
}

bool HttpClientFileRepo::hasFile(FileInfo fileInfo) const
{
    QNetworkRequest request(fileUrl(fileInfo));
    QNetworkReply* reply = m_manager.get(request);
    QByteArray body = reply->readAll();
    return (body == "yes");
}

QIODevice* HttpClientFileRepo::getFile(FileInfo* fileInfo) const
{
    // do an http get and return the QNetworkReply (which is a QIODevice)
    QNetworkRequest request(fileUrl(fileInfo));
    QNetworkReply* reply = m_manager.get(request);
    // here we can read any headers from the reply..
    // do things like check that it went alright..
    return reply;
}

QString HttpClientFileRepo::relativeFilePath(QString filePath){
    if (filePath.startsWith(m_path+'/')){
        return filePath.remove(0, m_path.length()+1);
    }else{
        return filePath;
    }
}

QUrl HttpClientFileRepo::fileUrl(FileInfo* fileInfo){
    QStringList tokens;
    tokens << m_path << fileInfo->fingerPrint() << fileInfo->filePath();
    return QUrl(tokens.join("/"));
}

QUrl HttpClientFileRepo::historyUrl()
{
    return QUrl(m_path+"/history");
}

QUrl HttpClientFileRepo::historyUrl(QString commit_name)
{
    return QUrl(m_path+"/history/"+commit_name);
}
