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
//#include <QTimer>
#include <QtConcurrentRun>
#include <QPalette>
#include <QSettings>
#include <QLocale>
#include <QShortcut>

#include "pluginlistwidget.h"
#include "addrepodialog.h"
#include "settingsdialog.h"

#define APP_STYLE ":/stylesheet.qss"

#define PLUGIN_LIST_ROLE_ID Qt::UserRole + 1
#define PLUGIN_LIST_ROLE_GROUP Qt::UserRole + 2

NatronPluginManager::NatronPluginManager(QWidget *parent)
    : QMainWindow(parent)
    , _comboStatus(nullptr)
    , _comboGroup(nullptr)
    , _lineEdit(nullptr)
    , _stack(nullptr)
    , _stackListIndex(0)
    , _stackViewIndex(0)
    , _plugins(nullptr)
    , _menuBar(nullptr)
    , _pluginList(nullptr)
    , _pluginView(nullptr)
    , _statusBar(nullptr)
    , _progBar(nullptr)
    , _availableLabel(nullptr)
    , _installedLabel(nullptr)
    , _updatesLabel(nullptr)
    , _cacheLabel(nullptr)
{
#ifdef Q_OS_DARWIN
    setWindowTitle(tr("Natron Plug-in Manager"));
#endif
    setWindowIcon(QIcon(DEFAULT_ICON));

    setupStyle();
    setupPlugins();
    setupMenu();
    setupPluginsComboBoxes();
    setupPluginList();
    setupStatus();

    const auto mainWidget = new QWidget(this);
    mainWidget->setObjectName("MainWidget");
    mainWidget->setContentsMargins(0, 0, 0, 0);
    const auto mainLayout = new QHBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    setCentralWidget(mainWidget);

    const auto pluginsWidget = new QWidget(this);
    pluginsWidget->setObjectName("PluginsWidget");
    const auto pluginsWidgetLayout = new QVBoxLayout(pluginsWidget);

    const auto pluginsComboWidget = new QWidget(this);
    pluginsComboWidget->setObjectName("PluginsComboWidget");
    const auto pluginsComboWidgetLayout = new QHBoxLayout(pluginsComboWidget);

    const auto comboStatusLabel = new QLabel(tr("Status"), this);
    comboStatusLabel->setObjectName("ComboStatusLabel");

    const auto comboGroupLabel = new QLabel(tr("Groups"), this);
    comboGroupLabel->setObjectName("ComboGroupLabel");

    const auto comboSearchLabel = new QLabel(tr("Search"), this);
    comboSearchLabel->setObjectName("ComboSearchLabel");

    pluginsComboWidgetLayout->addWidget(comboStatusLabel);
    pluginsComboWidgetLayout->addWidget(_comboStatus);
    pluginsComboWidgetLayout->addWidget(comboGroupLabel);
    pluginsComboWidgetLayout->addWidget(_comboGroup);
    pluginsComboWidgetLayout->addStretch();
    pluginsComboWidgetLayout->addWidget(comboSearchLabel);
    pluginsComboWidgetLayout->addWidget(_lineEdit);

    pluginsWidgetLayout->addWidget(pluginsComboWidget);
    pluginsWidgetLayout->addWidget(_pluginList);

    _stack = new QStackedWidget(this);
    _stackListIndex = _stack->addWidget(pluginsWidget);
    _stackViewIndex = _stack->addWidget(_pluginView);

    mainLayout->addWidget(_stack);

/*    QTimer::singleShot(0,
                       this,
                       SLOT(startup()));*/
    startup();
}

NatronPluginManager::~NatronPluginManager()
{
    saveWindowConfig();
}

const QSize NatronPluginManager::getConfigPluginIconSize()
{
    QSettings settings;
    return settings.value(PLUGINS_SETTINGS_ICON_SIZE,
                          QSize(PLUGINS_SETTINGS_ICON_SIZE_DEFAULT,
                                PLUGINS_SETTINGS_ICON_SIZE_DEFAULT)).toSize();
}

void NatronPluginManager::setConfigPluginIconSize(int iconSize)
{
    QSettings settings;
    settings.setValue(PLUGINS_SETTINGS_ICON_SIZE,
                      QSize(iconSize, iconSize));
}

const QSize NatronPluginManager::getConfigPluginLargeIconSize()
{
    QSettings settings;
    return settings.value(PLUGINS_SETTINGS_LARGE_ICON_SIZE,
                          QSize(PLUGINS_SETTINGS_LARGE_ICON_SIZE_DEFAULT,
                                PLUGINS_SETTINGS_LARGE_ICON_SIZE_DEFAULT)).toSize();
}

void NatronPluginManager::setConfigPluginLargeIconSize(int iconSize)
{
    QSettings settings;
    settings.setValue(PLUGINS_SETTINGS_LARGE_ICON_SIZE,
                      QSize(iconSize, iconSize));
}

const QSize NatronPluginManager::getConfigPluginGridSize()
{
    QSettings settings;
    return settings.value(PLUGINS_SETTINGS_GRID_SIZE,
                          QSize(PLUGINS_SETTINGS_GRID_WIDTH,
                                PLUGINS_SETTINGS_GRID_HEIGHT)).toSize();
}

void NatronPluginManager::setConfigPluginGridSize(QSize gridSize)
{
    QSettings settings;
    settings.setValue(PLUGINS_SETTINGS_GRID_SIZE, gridSize);
}

const QByteArray NatronPluginManager::getConfigWindowGeometry()
{
    QSettings settings;
    return settings.value("WindowGeometry").toByteArray();
}

const QByteArray NatronPluginManager::getConfigWindowState()
{
    QSettings settings;
    return settings.value("WindowState").toByteArray();
}

bool NatronPluginManager::getConfigWindowIsMaximized()
{
    QSettings settings;
    return settings.value("WindowMaximized", false).toBool();
}

void NatronPluginManager::saveWindowConfig()
{
    QSettings settings;
    settings.setValue("WindowGeometry", saveGeometry());
    settings.setValue("WindowState", saveState());
    settings.setValue("WindowMaximized", isMaximized());
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

    // TODO: probably breaks hdpi
    int pluginViewGoBackButton = 24;
    int pluginTitleFontSize = 16;
    int pluginGroupFontSize = 11;
#ifndef Q_OS_MAC
    pluginTitleFontSize = 12;
    pluginGroupFontSize = 9;
#endif

    QSettings settings;

    if (settings.value(PLUGINS_SETTINGS_TITLE_FONT_SIZE).toInt() > 0) {
        pluginTitleFontSize = settings.value(PLUGINS_SETTINGS_TITLE_FONT_SIZE).toInt();
    }
    if (settings.value(PLUGINS_SETTINGS_GROUP_FONT_SIZE).toInt() > 0) {
        pluginGroupFontSize = settings.value(PLUGINS_SETTINGS_GROUP_FONT_SIZE).toInt();
    }

    QFile styleFile(settings.value(PLUGINS_SETTINGS_KEY_STYLE, APP_STYLE).toString());
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString stylesheet = styleFile.readAll();
        styleFile.close();
        qApp->setStyleSheet(stylesheet
                            .arg(pluginTitleFontSize)
                            .arg(pluginGroupFontSize)
                            .arg(pluginViewGoBackButton));
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
            SIGNAL(updatedCache()),
            this,
            SLOT(updateFilterPlugins()));
    connect(_plugins,
            SIGNAL(statusError(QString)),
            this,
            SLOT(handlePluginsStatusError(QString)));
    connect(_plugins,
            SIGNAL(statusMessage(QString)),
            this,
            SLOT(handlePluginsStatusMessage(QString)));
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

    const auto fileMenu = new QMenu(tr("File"), this);
    _menuBar->addMenu(fileMenu);

    const auto addRepoAction = new QAction(tr("Add repository"), this);
    addRepoAction->setShortcut(QKeySequence(tr("Ctrl+A")));
    fileMenu->addAction(addRepoAction);
    connect(addRepoAction,
            SIGNAL(triggered()),
            this,
            SLOT(openAddRepoDialog()));

    fileMenu->addSeparator();

    const auto settingsAction = new QAction(tr("Settings"), this);
    settingsAction->setShortcut(QKeySequence(tr("Ctrl+S")));
    fileMenu->addAction(settingsAction);
    connect(settingsAction,
            SIGNAL(triggered()),
            this,
            SLOT(openSettingsDialog()));

    fileMenu->addSeparator();

    const auto fileQuitAction = new QAction(tr("Quit"), this);
    fileQuitAction->setShortcut(QKeySequence(tr("Ctrl+Q")));
    fileMenu->addAction(fileQuitAction);
    connect(fileQuitAction,
            SIGNAL(triggered()),
            this,
            SLOT(close()));

    const auto helpMenu = new QMenu(tr("Help"), this);
    _menuBar->addMenu(helpMenu);

    const auto helpAboutAction = new QAction(tr("About"), this);
    helpMenu->addAction(helpAboutAction);
    connect(helpAboutAction,
            SIGNAL(triggered()),
            this,
            SLOT(handleAboutActionTriggered()));

    const auto helpAboutQtAction = new QAction(tr("About Qt"), this);
    helpMenu->addAction(helpAboutQtAction);
    connect(helpAboutQtAction,
            SIGNAL(triggered()),
            this,
            SLOT(handleAboutQtActionTriggered()));
}

void NatronPluginManager::setupPluginsComboBoxes()
{
    _comboStatus = new QComboBox(this);
    _comboStatus->setObjectName("ComboStatus");
    _comboStatus->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _comboStatus->addItem(tr("All"),
                          Plugins::NATRON_PLUGIN_TYPE_NONE);
    _comboStatus->insertSeparator(1);
    _comboStatus->addItem(tr("Available"),
                          Plugins::NATRON_PLUGIN_TYPE_AVAILABLE);
    _comboStatus->addItem(tr("Installed"),
                          Plugins::NATRON_PLUGIN_TYPE_INSTALLED);
    _comboStatus->addItem(tr("Updates"),
                          Plugins::NATRON_PLUGIN_TYPE_UPDATE);
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

    _lineEdit = new QLineEdit(this);
    connect(_lineEdit, &QLineEdit::textChanged,
            this, [=]() { updateFilterPlugins(); });
    connect(new QShortcut(QKeySequence(Qt::Key_Escape), this),
            &QShortcut::activated, [=]() { _lineEdit->clear(); });
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

    _pluginView = new PluginViewWidget(this,
                                       _plugins,
                                       getConfigPluginLargeIconSize());
    connect(_pluginView,
            SIGNAL(goBack()),
            this,
            SLOT(showPlugins()));
    connect(this,
            SIGNAL(pluginStatusChanged(QString,int)),
            _pluginView,
            SLOT(setPluginStatus(QString,int)));
    connect(_pluginView,
            SIGNAL(installPlugin(QString)),
            this,
            SLOT(installPlugin(QString)));
    connect(_pluginView,
            SIGNAL(removePlugin(QString)),
            this,
            SLOT(removePlugin(QString)));
    connect(_pluginView,
            SIGNAL(updatePlugin(QString)),
            this,
            SLOT(updatePlugin(QString)));
}

void NatronPluginManager::setupStatus()
{
    _statusBar = new QStatusBar(this);
    _statusBar->setObjectName("StatusBar");
    _statusBar->setSizeGripEnabled(false);
    setStatusBar(_statusBar);

    _availableLabel = new QLabel(this);
    _availableLabel->setText("0");

    _installedLabel = new QLabel(this);
    _installedLabel->setText("0");

    _updatesLabel = new QLabel(this);
    _updatesLabel->setText("0");

    _cacheLabel = new QLabel(this);
    _cacheLabel->setText("0");

    const auto statusAvailableLabel = new QLabel(this);
    statusAvailableLabel->setObjectName("StatusAvailableLabel");
    statusAvailableLabel->setText(tr("Available"));

    const auto statusInstalledLabel = new QLabel(this);
    statusInstalledLabel->setObjectName("StatusInstalledLabel");
    statusInstalledLabel->setText(tr("Installed"));

    const auto statusCacheLabel = new QLabel(this);
    statusCacheLabel->setObjectName("StatusCacheLabel");
    statusCacheLabel->setText(tr("Cache"));

    const auto statusUpdatesLabel = new QLabel(this);
    statusUpdatesLabel->setObjectName("StatusUpdatesLabel");
    statusUpdatesLabel->setText(tr("Updates"));

    _statusBar->addPermanentWidget(statusCacheLabel);
    _statusBar->addPermanentWidget(_cacheLabel);
    _statusBar->addPermanentWidget(statusAvailableLabel);
    _statusBar->addPermanentWidget(_availableLabel);
    _statusBar->addPermanentWidget(statusUpdatesLabel);
    _statusBar->addPermanentWidget(_updatesLabel);
    _statusBar->addPermanentWidget(statusInstalledLabel);
    _statusBar->addPermanentWidget(_installedLabel);

    _progBar = new QProgressBar(this);
    _progBar->setObjectName("ProgressBar");
    _progBar->setMinimumWidth(100);
    _progBar->setMaximumWidth(100);
    _progBar->setRange(0, 1);
    _progBar->setValue(1);
    _progBar->setFormat("");
    _statusBar->addPermanentWidget(_progBar);
    _progBar->setHidden(true);
}

void NatronPluginManager::startup()
{
    const auto geo = getConfigWindowGeometry();
    if (!geo.isNull()) { restoreGeometry(getConfigWindowGeometry()); }
    const auto state = getConfigWindowState();
    if (!state.isNull()) { restoreState(state); }
    if (getConfigWindowIsMaximized()) { showMaximized(); }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    QFuture f = QtConcurrent::run(&Plugins::loadRepositories, _plugins);
#else
    QtConcurrent::run(_plugins, &Plugins::loadRepositories);
#endif
}

void NatronPluginManager::handleUpdatedPlugins()
{
    updatePluginStatusLabels();
    populatePlugins();
    updateFilterPlugins();
}

void NatronPluginManager::updatePluginStatusLabels()
{
    _availableLabel->setText(QString::number(_plugins->getAvailablePlugins().size()));
    _installedLabel->setText(QString::number(_plugins->getInstalledPlugins().size()));
    _updatesLabel->setText(QString::number(_plugins->getUpdatedPlugins().size()));

    const auto locale = this->locale();
    _cacheLabel->setText(locale.formattedDataSize(_plugins->getCacheSize()));
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
                      "<p>A <a href=\"%3\">plug-in manager</a> for <a href=\"https://natrongithub.github.io\">Natron</a>.</p>"
                      "<p style=\"font-size:small;\">%2</p>"
                      "<p style=\"font-size:small;\">Copyright &copy; <a href=\"https://github.com/rodlie\">Ole-André Rodlie</a>. All rights reserved.</p>")
                   .arg(title, lic, QString(APP_URL));
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
    _statusBar->showMessage(message);
    qDebug() << message;
}

void NatronPluginManager::handlePluginsStatusMessage(const QString &message)
{
    if (message.isEmpty()) { return; }
    _statusBar->showMessage(message, 2000);
    qDebug() << message;
}

void NatronPluginManager::handleDownloadStatusMessage(const QString &message,
                                                      qint64 value,
                                                      qint64 total)
{
    qDebug() << message << value << total;
    if (_progBar->isHidden()) { _progBar->show(); }
    _statusBar->showMessage(message, 2000);
    _progBar->setRange(0, total);
    _progBar->setValue(value);
    if (value == total && _progBar->isVisible()) { _progBar->hide(); }
}

void NatronPluginManager::populatePlugins()
{
    const auto plugins = _plugins->getPlugins();
    const auto groups = _plugins->getPluginGroups();

    _comboGroup->clear();
    _comboGroup->addItem(tr("All"));
    _comboGroup->insertSeparator(1);
    _comboGroup->addItems(groups);
    _comboGroup->adjustSize();

    _comboGroup->setEnabled(true);
    _comboStatus->setEnabled(true);

    _pluginList->clear();

    for (unsigned long i = 0; i< plugins.size(); ++i) {
        const auto item = new QListWidgetItem(); // the list takes ownership
        const auto plugin = plugins.at(i);
        item->setFlags(Qt::NoItemFlags);
        item->setData(PLUGIN_LIST_ROLE_GROUP, plugin.group);
        item->setData(PLUGIN_LIST_ROLE_ID, plugin.id);
        Plugins::PluginType type = Plugins::NATRON_PLUGIN_TYPE_NONE;
        if (_plugins->hasUpdatedPlugin(plugin.id)) {
            type = Plugins::NATRON_PLUGIN_TYPE_UPDATE;
        } else if (_plugins->hasAvailablePlugin(plugin.id)) {
            type = Plugins::NATRON_PLUGIN_TYPE_AVAILABLE;
        } else if (_plugins->hasInstalledPlugin(plugin.id)) {
            type = Plugins::NATRON_PLUGIN_TYPE_INSTALLED;
        }
        const auto pwidget = new PluginListWidget(plugin,
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
        connect(pwidget,
                SIGNAL(showPlugin(QString)),
                this,
                SLOT(showPlugin(QString)));

        item->setSizeHint(_pluginList->gridSize());
        _pluginList->addItem(item);
        _pluginList->setItemWidget(item, pwidget);
    }
}

void NatronPluginManager::handleComboStatusChanged(const QString &status)
{
    Q_UNUSED(status)
    updateFilterPlugins();
}

void NatronPluginManager::handleComboGroupChanged(const QString &group)
{
    Q_UNUSED(group)
    updateFilterPlugins();
}

void NatronPluginManager::updateFilterPlugins()
{
    filterPlugins(_comboStatus->currentText(),
                  _comboGroup->currentText(),
                  _lineEdit->text());
    updatePluginStatusLabels();
}

void NatronPluginManager::filterPlugins(const QString &status,
                                        const QString &group,
                                        const QString &filter)
{
    for (int i = 0; i < _pluginList->count(); ++i) {
        const auto item = _pluginList->item(i);
        const QString itemId = item->data(PLUGIN_LIST_ROLE_ID).toString();
        const QString itemGroup = item->data(PLUGIN_LIST_ROLE_GROUP).toString();
        bool visible = true;
        if (status == tr("Available")) {
            visible = _plugins->hasAvailablePlugin(itemId);
        } else if (status == tr("Installed")) {
            visible = _plugins->hasInstalledPlugin(itemId);
        } else if (status == tr("Updates")) {
            visible = _plugins->hasUpdatedPlugin(itemId);
        }
        if (!group.isEmpty()) {
            bool hasGroup = (itemGroup == group || group == tr("All"));
            if (visible && !hasGroup) { visible = false; }
        }
        if (!filter.isEmpty()) {
            bool isMatch = _plugins->getPlugin(itemId).label.startsWith(filter,
                                                                        Qt::CaseInsensitive);
            if (!isMatch && visible) { visible = false; }
        }
        item->setHidden(!visible);
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
    case Plugins::NATRON_PLUGIN_TYPE_UPDATE:
        updatePlugin(id);
        break;
    default:;
    }
}

void NatronPluginManager::installPlugin(const QString &id)
{
    const auto status = _plugins->installPlugin(id);
    if (!status.success) {
        QMessageBox::warning(this, tr("Install"), status.message);
    } else {
        emit pluginStatusChanged(id, Plugins::NATRON_PLUGIN_TYPE_INSTALLED);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QFuture f = QtConcurrent::run(
                                      &Plugins::checkRepositories,
                                      _plugins,
#else
        QtConcurrent::run(
                                      _plugins,
                                      &Plugins::checkRepositories,
#endif
                                      false,
                                      true);
    }
}

void NatronPluginManager::removePlugin(const QString &id)
{
    const auto status = _plugins->removePlugin(id);
    if (!status.success) {
        QMessageBox::warning(this, tr("Remove"), status.message);
    } else {
        emit pluginStatusChanged(id, Plugins::NATRON_PLUGIN_TYPE_AVAILABLE);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QFuture f = QtConcurrent::run(
                                      &Plugins::checkRepositories,
                                      _plugins,
#else
        QtConcurrent::run(
                                      _plugins,
                                      &Plugins::checkRepositories,
#endif
                                      false,
                                      true);
    }
}

void NatronPluginManager::updatePlugin(const QString &id)
{
    const auto status = _plugins->updatePlugin(id);
    if (!status.success) {
        QMessageBox::warning(this, tr("Update"), status.message);
    } else {
        emit pluginStatusChanged(id, Plugins::NATRON_PLUGIN_TYPE_INSTALLED);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QFuture f = QtConcurrent::run(
                                     &Plugins::checkRepositories,
                                     _plugins,
#else
        QtConcurrent::run(
                                     _plugins,
                                      &Plugins::checkRepositories,
#endif
                                      false,
                                      true);
    }
}

void NatronPluginManager::openAddRepoDialog()
{
    AddRepoDialog dialog(this, _plugins);
    dialog.exec();
}

void NatronPluginManager::openSettingsDialog()
{
    SettingsDialog dialog(this, _plugins);
    if (dialog.exec() == QDialog::Accepted) { updateSettings(); }
}

void NatronPluginManager::updateSettings()
{
    _plugins->checkRepositories();
}

void NatronPluginManager::showPlugins()
{
    if (_stack->currentIndex() == _stackListIndex) { return; }
    _stack->setCurrentIndex(_stackListIndex);
}

void NatronPluginManager::showPlugin(const QString &id)
{
    if (!_plugins->hasPlugin(id)) { return; }
    if (_stack->currentIndex() != _stackViewIndex) {
        _stack->setCurrentIndex(_stackViewIndex);
    }
    _pluginView->showPlugin(id);
}

void NatronPluginManager::closeEvent(QCloseEvent *e)
{
    if (_plugins->isBusy()) {
        QMessageBox::warning(this,
                             tr("Unable to quit"),
                             tr("The application is busy, please try again later."));
        e->ignore();
    } else { e->accept(); }
}
