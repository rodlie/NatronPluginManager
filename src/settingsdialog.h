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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>
#include <QTabWidget>

#include "plugins.h"

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:

    explicit SettingsDialog(QWidget *parent = nullptr,
                            Plugins *plugins = nullptr);

private:

    Plugins *_plugins;
    QTabWidget *_tabs;

    QPushButton *_applyButton;
    QPushButton *_cancelButton;

    QLineEdit *_pluginPath;

    void setupGeneral();

private slots:

    void handleApplyButton();
    void handleSelectButton();
};

#endif // SETTINGSDIALOG_H
