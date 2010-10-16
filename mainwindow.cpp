#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include <QtNetwork>
#include "output.h"
#include "localdiskrepo.h"
#include "httpclientrepo.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_repoModel = new RepoModel();
    ui->repoTreeView->setModel(m_repoModel);
    readSettings();
    setWindowTitle(tr("Dudley Server"));
    connect(ui->addRepoButton, SIGNAL(clicked()), this, SLOT(addRepoButtonPressed()));
    connect(ui->editRepoButton, SIGNAL(clicked()), this, SLOT(editRepoButtonPressed()));
    connect(ui->removeRepoButton, SIGNAL(clicked()), this, SLOT(removeRepoButtonPressed()));
    connect(ui->refreshRepoButton, SIGNAL(clicked()), this, SLOT(refreshRepoButtonPressed()));
    Output::outputTextEdit = ui->outputTextEdit;
    server = new HttpServer(m_repoModel, this);
    if (!server->listen(QHostAddress::Any, 54573)){
//    if (!server->listen(QHostAddress::Any)){
        QMessageBox::critical(this, tr("Dudley Server"),
                              tr("Dudley unable to start server: %1.").arg(server->errorString()));
        close();
        return;
    }
    Output::info(tr("The server is running on\n\nIP: %1\nport: %2\n\n")
                         .arg(bestIpAddress()).arg(server->serverPort()));
    trayIcon = new QSystemTrayIcon(QIcon(":/icons/dino1.png"), this);
    trayIcon->show();

}

void MainWindow::writeSettings()
{
    m_settings.beginGroup("repos");
    foreach(Repo* repo, m_repoModel->repoList()){
        m_settings.beginGroup(repo->name());
        m_settings.setValue("type", repo->type());
        m_settings.setValue("path", repo->path());
        m_settings.setValue("log_path", repo->logPath());
        m_settings.endGroup();
    }
    m_settings.endGroup();
}

void MainWindow::readSettings()
{
    m_settings.beginGroup("repos");
    foreach(QString repo_name, m_settings.childGroups()){
        m_settings.beginGroup(repo_name);
        QString type = m_settings.value("type").toString();
        QString path = m_settings.value("path").toString();
        QString log_path = m_settings.value("log_path").toString();
        this->addRepo(type, path, repo_name);
        m_settings.endGroup();
    }
    m_settings.endGroup();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete server;
    writeSettings();
    delete trayIcon;
}

void MainWindow::refreshRepoButtonPressed()
{
    // get currently selected repo
    QModelIndex i = ui->repoTreeView->currentIndex();
    if (i.isValid()){
        m_repoModel->repo(i)->updateState();
    }
}

void MainWindow::addRepoButtonPressed()
{
    RepoDialog *repoDialog = new RepoDialog(this);

//    connect(repoDialog, SIGNAL(newSettings(QString)), this, SLOT(addRepo(QString)));
    if (repoDialog->exec()){
        Output::debug("hellooo? returned from addRepoDialog");
        addRepo(repoDialog->type(), repoDialog->path(), repoDialog->name());
    }
}
void MainWindow::editRepoButtonPressed()
{
    // nothing yet..
}

void MainWindow::removeRepoButtonPressed()
{
    QModelIndex i = ui->repoTreeView->currentIndex();
    if (i.isValid()){
        m_repoModel->removeRepo(i);
    }
}

bool MainWindow::addRepo(QString type, QString path, QString name)
{
    Repo* repo = 0;
    Output::info(QString("loadRepo: %1 %2 %3").arg(type, path,name));
    if (type == "HttpClientFileRepo"){
        Output::debug("opening httpclientfilerepo");
        repo = new HttpClientRepo(this, path, name);
    }else if(type == "WorkingFileRepo"){
        Output::debug("opening workingfilerepo");
        repo = new LocalDiskRepo(this, path, name);
    }

    if (repo){
        m_repoModel->insertRepo(repo);

        if (repo->isReady()){
            return true;
        }else{
            return repo->initialize();
        }
    }else{
        Output::debug(QString("repo failed to create: %1 %2 %3").arg(type, name, path));
        return false;
    }
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
