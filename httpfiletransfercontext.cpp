#include "httpfiletransfercontext.h"

// this helps us track the params that were sent with a http multipart file upload
HttpFileTransferContext::HttpFileTransferContext(HttpRequest* request, HttpResponse* response, QString repo_name, QString path)
{
    m_request = request;
    m_response = response;
    m_repoName = repo_name;
    m_path = path;
}
