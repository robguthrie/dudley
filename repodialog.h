#ifndef REPODIALOG_H
#define REPODIALOG_H

#include <QDialog>

namespace Ui {
    class RepoDialog;
}

class RepoDialog : public QDialog {
    Q_OBJECT
public:
    RepoDialog(QWidget *parent = 0);
    ~RepoDialog();
    QString localPath();
    QString name();
    QString trackerUrl();
signals:
    void newSettings(QString path);

protected:
    void changeEvent(QEvent *e);

private slots:
    void selectDirectory();
//    void emitSettings();

private:
    Ui::RepoDialog *ui;
};

#endif // REPODIALOG_H
