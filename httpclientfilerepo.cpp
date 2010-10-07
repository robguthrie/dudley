#include "httpclientfilerepo.h"
#include <QDesktopServices>
#include <QTextStream>
#include "output.h"
#include "filerepostate.h"
#include "filerepostatelogger.h"

HttpClientFileRepo::HttpClientFileRepo(QObject *parent, QString path, QString name)
    :FileRepo(parent, path, name)
{
    Output::debug("constructed HttpClientFileRepo on path: "+path);
    m_log_path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    m_log_path.append(QString("/logs/%1/%2").arg(this->type(), this->name()));
    Output::debug("logs data location:"+m_log_path);
    m_state = new FileRepoState(this, m_log_path);

    // setup a recurring timer to ping the server and update the m_canReadData bool
    // read the pong queue and check that we have had a pong (ping reply) to our last ping?

    m_manager = new QNetworkAccessManager(this);
    connect(m_manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
}

QString HttpClientFileRepo::type()
{
    return QString("HttpClientFileRepo");
}

bool HttpClientFileRepo::canReadData() const
{
    // check that m_lastPingTime is within 10 Minutes
    return true;
}

void HttpClientFileRepo::ping()
{
    // set last ping time
    m_lastPingTime = QDateTime::currentDateTime();
    this->get(urlFor(QString("ping")));
}

void HttpClientFileRepo::updateState()
{
    this->get(urlFor(QString("history")));
}

QIODevice* HttpClientFileRepo::getFile(FileInfo* fileInfo)
{
    Output::debug("HttpClientFileRepo::getFile("+fileInfo->fileName()+")");
    QNetworkReply* reply = m_manager->get(QNetworkRequest(fileUrl(fileInfo)));
    connect(reply, SIGNAL(readyRead()), this, SLOT(alertReadyRead()));
    connect(reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(alertDownloadProgress(qint64, qint64)));
    if (reply->isOpen()){
        Output::debug("HttpClientFileRepo::getFile reply is open");
    }else{
        Output::debug("HttpClientFileRepo::getFile reply is not open");
    }

    if (reply->error()){
        Output::debug("ffff replyerror: "+QString::number(reply->error())+" "+reply->errorString());
    }

    if (reply->isReadable()){
        Output::debug("hcfr: reply is readable");
    }else{
        Output::debug("hcfr: reply is not readable");
    }

    if (reply->isRunning()){
        Output::debug("hcfr: reply is running");
    }
    if (reply->isFinished()){
        Output::debug("hcfr: reply is finished");
    }
    return reply;
}



void HttpClientFileRepo::alertReadyRead()
{
    QNetworkReply* reply = (QNetworkReply*) sender();
    Output::debug("HttpClientFileRepo::alertReadyRead() url:"+reply->url().toString()+" bytes:"+reply->bytesAvailable());
}
void HttpClientFileRepo::alertDownloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    QNetworkReply* reply = (QNetworkReply*) sender();
    Output::debug("HttpClientFileRepo::alertDownloadProgress() url:"+reply->url().toString());
    Output::debug(QString("HttpClientFileRepo::alertDownloadProgress() bytes received: %1 bytes total: %2 ").arg(QString::number(bytesReceived), QString::number(bytesTotal)));
}
// do an http get and return the QNetworkReply (which is a QIODevice i believe)
QIODevice* HttpClientFileRepo::get(QUrl url)
{
    Output::debug("httpclientfilerepo get:"+url.toString());
    return m_manager->get(QNetworkRequest(url));
}

void HttpClientFileRepo::requestFinished(QNetworkReply* reply)
{
    // handle replies of commits, commitlists and files and pings
    //check what the requet was for.. then process it
    QString request_url = reply->url().toString();
    QString regexstr = "("+QRegExp::escape(m_path)+")/(\\w+)/?(.*)";
    QRegExp rx(regexstr);

    if (reply->error()){
        Output::error("requestFinised reply error: "+QString::number(reply->error())+" "+reply->errorString());
        QList<QByteArray> headers = reply->rawHeaderList();
        foreach(QByteArray h, headers){
            Output::error(QTextStream(h).readAll());
        }
        // is it readable?
        // what teh fuckkk?
    }
    // need to check the response code.. 200?
    if (rx.exactMatch(request_url)){
        QStringList tokens = rx.capturedTexts();
        QString repo_url = tokens.at(1);
        QString action = tokens.at(2);
        QByteArray body = reply->readAll();
        QString bodystr = QTextStream(body).readAll();
        if (action == "ping"){
            // check body says pong
            // add the pong and its time to a limited length queue
            if (body == "pong"){
                m_lastPingTime = QDateTime::currentDateTime();
            }
        }else if (action == "file"){
            QString filename = tokens.at(3);
            Output::debug(QString("HttpClientFileRepo::requestFinsihed()  reply->bytesAvailable: %1").arg(reply->bytesAvailable()));
            Output::debug(QString("HttpClientFileRepo::requestFinished() file %1 size: %2").arg(filename, QString::number(reply->size())));
            // files will be written as bytes are available..
            // might not need to handle anything here
            // maybe update the filetransfer object by emitting a signal?

            reply->close();
        }else if (action == "history"){
            QStringList commit_list = bodystr.split("\n");
            foreach(QString commit_name, commit_list){
                if (!m_state->logger()->hasLogFile(commit_name)){
                    this->get(urlFor(QString("commit"), commit_name));
                }else{
                    Output::debug("dont need logfile:"+commit_name);
                }
            }
        }else if (action == "commit"){
            // this is a commit log..
            QString name = tokens.at(3);
            this->state()->importLog(name, body);
        }else{
            Output::error("action not recognised: "+action);
        }
    }else{
        Output::error("could not parse request url: "+request_url);
    }
}

QUrl HttpClientFileRepo::fileUrl(FileInfo* fileInfo){
    return urlFor(QString("file"), fileInfo->fingerPrint(), fileInfo->filePath());
}

QUrl HttpClientFileRepo::urlFor(QString a, QString b, QString c, QString d)
{
    QStringList tokens;
    tokens << m_path;
    if (a.length() > 0) tokens << a;
    if (b.length() > 0) tokens << b;
    if (c.length() > 0) tokens << c;
    if (d.length() > 0) tokens << d;
    // might need to add authentication in here
    return QUrl(tokens.join("/"));
}

QString HttpClientFileRepo::relativeFilePath(QString filePath){
    if (filePath.startsWith(m_path+'/')){
        return filePath.remove(0, m_path.length()+1);
    }else{
        return filePath;
    }
}


