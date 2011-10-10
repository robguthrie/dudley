#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSystemTrayIcon>
#include <QMainWindow>
#include <QSettings>
#include "httpserver.h"
#include "repomodel.h"
#include "repo.h"
#include "filetransfermanager.h"
#include "output.h"
#include "upnpwrapper.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void listenOnPort(int port);
    void changeEvent(QEvent *e);

private slots:
    void onViewRepoButtonClicked();
    void onSaveSettingsButtonPressed();
    void onPortRedirected(bool success);
    void refreshRepoButtonPressed();
    void removeRepoButtonPressed();
    void addRepoButtonPressed();
    void editRepoButtonPressed();
    bool addSynchronizer(QString name, QString local_path, QString tracker_url);

private:
    UpnpWrapper *m_upnp;
    QString m_selfUrl;
    Ui::MainWindow *ui;
    HttpServer* m_server;
    QString bestIpAddress();
    QSystemTrayIcon *trayIcon;
    RepoModel* m_repoModel;
    QSettings* m_settings;
    int m_listenPort;
    FileTransferManager* m_fileTransferManager;
    void writeSettings();
    void readSettings();
};

#endif // MAINWINDOW_H
