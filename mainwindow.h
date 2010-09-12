#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSystemTrayIcon>
#include <QMainWindow>
#include <server.h>
#include "repodialog.h"
#include "repotablemodel.h"
#include "filerepo.h"

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
    void removeRepoButtonPressed();
    void addRepoButtonPressed();
    bool addRepo(QString path, QString name = "noname existing repo");
    void removeRepo(FileRepo* repo);
private:
    Ui::MainWindow *ui;
    Server* server;
    QString bestIpAddress();
    QSystemTrayIcon *trayIcon;
    RepoTableModel *repoTableModel;

};

#endif // MAINWINDOW_H
