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

#include "settingsdialog.h"

#include <QDebug>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QUrl>
#include <QFileDialog>

SettingsDialog::SettingsDialog(QWidget *parent,
                               Plugins *plugins)
    : QDialog(parent)
    , _plugins(plugins)
    , _tabs(nullptr)
    , _applyButton(nullptr)
    , _cancelButton(nullptr)
    , _pluginPath(nullptr)
{
    if (!_plugins) { reject(); }

    setMinimumWidth(500);
    setObjectName("SettingsDialog");
    setWindowTitle(tr("Settings"));

    const auto mainLayout = new QVBoxLayout(this);

    _tabs = new QTabWidget(this);

    const auto buttonsWidget = new QWidget(this);
    const auto buttonsLayout = new QHBoxLayout(buttonsWidget);

    _applyButton = new QPushButton(tr("Apply"), this);
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

    mainLayout->addWidget(_tabs);
    mainLayout->addWidget(buttonsWidget);

    setupGeneral();
}

void SettingsDialog::setupGeneral()
{
    const auto generalWidget = new QWidget(this);
    const auto generalLayout = new QVBoxLayout(generalWidget);

    _tabs->addTab(generalWidget, tr("General"));

    const auto pluginPathEditWidget = new QWidget(this);
    const auto pluginPathEditLayout = new QHBoxLayout(pluginPathEditWidget);

    const auto pluginPathEditLabel = new QLabel(tr("Installation path"), this);
    _pluginPath = new QLineEdit(this);
    _pluginPath->setProperty("StyleEdit", true);
    _pluginPath->setText(_plugins->getUserPluginPath());

    const auto pluginPathEditButton = new QPushButton(tr("..."), this);
    pluginPathEditButton->setProperty("FileButton", true);
    connect(pluginPathEditButton,
            SIGNAL(released()),
            this,
            SLOT(handleSelectButton()));

    pluginPathEditLayout->addWidget(pluginPathEditLabel);
    pluginPathEditLayout->addStretch();
    pluginPathEditLayout->addWidget(_pluginPath);
    pluginPathEditLayout->addWidget(pluginPathEditButton);

    generalLayout->addWidget(pluginPathEditWidget);
    generalLayout->addStretch();
}

void SettingsDialog::handleApplyButton()
{
    bool changed = false;

    if (!_pluginPath->text().isEmpty() &&
        _pluginPath->text() != _plugins->getUserPluginPath())
    {
        _plugins->setUserPluginPath(_pluginPath->text());
        changed = true;
    }

    if (changed) { accept(); }
    else { reject(); }
}

void SettingsDialog::handleSelectButton()
{
    QString dir = QFileDialog::getExistingDirectory(this,
                                                    tr("Select directory"),
                                                    QDir::homePath(),
                                                    QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    if (!dir.isEmpty()) { _pluginPath->setText(dir); }
}
