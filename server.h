#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include "fileinfo.h"
#include "httprequest.h"
class RepoTableModel;

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(RepoTableModel* repoTableModel, QObject *parent = 0);

    QString browseFileIndex(QString repo_name, QList<FileInfo*> fileInfos);
    QString browseBreadCrumb(QStringList dirs) const;
    QString linkToBrowse(QStringList tokens) const;
    QString linkToFile(QString repo_name, FileInfo* f);
    QString browseDirIndex(QStringList path_dirs, QStringList sub_dirs);

protected slots:
    void acceptConnection();
    void processReadyRead();
    void processDisconnected();
    void processAboutToClose();
    void processError();
    void respondToRequest(HttpRequest request, QTcpSocket *socket);

private:
    RepoTableModel* repoTableModel;
    QSet<QTcpSocket*> m_sockets;
};

#endif // SERVER_H
