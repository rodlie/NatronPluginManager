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

#ifndef PLUGINVIEWWIDGET_H
#define PLUGINVIEWWIDGET_H

#include <QWidget>
#include <QString>
#include <QPushButton>
#include <QLabel>
#include <QTextBrowser>

#include "plugins.h"

class PluginViewWidget : public QWidget
{
    Q_OBJECT

public:

    explicit PluginViewWidget(QWidget *parent = nullptr,
                              Plugins *plugins = nullptr,
                              QSize iconSize = QSize(64, 64));

signals:

    void goBack();

public slots:

    void showPlugin(const QString &id);

private:

    Plugins *_plugins;
    QPushButton *_goBackButton;
    QLabel *_pluginIconLabel;
    QLabel *_pluginTitleLabel;
    QLabel *_pluginGroupLabel;
    QTextBrowser *_pluginDescBrowser;
    QSize _iconSize;

private slots:

    void handleGoBackButton();
};

#endif // PLUGINVIEWWIDGET_H
