#include "editrepodialog.h"
#include "ui_editrepodialog.h"
#include "synchronizer.h"
#include "repo.h"
EditRepoDialog::EditRepoDialog(QWidget *parent, Synchronizer *sync) :
    QDialog(parent),
    ui(new Ui::EditRepoDialog)
{
    ui->setupUi(this);
    ui->nameLabel->setText(sync->name());
    ui->localPathLabel->setText(sync->repo()->path());
    QString link_str("<a href=\"%1\">%1</a>");
    ui->localUrlLabel->setText(link_str.arg(sync->selfUrl().toString()));
    ui->trackerUrlLabel->setText(link_str.arg(sync->trackerUrl().toString()));
    ui->historyRecievedAtLabel->setText(sync->historyReceivedAt().toString());
//    ui->syncIsSyncedLabel;
//    ui->syncFilesToPull;
//    ui->repoLastCheckedForChangesLabel;
//    ui->syncAutoLabel;
}

EditRepoDialog::~EditRepoDialog()
{
    delete ui;
}
