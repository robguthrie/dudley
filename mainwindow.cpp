#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include <QtNetwork>
#include "output.h"
#include "workingfilerepo.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    repoTableModel = new RepoTableModel();
    server = new Server(repoTableModel, this);
    if (!server->listen(QHostAddress::Any, 54573)){
        QMessageBox::critical(this, tr("Dudley Server"),
                              tr("Dudley unable to start server: %1.").arg(server->errorString()));
        close();
        return;
    }


    addRepo("/home/rob/pics", "pics");
    addRepo("/home/rob/music", "music");
    ui->repoTableView->setModel(repoTableModel);
    setWindowTitle(tr("Dudley Server"));
    connect(ui->addRepoButton, SIGNAL(clicked()), this, SLOT(addRepoButtonPressed()));
    Output::outputTextEdit = ui->outputTextEdit;
    Output::info(tr("The server is running on\n\nIP: %1\nport: %2\n\n")
                         .arg(bestIpAddress()).arg(server->serverPort()));
    trayIcon = new QSystemTrayIcon(QIcon(":/icons/dino1.png"), this);
    trayIcon->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::addRepoButtonPressed()
{
    RepoDialog *repoDialog = new RepoDialog(this);

//    connect(repoDialog, SIGNAL(newSettings(QString)), this, SLOT(addRepo(QString)));
    if (repoDialog->exec()){
        Output::debug("hellooo? returned from addRepoDialog");
        addRepo(repoDialog->path(), repoDialog->name());
    }
}
bool MainWindow::addRepo(QString path, QString name)
{
    // opena file repo on this path.. store it in repoList
//    FileRepo* repo = new WorkingFileRepo(this, path, name);


    WorkingFileRepo* repo = new WorkingFileRepo(this, path, name);
    // check to see if the repo is initialized
    if (!repo->isReady()){
        // it is not ready.. try to initialized
        if (!repo->initialize())
            return false;
    }
    repoTableModel->insertRepo(repo);
    return true;
}

void MainWindow::removeRepoButtonPressed()
{

}



void MainWindow::removeRepo(FileRepo* repo)
{
    //
//    // opena file repo on this path.. store it in repoList
//    WorkingFileRepo* repo = new WorkingFileRepo(this, path, name);
//
//    if (repo->initialize())
//        repoTableModel->insertRepo(repo);
}

QString MainWindow::bestIpAddress()
{
    QString ipAddress;
    QList<QHostAddress> ipAddressesList = QNetworkInterface::allAddresses();
    // use the first non-localhost IPv4 address
    for (int i = 0; i < ipAddressesList.size(); ++i) {
        if (ipAddressesList.at(i) != QHostAddress::LocalHost &&
            ipAddressesList.at(i).toIPv4Address()) {
            ipAddress = ipAddressesList.at(i).toString();
            break;
        }
    }
    // if we did not find one, use IPv4 localhost
    if (ipAddress.isEmpty())
        ipAddress = QHostAddress(QHostAddress::LocalHost).toString();
    return ipAddress;
}

void MainWindow::changeEvent(QEvent *e)
{
    QMainWindow::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
