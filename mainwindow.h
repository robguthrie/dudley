#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSystemTrayIcon>
#include <QMainWindow>
#include <server.h>
#include "repodialog.h"
#include "repotablemodel.h"
#include "filerepo.h"
#include <QSettings>
namespace Ui {
    class MainWindow;
}

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void changeEvent(QEvent *e);

private slots:
    void refreshRepoButtonPressed();
    void removeRepoButtonPressed();
    void addRepoButtonPressed();
    void editRepoButtonPressed();
    bool addRepo(QString type, QString path, QString name);

private:
    Ui::MainWindow *ui;
    Server* server;
    QString bestIpAddress();
    QSystemTrayIcon *trayIcon;
    RepoModel *m_repoModel;
    QSettings m_settings;
    void writeSettings();
    void readSettings();
};

#endif // MAINWINDOW_H
