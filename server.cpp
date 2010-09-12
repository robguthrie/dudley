#include "server.h"
#include <QTcpSocket>
#include <QDataStream>
#include <QStringList>
#include <QDateTime>
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
    Output::debug("processReadyRead called");
    QTcpSocket* socket = (QTcpSocket*)sender(); // sender returns the pointer to the SIGNAL emitter
    HttpRequest request = HttpRequest::fromStream(socket);
    HttpResponse response;
    QByteArray body;
    // simple routing
    //process url -
    /*

      new routing plan
      METHOD    URL description                          Behaviour
      GET       /file/name/reponame/filename             return file
      GET       /file/fingerprint/reponame/fingerprint   return file
      GET       /history/reponame/                       list commits alphabetically
      GET       /history/reponame/commitname             return commit file content
      POST      /history/reponame/nodename/commitname    save commit under node name

      GET       /

      index request (browsable)
      /reponame only

      */
    QString uri = request.uri();
    QRegExp action_rx = QRegExp("/(file|history)/(.*)");

    if (action_rx.exactMatch(uri)){
        Output::debug(action_rx.capturedTexts().join(", "));
        QString action = action_rx.cap(1);
        QString tail = action_rx.cap(2);
        Output::debug("action:"+action);
        if (action == QString("file")){
            QRegExp repo_and_file_name_rx = QRegExp("(\\w+)/([^?*:;{}\\\\]+)");
            if (repo_and_file_name_rx.exactMatch(tail)){
                Output::debug("repo and filename request");
                Output::debug("request captured texts:"+repo_and_file_name_rx.capturedTexts().join(","));
                QStringList list = repo_and_file_name_rx.capturedTexts();
                QString repo_name = list.at(1);
                QString file_name = list.at(2);
                Output::debug("request repo_name: "+repo_name+" file_name: "+file_name);
                if (repoTableModel->hasRepo(repo_name)){
                    Output::debug("has repo!");
                    FileRepo* repo = repoTableModel->repo(repo_name);
                    if (repo->hasFileInfoByFilePath(file_name)){
                        Output::debug("repo has fileinfo!");
                        FileInfo* file_info = repo->fileInfoByFilePath(file_name);
                        QIODevice *file = repo->getFile(file_info);
                        if (file->open(QIODevice::ReadOnly)){
                            Output::debug("have result! - opened file");
                            response.setContentType(file_info->mimeType());
                            response.setContentLength(file->size());
                            body = file->readAll();
                        }else{
                            Output::debug("have result! - but could not open file");
                            // RESPONSE 500 server error could not open file
                        }
                    }else{
                        Output::debug("fileinfo not found in repo");
                        // 404
                    }
                }else{
                    Output::debug("has not repo!");
                    // 404 or 500...
                }
            }else{
                Output::debug("failed to get reponame and filename with regex");
                // return BAD REQUEST 400
            }
        }else if (action == QString("history")){
            Output::debug("history action");
            // identify the repo name

            QRegExp repo_and_commit("(\\w+)/([^/?*;{}\\\\]+)");
            QRegExp repo_only("(\\w+)/?");
            if (repo_and_commit.exactMatch(tail)){
                // return the commit itself as the body
                Output::debug("repo and commit found");
                QString repo_name = repo_and_commit.cap(1);
                QString commit_name = repo_and_commit.cap(2);
                if (repoTableModel->hasRepo(repo_name)){
                    Output::debug("has repo!");
                    FileRepo* repo = repoTableModel->repo(repo_name);
                    body = repo->state()->logger()->commit(commit_name);
                }else{
                    // repo not found 404
                    Output::debug("history action has not repo: "+repo_name);
                }
            }else if(repo_only.exactMatch(tail)){
                // return the commit list
                QString repo_name = repo_only.cap(1);
                if (repoTableModel->hasRepo(repo_name)){
                    Output::debug("repo only");
                    FileRepo* repo = repoTableModel->repo(repo_name);
                    body = repo->state()->logger()->commitList().join("\n").toUtf8();
                }else{
                    // repo not found 404
                    Output::debug("history action has not repo!");
                }

            }else{
                // bad 400
                Output::debug("couldnt match any regrexp");
            }
            // list history files
        }else{
            Output::debug("unknown action");
        }
    }else{
        // return BAD REQUEST 400
        Output::debug("unrecognised request BAD REQUEST 400");
    }




    QDataStream os(socket);
    response.setContentLength(body.size());
    QByteArray header = response.header();
    os.writeRawData(header, header.length());
    os.writeRawData(body, body.length());
//    os << "\n";

//    socket->waitForBytesWritten();
    socket->close();

}
