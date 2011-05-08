#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QtGui>
#include <QtNetwork>
#include "output.h"
#include "localdiskrepo.h"
#include "httpclientrepo.h"

MainWindow::MainWindow(QSettings* settings, QWidget *parent) :
    QMainWindow(parent),  ui(new Ui::MainWindow), m_settings(settings)
{
    ui->setupUi(this);
    ui->repoTreeView->setModel(&m_repoModel);
    readSettings();
    setWindowTitle(tr("Dudley Server"));
    connect(ui->addRepoButton, SIGNAL(clicked()), this, SLOT(addRepoButtonPressed()));
    connect(ui->editRepoButton, SIGNAL(clicked()), this, SLOT(editRepoButtonPressed()));
    connect(ui->removeRepoButton, SIGNAL(clicked()), this, SLOT(removeRepoButtonPressed()));

    m_fileTransferManager = new FileTransferManager(this);
    m_server = new HttpServer(this, &m_repoModel, m_fileTransferManager);
    if (!m_server->listen(QHostAddress::Any, 54573)){
        QMessageBox::critical(this, tr("Dudley Server"),
                              tr("Dudley unable to start server: %1.").arg(m_server->errorString()));
        close();
        return;
    }
    qDebug() << "The server is running on " << m_server->url().toString();
    trayIcon = new QSystemTrayIcon(QIcon(":/icons/dino1.png"), this);
    trayIcon->show();

}

void MainWindow::writeSettings()
{
    m_settings->beginGroup("repos");
    foreach(RepoModelItem rmi, m_repoModel.list()){
        m_settings->beginGroup(rmi.name());
        m_settings->setValue("tracker_url", rmi.s10r()->trackerUrl().toString());
        m_settings->setValue("local_path", rmi.repo()->path());
        m_settings->endGroup();
    }
    m_settings->endGroup();
}

void MainWindow::readSettings()
{
    m_settings->beginGroup("repos");
    foreach(QString name, m_settings->childGroups()){
        m_settings->beginGroup(name);
        QString local_path = m_settings->value("local_path").toString();
        QString tracker_url = m_settings->value("tracker_url").toString();
        addRepoModelItem(name, local_path, tracker_url);
        m_settings->endGroup();
    }
    m_settings->endGroup();
}

MainWindow::~MainWindow()
{
    delete ui;
    delete m_server;
    writeSettings();
    delete trayIcon;
}

void MainWindow::refreshRepoButtonPressed()
{
    // get currently selected repo
//    QModelIndex i = ui->repoTreeView->currentIndex();
//    if (i.isValid()){
//        m_repoModel->repo(i)->detectChanges();
//    }
}

void MainWindow::addRepoButtonPressed()
{
    RepoDialog *repoDialog = new RepoDialog(this);

    if (repoDialog->exec()){
        addRepoModelItem(repoDialog->name(),
                         repoDialog->localPath(),
                         repoDialog->trackerUrl());
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
        m_repoModel.remove(i);
    }
}

bool MainWindow::addRepoModelItem(QString name, QString local_path, QString tracker_url_string)
{
    Repo* repo = new LocalDiskRepo(this, local_path);
    if (!repo->isReady()){
        repo->initialize();
    }
    QUrl self_url = m_server->url();
    self_url.setPath(name);
    QUrl tracker_url(tracker_url_string);
    Synchronizer* s10r = new Synchronizer(this, repo, tracker_url, self_url);
    if (s10r->isReady()){
        m_repoModel.insert(RepoModelItem(s10r, name));
        return true;
    }

    qDebug() << "Failed to add s10r and repo" << name << local_path << tracker_url;
    return false;
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
