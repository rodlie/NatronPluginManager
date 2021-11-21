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
//#include <QTreeWidgetItem>
#include <QMessageBox>
#include <QMenu>
#include <QAction>
#include <QKeySequence>
#include <QApplication>
#include <QTimer>
#include <QtConcurrentRun>
#include <QPalette>
#include <QLabel>
//#include <QPushButton>
#include <QSettings>

#include "pluginlistwidget.h"

#define NATRON_STYLE ":/stylesheet.qss"
//#define NATRON_PLUGIN_DOC_HTML ":/doc.html"

#define PLUGIN_LIST_ROLE_ID Qt::UserRole+4
#define PLUGIN_LIST_ROLE_GROUP Qt::UserRole+5

NatronPluginManager::NatronPluginManager(QWidget *parent)
    : QMainWindow(parent)
    , _comboStatus(nullptr)
    , _comboGroup(nullptr)
    , _toolBar(nullptr)
    , _stack(nullptr)
    , _plugins(nullptr)
    //, _groupTree(nullptr)
    , _statusBar(nullptr)
    , _progBar(nullptr)
    , _menuBar(nullptr)
    , _pluginsList(nullptr)
{

    setMinimumWidth(350*2);
    setMinimumHeight((160*3)+80);
    setWindowIcon(QIcon(NATRON_ICON));

    setupStyle();
    setupPlugins();
    setupMenu();
    setupStatusBar();

    _toolBar = new QToolBar(this);
    _toolBar->setObjectName("ToolBar");
    _toolBar->setMovable(false);
    addToolBar(_toolBar);

    QLabel *comboStatusLabel = new QLabel(tr("Status"), this);
    comboStatusLabel->setObjectName("ComboStatusLabel");

    _comboStatus = new QComboBox(this);
    _comboStatus->setObjectName("ComboStatus");
    _comboStatus->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _comboStatus->addItem(tr("All"), Plugins::NATRON_PLUGIN_TYPE_NONE);
    _comboStatus->addItem(tr("Available"), Plugins::NATRON_PLUGIN_TYPE_AVAILABLE);
    _comboStatus->addItem(tr("Installed"), Plugins::NATRON_PLUGIN_TYPE_INSTALLED);
    _comboStatus->addItem(tr("Updates"), Plugins::NATRON_PLUGIN_TYPE_UPDATE);
    //_comboStatus->addItems(QStringList() << tr("All") << tr("Available") << tr("Installed") << tr("Updates"));

    QLabel *comboGroupLabel = new QLabel(tr("Groups"), this);
    comboGroupLabel->setObjectName("ComboGroupLabel");

    _comboGroup = new QComboBox(this);
    _comboGroup->setObjectName("ComboGroup");
    _comboGroup->setSizeAdjustPolicy(QComboBox::AdjustToContents);

    connect(_comboGroup, SIGNAL(currentTextChanged(QString)), this, SLOT(handleComboGroup(QString)));

    QWidget *toolBarSpacer = new QWidget(this);
    toolBarSpacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    _toolBar->addWidget(toolBarSpacer);
    _toolBar->addWidget(comboStatusLabel);
    _toolBar->addWidget(_comboStatus);
    _toolBar->addWidget(comboGroupLabel);
    _toolBar->addWidget(_comboGroup);
    /*QLabel *toolBarLabel = new QLabel(this);
    toolBarLabel->setText(QString("<h1 style=\"font-weight: normal;color:#ebebeb;\">Natron"
                                  "<br><span style=\"font-size: small;\">%1 v%2</span></h1>")
                          .arg(tr("Plug-in Manager"), qApp->applicationVersion(), NATRON_ICON));
    _toolBar->addWidget(toolBarLabel);*/

    QWidget *mainWidget = new QWidget(this);
    mainWidget->setObjectName("MainWidget");
    //mainWidget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *mainLayout = new QHBoxLayout(mainWidget);
    //mainLayout->setContentsMargins(0, 0, 0, 0);

    mainLayout->setSpacing(0);
    setCentralWidget(mainWidget);

  /*  QWidget *topWidget = new QWidget(this);
    topWidget->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *topLayout = new QVBoxLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);*/

    /*QWidget *bottomWidget = new QWidget(this);
    bottomWidget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomWidget);*/

    /*QWidget *leftWidget = new QWidget(this);
    leftWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    leftWidget->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    leftWidget->hide();*/

  /*  QWidget *rightWidget = new QWidget(this);
    rightWidget->setObjectName("rightWidget");
    //rightWidget->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);*/
    //rightWidget->setStyleSheet("background-color: /*#17181a*/#17181a; color: white;");

   /* _availablePluginsTree = new QTreeWidget(this);
    setupTreeWidget(_availablePluginsTree);

    _installedPluginsTree = new QTreeWidget(this);
    setupTreeWidget(_installedPluginsTree);

    setupPluginsTab();*/

  /*  QWidget *pluginInfoWidget = new QWidget(this);
    pluginInfoWidget->setContentsMargins(0, 0, 0, 0);

    QHBoxLayout *pluginInfoLayout = new QHBoxLayout(pluginInfoWidget);
    pluginInfoLayout->setContentsMargins(10, 0, 10, 0);
    rightLayout->addWidget(pluginInfoWidget);
*/
    /*pluginInfoLayout->addWidget(_pluginLabel);
    pluginInfoLayout->addStretch();
    pluginInfoLayout->addWidget(_pluginIcon);
*/
 //   mainLayout->addWidget(topWidget);
   // mainLayout->addWidget(bottomWidget);

   // bottomLayout->addWidget(leftWidget);
   // bottomLayout->addWidget(rightWidget);

    //mainLayout->addWidget(leftWidget);
    //mainLayout->addWidget(rightWidget);

    //leftLayout->addWidget(_leftTab);
  //  topLayout->addWidget(pluginInfoWidget);

    _pluginsList = new QListWidget(this);
    _pluginsList->setObjectName("PluginList");
//    _pluginsList->setStyleSheet("background-color: /*#17181a*/#17181a; color: white;");
    _pluginsList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //_pluginsList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pluginsList->setFrameShape(QFrame::NoFrame);
    _pluginsList->setViewMode(QListView::IconMode);
    _pluginsList->setGridSize(QSize(330,160));
    _pluginsList->setResizeMode(QListView::Adjust);
    _pluginsList->setEditTriggers(QAbstractItemView::NoEditTriggers);
   // _pluginsList->setUniformItemSizes(true);
            //_pluginsList->setItemDelegate(new PluginDelegate(this));

 //   _pluginsList->setViewMode    (QListWidget::IconMode  );
   // _pluginsList->setResizeMode  (QListWidget::Adjust    );

    QWidget *stackWidget1 = new QWidget(this);
    QVBoxLayout *stackWidgetLayout1 = new QVBoxLayout(stackWidget1);
    QLabel *stackLabel1 = new QLabel("<h1 style=\"text-align: center;\"><img src=\":/NatronPluginManager.png\"><br>Natron<br><span style=\"font-weight:normal;\">Plug-in Manager</span><br><br><span style=\"font-size:small;text-transform:uppercase;;\">Loading please wait ...</span></h1>", this);
    stackWidgetLayout1->addStretch();
    stackWidgetLayout1->addWidget(stackLabel1);
    stackWidgetLayout1->addStretch();

    _stack = new QStackedWidget(this);
    _stack->addWidget(stackWidget1);
    _stack->addWidget(_pluginsList);

    _stack->setCurrentIndex(0);
    mainLayout->addWidget(_stack);

    //_groupTree = new QTreeWidget(this);
    //_groupTree->setObjectName("GroupTree");
    /*_groupTree->setStyleSheet("background-color: #232528; color: white;font-size: 12pt;color:#97999d;");
    _groupTree->setStyleSheet(QString::fromUtf8("QTreeWidget {background-color: #232528; color: white;font-size: 12pt;color:#97999d;}"
                                  "QScrollBar:vertical {"
                                                "    border: 0px solid #999999;"
                                                "    background:transparent;"
                                                "    width:10px;    "
                                                "    margin: 0px 0px 0px 0px;"
                                                "}"
                                                "QScrollBar::handle:vertical {"
                                                "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                                "    stop: 0 rgb(32, 47, 130), stop: 0.5 rgb(32, 47, 130), stop:1 rgb(32, 47, 130));background-color:#97999d;"
                                                "    min-height: 0px;"
                                                "}"
                                                "QScrollBar::add-line:vertical {"
                                                "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                                "    stop: 0 rgb(32, 47, 130), stop: 0.5 rgb(32, 47, 130),  stop:1 rgb(32, 47, 130));"
                                                "    height: 0px;"
                                                "    subcontrol-position: bottom;"
                                                "    subcontrol-origin: margin;"
                                                "}"
                                                "QScrollBar::sub-line:vertical {"
                                                "    background: qlineargradient(x1:0, y1:0, x2:1, y2:0,"
                                                "    stop: 0  rgb(32, 47, 130), stop: 0.5 rgb(32, 47, 130),  stop:1 rgb(32, 47, 130));"
                                                "    height: 0 px;"
                                                "    subcontrol-position: top;"
                                                "    subcontrol-origin: margin;"
                                                "}"
                                                ));*/
    //_groupTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //_groupTree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
   /* _groupTree->setHeaderHidden(true);
    _groupTree->setFrameShape(QFrame::NoFrame);
    _groupTree->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);*/

  //  leftLayout->addWidget(_groupTree);
    //rightLayout->addWidget(_pluginDesc);

    _menuBar->setObjectName("MenuBar");
    //_menuBar->setStyleSheet("background-color:#232528;border:0;");
  //  setDefaultPluginInfo();

    //_statusBar->hide();
   // _groupTree->hide();
    QTimer::singleShot(100, this, SLOT(startup()));
}

NatronPluginManager::~NatronPluginManager()
{
}

const QSize NatronPluginManager::getConfigPluginIconSize()
{
    QSettings settings;
    return settings.value("PluginIconSize", QSize(48, 48)).toSize();
}

const QSize NatronPluginManager::getConfigPluginGridSize()
{
    QSettings settings;
    return settings.value("PluginGridSize", QSize(330,160)).toSize();
}

void NatronPluginManager::setupStyle()
{
    QPalette palette;
    palette.setColor(QPalette::Window, QColor(/*53, 53, 53*/"#232528"));
    palette.setColor(QPalette::WindowText, /*Qt::white*/"#ebebeb");
    palette.setColor(QPalette::Base, QColor(15, 15, 15));
    palette.setColor(QPalette::AlternateBase, QColor(/*53, 53, 53*/"#232528"));
    palette.setColor(QPalette::Link, /*Qt::white*/"#ebebeb");
    palette.setColor(QPalette::LinkVisited, /*Qt::white*/"#ebebeb");
    palette.setColor(QPalette::ToolTipText, Qt::black);
    palette.setColor(QPalette::Text, /*Qt::white*/"#ebebeb");
    palette.setColor(QPalette::Button, QColor(/*53, 53, 53*/"#232528"));
    palette.setColor(QPalette::ButtonText, /*Qt::white*/"#ebebeb");
    palette.setColor(QPalette::BrightText, Qt::red);
    palette.setColor(QPalette::Highlight, QColor(/*196, 110, 33*/"#2e82be"));
    palette.setColor(QPalette::HighlightedText, /*Qt::white*/"#ebebeb");
    palette.setColor(QPalette::Disabled, QPalette::Text, Qt::darkGray);
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);
    qApp->setStyle(QString("fusion"));
    qApp->setPalette(palette);

    QSettings settings;
    QFile styleFile(settings.value("stylesheet", NATRON_STYLE).toString());
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString stylesheet = styleFile.readAll();
        styleFile.close();
        qApp->setStyleSheet(stylesheet);
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
    std::vector<Plugins::PluginSpecs> myplugins =  _plugins->getPlugins();// _plugins->getPluginsInGroup(Plugins::NATRON_PLUGIN_TYPE_AVAILABLE, "Filter");
    QStringList groups = _plugins->getPluginGroups();
  //  _groupTree->clear();

    _comboGroup->clear();
    _comboGroup->addItem(tr("All"));
    _comboGroup->insertSeparator(1);
    _comboGroup->addItems(groups);
    _comboGroup->adjustSize();
    //_comboGroup->update();

    //if (_groupTree->isHidden()) { _groupTree->show(); }

    _pluginsList->clear();

    /*QStringList addedGroups;
    for (int i = 0; i < groups.size(); ++i) {
        QString groupLabel = groups.at(i);
        QStringList groupList;
        if (groupLabel.contains("/")) {
            groupList = groupLabel.split("/");
            groupLabel = groupLabel.split("/").takeFirst();
        }

        if (addedGroups.contains(groupLabel)) { continue; }
        addedGroups << groupLabel;

        QTreeWidgetItem *gItem = new QTreeWidgetItem(_groupTree);
        gItem->setText(0, groupLabel);

        for (int y = 1; y < groupList.size(); ++y) {
            QString childGroup = groupList.at(y);
            if (childGroup.isEmpty()) { continue; }
            QTreeWidgetItem *cItem = new QTreeWidgetItem();
            cItem->setText(0, childGroup);
            gItem->addChild(cItem);
        }

        for (int z = i; z < groups.size(); ++z) {
            QString zGroup = groups.at(z);
            if (zGroup.startsWith(QString("%1/").arg(groupLabel))) {
                QStringList zGroupList = zGroup.split("/");
                for (int x = 1; x < zGroupList.size(); ++x) {
                    QString childGroup = zGroupList.at(x);
                    if (childGroup.isEmpty()) { continue; }
                    QTreeWidgetItem *cItem = new QTreeWidgetItem();
                    cItem->setText(0, childGroup);
                    gItem->addChild(cItem);
                }
            }
        }*/


  //  }
    for (unsigned long i = 0; i< myplugins.size(); ++i) {
        QListWidgetItem *item = new QListWidgetItem(); // the list takes ownership
        item->setFlags(Qt::NoItemFlags);
        item->setData(PLUGIN_LIST_ROLE_GROUP, myplugins.at(i).group);
        item->setData(PLUGIN_LIST_ROLE_ID, myplugins.at(i).id);
        Plugins::PluginType type = Plugins::NATRON_PLUGIN_TYPE_NONE;
        if (_plugins->hasAvailablePlugin(myplugins.at(i).id)) {
            type = Plugins::NATRON_PLUGIN_TYPE_AVAILABLE;
        } else if (_plugins->hasInstalledPlugin(myplugins.at(i).id)) {
            type = Plugins::NATRON_PLUGIN_TYPE_INSTALLED;
        }
        PluginListWidget *pwidget = new PluginListWidget(myplugins.at(i),
                                                         type,
                                                         _pluginsList->gridSize(),
                                                         getConfigPluginIconSize()); // the list takes ownership
        connect(pwidget,
                SIGNAL(pluginButtonReleased(QString,int)),
                this,
                SLOT(handlePluginButtonReleased(QString,int)));
        connect(this,
                SIGNAL(pluginStatusChanged(QString,int)),
                pwidget,
                SLOT(setPluginStatus(QString,int)));

        item->setSizeHint(_pluginsList->gridSize());
        _pluginsList->addItem(item);
        _pluginsList->setItemWidget(item, pwidget);
    }
    _stack->setCurrentIndex(1);
}

void NatronPluginManager::handleComboGroup(const QString &group)
{
    qDebug() << group;
    filterPluginsGroup(group);
}

void NatronPluginManager::filterPluginsGroup(const QString &group)
{
    qDebug() << "group filter" << group;
    for (int i = 0; i < _pluginsList->count(); ++i) {
        QListWidgetItem *item = _pluginsList->item(i);
        bool hasGroup = (item->data(PLUGIN_LIST_ROLE_GROUP).toString() == group || group == tr("All"));
        item->setHidden(group.isEmpty() ? true : !hasGroup);
    }
}

void NatronPluginManager::handlePluginButtonReleased(const QString &id,
                                                     int type)
{
    qDebug() << "do something" << id << type;
    if (!_plugins->hasPlugin(id)) { return; }
    switch (type) {
    case Plugins::NATRON_PLUGIN_TYPE_AVAILABLE:
        installPlugin(id);
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
        QtConcurrent::run(_plugins, &Plugins::checkRepositories, false);
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

