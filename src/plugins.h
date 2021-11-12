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

#ifndef PLUGINS_H
#define PLUGINS_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <vector>

class Plugins : public QObject
{
    Q_OBJECT

public:

    struct PluginSpecs {
        QString id;
        QString label;
        double version = 0;
        QString icon;
        QString group;
        QString desc;
        QString path;
        QString folder;
    };

    struct PluginStatus {
        bool success = false;
        QString message;
    };

    enum PluginType {
        NATRON_PLUGIN_TYPE_AVAILABLE,
        NATRON_PLUGIN_TYPE_INSTALLED,
        NATRON_PLUGIN_TYPE_UPDATE
    };

    explicit Plugins(QObject *parent = nullptr);

    void scanForAvailablePlugins(const QString &path);
    void scanForInstalledPlugins(const QString &path);

    bool hasAvailablePlugin(const QString &id);
    bool hasInstalledPlugin(const QString &id);

    Plugins::PluginSpecs getAvailablePlugin(const QString &id);
    Plugins::PluginSpecs getInstalledPlugin(const QString &id);

    std::vector<Plugins::PluginSpecs> getAvailablePlugins();
    std::vector<Plugins::PluginSpecs> getInstalledPlugins();

    const QStringList getPluginGroups(Plugins::PluginType type);
    const std::vector<Plugins::PluginSpecs> getPluginsInGroup(Plugins::PluginType type,
                                                              const QString &group);

    const QString getValueFromFile(const QString &key,
                                   const QString &filename,
                                   bool toHtml = false);
    Plugins::PluginSpecs getPluginSpecs(const QString &path);
    bool isValidPlugin(const Plugins::PluginSpecs &plugin);
    bool folderHasPlugin(const QString &path);

    const QString getUserPluginPath();
    const QString getCachePath();

    Plugins::PluginStatus installPlugin(const QString &id);

private:

    std::vector<Plugins::PluginSpecs> _availablePlugins;
    std::vector<Plugins::PluginSpecs> _installedPlugins;

    static bool comparePluginsOrder(const Plugins::PluginSpecs &a,
                                    const Plugins::PluginSpecs &b)
    {
        return a.label < b.label;
    }
};

#endif // PLUGINS_H
