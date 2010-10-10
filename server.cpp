#include "server.h"
#include <QTcpSocket>
#include <QDataStream>
#include <QStringList>
#include <QDateTime>
#include <QUrl>
#include <output.h>
#include "httprequest.h"
#include "httpresponse.h"
#include "repotablemodel.h"
#include "filerepostatelogger.h"

Server::Server(RepoTableModel* model, QObject *parent)
    :QTcpServer(parent), repoTableModel(model)
{
    // need a function to be called on newConnection
    // it will read the connection
    // if it gets a GET request we will return a bit of fake html
    // later we will return files and indexes
    Output::debug("constructing a tcp server");
    m_socketsOpened = 0;
    m_socketsClosed = 0;
    m_requestsStarted = 0;
    m_requestsFinished = 0;
    m_responsesStarted = 0;
    m_responsesFinished = 0;
    connect(this, SIGNAL(newConnection()), this, SLOT(acceptConnection()));

}

void Server::printStatus(QString a)
{
    Output::info("Server Connection Status: "+a);
    Output::info("m_socketsOpened = "+QString::number(m_socketsOpened));
    Output::info("m_socketsClosed = "+QString::number(m_socketsClosed));
    Output::info("m_requestsStarted = "+QString::number(m_requestsStarted));
    Output::info("m_requestsFinished = "+QString::number(m_requestsFinished));
    Output::info("m_responsesStarted = "+QString::number(m_responsesStarted));
    Output::info("m_responsesFinished = "+QString::number(m_responsesFinished));
    Output::info("m_handledSockets.size() = "+QString::number(m_handledSockets.size()));
}

void Server::acceptConnection()
{
    printStatus("accept connection");
    if (this->hasPendingConnections()){
        m_socketsOpened++;

        QTcpSocket* socket = this->nextPendingConnection();
        connect(socket, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
        connect(socket, SIGNAL(aboutToClose()), this, SLOT(processAboutToClose()));
        connect(socket, SIGNAL(disconnected()), this, SLOT(processDisconnected()));
        connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(processError()));
    }

}

void Server::processDisconnected()
{
    m_socketsClosed++;
    QIODevice* socket = (QIODevice*) sender();
    if (m_handledSockets.contains(socket)){
        m_handledSockets.remove((QIODevice*) sender());
        Output::debug("socket disconnected and removed from m_handledSockets");
    }else{
        Output::debug("socket disconnected and was not in m_handledSockets");
    }
    printStatus("socket disconnected");
}

void Server::processAboutToClose()
{
    Output::debug("Server::processAboutToClose");
}

void Server::processError()
{
    QTcpSocket* socket = (QTcpSocket*) sender();
    Output::debug("Server::processError: "+socket->errorString());
}

void Server::processReadyRead()
{
    QTcpSocket* socket = (QTcpSocket*) sender();
    // we need to check that we can parse a complete header ..
    // using peek'ed data?
    // while canreadline
    // readlines.. and fail unless they pass regex
    // the last line needs to be crlf ..else its not valid

    if (m_handledSockets.contains(socket)){
        printStatus("processReadyRead - from socket which is being handled ");
    }else{
        // this is new socket connection
        m_handledSockets << (QIODevice*) socket;
        // the http request will call respondToRequest when it is ready
        m_requestsStarted++;
        printStatus("processReadyRead - new request (new or existing socket)");
        HttpRequest *request = new HttpRequest(this, socket);
    }

}

void Server::respondToRequest()
{
    HttpRequest* request = (HttpRequest*) sender();
    // create the HttpResponse object, and deal with it, and click send.
    m_responsesStarted++;
    printStatus("respondToRequest - new response started");
    HttpResponse* response = new HttpResponse(this, request, request->device());
    if (!request->isValid()){
        response->setResponseCode("400 Bad Request", "HTTP Request is invalid");
    }else{
        routeRequestToAction(request, response);
    }
    m_requestsFinished++;
    m_handledSockets.remove(request->device());
//    delete request;
    printStatus("respondToRequest - request finished and deleted");
    connect(response, SIGNAL(finished()), this, SLOT(responseFinished()));
    response->send();
}

void Server::responseFinished()
{
    HttpResponse* response = (HttpResponse*) sender();
    if (response->failed()){
        Output::debug("Server::responseFinished() with failure");
    }else{
        Output::debug("Server::responseFinished() successfully ");
    }
    // this socket may now be used to receive another request.
    if (response->protocol() == "HTTP/1.0"){
        response->destDevice()->close();
    }
    m_responsesFinished++;
    response->deleteLater();
//    delete response;
    printStatus("responseFinished - response finished and deleted");
}


void Server::routeRequestToAction(HttpRequest* request, HttpResponse* response){
    // simple routing
    /*
      GET       /reponame/file/fingerprint/reponame/filename             return file
      GET       /reponame/history/                       list commits alphabetically
      GET       /reponame/commit/commitname             return commit file content
      GET       /reponame/browse/dirname
      */

    QUrl uri(request->uri());
    QString uri_path = uri.path();
    if (uri_path == "/favicon.ico"){
        response->setResponseCode("404 Not Found");
        return;
    }

    QRegExp file_rx("/(\\w+)/(file)/(\\w{40})/([^?*:;{}\\\\]+)");
    QRegExp history_rx("/(\\w+)/(history)/?");
    QRegExp commit_rx("/(\\w+)/(commit)/?([^/?*;{}\\\\]*)");
    QRegExp browse_rx("/(\\w+)/(browse)/(.*)");
    QRegExp ping_rx("/(\\w+)/(ping)/(.*)");
    // dont forget to create a torrent action to return a bittorrent info file
    QList<QRegExp> routes;
    routes << file_rx << history_rx << commit_rx << browse_rx << ping_rx;
    bool routed_request = false;

    foreach(QRegExp route, routes){
        if (route.exactMatch(uri_path)){
            routed_request = true;
            QStringList tokens = route.capturedTexts();
            QString action = tokens.at(2);
            QString repo_name = tokens.at(1);
            if (FileRepo* repo = repoTableModel->repo(repo_name)){
                if (action == "file"){
                    QString fingerprint = tokens.at(3);
                    QString file_name = tokens.at(4);
                    actionFileRequest(response, repo, fingerprint, file_name);
                }else if (action == "history"){
                    actionHistoryRequest(response, repo);
                }else if (action == "commit"){
                    QString commit_name = tokens.at(3);
                    actionCommitRequest(response, repo, commit_name);
                }else if (action == "browse"){
                    QString dir_name("");
                    if (tokens.size() > 3) dir_name = tokens.at(3);
                    actionBrowseRequest(response, repo, dir_name);
                }else if (action == "ping"){
                    // return list of other active nodes for this repo?
                    repo->updateState();
                    response->setBody("pong");
                }else{
                    response->setResponseCode("500 Internal Server Error",
                                              "action is not being handled but route was matched");
                }
            }else{
                response->setResponseCode("404 Not Found",
                                          QByteArray("No repo found by name: ").append(repo_name));
            }
            // break the for loop.. we should only ever match one route
            break;
        }
    }
    if (!routed_request){
        // The request cannot be fulfilled due to bad syntax
        response->setResponseCode("400 Bad Request", "Could not route your request");
    }
}

void Server::actionFileRequest(HttpResponse* response, FileRepo* repo, QString fingerprint, QString file_name)
{
    QString repo_name = repo->name();
    if (repo->hasFileInfoByFingerPrint(fingerprint)){
        FileInfo* file_info = repo->fileInfoByFingerPrint(fingerprint);
        Output::debug("Server::actionFileRequest() "+repo->name()+"/"+file_name);
        QIODevice *file = repo->getFile(file_info);
        if (file->isOpen()){
            response->setContentType(file_info->mimeType());
            response->setContentLength(file_info->size());
            response->setContentDevice(file);
        }else{
            Output::debug(repo_name+" cant open response file:"+file_name);
            response->setResponseCode("503 Service Unavailable");
        }
    }else{
        Output::debug(repo_name+" file not found:"+file_name);
        response->setResponseCode("404 Not Found");
    }
}

void Server::actionHistoryRequest(HttpResponse* response, FileRepo* repo)
{
    response->setBody(repo->state()->logger()->logNames().join("\n").toUtf8());
}

void Server::actionCommitRequest(HttpResponse* response, FileRepo* repo, QString commit_name)
{
    if (repo->state()->logger()->hasLogFile(commit_name)){
        response->setBody(repo->state()->logger()->openLog(commit_name));
    }else{
        response->setResponseCode("404 Not Found");
    }
}

void Server::actionBrowseRequest(HttpResponse* response, FileRepo* repo, QString dir_name)
{
    QString repo_name = repo->name();
    QStringList dir_tokens = dir_name.split("/");
    QStringList path_tokens;
    path_tokens << repo_name;
    if (dir_tokens.at(0).length() != 0) path_tokens << dir_tokens;
    // add the title (with nav breadcrumb)
    QByteArray body;
    body.append(QString("<h1>%1</h1>\n").arg(browseBreadCrumb(path_tokens)));
    // add the list of subdirs
    QStringList sub_dirs = repo->state()->subDirs(dir_name);
    body.append(browseDirIndex(path_tokens, sub_dirs));
    // list the files in the directory
    QList<FileInfo*> matches = repo->state()->filesInDir(dir_name);
    body.append(browseFileIndex(repo_name, matches));
    response->setBody(body);
}

QString Server::browseDirIndex(QStringList path_dirs, QStringList sub_dirs)
{
    QString str;
    foreach(QString dir, sub_dirs){
        QStringList temp_dirs = path_dirs;
        temp_dirs << dir;
        str.append(linkToBrowse(temp_dirs)+"<br />\n");
    }

    return str;
}

QString Server::browseFileIndex(QString repo_name, QList<FileInfo*> fileInfos)
{
    QString table;
    foreach(FileInfo* f, fileInfos){
        QString link = linkToFile(repo_name, f);
        QString row = QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>\n").arg(link, f->humanSize(), f->mimeType());
        table.append(row);
    }
    return QString("<table>%1</table>").arg(table);
}

QString Server::browseBreadCrumb(QStringList dirs) const
{
    QStringList links;
    QStringList temp_dirs;
    foreach(QString dir, dirs){
        temp_dirs.append(dir);
        links << linkToBrowse(temp_dirs);
    }
    return links.join("/");
}

QString Server::linkToBrowse(QStringList tokens) const
{
    QString repo_name = tokens.takeFirst();
    QString name;
    if (tokens.isEmpty()){
        name = repo_name;
    }else{
        name = tokens.last();
    }
    QString file_path = tokens.join("/");
    QString str("<a href=\"/%1/browse/%2\">%3</a>");
    return str.arg(repo_name, file_path, name);
}

QString Server::linkToFile(QString repo_name, FileInfo* f)
{
    QString str("<a href=\"/%1/file/%2/%3\">%4</a>");
    return str.arg(repo_name, f->fingerPrint(), f->filePath(), f->fileName());
}
