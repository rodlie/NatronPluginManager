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

#include "pluginlistwidget.h"

#include <QVBoxLayout>
#include <QFrame>
#include <QLabel>
#include <QIcon>
#include <QPixmap>
#include <QFile>

PluginListWidget::PluginListWidget(const Plugins::PluginSpecs &plugin,
                                   Plugins::PluginType type,
                                   QSize widgetSize,
                                   QSize iconSize,
                                   QWidget *parent)
    : QWidget(parent)
    , _plugin(plugin)
    , _installButton(nullptr)
    , _removeButton(nullptr)
    , _updateButton(nullptr)
{
    setFixedSize(widgetSize);

    const auto mainLayout = new QVBoxLayout(this);

    const auto pluginFrame = new QFrame(this);
    const auto pluginFrameLayout = new QVBoxLayout(pluginFrame);

    const auto pluginHeader = new QWidget(pluginFrame);
    const auto pluginHeaderLayout = new QHBoxLayout(pluginHeader);

    const auto pluginFooter = new QWidget(this);
    const auto pluginsFooterLayout = new QHBoxLayout(pluginFooter);

    const auto pluginHeaderText = new QWidget(this);
    pluginHeaderText->setContentsMargins(0, 0, 0, 0);

    const auto pluginHeaderTextLayout = new QVBoxLayout(pluginHeaderText);
    pluginHeaderTextLayout->setContentsMargins(0, 0, 0, 0);
    pluginHeaderTextLayout->setSpacing(0);

    const auto pluginTitleLabel = new QLabel(_plugin.label, this);
    const auto pluginGroupLabel = new QLabel(_plugin.group, this);
    const auto pluginIconLabel = new QLabel(this);

    pluginTitleLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    pluginGroupLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    pluginTitleLabel->setProperty("TitleLabel", true);
    pluginGroupLabel->setProperty("GroupLabel", true);

    pluginIconLabel->setMinimumSize(iconSize);
    pluginIconLabel->setMaximumSize(iconSize);

    pluginIconLabel->setPixmap(QIcon(QString(DEFAULT_ICON)).pixmap(iconSize).scaled(iconSize,
                                                                                    Qt::KeepAspectRatio,
                                                                                    Qt::SmoothTransformation));

    QString pluginIconPath = QString("%1/%2").arg(_plugin.path, _plugin.icon);
    if (!_plugin.icon.isEmpty() && QFile::exists(pluginIconPath)) {
        QPixmap pluginPixmap = QIcon(pluginIconPath).pixmap(iconSize).scaled(iconSize,
                                                                             Qt::KeepAspectRatio,
                                                                             Qt::SmoothTransformation);
        if (!pluginPixmap.isNull()) { pluginIconLabel->setPixmap(pluginPixmap); }
    }

    const auto pluginTypeLabel = new QLabel(this);
    pluginTypeLabel->setObjectName("PluginTypeLabel");
    pluginTypeLabel->setText(plugin.isAddon ? tr("Addon") : tr("PyPlug"));

    _installButton = new QPushButton(tr("Install"), this);
    _removeButton = new QPushButton(tr("Remove"), this);
    _updateButton = new QPushButton(tr("Update"), this);

    _installButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _removeButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    _updateButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    _installButton->setProperty("InstallButton", true);
    _removeButton->setProperty("RemoveButton", true);
    _updateButton->setProperty("UpdateButton", true);

    pluginsFooterLayout->addWidget(pluginTypeLabel);
    pluginsFooterLayout->addStretch();
    pluginsFooterLayout->addWidget(_updateButton);
    pluginsFooterLayout->addWidget(_installButton);
    pluginsFooterLayout->addWidget(_removeButton);

    pluginHeaderTextLayout->addStretch();
    pluginHeaderTextLayout->addWidget(pluginTitleLabel);
    pluginHeaderTextLayout->addWidget(pluginGroupLabel);
    pluginHeaderTextLayout->addStretch();

    pluginHeaderLayout->addWidget(pluginIconLabel);
    pluginHeaderLayout->addWidget(pluginHeaderText);

    pluginFrameLayout->addWidget(pluginHeader);
    pluginFrameLayout->addWidget(pluginFooter);

    mainLayout->addWidget(pluginFrame);

    setPluginStatus(_plugin.id, type);

    connect(_installButton,
            SIGNAL(released()),
            this,
            SLOT(handleInstallButtonReleased()));
    connect(_removeButton,
            SIGNAL(released()),
            this,
            SLOT(handleRemoveButtonReleased()));
    connect(_updateButton,
            SIGNAL(released()),
            this,
            SLOT(handleUpdateButtonReleased()));
}

PluginListWidget::~PluginListWidget()
{
}

void PluginListWidget::setPluginStatus(const QString &id,
                                       int type)
{
    if (_plugin.id != id) { return; }
    switch(type) {
    case Plugins::NATRON_PLUGIN_TYPE_AVAILABLE:
        _installButton->setEnabled(true);
        _installButton->setHidden(false);
        _removeButton->setEnabled(false);
        _removeButton->setHidden(true);
        _updateButton->setEnabled(false);
        _updateButton->setHidden(true);
        break;
    case Plugins::NATRON_PLUGIN_TYPE_INSTALLED:
        _installButton->setEnabled(false);
        _installButton->setHidden(true);
        _removeButton->setEnabled(true);
        _removeButton->setHidden(false);
        _updateButton->setEnabled(false);
        _updateButton->setHidden(true);
        break;
    case Plugins::NATRON_PLUGIN_TYPE_UPDATE:
        _installButton->setEnabled(false);
        _installButton->setHidden(true);
        _removeButton->setEnabled(true);
        _removeButton->setHidden(false);
        _updateButton->setEnabled(true);
        _updateButton->setHidden(false);
        break;
    default:
        _installButton->setEnabled(false);
        _installButton->setHidden(true);
        _removeButton->setEnabled(false);
        _removeButton->setHidden(true);
        _updateButton->setEnabled(false);
        _updateButton->setHidden(true);
        break;
    }
}

void PluginListWidget::handleInstallButtonReleased()
{
    emit pluginButtonReleased(_plugin.id,
                              Plugins::NATRON_PLUGIN_TYPE_AVAILABLE);
}

void PluginListWidget::handleRemoveButtonReleased()
{
    emit pluginButtonReleased(_plugin.id,
                              Plugins::NATRON_PLUGIN_TYPE_INSTALLED);
}

void PluginListWidget::handleUpdateButtonReleased()
{
    emit pluginButtonReleased(_plugin.id,
                              Plugins::NATRON_PLUGIN_TYPE_UPDATE);
}

void PluginListWidget::mouseReleaseEvent(QMouseEvent *e)
{
    emit showPlugin(_plugin.id);
    QWidget::mouseReleaseEvent(e);
}
