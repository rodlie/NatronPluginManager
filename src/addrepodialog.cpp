/*
#
# Natron Plug-in Manager
#
# Copyright (c) Ole-André Rodlie. All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
#
*/

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
    connect(_applyButton,
            SIGNAL(released()),
            this,
            SLOT(handleApplyButton()));

    _cancelButton = new QPushButton(tr("Cancel"), this);
    _cancelButton->setProperty("RemoveButton", true);
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
    if (url.isEmpty() || !url.isValid()) {
        QMessageBox::warning(this,
                             windowTitle(),
                             tr("Please provide a valid url to a repository manifest XML."));
        return;
    }
    _plugins->addDownloadUrl(url);
    accept();
}
