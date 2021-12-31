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

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    _tabs = new QTabWidget(this);

    QWidget *buttonsWidget = new QWidget(this);
    QHBoxLayout *buttonsLayout = new QHBoxLayout(buttonsWidget);

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
    QWidget *generalWidget = new QWidget(this);
    QVBoxLayout *generalLayout = new QVBoxLayout(generalWidget);

    _tabs->addTab(generalWidget, tr("General"));

    QWidget *pluginPathEditWidget = new QWidget(this);
    QHBoxLayout *pluginPathEditLayout = new QHBoxLayout(pluginPathEditWidget);

    QLabel *pluginPathEditLabel = new QLabel(tr("Installation path"), this);
    _pluginPath = new QLineEdit(this);
    _pluginPath->setText(_plugins->getUserPluginPath());

    pluginPathEditLayout->addWidget(pluginPathEditLabel);
    pluginPathEditLayout->addStretch();
    pluginPathEditLayout->addWidget(_pluginPath);

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
