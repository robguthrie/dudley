#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSystemTrayIcon>
#include <QMainWindow>
#include <QSettings>
#include "httpserver.h"
#include "repodialog.h"
#include "repomodel.h"
#include "repo.h"
#include "filetransfermanager.h"
#include "output.h"

namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QSettings* settings, QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private slots:
    void refreshRepoButtonPressed();
    void removeRepoButtonPressed();
    void addRepoButtonPressed();
    void editRepoButtonPressed();
    bool addRepoModelItem(QString name, QString local_path, QString tracker_url);

private:
    QString m_selfUrl;
    Ui::MainWindow *ui;
    HttpServer* m_server;
    QString bestIpAddress();
    QSystemTrayIcon *trayIcon;
    RepoModel m_repoModel;
    QSettings* m_settings;
    FileTransferManager* m_fileTransferManager;
    void writeSettings();
    void readSettings();
};

#endif // MAINWINDOW_H
