#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "repodialog.h"
#include "editrepodialog.h"
#include <QtGui>
#include <QtNetwork>
#include "output.h"
#include "localdiskrepo.h"
#include "httpclientrepo.h"
#include "synchronizermodel.h"
#include <QFileSystemModel>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),  ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(tr("Dudley Server"));
    m_settings = new QSettings(QSettings::IniFormat, QSettings::UserScope, "Dinotech", "Dudley");
    m_upnp = new UpnpWrapper(this);
    m_repoModel = new RepoModel(this);
    m_fileTransferManager = new FileTransferManager(this);
    m_server = new HttpServer(this, m_repoModel, m_fileTransferManager);
    trayIcon = new QSystemTrayIcon(QIcon(":/icons/dino1.png"), this);
    trayIcon->show();
    readSettings();
    ui->repoListView->setModel(m_repoModel);
    connect(ui->addRepoButton, SIGNAL(clicked()), this, SLOT(addRepoButtonPressed()));
    connect(ui->removeRepoButton, SIGNAL(clicked()), this, SLOT(removeRepoButtonPressed()));
    connect(m_upnp, SIGNAL(portRedirected(bool)), this, SLOT(onPortRedirected(bool)));
    connect(ui->saveSettingsButton, SIGNAL(clicked()), this, SLOT(onSaveSettingsButtonPressed()));
    connect(ui->viewRepoButton, SIGNAL(clicked()), this, SLOT(onViewRepoButtonClicked()));
    listenOnPort(m_listenPort);
}

void MainWindow::onViewRepoButtonClicked()
{
    EditRepoDialog d(this, m_repoModel->get(ui->repoListView->currentIndex()));
    d.exec();
}

void MainWindow::onSaveSettingsButtonPressed()
{
    writeSettings();
    QMessageBox msgBox;
    msgBox.setText("Changes have been saved. However, you must restart for them to take effect.");
    msgBox.exec();
}

void MainWindow::listenOnPort(int port)
{
    if (!m_server->listen(QHostAddress::Any, port)){
        QMessageBox::critical(this, tr("Dudley Server"),
                              tr("Dudley unable to start server: %1.").arg(m_server->errorString()));
        close();
        return;
    }
    m_upnp->setPort(port);
    m_upnp->start();
}

void MainWindow::onPortRedirected(bool success)
{
    QString text;
    QString portstr = QString::number(m_server->serverPort());
    QString time = QDateTime::currentDateTime().toString();
    if (success){
        text = QString("redirected port %1 at %2").arg(portstr).arg(time);
    }else{
        text = QString("Failed to redirect port %1 at %2").arg(portstr).arg(time);
    }
    ui->upnpStatusLabel->setText(text);
}

void MainWindow::writeSettings()
{
    m_listenPort = ui->manualPortSpinBox->value();
    m_settings->setValue("listen_port", QVariant(m_listenPort));
    m_settings->beginGroup("repos");
    foreach(Synchronizer *s, m_repoModel->list()){
        m_settings->beginGroup(s->name());
        m_settings->setValue("tracker_url", s->trackerUrl().toString());
        m_settings->setValue("local_path", s->repo()->path());
        m_settings->endGroup();
    }
    m_settings->endGroup();
    qDebug() << m_settings->fileName();
}

void MainWindow::readSettings()
{
    m_listenPort = m_settings->value("listen_port").toInt();
    if (m_listenPort < 1001){
       m_listenPort = 22335;
    }
    ui->manualPortSpinBox->setValue(m_listenPort);
    m_settings->beginGroup("repos");
    foreach(QString name, m_settings->childGroups()){
        m_settings->beginGroup(name);
        QString local_path = m_settings->value("local_path").toString();
        QString tracker_url = m_settings->value("tracker_url").toString();
        addSynchronizer(name, local_path, tracker_url);
        m_settings->endGroup();
    }
    m_settings->endGroup();
}

MainWindow::~MainWindow()
{
    writeSettings();
    delete m_settings;
    delete ui;
    delete m_server;
    delete trayIcon;
}

void MainWindow::refreshRepoButtonPressed()
{
    // get currently selected repo
    QModelIndex i = ui->repoListView->currentIndex();
    if (i.isValid()){
        m_repoModel->get(i)->repo()->detectChanges();
    }
}

void MainWindow::addRepoButtonPressed()
{
    RepoDialog *repoDialog = new RepoDialog(this);

    if (repoDialog->exec()){
        addSynchronizer(repoDialog->name(),
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
    QModelIndex i = ui->repoListView->currentIndex();
    if (i.isValid()){
        m_repoModel->remove(i);
    }
}

bool MainWindow::addSynchronizer(QString name, QString local_path, QString tracker_url_string)
{
    Repo* repo = new LocalDiskRepo(this, local_path);
    if (!repo->isReady()){
        repo->initialize();
    }
    QUrl self_url = m_server->url();
    self_url.setPath(name);
    QUrl tracker_url(tracker_url_string);
    Synchronizer* s = new Synchronizer(this, name, repo, tracker_url, self_url);
    if (s->isReady()){
        m_repoModel->insert(s);
        qDebug() <<  "Successfully added repo:" << name << local_path << tracker_url_string;
        return true;
    }else{
        QMessageBox::critical(this, tr("Adding Repo Failed"),
                              tr("Failed to add Repo. Check that the Tracker Url and Local Path are correct"));
        return false;
    }

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
