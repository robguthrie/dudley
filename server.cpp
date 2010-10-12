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

Server::Server(RepoModel* model, QObject *parent)
    :QTcpServer(parent), repoTableModel(model)
{
    // need a function to be called on newConnection
    // it will read the connection
    // if it gets a GET request we will return a bit of fake html
    // later we will return files and indexes
    //    Output::debug("constructing a tcp server");
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
//    printStatus("socket disconnected");
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
//        printStatus("processReadyRead - from socket which is being handled ");
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
//    printStatus("respondToRequest - new response started");
    HttpResponse* response = new HttpResponse(this, request, request->device());
    if (!request->isValid()){
        response->setResponseCode("400 Bad Request", "HTTP Request is invalid");
    }else{
        // at this point we should authorize them
        // then send the 100-continue
        request->accept();
        routeRequestToAction(request, response);
    }
    m_requestsFinished++;
    m_handledSockets.remove(request->device());
//    printStatus("respondToRequest - request finished and deleted");
    connect(response, SIGNAL(finished()), this, SLOT(responseFinished()));
    response->send();
}

void Server::responseFinished()
{
    HttpResponse* response = (HttpResponse*) sender();
    if (response->failed()){
        Output::debug("Server::responseFinished() with failure");
    }else{
//        Output::debug("Server::responseFinished() successfully ");
    }
    // this socket may now be used to receive another request.
    if (response->protocol() == "HTTP/1.0"){
        response->destDevice()->close();
    }
    m_responsesFinished++;
    response->deleteLater();
//    delete response;
    printStatus("Server::responseFinished");
}


void Server::routeRequestToAction(HttpRequest* request, HttpResponse* response){
    QUrl uri(request->uri());
    QString uri_path = uri.path();

    QMap<QString, QRegExp> routes;
    routes["favicon"] = QRegExp("/favicon.ico");
    routes["file"] = QRegExp("/(file)/(\\w+)/(\\w{40})/([^?*:;{}\\\\]+)");
    routes["history"] = QRegExp("/(history)/(\\w+)/?");
    routes["commit"] = QRegExp("/(commit)/(\\w+)/([^/?*;{}\\\\]*)");
    routes["browse"] = QRegExp ("/(browse)/(\\w+)(/.*)?");
    routes["ping"] = QRegExp ("/(ping)/(\\w+)(/.*)?");
    // naked is an overloaded actionless route for each repo
    // it should give the most normal looking urls for your files
    QStringList repo_names = repoTableModel->repoNames();
    routes["naked"] = QRegExp("/("+(repo_names.join("|"))+")(/.*)?");
    // dont forget to create a torrent action to return a bittorrent info file
    QMap<QString, QRegExp>::const_iterator i;
    QString key;
    QRegExp regex;
    bool routed_request = false;
    for (i = routes.begin(); i != routes.end(); ++i){
        key = i.key();
        regex = i.value();
        if(regex.exactMatch(uri_path)){
            routed_request = true;
            Output::debug("matched "+key+" route");
            if (key == "favicon"){
                actionFaviconRequest(response);
            }else if (key == "file"){
                QString repo_name = regex.cap(2);
                QString fingerprint = regex.cap(3);
                actionFileRequestByFingerprint(response, repo_name, fingerprint);
            }else if (key == "history"){
                QString repo_name = regex.cap(2);
                actionHistoryRequest(response, repo_name);
            }else if (key == "commit"){
                QString repo_name = regex.cap(2);
                QString commit_name = regex.cap(3);
                actionCommitRequest(response, repo_name, commit_name);
            }else if (key == "browse"){
                QString repo_name = regex.cap(2);
                QString dir_name("");
                if (regex.captureCount() == 3) dir_name = regex.cap(3);
                actionBrowseRequest(response, repo_name, dir_name);
            }else if (key == "naked"){
                QString repo_name = regex.cap(1);
                QString dir_name("");
                if (regex.captureCount() == 2) dir_name = regex.cap(2);
                if (dir_name.startsWith("/")) dir_name = dir_name.remove(0,1);
                if (actionFileRequestByFileName(response, repo_name, dir_name)){
                    // supposed to be blank (it's in the condition)
                }else{
                    // repo_name cant be wrong due to way i create the regex
                    actionBrowseRequest(response, repo_name, dir_name);
                }
            }else if (key == "ping"){
                response->setBody("pong");
            }else{
                response->setResponseCode("500 Internal Server Error",
                                          "action is not being handled but route was matched");
            }
            // break the for loop.. we should only ever match one route
            break;
        } // end if matched route
    }// end for each in routes

    if (!routed_request){
        // The request cannot be fulfilled due to bad syntax
        response->setResponseCode("400 Bad Request", "Could not route your request");
    }
}

void Server::actionFaviconRequest(HttpResponse* response)
{
    QFile *f  = new QFile(":/icons/dino1.png");
    if(f->open(QIODevice::ReadOnly)){
        response->setMaxAge(60*60*24);
        response->setResponseCode("200 OK");
        response->setContentType("image/png");
        response->setContentLength(f->size());
        response->setContentDevice(f);
    }else{
        response->setResponseCode("404 Not Found", "could not open the favicon file");
    }
}

bool Server::actionFileRequestByFileName(HttpResponse* response, QString repo_name, QString file_path)
{
    Output::debug("actionfilerequestbyfilename:"+repo_name+" "+file_path);
    if (FileRepo* repo = repoTableModel->repo(repo_name)){
        if (repo->hasFileInfoByFilePath(file_path)){
            FileInfo* file_info = repo->fileInfoByFilePath(file_path);
            setFileResponse(response, repo, file_info);
            return true;
        }else{
            response->setResponseCode("404 Not Found");
        }
    }else{
        response->setResponseCode("500 Internal Server Error", "Could not open repo");
    }
    return false;
}


bool Server::actionFileRequestByFingerprint(HttpResponse* response, QString repo_name, QString fingerprint)
{
    if (FileRepo* repo = repoTableModel->repo(repo_name)){
        if (repo->hasFileInfoByFingerPrint(fingerprint)){
            FileInfo* file_info = repo->fileInfoByFingerPrint(fingerprint);
            setFileResponse(response, repo, file_info);
            return true;
        }else{
            response->setResponseCode("404 Not Found");
        }
    }else{
        response->setResponseCode("500 Internal Server Error", "Could not open repo");
    }
    return false;
}

void Server::setFileResponse(HttpResponse* response, FileRepo* repo, FileInfo* file_info)
{
    Output::debug("about to call getFile("+file_info->fileName()+") on repo:"+repo->name());
    QIODevice *file = repo->getFile(file_info);
    Output::debug("called getFile("+file_info->fileName()+") on repo:"+repo->name());
    response->setLastModified(file_info->lastModified());
    response->setCacheNeverExpires();
    response->setContentType(file_info->mimeType());
    response->setContentLength(file_info->size());
    response->setContentDevice(file);
    Output::debug("set content device on the response for: "+response->request()->uri());
}

void Server::actionHistoryRequest(HttpResponse* response, QString repo_name)
{
    if (FileRepo* repo = repoTableModel->repo(repo_name)){
        response->setResponseCode("200 OK");
        response->setBody(repo->state()->logger()->logNames().join("\n").toUtf8());
    }else{
        response->setResponseCode("500 Internal Server Error", "Could not open repo");
    }
}

void Server::actionCommitRequest(HttpResponse* response, QString repo_name, QString commit_name)
{
    if (FileRepo* repo = repoTableModel->repo(repo_name)){
        if (repo->state()->logger()->hasLogFile(commit_name)){
            response->setResponseCode("200 OK");
            response->setBody(repo->state()->logger()->openLog(commit_name));
        }else{
            response->setResponseCode("404 Not Found");
        }
    }else{
        response->setResponseCode("500 Internal Server Error", "Could not open repo");
    }
}

void Server::actionBrowseRequest(HttpResponse* response, QString repo_name, QString dir_name)
{
    if (FileRepo* repo = repoTableModel->repo(repo_name)){
        if (dir_name.startsWith("/")) dir_name = dir_name.remove(0,1);
        Output::debug("browse repo_name:"+repo_name+" dir_name: "+dir_name);
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
        response->setResponseCode("200 OK");
        response->setBody(body);
        response->setMaxAge(60*15);
    }else{
        response->setResponseCode("500 Internal Server Error", "Could not open repo");
    }

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
    QString str("<a href=\"/%1/%2\">%3</a>");
    return str.arg(repo_name, file_path, name);
}

QString Server::linkToFile(QString repo_name, FileInfo* f)
{
    QString str("<a href=\"/%1/%2\">%4</a>");
    return str.arg(repo_name, f->filePath(), f->fileName());
}
