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
#include <QTreeWidget>
//#include <QTextBrowser>
//#include <QLabel>
//#include <QTabWidget>
//#include <QStatusBar>
//#include <QProgressBar>
//#include <QPushButton>
#include <QMenuBar>
#include <QCloseEvent>
#include <QListWidget>
#include <QStackedWidget>
#include <QToolBar>
#include <QComboBox>

class NatronPluginManager : public QMainWindow
{
    Q_OBJECT

public:

    NatronPluginManager(QWidget *parent = nullptr);
    ~NatronPluginManager();
    void installPlugins();
    void removePlugins();
    bool isPluginTreeItemChecked(QTreeWidgetItem *item);
    void setDefaultPluginInfo();
    const QStringList getCheckedPlugins(QTreeWidget* tree);
    const QStringList getCheckedAvailablePlugins();
    const QStringList getCheckedInstalledPlugins();

private:

    QComboBox *_comboStatus;
    QComboBox *_comboGroup;
    QToolBar *_toolBar;
    QStackedWidget *_stack;
    QString _appDoc;
    Plugins *_plugins;
    QTreeWidget *_groupTree;
   // QTreeWidget *_availablePluginsTree;
   // QTreeWidget *_installedPluginsTree;
    //QTextBrowser *_pluginDesc;
    //QLabel *_pluginLabel;
    //QLabel *_pluginIcon;
//    QTabWidget *_leftTab;
  //  QStatusBar *_statusBar;
  //  QProgressBar *_progBar;
   // QPushButton *_installButton;
   // QPushButton *_removeButton;
  //  QPushButton *_updateButton;
    QMenuBar *_menuBar;
    QListWidget *_pluginsTree;

    QWidget *generatePluginWidget(const Plugins::PluginSpecs &plugin);

private slots:

    void setupStyle();
    void setupPalette();
    void setupPlugins();
    void setupMenu();
    void setupStatusBar();
    void setupButtons();
    void setupTreeWidget(QTreeWidget *tree);
    void setupPluginsTab();
    void setupPluginInfo();

    void startup();
    void loadRepositories();
    void handleUpdatedPlugins();
    void handleAboutActionTriggered();
    void handleAboutQtActionTriggered();
    void handlePluginsStatusError(const QString &message);
    void handlePluginsStatusMessage(const QString &message);
    void handleDownloadStatusMessage(const QString &message,
                                     qint64 value,
                                     qint64 total);
    void handleItemActivated(QTreeWidgetItem *item,
                             QTreeWidgetItem* prev);
    void handleInstallButton();
    void handleRemoveButton();
    void updatePlugins();
    void populatePluginsTree(Plugins::PluginType type,
                             QTreeWidget *tree);

    void populatePlugins(const QString &group = QString());

    void handleComboGroup(const QString &group);
    void filterPluginsGroup(const QString &group);

protected:

     void closeEvent(QCloseEvent *e);
};
#endif // NATRONPLUGINMANAGER_H
