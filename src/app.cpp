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
#include <QLabel>
#include <QPushButton>
#include <QSettings>

//#include "plugindelegate.h"

#define NATRON_ICON ":/NatronPluginManager.png"
#define NATRON_STYLE ":/stylesheet.qss"
#define NATRON_PLUGIN_DOC_HTML ":/doc.html"

#define PLUGIN_TREE_ROLE_ID Qt::UserRole+1
#define PLUGIN_TREE_ROLE_TYPE Qt::UserRole+2

#define PLUGIN_LIST_ROLE_GROUP Qt::UserRole+3

NatronPluginManager::NatronPluginManager(QWidget *parent)
    : QMainWindow(parent)
    , _comboStatus(nullptr)
    , _comboGroup(nullptr)
    , _toolBar(nullptr)
    , _stack(nullptr)
    , _plugins(nullptr)
    , _groupTree(nullptr)
    //, _availablePluginsTree(nullptr)
    //, _installedPluginsTree(nullptr)
    //, _pluginDesc(nullptr)
    //, _pluginLabel(nullptr)
    //, _pluginIcon(nullptr)
    //, _leftTab(nullptr)
    //, _statusBar(nullptr)
    //, _progBar(nullptr)
    //, _installButton(nullptr)
    //, _removeButton(nullptr)
    //, _updateButton(nullptr)
    , _menuBar(nullptr)
    , _pluginsTree(nullptr)
{
  //  setMinimumSize(900, 600);
    setMinimumWidth(340);
    showMaximized();

    setWindowIcon(QIcon(NATRON_ICON));

    //setupPalette();
    setupStyle();
    setupPlugins();
    setupMenu();
  //  setupStatusBar();
  //  setupButtons();
    //setupPluginInfo();

    _toolBar = new QToolBar(this);
    _toolBar->setObjectName("ToolBar");
    _toolBar->setMovable(false);
    addToolBar(_toolBar);

    QLabel *comboStatusLabel = new QLabel(tr("Status"), this);
    comboStatusLabel->setObjectName("ComboStatusLabel");

    _comboStatus = new QComboBox(this);
    _comboStatus->setObjectName("ComboStatus");
    _comboStatus->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    _comboStatus->addItems(QStringList() << tr("All") << tr("Available") << tr("Installed") << tr("Updates"));

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
    mainWidget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *mainLayout = new QHBoxLayout(mainWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    mainLayout->setSpacing(0);
    setCentralWidget(mainWidget);

  /*  QWidget *topWidget = new QWidget(this);
    topWidget->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *topLayout = new QVBoxLayout(topWidget);
    topLayout->setContentsMargins(0, 0, 0, 0);*/

    /*QWidget *bottomWidget = new QWidget(this);
    bottomWidget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *bottomLayout = new QHBoxLayout(bottomWidget);*/

    QWidget *leftWidget = new QWidget(this);
    leftWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    leftWidget->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *leftLayout = new QVBoxLayout(leftWidget);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftLayout->setSpacing(0);
    leftWidget->hide();

    QWidget *rightWidget = new QWidget(this);
    rightWidget->setObjectName("rightWidget");
    //rightWidget->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *rightLayout = new QVBoxLayout(rightWidget);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    rightLayout->setSpacing(0);
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

    mainLayout->addWidget(leftWidget);
    mainLayout->addWidget(rightWidget);

    //leftLayout->addWidget(_leftTab);
  //  topLayout->addWidget(pluginInfoWidget);

    _pluginsTree = new QListWidget(this);
    _pluginsTree->setObjectName("PluginList");
//    _pluginsTree->setStyleSheet("background-color: /*#17181a*/#17181a; color: white;");
    _pluginsTree->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //_pluginsTree->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    _pluginsTree->setFrameShape(QFrame::NoFrame);
    _pluginsTree->setViewMode(QListView::IconMode);
    _pluginsTree->setGridSize(QSize(330,160));
    _pluginsTree->setResizeMode(QListView::Adjust);
    _pluginsTree->setEditTriggers(QAbstractItemView::NoEditTriggers);
   // _pluginsTree->setUniformItemSizes(true);
            //_pluginsTree->setItemDelegate(new PluginDelegate(this));

 //   _pluginsTree->setViewMode    (QListWidget::IconMode  );
   // _pluginsTree->setResizeMode  (QListWidget::Adjust    );

    QWidget *stackWidget1 = new QWidget(this);
    QVBoxLayout *stackWidgetLayout1 = new QVBoxLayout(stackWidget1);
    QLabel *stackLabel1 = new QLabel("<h1 style=\"text-align: center;\"><img src=\":/NatronPluginManager.png\"><br>Natron<br><span style=\"font-weight:normal;\">Plug-in Manager</span><br><br><span style=\"font-size:small;text-transform:uppercase;;\">Loading please wait ...</span></h1>", this);
    stackWidgetLayout1->addStretch();
    stackWidgetLayout1->addWidget(stackLabel1);
    stackWidgetLayout1->addStretch();

    _stack = new QStackedWidget(this);
    _stack->addWidget(stackWidget1);
    _stack->addWidget(_pluginsTree);

    _stack->setCurrentIndex(0);
    rightLayout->addWidget(_stack);

    _groupTree = new QTreeWidget(this);
    _groupTree->setObjectName("GroupTree");
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
    _groupTree->setHeaderHidden(true);
    _groupTree->setFrameShape(QFrame::NoFrame);
    _groupTree->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    leftLayout->addWidget(_groupTree);
    //rightLayout->addWidget(_pluginDesc);

    _menuBar->setObjectName("MenuBar");
    //_menuBar->setStyleSheet("background-color:#232528;border:0;");
  //  setDefaultPluginInfo();

    //_statusBar->hide();
    _groupTree->hide();
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
    /*if (_appDoc.isEmpty()) {
        QFile doc(NATRON_PLUGIN_DOC_HTML);
        if (doc.open(QIODevice::ReadOnly | QIODevice::Text)) {
            _appDoc = doc.readAll();
            doc.close();
        }
    }*/
 //   _pluginDesc->setHtml(_appDoc);
    /*_pluginLabel->setText(QString("<h1 style=\"font-weight: normal;\">Natron"
                                  "<br><span style=\"font-size: small;\">%1 v%2</span></h1>")
                          .arg(tr("Plug-in Manager"), qApp->applicationVersion()));
    _pluginIcon->setPixmap(QIcon(QString(NATRON_ICON)).pixmap(48, 48).scaled(48,
                                                                             48,
                                                                             Qt::KeepAspectRatio,
                                                                             Qt::SmoothTransformation));*/
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
    return QStringList();//return getCheckedPlugins(_availablePluginsTree);
}

const QStringList NatronPluginManager::getCheckedInstalledPlugins()
{
    return QStringList();//return getCheckedPlugins(_installedPluginsTree);
}

QWidget *NatronPluginManager::generatePluginWidget(const Plugins::PluginSpecs &plugin)
{
    QWidget *pluginContainer = new QWidget(this);
    pluginContainer->setFixedSize(_pluginsTree->gridSize());
    QVBoxLayout *pluginContainerLayout = new QVBoxLayout(pluginContainer);

    QFrame *pluginWidget = new QFrame(this);

    //pluginWidget->setObjectName("pluginWidget");
    //pluginWidget->setFrameShape(QFrame::StyledPanel);
    //pluginWidget->setStyleSheet(".QFrame {background-color:#191919;border: 2px solid #252525; border-radius: 10%;}");

    pluginContainerLayout->addWidget(pluginWidget);

    //pluginWidget->setFrameShape(QFrame::StyledPanel);
   //pluginWidget->setStyleSheet("QFrame { border: 1px solid red; }");

    QVBoxLayout *pluginLayout = new QVBoxLayout(pluginWidget);

    QWidget *pluginHeader = new QWidget(pluginWidget);
    QHBoxLayout *pluginHeaderLayout = new QHBoxLayout(pluginHeader);

    QLabel *pluginLabel = new QLabel(pluginWidget);
    //pluginLabel->setStyleSheet("color: #ebebeb;");
    QLabel *pluginIcon = new QLabel(pluginWidget);
    pluginIcon->setMinimumSize(48, 48);
    pluginIcon->setMaximumSize(48, 48);



    pluginLabel->setText(QString("<h3 style=\"font-weight: normal;color:#ebebeb;\">%1<br>"
                                  "<span class=\"pluginLabelCategory\" style=\"font-size: small;color:#626362;\">%2</span></h3>")
                          .arg(plugin.label,
                               plugin.group));
    if (plugin.icon.isEmpty()) {
        pluginIcon->setPixmap(QIcon(QString(NATRON_ICON)).pixmap(48,
                                                                  48).scaled(48,
                                                                             48,
                                                                             Qt::KeepAspectRatio,
                                                                             Qt::SmoothTransformation));
    } else {
        pluginIcon->setPixmap(QIcon(QString("%1/%2").arg(plugin.path,
                                                          plugin.icon)).pixmap(48,
                                                                              48).scaled(48,
                                                                                         48,
                                                                                         Qt::KeepAspectRatio,
                                                                                         Qt::SmoothTransformation));
    }

    pluginLayout->addWidget(pluginHeader);
    pluginHeaderLayout->addWidget(pluginIcon);
    pluginHeaderLayout->addWidget(pluginLabel);

    QWidget *pluginBottom = new QWidget(this);
    QHBoxLayout *pluginBottomLayout = new QHBoxLayout(pluginBottom);

    QString buttonLabel;
    //QString buttonStyle;
    if (_plugins->hasAvailablePlugin(plugin.id)) {
        buttonLabel = tr("Install");
     //   buttonStyle = "border: 2px solid #2e82be; border-radius: 10%;;color:#ebebeb;padding: 0.5em;";
    } else if (_plugins->hasInstalledPlugin(plugin.id)) {
        buttonLabel = tr("Remove");
       // buttonStyle = "border: 2px solid #be2e44; border-radius: 10%;;color:#ebebeb;padding: 0.5em;";
    }
    QPushButton *button = new QPushButton(buttonLabel,pluginWidget);
    if (_plugins->hasAvailablePlugin(plugin.id)) {
        button->setProperty("InstallButton", true);
    } else if (_plugins->hasInstalledPlugin(plugin.id)) {
        button->setProperty("RemoveButton", true);
    }

   // button->setStyleSheet(buttonStyle);
    button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    pluginBottomLayout->addStretch();
    pluginBottomLayout->addWidget(button);
    pluginLayout->addWidget(pluginBottom);

    //QWidget *descWidget = new QWidget(pluginWidget);

   // QTextBrowser *browser = new QTextBrowser(pluginWidget);
    //browser->setStyleSheet("");
   // browser->setFrameShape(QFrame::NoFrame);
  //  browser->setHtml(plugin.desc.isEmpty()?"opk pok po refoij eofij eorjfoeri oerij foreijfoerof reo iejrg åoirj gåiowerjg åqweiogj tgrjbnnoyrtjn qwf epokfåqre otij rtpij oirjf poirjf wporijf weeråwj åwerigj åweifj erifj åqweifj åweoijf åwoiejf åtoijyt åhtiojh qåewifj åiojf qrå  g rugh qerpiug peiurg hpweruig hfijerofj roifj reojfåeoirjg åqreiojg åqeroigj qeåroigj qeåroigj qerk pok pok po k":plugin.desc);
    /*QString desc = plugin.desc.replace("\\n", "<br>").replace("\\", "").simplified();
    if (desc.isEmpty()) {
        desc = QString("<p>%1.</p>").arg(tr("No description available"));
    }

    desc = desc.replace(QRegExp("((?:https?|ftp)://\\S+)"),
                        "<a href=\"\\1\">\\1</a>");

    _pluginDesc->setHtml(desc);*/
//    pluginLayout->addWidget(browser);
    return pluginContainer;
}

void NatronPluginManager::setupStyle()
{
    qApp->setStyle(QString("fusion"));

    QPalette palette;
    palette.setColor(QPalette::Window, QColor(/*53, 53, 53*/"#232528"));
    palette.setColor(QPalette::WindowText, /*Qt::white*/"#ebebeb");
    palette.setColor(QPalette::Base, QColor(15, 15, 15));
    palette.setColor(QPalette::AlternateBase, QColor(/*53, 53, 53*/"#232528"));
    palette.setColor(QPalette::Link, Qt::white);
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

    qApp->setPalette(palette);



    QSettings settings;
    QFile styleFile(settings.value("stylesheet", NATRON_STYLE).toString());
    if (styleFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QString stylesheet = styleFile.readAll();
        styleFile.close();
        qApp->setStyleSheet(stylesheet);
    }
}

void NatronPluginManager::setupPalette()
{
    /*QPalette palette;
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
    palette.setColor(QPalette::Disabled, QPalette::ButtonText, Qt::darkGray);*/

    //qApp->setPalette(palette);

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
    /*_statusBar = new QStatusBar(this);
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

    _statusBar->addPermanentWidget(_progBar);*/
}

void NatronPluginManager::setupButtons()
{
    /*_installButton = new QPushButton(tr("Install"), this);
    _installButton->setEnabled(false);
    _installButton->setIcon(QApplication::style()->standardPixmap(QStyle::SP_DialogOkButton));

    _removeButton = new QPushButton(tr("Remove"), this);
    _removeButton->setEnabled(false);

    connect(_installButton,
            SIGNAL(released()),
            this,
            SLOT(handleInstallButton()));
    connect(_removeButton,
            SIGNAL(released()),
            this,
            SLOT(handleRemoveButton()));*/
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
   /* _leftTab = new QTabWidget(this);
    _leftTab->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    _leftTab->setTabPosition(QTabWidget::West);
*/
    /*QWidget *installTabWidget = new QWidget(this);
    installTabWidget->setContentsMargins(0, 0, 0, 0);
    QVBoxLayout *installTabLayout = new QVBoxLayout(installTabWidget);
    installTabLayout->setContentsMargins(0, 0, 0, 0);
    installTabLayout->setSpacing(0);

    QWidget *installButtonWidget = new QWidget(this);
    installButtonWidget->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *installButtonLayout = new QHBoxLayout(installButtonWidget);
    installButtonLayout->setContentsMargins(2, 2, 2, 2);
*/
    //installButtonLayout->addWidget(_installButton);
    //installTabLayout->addWidget(_availablePluginsTree);
  /*  installTabLayout->addWidget(installButtonWidget);

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
    //removeTabLayout->addWidget(_installedPluginsTree);
    removeTabLayout->addWidget(removeButtonWidget);

    _leftTab->addTab(installTabWidget, tr("Available"));
  //  _leftTab->addTab(removeTabWidget, tr("Installed"));
    //_leftTab->addTab(new QWidget(), tr("Updates"));
*/
    //_leftTab->hide();
}

void NatronPluginManager::setupPluginInfo()
{
    /*_pluginLabel = new QLabel(this);
    _pluginIcon = new QLabel(this);

    _pluginDesc = new QTextBrowser(this);
    _pluginDesc->setReadOnly(true);
    _pluginDesc->setOpenLinks(true);
    _pluginDesc->setOpenExternalLinks(true);

    _pluginLabel->hide();
    _pluginDesc->hide();
    _pluginIcon->hide();*/
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

    /*populatePluginsTree(Plugins::NATRON_PLUGIN_TYPE_INSTALLED,
                        _installedPluginsTree);
    populatePluginsTree(Plugins::NATRON_PLUGIN_TYPE_AVAILABLE,
                        _availablePluginsTree);*/
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
    /*if (!_statusBar) { return; }
    _statusBar->showMessage(message, 1000);*/
    qDebug() << message;
}

void NatronPluginManager::handleDownloadStatusMessage(const QString &message,
                                                      qint64 value,
                                                      qint64 total)
{
    /*if (_progBar->isHidden()) { _progBar->show(); }
    _statusBar->showMessage(message, 1000);
    _progBar->setRange(0, total);
    _progBar->setValue(value);
    if (value == total && _progBar->isVisible()) { _progBar->hide(); }*/
    qDebug() << message << value << total;
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

    /*_pluginLabel->setText(QString("<h1 style=\"font-weight: normal;\">%1<br>"
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
    }*/

    QString desc = specs.desc.replace("\\n", "<br>").replace("\\", "").simplified();
    if (desc.isEmpty()) {
        desc = QString("<p>%1.</p>").arg(tr("No description available"));
    }

    desc = desc.replace(QRegExp("((?:https?|ftp)://\\S+)"),
                        "<a href=\"\\1\">\\1</a>");

   // _pluginDesc->setHtml(desc);
}

void NatronPluginManager::handleInstallButton()
{
    /*QStringList plugins = getCheckedAvailablePlugins();
    if (plugins.size() > 0) { installPlugins(); }
    else {
        QMessageBox::information(this,
                                 tr("Install"),
                                 tr("No plug-ins marked for installation."));
    }*/
}

void NatronPluginManager::handleRemoveButton()
{
    /*QStringList plugins = getCheckedInstalledPlugins();
    if (plugins.size() > 0) { removePlugins(); }
    else {
        QMessageBox::information(this,
                                 tr("Install"),
                                 tr("No plug-ins marked for removal."));
    }*/
}

void NatronPluginManager::updatePlugins()
{
    //_installButton->setEnabled(false);
    //_removeButton->setEnabled(false);
    //setDefaultPluginInfo();
    _plugins->checkRepositories();


}

void NatronPluginManager::populatePluginsTree(Plugins::PluginType type,
                                              QTreeWidget *tree)
{
    /*if (!tree) { return; }

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
        //tree->expandItem(groupItem); // add to settings?
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
*/
    std::vector<Plugins::PluginSpecs> myplugins =  _plugins->getPlugins();// _plugins->getPluginsInGroup(Plugins::NATRON_PLUGIN_TYPE_AVAILABLE, "Filter");

    _groupTree->clear();

    QStringList groups = _plugins->getPluginGroups(Plugins::NATRON_PLUGIN_TYPE_AVAILABLE);
    _pluginsTree->clear();

    for (int i = 0; i < groups.size(); ++i) {
        QTreeWidgetItem *gItem = new QTreeWidgetItem(_groupTree);
        gItem->setText(0, groups.at(i));
    }

    //_pluginDesc->hide();

    for (unsigned long i = 0; i< myplugins.size(); ++i) {
        //QPushButton *button = new QPushButton("Install", this);
        //button->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);



        QListWidgetItem *item = new QListWidgetItem();
        QWidget *widget = generatePluginWidget(myplugins.at(i));
        item->setSizeHint(/*widget->sizeHint()*/_pluginsTree->gridSize());
        _pluginsTree->addItem(item);
        _pluginsTree->setItemWidget(item, widget);

        qDebug() << "add group" << myplugins.at(i).group;


    //    _pluginsTree->addItem(item);
        //item->setText(myplugins.at(i).label);
        //item->setIcon(QIcon(":/NatronPluginManager.png"));
  /*      item->setText(myplugins.at(i).label);
        QString iconPath = QString("%1/%2").arg(myplugins.at(i).path, myplugins.at(i).icon);
        QIcon ico(iconPath);
        if (ico.isNull() || !QFile::exists(iconPath) || myplugins.at(i).icon.isEmpty()) { ico = QIcon(":/NatronPluginManager.png"); }
        item->setIcon(ico);
        item->setData(NATRON_PLUGIN_ITEM_ROLE_ID, myplugins.at(i).id);
        item->setData(NATRON_PLUGIN_ITEM_ROLE_TITLE, myplugins.at(i).label);
        QString desc = myplugins.at(i).desc;
        item->setData(NATRON_PLUGIN_ITEM_ROLE_DESC, desc);
        item->setData(NATRON_PLUGIN_ITEM_ROLE_VERSION, myplugins.at(i).version);
        item->setData(NATRON_PLUGIN_ITEM_ROLE_ICON, QString("%1/%2").arg(myplugins.at(i).path, myplugins.at(i).icon));
*/
        //_pluginsTree->addw
       // _pluginsTree->setItemWidget(item, generatePluginWidget(myplugins.at(i)));
     //   item->setSizeHint(_pluginsTree->gridSize());

    }
    /*for (int i = 0; i < _pluginsTree->count(); ++i) {
        _pluginsTree->item(i)->setSizeHint(_pluginsTree->gridSize());
    }*/
    //_pluginsTree->update();

    // _pluginsTree->update();
}

void NatronPluginManager::populatePlugins(const QString &group)
{
    std::vector<Plugins::PluginSpecs> myplugins =  _plugins->getPlugins();// _plugins->getPluginsInGroup(Plugins::NATRON_PLUGIN_TYPE_AVAILABLE, "Filter");
    QStringList groups = _plugins->getPluginGroups();
    _groupTree->clear();

    _comboGroup->clear();
    _comboGroup->addItem(tr("All"));
    _comboGroup->insertSeparator(1);
    _comboGroup->addItems(groups);
    _comboGroup->adjustSize();
    //_comboGroup->update();

    if (_groupTree->isHidden()) { _groupTree->show(); }

    _pluginsTree->clear();

    QStringList addedGroups;
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
        }


    }
    for (unsigned long i = 0; i< myplugins.size(); ++i) {
        QListWidgetItem *item = new QListWidgetItem();
        item->setData(PLUGIN_LIST_ROLE_GROUP, myplugins.at(i).group);
        QWidget *widget = generatePluginWidget(myplugins.at(i));
        item->setSizeHint(/*widget->sizeHint()*/_pluginsTree->gridSize());
        _pluginsTree->addItem(item);
        _pluginsTree->setItemWidget(item, widget);
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
    for (int i = 0; i < _pluginsTree->count(); ++i) {
        QListWidgetItem *item = _pluginsTree->item(i);
        bool hasGroup = (item->data(PLUGIN_LIST_ROLE_GROUP).toString() == group || group == tr("All"));
        item->setHidden(group.isEmpty() ? true : !hasGroup);
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

