#include "addrepodialog.h"

#include <QDebug>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QApplication>
#include <QStyle>
#include <QMessageBox>
#include <QUrl>

AddRepoDialog::AddRepoDialog(QWidget *parent,
                             Plugins *plugins)
    : QDialog(parent)
    , _plugins(plugins)
    , _urlEdit(nullptr)
    , _applyButton(nullptr)
    , _cancelButton(nullptr)
    , _validRepo(false)
{
    if (!_plugins) { reject(); }

    setMinimumWidth(500);
    setObjectName("AddRepoDialog");
    setWindowTitle(tr("Add repository"));

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *urlEditWidget = new QWidget(this);
    QHBoxLayout *urlEditLayout = new QHBoxLayout(urlEditWidget);

    QLabel *urlEditLabel = new QLabel(tr("Manifest URL"), this);
    _urlEdit = new QLineEdit(this);
    _urlEdit->setPlaceholderText("https://");

    urlEditLayout->addWidget(urlEditLabel);
    urlEditLayout->addWidget(_urlEdit);

    QWidget *buttonsWidget = new QWidget(this);
    QHBoxLayout *buttonsLayout = new QHBoxLayout(buttonsWidget);

    _applyButton = new QPushButton(tr("Add"), this);
    _applyButton->setProperty("InstallButton", true);
    //_applyButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogOkButton));
    connect(_applyButton,
            SIGNAL(released()),
            this,
            SLOT(handleApplyButton()));

    _cancelButton = new QPushButton(tr("Cancel"), this);
    _cancelButton->setProperty("RemoveButton", true);
    //_cancelButton->setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton));
    connect(_cancelButton,
            SIGNAL(released()),
            this,
            SLOT(reject()));

    buttonsLayout->addStretch();
    buttonsLayout->addWidget(_applyButton);
    buttonsLayout->addSpacing(5);
    buttonsLayout->addWidget(_cancelButton);

    mainLayout->addWidget(urlEditWidget);
    mainLayout->addWidget(buttonsWidget);
}

void AddRepoDialog::handleApplyButton()
{
    QUrl url = QUrl::fromUserInput(_urlEdit->text());
    qDebug() << "add repo?" << url;
    if (url.isEmpty() || !url.isValid()) { return; }
    _plugins->addDownloadUrl(url);
}
