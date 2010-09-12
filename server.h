#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>
class RepoTableModel;

class Server : public QTcpServer
{
    Q_OBJECT
public:
    Server(RepoTableModel* repoTableModel, QObject *parent = 0);

protected slots:
    void acceptConnection();
    void processReadyRead();

private:
    RepoTableModel* repoTableModel;
};

#endif // SERVER_H
