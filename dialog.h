#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include "server.h"

class QLabel;
class QPushButton;

class Dialog : public QDialog
{
    Q_OBJECT
public:
    Dialog(QWidget *parent = 0);

private:
    QLabel *statusLabel;
    QPushButton *quitButton;
    Server server;
};

#endif // DIALOG_H
