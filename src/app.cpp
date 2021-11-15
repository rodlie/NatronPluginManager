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
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QKeySequence>
#include <QApplication>
#include <QTimer>
#include <QtConcurrentRun>
#include <QPalette>

#define NATRON_ICON ":/natron.png"

#define PLUGIN_TREE_ROLE_ID Qt::UserRole+1
#define PLUGIN_TREE_ROLE_TYPE Qt::UserRole+2

NatronPluginManager::NatronPluginManager(QWidget *parent)
    : QMainWindow(parent)
    , _plugins(nullptr)
    , _availablePluginsTree(nullptr)
    , _installedPluginsTree(nullptr)
    , _pluginDesc(nullptr)
    , _pluginLabel(nullptr)
    , _pluginIcon(nullptr)
    , _leftTab(nullptr)
    , _statusBar(nullptr)
    , _progBar(nullptr)
    , _installButton(nullptr)
    , _removeButton(nullptr)
    , _updateButton(nullptr)
    , _menuBar(nullptr)
{
    setMinimumSize(780, 440);
    setWindowIcon(QIcon(NATRON_ICON));

    setupPalette();
    setupPlugins();
    setupMenu();
    setupStatusBar();
    setupButtons();
    setupPluginInfo();

    QWidget *mainWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(mainWidget);
    mainLayout->setSpacing(5);
    setCentralWidget(mainWidget);

    QWidget *topWidget = new QWidget(this);
    topWidget->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *topLayout = new QVBoxLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *bottomWidget = new QWidget(this);
    bottomWidget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomWidget);

    QWidget *leftWidget = new QWidget(this);
    leftWidget->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);

    QWidget *rightWidget = new QWidget(this);
    rightWidget->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);

    _availablePluginsTree = new QTreeWidget(this);
    setupTreeWidget(_availablePluginsTree);

    _installedPluginsTree = new QTreeWidget(this);
    setupTreeWidget(_installedPluginsTree);

    setupPluginsTab();

    QWidget *pluginInfoWidget = new QWidget(this);
    pluginInfoWidget->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *pluginInfoLayout = new QHBoxLayout(pluginInfoWidget);
    pluginInfoLayout->setContentsMargins(10, 0, 10, 0);
    rightLayout->addWidget(pluginInfoWidget);

    pluginInfoLayout->addWidget(_pluginLabel);
    pluginInfoLayout->addStretch();
    pluginInfoLayout->addWidget(_pluginIcon);

    mainLayout->addWidget(topWidget);
    mainLayout->addWidget(bottomWidget);

    bottomLayout->addWidget(leftWidget);
    bottomLayout->addWidget(rightWidget);

    leftLayout->addWidget(_leftTab);
    topLayout->addWidget(pluginInfoWidget);
    rightLayout->addWidget(_pluginDesc);

    setDefaultPluginInfo();

    QTimer::singleShot(100, this, SLOT(startup()));
}

NatronPluginManager::~NatronPluginManager()
{
}

void NatronPluginManager::installPlugins()
{
    QStringList plugins = getCheckedAvailablePlugins();
    for (int i = 0; i < plugins.size(); ++i) {
        Plugins::PluginStatus result = _plugins->installPlugin(plugins.at(i));
        if (!result.success) {
            QMessageBox::warning(this,
                                 tr("Failed to install plugin"),
                                 result.message);
        }
    }
    updatePlugins();
}

void NatronPluginManager::removePlugins()
{
    QStringList plugins = getCheckedInstalledPlugins();
    for (int i = 0; i < plugins.size(); ++i) {
        Plugins::PluginStatus result = _plugins->removePlugin(plugins.at(i));
        if (!result.success) {
            QMessageBox::warning(this,
                                 tr("Failed to remove plugin"),
                                 result.message);
        }
    }
    updatePlugins();
}

bool NatronPluginManager::isPluginTreeItemChecked(QTreeWidgetItem *item)
{
    if (item && item->checkState(0) == Qt::Checked) { return true; }
    return false;
}

void NatronPluginManager::setDefaultPluginInfo()
{
    _pluginDesc->setHtml(QString("<h3>Lorem ipsum</h3><p>Lorem ipsum dolor sit amet, consectetur adipiscing elit. "
                                 "In auctor ante in urna ornare, eu tincidunt felis condimentum. Vestibulum ut molestie mi. "
                                 "Aenean sit amet feugiat tellus, sit amet tempor augue. Nullam sollicitudin posuere ipsum, "
                                 "quis lobortis augue dignissim id. Suspendisse nec efficitur lectus, eget lacinia metus. "
                                 "Nullam eu leo a justo pellentesque dictum. Curabitur ultricies, tellus et condimentum elementum, "
                                 "odio nisl ultricies quam, nec ultrices diam mauris vitae erat.</p>") );
    _pluginLabel->setText(QString("<h1 style=\"font-weight: normal;\">Natron"
                                  "<br><span style=\"font-size: small;\">%1 v%2</span></h1>")
                          .arg(tr("Plug-in Manager"), qApp->applicationVersion()));
    _pluginIcon->setPixmap(QIcon(QString(NATRON_ICON)).pixmap(48, 48).scaled(48,
                                                                             48,
                                                                             Qt::KeepAspectRatio,
                                                                             Qt::SmoothTransformation));
}

const QStringList NatronPluginManager::getCheckedPlugins(QTreeWidget *tree)
{
    QStringList plugins;
    for (int i = 0; i < tree->topLevelItemCount(); ++i) {
        QTreeWidgetItem *item = tree->topLevelItem(i);
        for (int y = 0; y < item->childCount(); ++y) {
            QTreeWidgetItem *child = item->child(y);
            if (child->checkState(0) == Qt::Checked) {
                plugins << child->data(0, PLUGIN_TREE_ROLE_ID).toString();
            }
        }
    }
    return plugins;
}

const QStringList NatronPluginManager::getCheckedAvailablePlugins()
{
    return getCheckedPlugins(_availablePluginsTree);
}

const QStringList NatronPluginManager::getCheckedInstalledPlugins()
{
    return getCheckedPlugins(_installedPluginsTree);
}

void NatronPluginManager::setupPalette()
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(53, 53, 53));
    palette.setColor(QPalette::WindowText, Qt::white);
    palette.setColor(QPalette::Base, QColor(15, 15, 15));
    palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    palette.setColor(QPalette::Link, Qt::white);
    palette.setColor(QPalette::LinkVisited, Qt::white);
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, Qt::white);
    palette.setColor(QPalette::Button, QColor(53, 53, 53));
    palette.setColor(QPalette::ButtonText, Qt::white);
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(196, 110, 33));
    palette.setColor(QPalette::HighlightedText, Qt::white);
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);

    qApp->setPalette(palette);
    qApp->setStyle(QString("fusion"));
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
    _statusBar->setSizeGripEnabled(true);
    connect(_plugins,
            SIGNAL(statusMessage(QString)),
            this,
            SLOT(handlePluginsStatusMessage(QString)));

    setStatusBar(_statusBar);

    _progBar = new QProgressBar(this);
    _progBar->setMaximumWidth(100);
    _progBar->setRange(0, 1);
    _progBar->setValue(1);
    _progBar->setFormat("");
    _progBar->hide();

    _statusBar->addPermanentWidget(_progBar);
}

void NatronPluginManager::setupButtons()
{
    _installButton = new QPushButton(tr("Install"), this);
    _installButton->setEnabled(false);

    _removeButton = new QPushButton(tr("Remove"), this);
    _removeButton->setEnabled(false);

    connect(_installButton,
            SIGNAL(released()),
            this,
            SLOT(handleInstallButton()));
    connect(_removeButton,
            SIGNAL(released()),
            this,
            SLOT(handleRemoveButton()));
}

void NatronPluginManager::setupTreeWidget(QTreeWidget *tree)
{
    tree->setHeaderHidden(true);
    tree->setFrameShape(QFrame::NoFrame);
    connect(tree,
            SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
            this,
            SLOT(handleItemActivated(QTreeWidgetItem*,QTreeWidgetItem*)));
}

void NatronPluginManager::setupPluginsTab()
{
    _leftTab = new QTabWidget(this);
    _leftTab->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    _leftTab->setTabPosition(QTabWidget::West);

    QWidget *installTabWidget = new QWidget(this);
    installTabWidget->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *installTabLayout = new QVBoxLayout(installTabWidget);
    installTabLayout->setContentsMargins(0, 0, 0, 0);
    installTabLayout->setSpacing(0);

    QWidget *installButtonWidget = new QWidget(this);
    installButtonWidget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *installButtonLayout = new QHBoxLayout(installButtonWidget);
    installButtonLayout->setContentsMargins(2, 2, 2, 2);

    installButtonLayout->addWidget(_installButton);
    installTabLayout->addWidget(_availablePluginsTree);
    installTabLayout->addWidget(installButtonWidget);

    QWidget *removeTabWidget = new QWidget(this);
    removeTabWidget->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *removeTabLayout = new QVBoxLayout(removeTabWidget);
    removeTabLayout->setContentsMargins(0, 0, 0, 0);
    removeTabLayout->setSpacing(0);

    QWidget *removeButtonWidget = new QWidget(this);
    removeButtonWidget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *removeButtonLayout = new QHBoxLayout(removeButtonWidget);
    removeButtonLayout->setContentsMargins(2, 2, 2, 2);

    removeButtonLayout->addWidget(_removeButton);
    removeTabLayout->addWidget(_installedPluginsTree);
    removeTabLayout->addWidget(removeButtonWidget);

    _leftTab->addTab(installTabWidget, tr("Available"));
    _leftTab->addTab(removeTabWidget, tr("Installed"));
    //_leftTab->addTab(new QWidget(), tr("Updates"));
}

void NatronPluginManager::setupPluginInfo()
{
    _pluginLabel = new QLabel(this);
    _pluginIcon = new QLabel(this);

    _pluginDesc = new QTextBrowser(this);
    _pluginDesc->setReadOnly(true);
    _pluginDesc->setOpenLinks(true);
    _pluginDesc->setOpenExternalLinks(true);
}

void NatronPluginManager::startup()
{
    QtConcurrent::run(this, &NatronPluginManager::loadRepositories);
}

void NatronPluginManager::loadRepositories()
{
    _plugins->loadRepositories();
}

void NatronPluginManager::handleUpdatedPlugins()
{
    qDebug() << "AVAILABLE" << _plugins->getAvailablePlugins().size();
    qDebug() << "INSTALLED" << _plugins->getInstalledPlugins().size();

    populatePluginsTree(Plugins::NATRON_PLUGIN_TYPE_INSTALLED,
                        _installedPluginsTree);
    populatePluginsTree(Plugins::NATRON_PLUGIN_TYPE_AVAILABLE,
                        _availablePluginsTree);
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

void NatronPluginManager::handleItemActivated(QTreeWidgetItem *item,
                                              QTreeWidgetItem */*prev*/)
{
    if (!item) { return; }
    QString id = item->data(0, PLUGIN_TREE_ROLE_ID).toString();
    Plugins::PluginSpecs specs;
    int type = item->data(0, PLUGIN_TREE_ROLE_TYPE).toInt();
    switch(type) {
    case Plugins::NATRON_PLUGIN_TYPE_AVAILABLE:
        specs = _plugins->getAvailablePlugin(id);
        break;
    case Plugins::NATRON_PLUGIN_TYPE_INSTALLED:
        specs = _plugins->getInstalledPlugin(id);
        break;
    default:;
    }
    if (!_plugins->isValidPlugin(specs) || specs.id != id) { return; }

    _pluginLabel->setText(QString("<h1 style=\"font-weight: normal;\">%1<br>"
                                  "<span style=\"font-size: small;\">%2 v%4</span></h1>")
                          .arg(specs.label,
                               specs.id,
                               QString::number(specs.version)));
    if (specs.icon.isEmpty()) {
        _pluginIcon->setPixmap(QIcon(QString(NATRON_ICON)).pixmap(48,
                                                                  48).scaled(48,
                                                                             48,
                                                                             Qt::KeepAspectRatio,
                                                                             Qt::SmoothTransformation));
    } else {
        _pluginIcon->setPixmap(QIcon(QString("%1/%2").arg(specs.path,
                                                          specs.icon)).pixmap(48,
                                                                              48).scaled(48,
                                                                                         48,
                                                                                         Qt::KeepAspectRatio,
                                                                                         Qt::SmoothTransformation));
    }

    QString desc = specs.desc.replace("\\n", "<br>").replace("\\", "").simplified();
    if (desc.isEmpty()) {
        desc = QString("<p>%1.</p>").arg(tr("No description available"));
    }

    desc = desc.replace(QRegExp("((?:https?|ftp)://\\S+)"),
                        "<a href=\"\\1\">\\1</a>");

    _pluginDesc->setHtml(desc);
}

void NatronPluginManager::handleInstallButton()
{
    QStringList plugins = getCheckedAvailablePlugins();
    if (plugins.size() > 0) { installPlugins(); }
    else {
        QMessageBox::information(this,
                                 tr("Install"),
                                 tr("No plug-ins marked for installation."));
    }
}

void NatronPluginManager::handleRemoveButton()
{
    QStringList plugins = getCheckedInstalledPlugins();
    if (plugins.size() > 0) { removePlugins(); }
    else {
        QMessageBox::information(this,
                                 tr("Install"),
                                 tr("No plug-ins marked for removal."));
    }
}

void NatronPluginManager::updatePlugins()
{
    _installButton->setEnabled(false);
    _removeButton->setEnabled(false);
    setDefaultPluginInfo();
    _plugins->checkRepositories();
}

void NatronPluginManager::populatePluginsTree(Plugins::PluginType type,
                                              QTreeWidget *tree)
{
    if (!tree) { return; }

    std::vector<Plugins::PluginSpecs> plugins;
    switch (type) {
    case Plugins::NATRON_PLUGIN_TYPE_AVAILABLE:
        plugins = _plugins->getAvailablePlugins();
        break;
    case Plugins::NATRON_PLUGIN_TYPE_INSTALLED:
        plugins = _plugins->getInstalledPlugins();
        break;
    default:;
    }
    if (plugins.size() < 1) { return; }

    tree->clear();

    QStringList groups = _plugins->getPluginGroups(type);
    for (int i = 0; i < groups.size(); ++i) {
        QTreeWidgetItem *groupItem = new QTreeWidgetItem(tree);
        groupItem->setText(0, groups.at(i));
        QFont font = groupItem->font(0);
        font.setBold(true);
        groupItem->setFont(0, font);
        tree->addTopLevelItem(groupItem);
        std::vector<Plugins::PluginSpecs> plugins = _plugins->getPluginsInGroup(type, groups.at(i));
        for (unsigned long y = 0; y < plugins.size(); ++y) {
            Plugins::PluginSpecs specs = plugins.at(y);
            if (specs.id.isEmpty()) { continue; }
            QTreeWidgetItem *pluginItem = new QTreeWidgetItem();
            QString iconPath = QString("%1/%2").arg(specs.path, specs.icon);
            QIcon fallbackIcon = QIcon(NATRON_ICON);
            QIcon pluginIcon;
            if (QFile::exists(iconPath) && !specs.icon.isEmpty()) {
                pluginIcon = QIcon(iconPath);
            }
            if (pluginIcon.isNull()) { pluginIcon = fallbackIcon; }
            pluginItem->setIcon(0, pluginIcon);
            pluginItem->setData(0, PLUGIN_TREE_ROLE_ID, specs.id);
            pluginItem->setData(0, PLUGIN_TREE_ROLE_TYPE, type);
            pluginItem->setText(0, specs.label);
            pluginItem->setCheckState(0, Qt::Unchecked);

            groupItem->addChild(pluginItem);
        }
        tree->expandItem(groupItem); // add to settings?
    }

    switch (type) {
    case Plugins::NATRON_PLUGIN_TYPE_AVAILABLE:
        _installButton->setEnabled(_availablePluginsTree->topLevelItemCount() > 0? true:false);
        break;
    case Plugins::NATRON_PLUGIN_TYPE_INSTALLED:
        _removeButton->setEnabled(_installedPluginsTree->topLevelItemCount() >  0? true:false);
        break;
    default:;
    }
}

void NatronPluginManager::closeEvent(QCloseEvent *e)
{
    if (_plugins->isBusy()) {
        QMessageBox::warning(this,
                             tr("Unable to quit"),
                             tr("The application is currently working, please try again later."));
        e->ignore();
    } else {
        e->accept();
    }
}

