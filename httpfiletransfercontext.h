#ifndef HTTPFILETRANSFERCONTEXT_H
#define HTTPFILETRANSFERCONTEXT_H

#include <QString>
#include "fileinfo.h"
class HttpRequest;
class HttpResponse;
class HttpFileTransferContext{
public:
    HttpFileTransferContext(HttpRequest* request, HttpResponse* response, QString repo_name, QString path);
    QString m_repoName;
    QString m_path;
    HttpRequest* m_request;
    HttpResponse* m_response;
    FileInfo* m_fileInfo;
};

#endif // HTTPFILETRANSFERCONTEXT_H
