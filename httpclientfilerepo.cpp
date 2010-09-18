#include "httpclientfilerepo.h"
#include <QDesktopServices>
#include "output.h"
HttpClientFileRepo::HttpClientFileRepo(QObject *parent, QString path, QString name)
    :FileRepo(parent, path, name)
{
    Output::debug("constructed HttpClientFileRepo on path: "+path);
    m_log_path = QDesktopServices::storageLocation(QDesktopServices::DataLocation);
    m_log_path.append(QString("/logs/%1/%2/").arg(this->type(), this->name()));
    Output::debug("logs data location:"+m_log_path);
    m_state = new FileRepoState(this, m_log_path);
}

bool HttpClientFileRepo::canReadData() const
{
    return false;
}

QString HttpClientFileRepo::type()
{
    return QString("HttpClientFileRepo");
}

void HttpClientFileRepo::updateState()
{
    // request state updates from host and rebuild our state
}


bool HttpClientFileRepo::hasFile(FileInfo fileInfo) const
{
    return false;
}

QIODevice* HttpClientFileRepo::getFile(FileInfo* fileInfo) const
{
    // do an http get and return the socket
    return 0;
}

QString HttpClientFileRepo::relativeFilePath(QString filePath){
    if (filePath.startsWith(m_path+'/')){
        return filePath.remove(0, m_path.length()+1);
    }else{
        return filePath;
    }
}
