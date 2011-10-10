#include <QUrl>
#include <QCoreApplication>
#include "httpcontroller.h"
#include "repo.h"
#include "state.h"
#include "statelogger.h"
#include "output.h"
#include "httpview.h"
#include "httpserver.h"
#include "parser.h"
#include "serializer.h"

HttpController::HttpController(HttpServer *parent, QTcpSocket* socket) :
    QObject(parent), m_socket(socket), m_server(parent)
{
    // need to hookup the repoModel and transferManager..
    m_state = BeforeStart;
    m_repoModel = m_server->repoModel();
    m_transferManager = m_server->transferManager();
    m_responseRepo = 0;
    setupRoutes();

    m_request = new HttpRequest(this);
    m_response = new HttpResponse(this);

    connect(this, SIGNAL(requestFinished()), m_server, SLOT(processRequestFinished()));
    connect(this, SIGNAL(responseFinished()), m_server, SLOT(processResponseFinished()));
    connect(this, SIGNAL(responseWritten()), m_server, SLOT(processResponseWritten()));
    connect(m_request, SIGNAL(fileUploadStarted()), this, SLOT(processUploadStarted()));
    connect(m_request, SIGNAL(headersFinished()), this, SLOT(routeRequest()));
    connect(m_request, SIGNAL(finished()), this, SLOT(processRequestFinished()));
    connect(m_response, SIGNAL(finished()), this, SLOT(processResponseFinished()));
    connect(m_socket,  SIGNAL(readyRead()), this, SLOT(processReadyRead()));
}

void HttpController::start()
{
    qDebug() << "start controller";
    qDebug() << statusReport();
    setState(ReadingRequest);
    processReadyRead();
}

void HttpController::setState(States state)
{
    m_state = state;
    switch(state){
    case ResponseFinished:
        emit responseFinished();
        break;
    case ResponseWritten:
        emit responseWritten();
        break;
    default:
        break;

    }
}

HttpController::States HttpController::state() const
{
    return m_state;
}

void HttpController::processReadyRead()
{
    while (m_state < RequestFinished && m_socket->canReadLine()){
        m_request->parseLine(m_socket->readLine());
    }
}

void HttpController::setupRoutes()
{
    m_routes["root"] = QRegExp("/?");
    m_routes["favicon"] = QRegExp("/favicon.ico");

    m_routes["file"] = QRegExp("/(file)/(\\w+)/(\\w{40})/([^?*:;{}\\\\]+)");
    m_route_args["file"] = "action repo_name fingerprint file_path";

    m_routes["history"] = QRegExp("/(history)/(\\w+)/?");
    m_route_args["history"] = "action repo_name";

    m_routes["commit"] = QRegExp("/(commit)/(\\w+)/([^/?*;{}\\\\]+)");
    m_route_args["commit"] = "action repo_name commit_name";

    m_routes["browse"] = QRegExp ("/(browse)/(\\w+)(/.*)?");
    m_route_args["browse"] = "action repo_name file_path";

    m_routes["ping"] = QRegExp ("/(ping)/(\\w+)(/.*)?");
    m_route_args["ping"] = "action repo_name something";

    // naked is an overloaded nice looking route for each repo
    // it should give the most normal looking urls for your files
    // i mean overloaded in that it does not specify the action name in the path
    QStringList repo_names = m_repoModel->names();
    m_routes["naked"] = QRegExp("/("+(repo_names.join("|"))+")(/.*)?");
    m_route_args["naked"] = "repo_name file_path";
}

// when the request has received its headers.. it signals headersFinished()
// which is our queue to respond to the request
void HttpController::routeRequest()
{
    setState(RoutingRequest);
    QUrl uri(m_request->uri());
    QString uri_path = uri.path();

    // dont forget to create a torrent action to return a bittorrent info file
    QMap<QString, QRegExp>::const_iterator i;
    for (i = m_routes.begin(); i != m_routes.end(); ++i){
        m_route_key = i.key();
        QRegExp regex = i.value();
        if(regex.exactMatch(uri_path)){
            // create the params hash now we know the route
            QStringList keys = m_route_args[m_route_key].trimmed().split(" ");
            for(int i = 1; i <= regex.captureCount(); ++i){
                // argument names start at 0 (from the string split)
                // argument values start at 1 (from the regex)
                m_params.insert(keys[(i-1)].toAscii(), regex.cap(i).toAscii());
            }

            QStringList clean_keys = QString("path file_path commit_name").split(" ");
            foreach(QString k, clean_keys){
                if (m_params.contains(k.toAscii())){
                    m_params.insert(k.toAscii(), cleanPath(m_params[k.toAscii()]).toAscii());
                }
            }

            setState(RoutedRequest);
            break; // break the for loop.. we should only ever match one route
        }
    }
}

// when a request is finished, another request may come in on the existing
// socket.. we need to let the httpserver know that we dont want anymore request data
// on this socket even though we may start sending our reply
void HttpController::processRequestFinished()
{
    emit requestFinished();
    if (m_request->state() == HttpMessage::Invalid){
        m_response->setResponse("400 Bad Request", "Request is invalid");
        return;
    }

    if (m_state == RoutingRequest){
        // do some kinda fail
        m_response->setResponse("400 Bad Request", "Could not route your request");
        return;
    }

    setState(ActioningRequest);
    if (m_route_key == "root"){
        actionRoot();
    }else if (m_route_key == "favicon"){
        actionFavicon();
    }else if (m_route_key == "file"){
        actionFileByFingerprint();
    }else if (m_route_key == "history"){
        actionHistory();
    }else if (m_route_key == "commit"){
        actionCommit();
    }else if (m_route_key == "browse"){
        actionBrowse();
    }else if (m_route_key == "ping"){
        m_response->setResponse("200 OK", "pong");
    }else if (m_route_key == "naked"){
        if (m_request->method() == "POST"){
            actionBrowserUpload();
        }else{
            // detect if this is a file or directory request before deciding action
            if (m_repoModel->contains(m_params["repo_name"]) &&
                m_repoModel->get(m_params["repo_name"])->repo()->state()->knownFilePath(m_params["file_path"])){
                actionFileByFileName();
            }else{
                actionBrowse();
            }
        }
    }else{
        m_response->setResponse("500 Internal Server Error",
                       "action is not being handled but route was matched");
    }
}

void HttpController::processResponseFinished()
{
    setState(ResponseFinished);
}

void HttpController::sendResponse()
{
    qDebug("sending response now");
    FileTransfer* ft;
    if (m_responseRepo){
        // the response is a file from a repo
        ft = m_transferManager->copy(m_responseFileInfo.filePath(), m_responseFileInfo.size());
        ft->setSource(m_responseRepo->path(), m_response->contentDevice());
        ft->setDest(m_socket->peerAddress().toString(), m_socket);
    }else{
        // the response is generated
        ft = m_transferManager->copy(m_request->uri(), m_response->contentLength());
        ft->setSource("Dudley", m_response->contentDevice());
        ft->setDest(m_socket->peerAddress().toString(), m_socket);
    }

    // we can only rely on m_socket->bytesWritten to trip another read
    connect(ft, SIGNAL(finished()), this, SIGNAL(responseWritten()));
    ft->start();
    m_socket->write(m_response->headers());
}

void HttpController::actionRoot()
{
    QByteArray body = "<h1>Dudley</h1>";
    QStringList repo_names = m_repoModel->names();
    foreach(QString name, repo_names){
        body.append(QString("<a href='%1'>%1</a><br />").arg(name));
    }
    m_response->setResponse("200 OK", body);
}

// this guy gets called once for each file in a mime message that we receive
// a form-data post could have a multiple parts which are files
void HttpController::processUploadStarted()
{
    if (m_request->hasPendingFileUploads()){
        if (Synchronizer* s = m_repoModel->get(m_params["repo_name"])){
            qDebug() << "beginning file upload, bytes transferred: "
                     << m_request->contentBytesTransferred();
            HttpMessage* message = m_request->getNextFileUploadMessage();
            QString file_path = m_params["file_path"]+"/"+message->formFieldFileName();
            QIODevice* dest_device = s->repo()->putFile(file_path);
            message->setContentDevice(dest_device);
        }else{
            m_response->setResponse("500 Internal Server Error", "Could not open repo");
        }
    }
}

void HttpController::actionBrowserUpload()
{
    m_response->setResponse("200 OK", "upload finished:\n"+statusReport().replace("  ","&nbsp;&nbsp;").replace("\n", "<br />"));
}

void HttpController::actionFavicon()
{
    QFile *f  = new QFile(":/icons/dino1.png");
    if(f->open(QIODevice::ReadOnly)){
        m_response->setResponseCode("200 OK");
        m_response->setHeader("Content-Type", "image/png");
        m_response->setHeader("Content-Length", f->size());
        m_response->setContentDevice(f);
        m_response->setState(HttpMessage::Finished);
    }else{
        m_response->setResponse("404 Not Found", "could not open the favicon file");
    }
}

void HttpController::actionFileByFileName()
{
    qDebug() << "actionfilerequestbyfilename:"
             << m_params["repo_name"]
             << " "
             << m_params["file_path"];
    if (Synchronizer* s = m_repoModel->get(m_params["repo_name"])){
        if (s->repo()->state()->knownFilePath(m_params["file_path"])){
            FileInfo file_info = s->repo()->state()->fileInfoByFilePath(m_params["file_path"]);
            setResponseContentFromRepo(s->repo(), file_info);
        }else{
            m_response->setResponse("404 Not Found", "");
        }
    }else{
        m_response->setResponse("500 Internal Server Error", "Could not open repo");
    }
}

void HttpController::actionFileByFingerprint()
{
    if (Synchronizer* s = m_repoModel->get(m_params["repo_name"])){
        if (s->repo()->state()->knownFingerPrint(m_params["fingerprint"])){
            FileInfo file_info = s->repo()->state()->fileInfoByFingerPrint(m_params["fingerprint"]);
            // need to call hasFile here
            setResponseContentFromRepo(s->repo(), file_info);
        }else{
            m_response->setResponse("404 Not Found");
        }
    }else{
        m_response->setResponse("500 Internal Server Error", "Could not open repo");
    }
}

void HttpController::setResponseContentFromRepo(Repo* repo, FileInfo file_info)
{

    QIODevice *file = repo->getFile(file_info);
    m_responseRepo = repo;
    m_responseFileInfo = file_info;
    m_response->setHeader("Last-Modified", file_info.modifiedAt());
    m_response->setHeader("Content-Type", file_info.mimeType());
    m_response->setHeader("Content-Length", file_info.size());
    m_response->setContentDevice(file);
    m_response->setState(HttpMessage::Finished);
}

void HttpController::actionHistory()
{
    if (Synchronizer* s = m_repoModel->get(m_params["repo_name"])){
        // should be jsoRepon.. actually unnecessary in current design.
        QJson::Serializer serializer;
        QByteArray ba = serializer.serialize(QVariant(s->repo()->logger()->logNames()));
        m_response->setResponse("200 OK", ba);
    }else{
        m_response->setResponse("500 Internal Server Error", "Could not open repo");
    }
}

void HttpController::actionCommit()
{
    if (Synchronizer* s = m_repoModel->get(m_params["repo_name"])){
        if (s->repo()->logger()->hasStateDiff(m_params["commit_name"])){
            m_response->setResponse("200 OK", s->repo()->logger()->loadStateDiffJson(m_params["commit_name"]));
        }else{
            m_response->setResponse("404 Not Found");
        }
    }else{
        m_response->setResponse("500 Internal Server Error", "Could not open repo");
    }
}

void HttpController::actionBrowse()
{
    QString file_path = m_params["file_path"];
    QString repo_name = m_params["repo_name"];
    qDebug() << "browse repo name:" << repo_name
             << " dir name: " << file_path;
    if (Synchronizer* s = m_repoModel->get(repo_name)){
        QList<FileInfo> files = s->repo()->state()->filesInDir(file_path);
        QStringList sub_dirs = s->repo()->state()->subDirectories(file_path);
        m_response->setResponse("200 OK", HttpView::browse(repo_name, file_path, sub_dirs, files));
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

QByteArray HttpController::statusReport(bool show_headers) const
{
    QByteArray s = m_request->inspect(show_headers).toAscii();
    s += m_response->inspect(show_headers).toAscii();
    return s;
}

HttpRequest* HttpController::request() const
{
    return m_request;
}

HttpResponse* HttpController::response() const
{
    return m_response;
}

QTcpSocket* HttpController::socket() const
{
    return m_socket;
}
