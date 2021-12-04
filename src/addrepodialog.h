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

#ifndef ADDREPODIALOG_H
#define ADDREPODIALOG_H

#include <QDialog>
#include <QPushButton>
#include <QLineEdit>

#include "plugins.h"

class AddRepoDialog : public QDialog
{
    Q_OBJECT

public:

    explicit AddRepoDialog(QWidget *parent = nullptr,
                           Plugins *plugins = nullptr);

private:

    Plugins *_plugins;
    QLineEdit *_urlEdit;
    QPushButton *_applyButton;
    QPushButton *_cancelButton;
    bool _validRepo;

private slots:

    void handleApplyButton();
};

#endif // ADDREPODIALOG_H
