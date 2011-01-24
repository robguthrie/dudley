/********************************************************************************
** Form generated from reading UI file 'repodialog.ui'
**
** Created: Sun Jan 16 21:18:45 2011
**      by: Qt User Interface Compiler version 4.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_REPODIALOG_H
#define UI_REPODIALOG_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_RepoDialog
{
public:
    QVBoxLayout *verticalLayout;
    QLabel *label_3;
    QFormLayout *formLayout;
    QLabel *label_4;
    QComboBox *typeComboBox;
    QLabel *label;
    QHBoxLayout *horizontalLayout;
    QLineEdit *localPathLineEdit;
    QToolButton *selectDirectoryToolButton;
    QLabel *label_2;
    QLineEdit *nameLineEdit;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *RepoDialog)
    {
        if (RepoDialog->objectName().isEmpty())
            RepoDialog->setObjectName(QString::fromUtf8("RepoDialog"));
        RepoDialog->resize(400, 212);
        verticalLayout = new QVBoxLayout(RepoDialog);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        label_3 = new QLabel(RepoDialog);
        label_3->setObjectName(QString::fromUtf8("label_3"));

        verticalLayout->addWidget(label_3);

        formLayout = new QFormLayout();
        formLayout->setObjectName(QString::fromUtf8("formLayout"));
        label_4 = new QLabel(RepoDialog);
        label_4->setObjectName(QString::fromUtf8("label_4"));

        formLayout->setWidget(0, QFormLayout::LabelRole, label_4);

        typeComboBox = new QComboBox(RepoDialog);
        typeComboBox->setObjectName(QString::fromUtf8("typeComboBox"));

        formLayout->setWidget(0, QFormLayout::FieldRole, typeComboBox);

        label = new QLabel(RepoDialog);
        label->setObjectName(QString::fromUtf8("label"));

        formLayout->setWidget(1, QFormLayout::SpanningRole, label);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        localPathLineEdit = new QLineEdit(RepoDialog);
        localPathLineEdit->setObjectName(QString::fromUtf8("localPathLineEdit"));

        horizontalLayout->addWidget(localPathLineEdit);

        selectDirectoryToolButton = new QToolButton(RepoDialog);
        selectDirectoryToolButton->setObjectName(QString::fromUtf8("selectDirectoryToolButton"));

        horizontalLayout->addWidget(selectDirectoryToolButton);


        formLayout->setLayout(2, QFormLayout::FieldRole, horizontalLayout);

        label_2 = new QLabel(RepoDialog);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        formLayout->setWidget(3, QFormLayout::SpanningRole, label_2);

        nameLineEdit = new QLineEdit(RepoDialog);
        nameLineEdit->setObjectName(QString::fromUtf8("nameLineEdit"));

        formLayout->setWidget(4, QFormLayout::FieldRole, nameLineEdit);


        verticalLayout->addLayout(formLayout);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(RepoDialog);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(RepoDialog);
        QObject::connect(buttonBox, SIGNAL(rejected()), RepoDialog, SLOT(reject()));
        QObject::connect(buttonBox, SIGNAL(accepted()), RepoDialog, SLOT(accept()));

        QMetaObject::connectSlotsByName(RepoDialog);
    } // setupUi

    void retranslateUi(QDialog *RepoDialog)
    {
        RepoDialog->setWindowTitle(QApplication::translate("RepoDialog", "Dialog", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("RepoDialog", "Add Repo", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("RepoDialog", "Type", 0, QApplication::UnicodeUTF8));
        typeComboBox->clear();
        typeComboBox->insertItems(0, QStringList()
         << QApplication::translate("RepoDialog", "WorkingFileRepo", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("RepoDialog", "HttpClientFileRepo", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("RepoDialog", "s3FileRepo", 0, QApplication::UnicodeUTF8)
        );
        label->setText(QApplication::translate("RepoDialog", "Path", 0, QApplication::UnicodeUTF8));
        selectDirectoryToolButton->setText(QApplication::translate("RepoDialog", "...", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("RepoDialog", "Name", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class RepoDialog: public Ui_RepoDialog {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_REPODIALOG_H
