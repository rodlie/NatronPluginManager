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

#include "pluginviewwidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFile>
#include <QIcon>
#include <QPixmap>
#include <QRegExp>

PluginViewWidget::PluginViewWidget(QWidget *parent,
                                   Plugins *plugins,
                                   QSize iconSize)
    : QWidget(parent)
    , _plugins(plugins)
    , _goBackButton(nullptr)
    , _pluginIconLabel(nullptr)
    , _pluginTitleLabel(nullptr)
    , _pluginGroupLabel(nullptr)
    , _pluginDescBrowser(nullptr)
    , _iconSize(iconSize)
{
    setObjectName("PluginViewWidget");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    QWidget *headerWidget = new QWidget(this);
    headerWidget->setSizePolicy(QSizePolicy::Expanding,
                                QSizePolicy::Fixed);

    QHBoxLayout *headerLayout = new QHBoxLayout(headerWidget);

    _goBackButton = new QPushButton(tr("❮"), this);
    _goBackButton->setSizePolicy(QSizePolicy::Fixed,
                                 QSizePolicy::Expanding);
    _goBackButton->setObjectName("GoBackButton");
    connect(_goBackButton,
            SIGNAL(released()),
            this,
            SLOT(handleGoBackButton()));

    _pluginIconLabel = new QLabel(this);
    _pluginIconLabel->setMinimumSize(_iconSize);
    _pluginIconLabel->setMaximumSize(_iconSize);

    _pluginIconLabel->setPixmap(QIcon(QString(DEFAULT_ICON)).pixmap(_iconSize).scaled(iconSize,
                                                                                      Qt::KeepAspectRatio,
                                                                                      Qt::SmoothTransformation));

    QWidget *pluginHeaderWidget = new QWidget(this);
    pluginHeaderWidget->setObjectName("PluginViewHeaderWidget");
    QVBoxLayout *pluginHeaderLayout = new QVBoxLayout(pluginHeaderWidget);

    _pluginTitleLabel = new QLabel(tr("Title"), this);
    _pluginTitleLabel->setObjectName("PluginViewTitleLabel");

    _pluginGroupLabel = new QLabel(tr("Group"), this);
    _pluginGroupLabel->setObjectName("PluginViewGroupLabel");

    pluginHeaderLayout->addStretch();
    pluginHeaderLayout->addWidget(_pluginTitleLabel);
    pluginHeaderLayout->addWidget(_pluginGroupLabel);
    pluginHeaderLayout->addStretch();

    headerLayout->addWidget(_goBackButton);
    headerLayout->addWidget(_pluginIconLabel);
    headerLayout->addWidget(pluginHeaderWidget);

    _pluginDescBrowser = new QTextBrowser(this);
    _pluginDescBrowser->setObjectName("PluginViewBrowser");
    _pluginDescBrowser->setOpenLinks(true);
    _pluginDescBrowser->setOpenExternalLinks(true);
    _pluginDescBrowser->setReadOnly(true);

    mainLayout->addWidget(headerWidget);
    mainLayout->addWidget(_pluginDescBrowser);
}

void PluginViewWidget::showPlugin(const QString &id)
{
    if (!_plugins || id.isEmpty()) { return; }
    Plugins::PluginSpecs plugin = _plugins->getPlugin(id);
    if (!_plugins->isValidPlugin(plugin)) { return; }

    _pluginTitleLabel->setText(plugin.label);
    _pluginGroupLabel->setText(plugin.group);

    _pluginIconLabel->setPixmap(QIcon(QString(DEFAULT_ICON)).pixmap(_iconSize).scaled(_iconSize,
                                                                                      Qt::KeepAspectRatio,
                                                                                      Qt::SmoothTransformation));
    QString pluginIconPath = QString("%1/%2").arg(plugin.path, plugin.icon);
    if (!plugin.icon.isEmpty() && QFile::exists(pluginIconPath)) {
        QPixmap pluginPixmap = QIcon(pluginIconPath).pixmap(_iconSize).scaled(_iconSize,
                                                                             Qt::KeepAspectRatio,
                                                                             Qt::SmoothTransformation);
        if (!pluginPixmap.isNull()) { _pluginIconLabel->setPixmap(pluginPixmap); }
    }

    QString desc = plugin.desc.replace("\\n", "<br>").replace("\\", "").simplified();
    if (desc.isEmpty()) {
        desc = QString("<p>%1.</p>").arg(tr("No description available"));
    }

    desc = desc.replace(QRegExp("((?:https?|ftp)://\\S+)"),
                        "<a href=\"\\1\">\\1</a>");

    _pluginDescBrowser->setHtml(desc);
}

void PluginViewWidget::handleGoBackButton()
{
    emit goBack();
}
