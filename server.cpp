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
    connect(this, SIGNAL(newConnection()), this, SLOT(acceptConnection()));
}

void Server::acceptConnection()
{
    if (this->hasPendingConnections()){
        QTcpSocket* socket = this->nextPendingConnection();
        connect(socket, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
        connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    }
}

void Server::processReadyRead()
{
    QTcpSocket* socket = (QTcpSocket*)sender(); // sender returns the pointer to the SIGNAL emitter
    HttpRequest request = HttpRequest::fromStream(socket);
    HttpResponse* response = new HttpResponse(socket);

    // simple routing
    /*
      GET       /reponame/file/fingerprint/reponame/filename             return file
      GET       /reponame/history/                       list commits alphabetically
      GET       /reponame/commit/commitname             return commit file content
      GET       /reponame/browse/dirname
      */

    QUrl uri(request.uri());
    QString uri_path = uri.path();
    Output::debug("HTTP REQUEST for: "+uri_path);
    QRegExp file_rx("/(\\w+)/(file)/(\\w{40})/([^?*:;{}\\\\]+)");
    QRegExp history_rx("/(\\w+)/(history)/?");
    QRegExp commit_rx("/(\\w+)/(commit)/?([^/?*;{}\\\\]*)");
    QRegExp browse_rx("/(\\w+)/(browse)/(.*)");
    QRegExp ping_rx("/(\\w+)/(ping)/(.*)");
    // dont forget to create a torrent action to return a bittorrent info file
    QList<QRegExp> routes;
    routes << file_rx << history_rx << commit_rx << browse_rx << ping_rx;
    bool routed_request = false;
    bool response_ready = true;
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
                    Output::debug(repo_name+": file request: "+file_name);
                    if (repo->hasFileInfoByFingerPrint(fingerprint)){
                        FileInfo* file_info = repo->fileInfoByFingerPrint(fingerprint);
                        QIODevice *file = repo->getFile(file_info);
                        if (file->isOpen()){
                            Output::debug(repo_name+": file is already open"+file_info->fileName());
                        }else{
                            Output::debug(repo_name+": opening file: "+file_info->fileName());
                            file->open(QIODevice::ReadOnly);
                            if (!file->isOpen()){
                                Output::debug(repo_name+" cant open response file:"+file_name);
                                response->setResponseCode("503 Service Unavailable");
                            }
                        }

                        if (file->isReadable()) Output::debug("server: file is readable");
                        if (file->atEnd()) Output::debug("server: file is at end");
                        Output::debug("bytes available: "+QString::number(file->bytesAvailable()));

                        if (response->body.size() < file_info->size()){
                            // still more bytes to come according to the file_info
                            response_ready = false;
                            connect(file, SIGNAL(readyRead()), response, SLOT(bodyBytesAvailable()));
                        }else{
                            response->body = file->readAll();
                        }
                    }else{
                        Output::debug(repo_name+" file not found:"+file_name);
                        response->setResponseCode("404 Not Found");
                    }
                }else if (action == "history"){
                    // url looks like repo_name/history return the list of log files
                    response->body = repo->state()->logger()->logNames().join("\n").toUtf8();
                }else if (action == "commit"){
                    QString commit_name = tokens.at(3);
                    if (repo->state()->logger()->hasLogFile(commit_name)){
                        response->body = repo->state()->logger()->openLog(commit_name);
                    }else{
                        response->setResponseCode("404 Not Found");
                    }
                }else if (action == "browse"){
                    QString dir_name("");
                    if (tokens.size() > 3){
                        dir_name = tokens.at(3);
                    }
                    QStringList dir_tokens = dir_name.split("/");
                    QStringList path_tokens;
                    path_tokens << repo_name;
                    if (dir_tokens.at(0).length() != 0) path_tokens << dir_tokens;
                    // add the title (with nav breadcrumb)
                    Output::debug("got here2 path_tokens:"+ path_tokens.join(","));
                    response->body.append(QString("<h1>%1</h1>\n").arg(browseBreadCrumb(path_tokens)));
                    // add the list of subdirs
                    QStringList sub_dirs = repo->state()->subDirs(dir_name);
                    response->body.append(browseDirIndex(path_tokens, sub_dirs));
                    // list the files in the directory
                    Output::debug("files in dir, dir_name:"+dir_name);
                    QList<FileInfo*> matches = repo->state()->filesInDir(dir_name);
                    response->body.append(browseFileIndex(repo_name, matches));
                }else if (action == "ping"){
                    // return list of other active nodes for this repo?
                    repo->updateState();
                    response->body.append("pong");
                }else{
                    response->setResponseCode("500 Internal Server Error");
                    response->setErrorMessage("action is not being handled but route was matched");
                }
            }else{
                response->setResponseCode("404 Not Found");
                response->setErrorMessage("No repo found by name: "+repo_name);
            }

            break;
        }
    }
    if (!routed_request){
        // The request cannot be fulfilled due to bad syntax
        response->setResponseCode("400 Bad Request");
    }

    //send response here
    if (response_ready) response->send();
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
        QString row = QString("<tr><td>%1</td><td>%2</td></tr>\n").arg(link, f->humanSize());
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
    Output::debug("linkToBrowse:"+tokens.join(","));
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
