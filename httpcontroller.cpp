#include "httpcontroller.h"
#include "repo.h"
#include "output.h"
#include <QUrl>

HttpController::HttpController(HttpServer *parent, QTcpSocket* socket) :
    QObject(parent), m_socket(socket), m_server(parent)
{
    // need to hookup the repoModel and transferManager..
    m_repoModel = m_server->repoModel();
    m_transferManager = m_server->transferManager();

    m_request = new HttpRequest(this, m_socket);
    connect(m_request, SIGNAL(headersFinished()), this, SLOT(respondToRequest()));
    connect(m_request, SIGNAL(finished()), this, SLOT(processRequestFinished()));
    connect(m_socket,  SIGNAL(readyRead()), m_request, SLOT(processReadyRead()));

    m_response = new HttpResponse(this, m_socket);
    // the ready signal lines the response up to be sent by the controller
    connect(m_response, SIGNAL(ready()), this, SLOT(processResponseReady()));
    connect(m_response, SIGNAL(finished()), this, SLOT(processResponseFinished()));
    connect(this, SIGNAL(responseReady(QTcpSocket*)),
            m_server, SLOT(processResponseReady(QTcpSocket*)));

    // this kicks it all off...
    m_request->processReadyRead();
}

HttpController::~HttpController()
{
    delete m_request;
    delete m_response;
}

// when the request has received its headers.. it signals headersFinished()
// which is our queue to respond to the request
void HttpController::respondToRequest()
{
    if (m_request->isValid()){
        routeRequestToAction();
    }else{
        m_response->setResponse("400 Bad Request", "HTTP Request is invalid");
    }
}

// when a request is finished, another request may come in on the existing
// socket.. we need to let the httpserver know that we dont want anymore request data
// on this socket
void HttpController::processRequestFinished()
{
    disconnect(m_socket,  SIGNAL(readyRead()), m_request, SLOT(processReadyRead()));
    emit requestFinished(m_socket);
}

// when the response signals it is ready to be sent back to the client
// the http server maintains the correct order of responses.. so we must
// signal that we are ready and wait to be told to send our response.
void HttpController::processResponseReady()
{
    emit responseReady(m_socket);
}

bool HttpController::responseIsReady() const
{
    return m_response->isReady();
}

void HttpController::sendResponse()
{
    // send the response headers then use filetransfer to move the body
    // no need to wait for a signal .. just fill the buffer with body
    // we may need to fake or make a file_info for the body

    // write the header from the response, then its body to the socket.
    m_socket->write(m_response->headers());
    FileTransfer*  ft = m_transferManager->copy("not sure source", m_response->contentDevice(),
                            m_socket->peerAddress().toString(), m_socket, m_response->fileInfo());
    connect(ft, SIGNAL(finished()), m_response, SIGNAL(finished()));
}

void HttpController::processResponseFinished()
{
    emit responseFinished(m_socket);
    //  close if its http 1.0
    if (m_response->protocol() == "HTTP/1.0"){
        m_socket->close();
    }
}

void HttpController::routeRequestToAction()
{
    QUrl uri(m_request->uri());
    QString uri_path = uri.path();

    QMap<QString, QRegExp> routes;
    QHash<QString, QString> route_args;

    routes["root"] = QRegExp("/?");
    routes["favicon"] = QRegExp("/favicon.ico");

    routes["file"] = QRegExp("/(file)/(\\w+)/(\\w{40})/([^?*:;{}\\\\]+)");
    route_args["file"] = "action repo_name fingerprint file_path";

    routes["history"] = QRegExp("/(history)/(\\w+)/?");
    route_args["history"] = "action repo_name";

    routes["commit"] = QRegExp("/(commit)/(\\w+)/([^/?*;{}\\\\]*)");
    route_args["commit"] = "action repo_name commit_name";

    routes["browse"] = QRegExp ("/(browse)/(\\w+)(/.*)?");
    route_args["browse"] = "action repo_name path";

    routes["ping"] = QRegExp ("/(ping)/(\\w+)(/.*)?");
    route_args["ping"] = "action repo_name something";


    // naked is an overloaded nice looking route for each repo
    // it should give the most normal looking urls for your files
    // i mean overloaded in that it does not specify the action name in the path
    QStringList repo_names = m_repoModel->repoNames();
    routes["naked"] = QRegExp("/("+(repo_names.join("|"))+")(/.*)?");
    route_args["naked"] = "repo_name path";

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

            // at this point we should authorize them and send the 100-continue
            //            m_request->accept();

            g_log->debug("matched "+key+" route");

            // create the params hash now we know the route
            QStringList keys = route_args[key].trimmed().split(" ");
            for(int i = 1; i <= regex.captureCount(); ++i){
                // argument names start at 0 (from the string split)
                // argument values start at 1 (from the regex)
                m_params.insert(keys[(i-1)].toAscii(), regex.cap(i).toAscii());
            }

            if (key == "root"){
                actionRootRequest();
            }else if (key == "favicon"){
                actionFaviconRequest();
            }else if (key == "file"){
                actionFileRequestByFingerprint();
            }else if (key == "history"){
                actionHistoryRequest();
            }else if (key == "commit"){
                actionCommitRequest();
            }else if (key == "browse"){
                actionBrowseRequest();
            }else if (key == "ping"){
                m_response->send("200 OK", "pong");
            }else if (key == "naked"){
                if (m_request->method() == "POST"){
                    // this is a file upload (via web form) to a repo/path
                    actionUploadRequest();
                }else{
                    // want to support upload of files to the current browse path.
                    // path might refer to a file or a dir at at this point
                    if ((m_repoModel->hasRepo(m_params["repo_name"])) &&
                         m_repoModel->repo(m_params["repo_name"])->hasFileInfoByFilePath(m_params["path"])){
                        actionFileRequestByFileName();
                    }else{
                        // repo_name cant be wrong due to way i create the regex
                        actionBrowseRequest();
                    }
                }
            }else{
                m_response->send("500 Internal Server Error",
                               "action is not being handled but route was matched");
            }
            break; // break the for loop.. we should only ever match one route
        }
    }

    if (!routed_request){
        // The request cannot be fulfilled due to bad syntax
        m_response->send("400 Bad Request", "Could not route your request");
    }
}

void HttpController::actionRootRequest()
{
    QByteArray body = "<h1>Dudley</h1>";
    QStringList repo_names = m_repoModel->repoNames();
    foreach(QString name, repo_names){
        body.append(QString("<a href='%1'>%1</a><br />").arg(name));
    }
    m_response->send(body);
}

void HttpController::actionUploadRequest()
{
    connect(m_request, SIGNAL(fileUploadStarted()), this, SLOT(processFileUploadStarted()));
    connect(m_request, SIGNAL(finished()), this, SLOT(processFileUploadFinished()));
}

void HttpController::processFileUploadStarted()
{
    if (m_request->hasPendingFileUploads()){
        if (Repo* repo = m_repoModel->repo(m_params["repo_name"])){
            HttpMessage* message = m_request->getNextFileUploadMessage();
            QIODevice* source_file = message->contentDevice(); // returns a qbuffer pointer to the data
            QString file_path = m_params["path"]+"/"+message->formFieldFileName();
            FileInfo* fi = new FileInfo(this, file_path);
            // we only know the size of an uploaded file when it finishes correctly
            // so connect the complete(size) signal to the fileinfo setSize
            connect(message, SIGNAL(complete(qint64)), fi, SLOT(setSize(qint64)));
            QString source_name = m_socket->peerAddress().toString();
            QString dest_name = repo->name();
            QIODevice* dest_file = repo->putFile(fi);
            FileTransfer* ft = m_transferManager->copy(source_name, source_file,
                                                     dest_name, dest_file, fi);
            m_transfers << ft;
        }else{
            m_response->send("500 Internal Server Error", "Could not open repo");
        }
    }
}

void HttpController::processFileUploadFinished()
{
    // oh my god there is still someone waiting for a response after that.
    // well the clent's html form should use a background upload so we can just
    // give the file stats for now.
    QCoreApplication::processEvents();
    // we only know its size and name and that we got to the end
    // find the response object waiting for us in the fuc (heh)
    // the sourceParent is the request object

    // now return a status for all the uploads that occured in the request
    QString body = "transfers:<br />";
    foreach(FileTransfer* ft, m_transfers){
        body += ft->statusLine()+"<br />";
    }

    m_response->setResponse("200 OK", body.toAscii());
}

void HttpController::actionFaviconRequest()
{
    QFile *f  = new QFile(":/icons/dino1.png");
    if(f->open(QIODevice::ReadOnly)){
        m_response->setResponse("200 OK");
        m_response->setHeader("Content-Type", "image/png");
        m_response->setContentLength(f->size());
        m_response->setContentDevice(f);
    }else{
        m_response->setResponse("404 Not Found", "could not open the favicon file");
    }
}

void HttpController::actionFileRequestByFileName()
{
    g_log->debug("actionfilerequestbyfilename:"+m_params["repo_name"]+" "+m_params["file_path"]);
    if (Repo* repo = m_repoModel->repo(m_params["repo_name"])){
        if (repo->hasFileInfoByFilePath(m_params["file_path"])){
            FileInfo* file_info = repo->fileInfoByFilePath(m_params["file_path"]);
            setFileResponse(repo, file_info);
        }else{
            m_response->setResponse("404 Not Found");
        }
    }else{
        m_response->setResponse("500 Internal Server Error", "Could not open repo");
    }
}


void HttpController::actionFileRequestByFingerprint()
{
    if (Repo* repo = m_repoModel->repo(m_params["repo_name"])){
        if (repo->hasFileInfoByFingerPrint(m_params["fingerprint"])){
            FileInfo* file_info = repo->fileInfoByFingerPrint(m_params["fingerprint"]);
            // need to call hasFile here
            m_response->setResponseFile(repo, file_info);
        }else{
            m_response->setResponse("404 Not Found");
        }
    }else{
        m_response->setResponse("500 Internal Server Error", "Could not open repo");
    }
}

void HttpController::actionHistoryRequest()
{
    if (Repo* repo = m_repoModel->repo(m_params["repo_name"])){
        m_response->setResponse("200 OK", repo->state()->logger()->logNames().join("\n").toUtf8());
    }else{
        m_response->setResponse("500 Internal Server Error", "Could not open repo");
    }
}

void HttpController::actionCommitRequest()
{
    if (Repo* repo = m_repoModel->repo(m_params["repo_name"])){
        if (repo->state()->logger()->hasLogFile(m_params["commit_name"])){
            m_response->setResponse("200 OK", repo->state()->logger()->openLog(m_params["commit_name"]));
        }else{
            m_response->setResponse("404 Not Found");
        }
    }else{
        m_response->setResponse("500 Internal Server Error", "Could not open repo");
    }
}

void HttpController::actionBrowseRequest()
{
    g_log->debug("browse repo name:"+m_params["repo_name"]+" dir name: "+m_params["path"]);
    if (Repo* repo = m_repoModel->repo(m_params["repo_name"])){
        QStringList path_tokens;
        path_tokens << m_params["repo_name"];
        if (m_params["path"].trimmed().length() > 0)
            path_tokens << path.split("/");
        // add the title (with nav breadcrumb)
        QByteArray body;
        body.append(QString("<h1>%1</h1>\n").arg(browseBreadCrumb(path_tokens)));
        // add the list of subdirs
        QStringList sub_dirs = repo->state()->subDirs(m_params["path"]);
        body.append(browseDirIndex(path_tokens, sub_dirs));
        // list the files in the directory
        QList<FileInfo*> matches = repo->state()->filesInDir(path);
        body.append(browseFileIndex(m_params["repo_name"], matches));
        body.append(browseUploadForm(m_params["repo_name"]+"/"+path));
        m_response->setResponse("200 OK", body);
    }else{
        m_response->setResponse("500 Internal Server Error", "Could not open repo");
    }
}

QString HttpController::cleanPath(QString path)
{
    path = path.trimmed();
    if (path.startsWith("/")) path.remove(0,1);
    if (path.endsWith("/")) path.chop(1);
    return path;
}

QString HttpController::browseUploadForm(QString path = "")
{
    QString form =
       "<h1>Upload a File</h1>"
       "<form method='post' action='/"+path+"' enctype='multipart/form-data'>"
       "File1: <input type='file' name='file1' /><br/>"
       "File2: <input type='file' name='file2' /><br/>"
       "<input type='submit' value='Upload File' />"
       "</form>";
    return form;
}

QString HttpController::browseDirIndex(QStringList path_dirs, QStringList sub_dirs)
{
    QString str;
    foreach(QString dir, sub_dirs){
        QStringList temp_dirs = path_dirs;
        temp_dirs << dir;
        str.append(linkToBrowse(temp_dirs)+"<br />\n");
    }
    return str;
}

QString HttpController::linkToBrowse(QStringList tokens) const
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

QString HttpController::browseFileIndex(QString repo_name, QList<FileInfo*> fileInfos)
{
    QString table;
    foreach(FileInfo* f, fileInfos){
        QString link = linkToFile(repo_name, f);
        QString row = QString("<tr><td>%1</td><td>%2</td><td>%3</td></tr>\n").arg(link, humanSize(f->size()), f->mimeType());
        table.append(row);
    }
    return QString("<table>%1</table>").arg(table);
}

QString HttpController::browseBreadCrumb(QStringList dirs) const
{
    QStringList links;
    QStringList temp_dirs;
    foreach(QString dir, dirs){
        temp_dirs.append(dir);
        links << linkToBrowse(temp_dirs);
    }
    return links.join("/");
}


QString HttpController::linkToFile(QString repo_name, FileInfo* f)
{
    QString str("<a href=\"/%1/%2\">%4</a>");
    return str.arg(repo_name, f->filePath(), f->fileName());
}
