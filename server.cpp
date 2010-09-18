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

void Server::processReadyRead()
{
    QTcpSocket* socket = (QTcpSocket*)sender(); // sender returns the pointer to the SIGNAL emitter
    HttpRequest request = HttpRequest::fromStream(socket);
    HttpResponse response;
    QByteArray body;
    // simple routing
    //process url -
    /*

      http://host:52523/fingerprint/reponame
      new routing plan
      GET       /file/fingerprint/reponame/filename             return file
      GET       /file/0b1db43bb7f1024d6433d7b5fc1ceda255fb6104/pics/holiday/on_a_boat.jpg
      GET       /history/reponame/                       list commits alphabetically
      GET       /history/reponame/commitname             return commit file content
      GET       /browse/reponame/begins_with_filename
      POST      /history/reponame/nodename/commitname    save commit under node name
      GET       /

      index request (browsable)
      /reponame only

      */
    QUrl uri(request.uri());
    QString uri_path = uri.path();
    // match just an action
    QRegExp action_rx = QRegExp("/(file|history|browse)/(.*)");
    // action, fingerprint, reponame, filename
    // eg: file/0b1db43bb7f1024d6433d7b5fc1ceda255fb6104/pics/holiday/on_a_boat.jpg
    QRegExp file_request_rx = QRegExp("/(file)/(\\w{40})/(\\w+)/([^?*:;{}\\\\]+)");

    // match action: history, repo_name, commit_name
    QRegExp history_file_request_rx("/(history)/(\\w+)/([^/?*;{}\\\\]+)");

    // match history, repo_name
    QRegExp history_list_request_rx("/(history)/(\\w+)/?");

    // action: browse, repo, path prefix
    QRegExp browse_repo_request_rx = QRegExp("/(browse)/(.*)");
    QStringList tokens;
    QString action;
    QString fingerprint;
    QString repo_name;
    QString file_name;
    FileInfo* file_info;

    if (file_request_rx.exactMatch(uri_path)){
        Output::debug("matched file request");
        tokens = file_request_rx.capturedTexts();
        action = tokens.at(1);
        fingerprint = tokens.at(2);
        repo_name = tokens.at(3);
        file_name = tokens.at(4);
    }else if(history_file_request_rx.exactMatch(uri_path)){
        Output::debug("matched history file request");
        tokens = history_file_request_rx.capturedTexts();
        action = tokens.at(1);
        repo_name = tokens.at(2);
        file_name = tokens.at(3);
    }else if(history_list_request_rx.exactMatch(uri_path)){
        Output::debug("matched history list request");
        tokens = history_list_request_rx.capturedTexts();
        action = tokens.at(1);
        repo_name = tokens.at(2);
    }else if(browse_repo_request_rx.exactMatch(uri_path)){
        Output::debug("matched browse request");
        tokens = browse_repo_request_rx.capturedTexts();
        action = tokens.at(1);
        file_name = tokens.at(2);
        repo_name = file_name.split("/").first();
    }else{
        // unable to handle  it..
        // set the response to a fail.
        Output::debug("failed to match request:"+uri_path);
        body.append(QString("failed to match request:"+uri_path));
    }
    Output::debug(tokens.join(","));
    // get the repo instance by name..
    if (FileRepo* repo = repoTableModel->repo(repo_name)){
        if (action == "file"){
            if (repo->hasFileInfoByFingerPrint(fingerprint)){
                file_info = repo->fileInfoByFingerPrint(fingerprint);
                QIODevice *file = repo->getFile(file_info);
                if (file->open(QIODevice::ReadOnly)){
                    Output::debug("have result! - opened file");
                    response.setContentType(file_info->mimeType());
                    body = file->readAll();
                }else{
                    Output::debug("have result! - but could not open file");
                    // RESPONSE 500 server error could not open file
                }
            }else{
                Output::debug("fileinfo not found in repo");
                // 404
            }
        }else if (action == "history"){
            if (file_name.length() > 0){
                // we want to return a commit log file
                if (repo->state()->logger()->hasCommit(file_name)){
                    body = repo->state()->logger()->commit(file_name);
                }else{
                    Output::error("could not open commit file: "+file_name);
                }
            }else{
                // return the list of commit log files
                body = repo->state()->logger()->commitList().join("\n").toUtf8();
            }
        }else if (action == "browse"){
            QStringList tokens;
            if (file_name.endsWith("/"))
                file_name.chop(1);

            if (file_name.length() > 0){
                tokens << file_name.split('/');
            }
            QStringList dir_tokens = tokens;
            dir_tokens.removeFirst(); // remove reponame
            QString dir_name = dir_tokens.join("/");

            // add the title (with nav breadcrumb)
            body.append(QString("<h1>%1</h1>\n").arg(browseBreadCrumb(tokens.join("/"))));
            // add the list of subdirs
            QStringList sub_dirs = repo->state()->subDirs(dir_name);
            body.append(browseDirIndex(tokens, sub_dirs));
            // list the files in the directory
            QList<FileInfo*> matches = repo->state()->filesInDir(dir_name);
            body.append(browseFileIndex(repo_name, matches));

        }
    }

    // return an html document with links to files!
    if (action == "browse" && file_name.length() == 0){
        Output::debug("zero filename");
        QStringList repo_names = repoTableModel->repoNames();
        body.append("<h1>Dudley</h1>\n");
        foreach(QString repo_name, repo_names){
            body.append(linkToBrowse(repo_name)+"<br />\n");
        }
    }

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
    QString str("<a href=\"/browse/%1\">%2</a>");
    Output::debug("dirnamemememe:"+dir_name);
    return str.arg(dir_name, dir_name.split("/").last());
}

QString Server::linkToFile(QString repo_name, FileInfo* f)
{
    QString str("<a href=\"/file/%1/%2/%3\">%4</a>");
    return str.arg(f->fingerPrint(), repo_name, f->filePath(), f->fileName());
}
