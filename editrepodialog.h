#ifndef EDITREPODIALOG_H
#define EDITREPODIALOG_H

#include <QDialog>
class Synchronizer;
namespace Ui {
    class EditRepoDialog;
}

class EditRepoDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EditRepoDialog(QWidget *parent, Synchronizer* sync);
    ~EditRepoDialog();

private:
    Ui::EditRepoDialog *ui;
};

#endif // EDITREPODIALOG_H
