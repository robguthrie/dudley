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
    Output::debug("accepting a connection");
    if (this->hasPendingConnections()){
        QTcpSocket* socket = this->nextPendingConnection();
        connect(socket, SIGNAL(readyRead()), this, SLOT(processReadyRead()));
        connect(socket, SIGNAL(disconnected()), socket, SLOT(deleteLater()));
    }
}

void Server::parseRequest(){
}
void Server::processReadyRead()
{
    QTcpSocket* socket = (QTcpSocket*)sender(); // sender returns the pointer to the SIGNAL emitter
    HttpRequest request = HttpRequest::fromStream(socket);
    HttpResponse response;
    QByteArray body;
    // simple routing
    /*
      GET       /reponame/file/fingerprint/reponame/filename             return file
      GET       /reponame/history/                       list commits alphabetically
      GET       /reponame/history/commitname             return commit file content
      GET       /reponame/browse/dirname
      */
    QUrl uri(request.uri());
    QString uri_path = uri.path();
    QRegExp has_file_rx("/(\\w+)/(hasfile)/(\\w{40})/([^?*:;{}\\\\]+)");
    QRegExp file_rx("/(\\w+)/(file)/(\\w{40})/([^?*:;{}\\\\]+)");
    QRegExp history_rx("/(\\w+)/(history)/?([^/?*;{}\\\\]*)");
    QRegExp browse_rx("/(\\w+)/(browse)/(.*)/?");
    QRegExp ping_rx("/(\\w+)/(ping)/(.+)");
    QList<QRegExp> routes;
    routes << file_rx << history_rx << browse_rx << ping_rx;
    bool routed_request = false;
    foreach(QRegExp route, routes){
        if (route.exactMatch(uri_path)){
            QStringList tokens = route.capturedTexts();
            QString action = tokens.at(2);
            QString repo_name = tokens.at(1);
            if (FileRepo* repo = repoTableModel->repo(repo_name)){
                if (action == "hasfile"){
                    QString fingerprint = tokens.at(3);
                    QString file_name = tokens.at(4);
                    if (repo->hasFileInfoByFingerPrint(fingerprint)){
                        body.append("yes");
                    }else{
                        response.setResponseCode("404 Not Found");
                    }
                }else if (action == "file"){
                    QString fingerprint = tokens.at(3);
                    QString file_name = tokens.at(4);
                    if (repo->hasFileInfoByFingerPrint(fingerprint)){
                        file_info = repo->fileInfoByFingerPrint(fingerprint);
                        QIODevice *file = repo->getFile(file_info);
                        if (file->open(QIODevice::ReadOnly)){
                            Output::debug("responding with file:"+fingerprint+file_name);
                            response.setContentType(file_info->mimeType());
                            body = file->readAll();
                        }else{
                            response.setResponseCode("503 Service Unavailable");
                        }
                    }else{
                        response.setResponseCode("404 Not Found");
                    }
                }else if (action == "history"){
                    if (tokens.size() == 3){
                        // url looks like repo_name/history return the list of log files
                        body = repo->state()->logger()->commitList().join("\n").toUtf8();
                    }else{
                        QString commit_name = tokens.at(3);
                        if (repo->state()->logger()->hasCommit(commit_name)){
                            body = repo->state()->logger()->readCommit(commit_name);
                        }else{
                            response.setResponseCode("404 Not Found");
                        }
                    }
                }else if (action == "browse"){
                    QString dir_name = tokens.at(3);
                    QStringList  dir_tokens = dir_name.split("/");
                    QString path_tokens;
                    path_tokens << repo_name << dir_tokens;
                    // add the title (with nav breadcrumb)
                    body.append(QString("<h1>%1</h1>\n").arg(browseBreadCrumb(path_tokens.join("/"))));
                    // add the list of subdirs
                    QStringList sub_dirs = repo->state()->subDirs(dir_name);
                    body.append(browseDirIndex(dir_tokens, sub_dirs));
                    // list the files in the directory
                    QList<FileInfo*> matches = repo->state()->filesInDir(dir_name);
                    body.append(browseFileIndex(repo_name, matches));
                }else if (action == "ping"){
                    // return list of other active nodes for this repo?
                    body.append("pong");
                }else{
                    response.setResponseCode("500 Internal Server Error");
                    response.setErrorMessage("action is not being handled but route was matched");
                }
            }else{
                response.setResponseCode("404 Not Found");
                response.setErrorMessage("No repo found by name: "+repo_name);
            }
            routed_request = true;
            break;
        }
    }
    if (!routed_request){
        // The request cannot be fulfilled due to bad syntax
        response.setResponseCode("400 Bad Request");
    }

//    // return an html document with links to files!
//    if (action == "browse" && file_name.length() == 0){
//        QStringList repo_names = repoTableModel->repoNames();
//        body.append("<h1>Dudley</h1>\n");
//        foreach(QString repo_name, repo_names){
//            body.append(linkToBrowse(repo_name)+"<br />\n");
//        }
//    }

    QDataStream os(socket);
    response.setContentLength(body.size());
    QByteArray header = response.header();
    os.writeRawData(header, header.length());
    os.writeRawData(body, body.length());
    // socket->waitForBytesWritten();
    socket->close();
}

QString Server::browseDirIndex(QStringList path_dirs, QStringList sub_dirs)
{
    QString str;
    foreach(QString dir, sub_dirs){
        QStringList temp_dirs = path_dirs;
        temp_dirs << dir;
        str.append(linkToBrowse(temp_dirs.join("/"))+"<br />\n");
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

QString Server::browseBreadCrumb(QString dir_name) const
{
    QStringList dirs = dir_name.split('/');
    QString str;
    QStringList temp_dirs;
    foreach(QString dir, dirs){
        temp_dirs.append(dir);
        str.append("/"+linkToBrowse(temp_dirs.join("/")));
    }
    return str;
}

QString Server::linkToBrowse(QString dir_name) const
{
    QStringList tokens = dir_name.split("/");
    QString repo_name = tokens.takeFirst();
    QString name = tokens.last();
    QString file_path = tokens.join("/");
    QString str("<a href=\"/%1/browse/%2\">%3</a>");
    return str.arg(repo_name, file_path, name);
}

QString Server::linkToFile(QString repo_name, FileInfo* f)
{
    QString str("<a href=\"/%1/file/%2/%3\">%4</a>");
    return str.arg(f->fingerPrint(), repo_name, f->filePath(), f->fileName());
}
