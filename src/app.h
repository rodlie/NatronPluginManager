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

#ifndef NATRONPLUGINMANAGER_H
#define NATRONPLUGINMANAGER_H

#include <QMainWindow>
#include <QMenuBar>
#include <QCloseEvent>
#include <QListWidget>
#include <QStackedWidget>
#include <QComboBox>
#include <QSize>
#include <QByteArray>
#include <QStatusBar>
#include <QProgressBar>
#include <QLabel>
#include <QLineEdit>

#include "plugins.h"
#include "pluginviewwidget.h"

class NatronPluginManager : public QMainWindow
{
    Q_OBJECT

public:

    NatronPluginManager(QWidget *parent = nullptr);
    ~NatronPluginManager();

    const QSize getConfigPluginIconSize();
    void setConfigPluginIconSize(int iconSize);

    const QSize getConfigPluginLargeIconSize();
    void setConfigPluginLargeIconSize(int iconSize);

    const QSize getConfigPluginGridSize();
    void setConfigPluginGridSize(QSize gridSize);

    const QByteArray getConfigWindowGeometry();
    const QByteArray getConfigWindowState();
    bool getConfigWindowIsMaximized();

    void saveWindowConfig();

signals:

    void pluginStatusChanged(const QString &id,
                             int type);

private:

    QComboBox *_comboStatus;
    QComboBox *_comboGroup;
    QLineEdit *_lineEdit;
    QStackedWidget *_stack;
    int _stackListIndex;
    int _stackViewIndex;
    Plugins *_plugins;
    QMenuBar *_menuBar;
    QListWidget *_pluginList;
    PluginViewWidget *_pluginView;
    QStatusBar *_statusBar;
    QProgressBar *_progBar;
    QLabel *_availableLabel;
    QLabel *_installedLabel;
    QLabel *_updatesLabel;
    QLabel *_cacheLabel;

private slots:

    void setupStyle();
    void setupPlugins();
    void setupMenu();
    void setupPluginsComboBoxes();
    void setupPluginList();
    void setupStatus();

    void startup();
    void handleUpdatedPlugins();
    void updatePluginStatusLabels();
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

    void updateFilterPlugins();
    void filterPlugins(const QString &status,
                       const QString &group,
                       const QString &filter = QString());

    void handlePluginButtonReleased(const QString &id,
                                    int type);

    void installPlugin(const QString &id);
    void removePlugin(const QString &id);
    void updatePlugin(const QString &id);

    void openAddRepoDialog();
    void openSettingsDialog();

    void updateSettings();

    void showPlugins();
    void showPlugin(const QString &id = QString());

protected:

     void closeEvent(QCloseEvent *e);
};
#endif // NATRONPLUGINMANAGER_H
