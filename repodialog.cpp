#include "repodialog.h"
#include "ui_repodialog.h"
#include <QFileDialog>

RepoDialog::RepoDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RepoDialog)
{
    ui->setupUi(this);
    connect(ui->selectDirectoryToolButton, SIGNAL(clicked()), this, SLOT(selectDirectory()));
}

RepoDialog::~RepoDialog()
{
    delete ui;
}

QString RepoDialog::name()
{
    return ui->nameLineEdit->text();
}

QString RepoDialog::localPath()
{
    return ui->localPathLineEdit->text();
}

QString RepoDialog::trackerUrl()
{
    return ui->trackerUrlLineEdit->text();
}

void RepoDialog::selectDirectory()
{
    ui->localPathLineEdit->setText(
        QFileDialog::getExistingDirectory(this, tr("Open Directory"), "/home",
                                         QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks));
}

void RepoDialog::changeEvent(QEvent *e)
{
    QDialog::changeEvent(e);
    switch (e->type()) {
    case QEvent::LanguageChange:
        ui->retranslateUi(this);
        break;
    default:
        break;
    }
}
