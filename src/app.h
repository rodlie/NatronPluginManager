/*
#
# Natron Plug-in Manager
#
# Copyright (c) 2021 Ole-André Rodlie. All rights reserved.
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

#ifndef NATRONPLUGINMANAGER_H
#define NATRONPLUGINMANAGER_H

#include "plugins.h"

#include <QMainWindow>
#include <QStatusBar>
#include <QProgressBar>
#include <QMenuBar>
#include <QCloseEvent>
#include <QListWidget>
#include <QStackedWidget>
#include <QComboBox>

class NatronPluginManager : public QMainWindow
{
    Q_OBJECT

public:

    NatronPluginManager(QWidget *parent = nullptr);
    ~NatronPluginManager();

    const QSize getConfigPluginIconSize();
    const QSize getConfigPluginGridSize();
    const QSize getConfigWindowSize();

signals:

    void pluginStatusChanged(const QString &id,
                             int type);

private:

    QComboBox *_comboStatus;
    QComboBox *_comboGroup;
    QStackedWidget *_stack;
    Plugins *_plugins;
    QStatusBar *_statusBar;
    QProgressBar *_progBar;
    QMenuBar *_menuBar;
    QListWidget *_pluginList;

private slots:

    void setupStyle();
    void setupPlugins();
    void setupMenu();
    void setupStatusBar();
    void setupPluginsComboBoxes();
    void setupPluginList();

    void startup();
    void handleUpdatedPlugins();
    void handleAboutActionTriggered();
    void handleAboutQtActionTriggered();
    void handlePluginsStatusError(const QString &message);
    void handlePluginsStatusMessage(const QString &message);
    void handleDownloadStatusMessage(const QString &message,
                                     qint64 value,
                                     qint64 total);
    void populatePlugins();

    void handleComboStatusChanged(const QString &status);
    void handleComboGroupChanged(const QString &group);

    void filterPluginsStatus(const QString &status);
    void filterPluginsGroup(const QString &group);

    void handlePluginButtonReleased(const QString &id,
                                    int type);
    void installPlugin(const QString &id);
    void removePlugin(const QString &id);

protected:

     void closeEvent(QCloseEvent *e);
};
#endif // NATRONPLUGINMANAGER_H
