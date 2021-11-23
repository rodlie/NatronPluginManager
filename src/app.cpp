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

#include "app.h"

#include <QDebug>
#include <QFile>
#include <QIcon>
#include <QFont>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QKeySequence>
#include <QApplication>
#include <QTimer>
#include <QtConcurrentRun>
#include <QPalette>
#include <QLabel>
#include <QSettings>

#include "pluginlistwidget.h"

#define NATRON_STYLE ":/stylesheet.qss"

#define PLUGIN_LIST_ROLE_ID Qt::UserRole+1
#define PLUGIN_LIST_ROLE_GROUP Qt::UserRole+2

#define APP_STACK_LOADING 0
#define APP_STACK_PLUGINS 1

NatronPluginManager::NatronPluginManager(QWidget *parent)
    : QMainWindow(parent)
    , _comboStatus(nullptr)
    , _comboGroup(nullptr)
    , _stack(nullptr)
    , _plugins(nullptr)
    , _statusBar(nullptr)
    , _progBar(nullptr)
    , _menuBar(nullptr)
    , _pluginList(nullptr)
{

    setMinimumSize(getConfigWindowSize());
    setWindowIcon(QIcon(NATRON_ICON));

    setupStyle();
    setupPlugins();
    setupMenu();
    setupStatusBar();
    setupPluginsComboBoxes();
    setupPluginList();

    QWidget *mainWidget = new QWidget(this);
    mainWidget->setObjectName("MainWidget");
    mainWidget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *mainLayout = new QHBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setCentralWidget(mainWidget);

    QWidget *pluginsWidget = new QWidget(this);
    pluginsWidget->setObjectName("PluginsWidget");
    QVBoxLayout *pluginsWidgetLayout = new QVBoxLayout(pluginsWidget);

    QWidget *pluginsComboWidget = new QWidget(this);
    pluginsComboWidget->setObjectName("PluginsComboWidget");
    QHBoxLayout *pluginsComboWidgetLayout = new QHBoxLayout(pluginsComboWidget);

    QLabel *comboStatusLabel = new QLabel(tr("Status"), this);
    comboStatusLabel->setObjectName("ComboStatusLabel");

    QLabel *comboGroupLabel = new QLabel(tr("Groups"), this);
    comboGroupLabel->setObjectName("ComboGroupLabel");

    pluginsComboWidgetLayout->addWidget(comboStatusLabel);
    pluginsComboWidgetLayout->addWidget(_comboStatus);
    pluginsComboWidgetLayout->addWidget(comboGroupLabel);
    pluginsComboWidgetLayout->addWidget(_comboGroup);
    pluginsComboWidgetLayout->addStretch();

    pluginsWidgetLayout->addWidget(pluginsComboWidget);
    pluginsWidgetLayout->addWidget(_pluginList);

    QWidget *loadingWidget = new QWidget(this);
    QVBoxLayout *loadingWidgetLayout = new QVBoxLayout(loadingWidget);
    QLabel *loadingWidgetLabel = new QLabel("<h1 style=\"text-align: center;\">"
                                            "<img src=\":/NatronPluginManager.png\">"
                                            "<br>Natron<br>"
                                            "<span style=\"font-weight:normal;\">Plug-in Manager</span>"
                                            "</h1>", this);
    loadingWidgetLayout->addStretch();
    loadingWidgetLayout->addWidget(loadingWidgetLabel);
    loadingWidgetLayout->addStretch();

    _stack = new QStackedWidget(this);
    _stack->addWidget(loadingWidget);
    _stack->addWidget(pluginsWidget);

    _stack->setCurrentIndex(APP_STACK_LOADING);
    mainLayout->addWidget(_stack);

    QTimer::singleShot(100,
                       this,
                       SLOT(startup()));
}

NatronPluginManager::~NatronPluginManager()
{
}

const QSize NatronPluginManager::getConfigPluginIconSize()
{
    QSettings settings;
    return settings.value("PluginIconSize",
                          QSize(48,48)).toSize();
}

const QSize NatronPluginManager::getConfigPluginGridSize()
{
    QSettings settings;
    return settings.value("PluginGridSize",
                          QSize(330,160)).toSize();
}

const QSize NatronPluginManager::getConfigWindowSize()
{
    QSettings settings;
    return settings.value("WindowSize",
                          QSize(710,610)).toSize();
}

void NatronPluginManager::setupStyle()
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor("#232528"));
    palette.setColor(QPalette::WindowText, QColor("#ebebeb"));
    palette.setColor(QPalette::Base, QColor(15, 15, 15));
    palette.setColor(QPalette::AlternateBase, QColor("#232528"));
    palette.setColor(QPalette::Link, QColor("#ebebeb"));
    palette.setColor(QPalette::LinkVisited, QColor("#ebebeb"));
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, QColor("#ebebeb"));
    palette.setColor(QPalette::Button, QColor("#232528"));
    palette.setColor(QPalette::ButtonText, QColor("#ebebeb"));
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor("#2e82be"));
    palette.setColor(QPalette::HighlightedText, QColor("#ebebeb"));
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    qApp->setStyle(QString("fusion"));
    qApp->setPalette(palette);

    double pluginTitleFontSize = 16;
    double pluginGroupFontSize = 11;
#ifndef Q_OS_MAC
    pluginTitleFontSize = 12;
    pluginGroupFontSize = 9;
#endif

    QSettings settings;
    QFile styleFile(settings.value("stylesheet", NATRON_STYLE).toString());
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString stylesheet = styleFile.readAll();
        styleFile.close();
        qApp->setStyleSheet(stylesheet.arg(pluginTitleFontSize).arg(pluginGroupFontSize));
    }
}

void NatronPluginManager::setupPlugins()
{
    _plugins = new Plugins(this);
    connect(_plugins,
            SIGNAL(updatedPlugins()),
            this,
            SLOT(handleUpdatedPlugins()));
    connect(_plugins,
            SIGNAL(statusError(QString)),
            this,
            SLOT(handlePluginsStatusError(QString)));
    connect(_plugins,
            SIGNAL(statusDownload(QString,qint64,qint64)),
            this,
            SLOT(handleDownloadStatusMessage(QString,qint64,qint64)));
}

void NatronPluginManager::setupMenu()
{
    _menuBar = new QMenuBar(this);
    _menuBar->setObjectName("MenuBar");
    setMenuWidget(_menuBar);

    QMenu *fileMenu = new QMenu(tr("File"), this);
    _menuBar->addMenu(fileMenu);

    QAction *fileQuitAction = new QAction(tr("Quit"), this);
    fileQuitAction->setShortcut(QKeySequence(tr("Ctrl+Q")));
    fileMenu->addAction(fileQuitAction);
    connect(fileQuitAction,
            SIGNAL(triggered()),
            this,
            SLOT(close()));

    QMenu *helpMenu = new QMenu(tr("Help"), this);
    _menuBar->addMenu(helpMenu);

    QAction *helpAboutAction = new QAction(tr("About"), this);
    helpMenu->addAction(helpAboutAction);
    connect(helpAboutAction,
            SIGNAL(triggered()),
            this,
            SLOT(handleAboutActionTriggered()));

    QAction *helpAboutQtAction = new QAction(tr("About Qt"), this);
    helpMenu->addAction(helpAboutQtAction);
    connect(helpAboutQtAction,
            SIGNAL(triggered()),
            this,
            SLOT(handleAboutQtActionTriggered()));
}

void NatronPluginManager::setupStatusBar()
{
    _statusBar = new QStatusBar(this);
    _statusBar->setObjectName("StatusBar");
    _statusBar->setSizeGripEnabled(false);
    connect(_plugins,
            SIGNAL(statusMessage(QString)),
            this,
            SLOT(handlePluginsStatusMessage(QString)));

    setStatusBar(_statusBar);

    _progBar = new QProgressBar(this);
    _progBar->setObjectName("ProgressBar");
    _progBar->setMaximumWidth(100);
    _progBar->setRange(0, 1);
    _progBar->setValue(1);
    _progBar->setFormat("");
    _progBar->hide();

    _statusBar->addPermanentWidget(_progBar);
}

void NatronPluginManager::setupPluginsComboBoxes()
{
    _comboStatus = new QComboBox(this);
    _comboStatus->setObjectName("ComboStatus");
    _comboStatus->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _comboStatus->addItem(tr("All"), Plugins::NATRON_PLUGIN_TYPE_NONE);
    _comboStatus->insertSeparator(1);
    _comboStatus->addItem(tr("Available"), Plugins::NATRON_PLUGIN_TYPE_AVAILABLE);
    _comboStatus->addItem(tr("Installed"), Plugins::NATRON_PLUGIN_TYPE_INSTALLED);
    //_comboStatus->addItem(tr("Updates"), Plugins::NATRON_PLUGIN_TYPE_UPDATE);
    _comboStatus->setEnabled(false);

    _comboGroup = new QComboBox(this);
    _comboGroup->setObjectName("ComboGroup");
    _comboGroup->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _comboGroup->setEnabled(false);

    connect(_comboGroup,
            SIGNAL(currentTextChanged(QString)),
            this,
            SLOT(handleComboGroupChanged(QString)));
    connect(_comboStatus,
            SIGNAL(currentTextChanged(QString)),
            this,
            SLOT(handleComboStatusChanged(QString)));
}

void NatronPluginManager::setupPluginList()
{
    _pluginList = new QListWidget(this);
    _pluginList->setObjectName("PluginList");
    _pluginList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pluginList->setFrameShape(QFrame::NoFrame);
    _pluginList->setViewMode(QListView::IconMode);
    _pluginList->setGridSize(getConfigPluginGridSize());
    _pluginList->setResizeMode(QListView::Adjust);
    _pluginList->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

void NatronPluginManager::startup()
{
    QtConcurrent::run(_plugins, &Plugins::loadRepositories);
}

void NatronPluginManager::handleUpdatedPlugins()
{
    qDebug() << "AVAILABLE PLUGINS" << _plugins->getAvailablePlugins().size();
    qDebug() << "INSTALLED PLUGINS" << _plugins->getInstalledPlugins().size();

    populatePlugins();
}

void NatronPluginManager::handleAboutActionTriggered()
{
    QString title = QString("%1 v%2").arg(qApp->applicationDisplayName(),
                                          qApp->applicationVersion());
    QString lic = tr("This program is free software; you can redistribute it and/or modify it "
                     "under the terms of the GNU General Public License as published by the "
                     "Free Software Foundation; either version 2 of the License, "
                     "or (at your option) any later version.");
    QString text = tr("<h3 style=\"font-weight:normal;\">%1</h3>"
                      "<p>A plug-in (<a href=\"https://github.com/NatronGitHub/natron-plugins\">PyPlug</a>) manager for <a href=\"https://natrongithub.github.io\">Natron</a>.</p>"
                      "<p style=\"font-size:small;\">%2</p>"
                      "<p style=\"font-size:small;\">Copyright &copy; <a href=\"https://github.com/rodlie\">Ole-André Rodlie</a>. All rights reserved.</p>")
                   .arg(title, lic);
    QMessageBox::about(this, tr("About"), text);
}

void NatronPluginManager::handleAboutQtActionTriggered()
{
    QMessageBox::aboutQt(this, tr("About Qt"));
}

void NatronPluginManager::handlePluginsStatusError(const QString &message)
{
    if (message.isEmpty()) { return; }
    QMessageBox::warning(this, tr("Failure"), message);
}

void NatronPluginManager::handlePluginsStatusMessage(const QString &message)
{
    if (!_statusBar) { return; }
    _statusBar->showMessage(message, 1000);
}

void NatronPluginManager::handleDownloadStatusMessage(const QString &message,
                                                      qint64 value,
                                                      qint64 total)
{
    if (_progBar->isHidden()) { _progBar->show(); }
    _statusBar->showMessage(message, 1000);
    _progBar->setRange(0, total);
    _progBar->setValue(value);
    if (value == total && _progBar->isVisible()) { _progBar->hide(); }
}

void NatronPluginManager::populatePlugins()
{
    std::vector<Plugins::PluginSpecs> plugins =  _plugins->getPlugins();
    QStringList groups = _plugins->getPluginGroups();

    _comboGroup->clear();
    _comboGroup->addItem(tr("All"));
    _comboGroup->insertSeparator(1);
    _comboGroup->addItems(groups);
    _comboGroup->adjustSize();

    _comboGroup->setEnabled(true);
    _comboStatus->setEnabled(true);

    _pluginList->clear();

    for (unsigned long i = 0; i< plugins.size(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(); // the list takes ownership
        Plugins::PluginSpecs plugin = plugins.at(i);
        item->setFlags(Qt::NoItemFlags);
        item->setData(PLUGIN_LIST_ROLE_GROUP, plugin.group);
        item->setData(PLUGIN_LIST_ROLE_ID, plugin.id);
        Plugins::PluginType type = Plugins::NATRON_PLUGIN_TYPE_NONE;
        if (_plugins->hasAvailablePlugin(plugin.id)) {
            type = Plugins::NATRON_PLUGIN_TYPE_AVAILABLE;
        } else if (_plugins->hasInstalledPlugin(plugin.id)) {
            type = Plugins::NATRON_PLUGIN_TYPE_INSTALLED;
        }
        PluginListWidget *pwidget = new PluginListWidget(plugin,
                                                         type,
                                                         _pluginList->gridSize(),
                                                         getConfigPluginIconSize()); // the list takes ownership
        connect(pwidget,
                SIGNAL(pluginButtonReleased(QString,int)),
                this,
                SLOT(handlePluginButtonReleased(QString,int)));
        connect(this,
                SIGNAL(pluginStatusChanged(QString,int)),
                pwidget,
                SLOT(setPluginStatus(QString,int)));

        item->setSizeHint(_pluginList->gridSize());
        _pluginList->addItem(item);
        _pluginList->setItemWidget(item, pwidget);
    }

    _stack->setCurrentIndex(APP_STACK_PLUGINS);
}

void NatronPluginManager::handleComboStatusChanged(const QString &status)
{
    filterPluginsStatus(status);
}

void NatronPluginManager::handleComboGroupChanged(const QString &group)
{
    filterPluginsGroup(group);
}

void NatronPluginManager::filterPluginsStatus(const QString &status)
{
    for (int i = 0; i < _pluginList->count(); ++i) {
        QListWidgetItem *item = _pluginList->item(i);
        bool visible = true;
        if (status == tr("Available")) {
            visible = _plugins->hasAvailablePlugin(item->data(PLUGIN_LIST_ROLE_ID).toString());
        } else if (status == tr("Installed")) {
            visible = _plugins->hasInstalledPlugin(item->data(PLUGIN_LIST_ROLE_ID).toString());
        }
        item->setHidden(!visible);
    }
}

void NatronPluginManager::filterPluginsGroup(const QString &group)
{
    for (int i = 0; i < _pluginList->count(); ++i) {
        QListWidgetItem *item = _pluginList->item(i);
        bool hasGroup = (item->data(PLUGIN_LIST_ROLE_GROUP).toString() == group || group == tr("All"));
        item->setHidden(group.isEmpty() ? true : !hasGroup);
    }
}

void NatronPluginManager::handlePluginButtonReleased(const QString &id,
                                                     int type)
{
    if (!_plugins->hasPlugin(id)) { return; }
    switch (type) {
    case Plugins::NATRON_PLUGIN_TYPE_AVAILABLE:
        installPlugin(id);
        break;
    case Plugins::NATRON_PLUGIN_TYPE_INSTALLED:
        removePlugin(id);
        break;
    default:;
    }
}

void NatronPluginManager::installPlugin(const QString &id)
{
    Plugins::PluginStatus status = _plugins->installPlugin(id);
    if (!status.success) {
        QMessageBox::warning(this, tr("Install"), status.message);
    } else {
        emit pluginStatusChanged(id, Plugins::NATRON_PLUGIN_TYPE_INSTALLED);
        QtConcurrent::run(_plugins,
                          &Plugins::checkRepositories,
                          false);
    }
}

void NatronPluginManager::removePlugin(const QString &id)
{
    Plugins::PluginStatus status = _plugins->removePlugin(id);
    if (!status.success) {
        QMessageBox::warning(this, tr("Remove"), status.message);
    } else {
        emit pluginStatusChanged(id, Plugins::NATRON_PLUGIN_TYPE_AVAILABLE);
        QtConcurrent::run(_plugins,
                          &Plugins::checkRepositories,
                          false);
    }
}

void NatronPluginManager::closeEvent(QCloseEvent *e)
{
    if (_plugins->isBusy()) {
        QMessageBox::warning(this,
                             tr("Unable to quit"),
                             tr("The application is busy, please try again later."));
        e->ignore();
    } else {
        e->accept();
    }
}

