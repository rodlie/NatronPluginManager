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

#ifndef PLUGINLISTWIDGET_H
#define PLUGINLISTWIDGET_H

#include <QWidget>
#include <QSize>
#include <QPushButton>
#include <QMouseEvent>

#include "plugins.h"

class PluginListWidget : public QWidget
{
    Q_OBJECT

public:

    explicit PluginListWidget(const Plugins::PluginSpecs &plugin,
                              Plugins::PluginType type,
                              QSize widgetSize,
                              QSize iconSize,
                              QWidget *parent = nullptr);
   ~PluginListWidget();

signals:

    void pluginButtonReleased(QString id,
                              int type);
    void showPlugin(const QString &id);

public slots:

    void setPluginStatus(const QString &id,
                         int type);

private:

    Plugins::PluginSpecs _plugin;
    QPushButton *_installButton;
    QPushButton *_removeButton;
    QPushButton *_updateButton;

private slots:

    void handleInstallButtonReleased();
    void handleRemoveButtonReleased();
    void handleUpdateButtonReleased();

protected:

    void mouseReleaseEvent(QMouseEvent *e);
};

#endif // PLUGINLISTWIDGET_H
