#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
#include "fileinfo.h"
class RepoTableModel;

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(RepoTableModel* repoTableModel, QObject *parent = 0);

    QString browseFileIndex(QString repo_name, QList<FileInfo*> fileInfos);
    QString browseBreadCrumb(QString dir_name) const;
    QString linkToBrowse(QString dir_name) const;
    QString linkToFile(QString repo_name, FileInfo* f);
    QString browseDirIndex(QStringList path_dirs, QStringList sub_dirs);
protected slots:
    void acceptConnection();
    void processReadyRead();

private:
    RepoTableModel* repoTableModel;
};

#endif // SERVER_H
