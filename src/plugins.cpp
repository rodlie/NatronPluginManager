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

#include "plugins.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDirIterator>
#include <QTextStream>
#include <QSettings>
#include <QStandardPaths>
#include <QRegExp>

Plugins::Plugins(QObject *parent)
    : QObject(parent)
{
}

void Plugins::scanForAvailablePlugins(const QString &path)
{
    if (!QFile::exists(path)) { return; }
    _availablePlugins.clear();
    QDirIterator it(path,
                    QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString item = it.next();
        if (!folderHasPlugin(item)) { continue; }
        PluginSpecs plugin = getPluginSpecs(item);
        if (!hasAvailablePlugin(plugin.id) &&
            !hasInstalledPlugin(plugin.id))
        { _availablePlugins.push_back(plugin); }
    }
}

void Plugins::scanForInstalledPlugins(const QString &path)
{
    if (!QFile::exists(path)) { return; }
    _installedPlugins.clear();
    QDirIterator it(path,
                    QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString item = it.next();
        if (!folderHasPlugin(item)) { continue; }
        PluginSpecs plugin = getPluginSpecs(item);
        if (!hasInstalledPlugin(plugin.id)) {
            _installedPlugins.push_back(plugin);
        }
    }
}

bool Plugins::hasAvailablePlugin(const QString &id)
{
    for (unsigned long i = 0; i < _availablePlugins.size(); ++i) {
        if (id == _availablePlugins.at(i).id) { return true; }
    }
    return false;
}

bool Plugins::hasInstalledPlugin(const QString &id)
{
    for (unsigned long i = 0; i < _installedPlugins.size(); ++i) {
        if (id == _installedPlugins.at(i).id) { return true; }
    }
    return false;
}

Plugins::PluginSpecs Plugins::getAvailablePlugin(const QString &id)
{
    for (unsigned long i = 0; i < _availablePlugins.size(); ++i) {
        if (id == _availablePlugins.at(i).id) { return _availablePlugins.at(i); }
    }
    return PluginSpecs();
}

Plugins::PluginSpecs Plugins::getInstalledPlugin(const QString &id)
{
    for (unsigned long i = 0; i < _installedPlugins.size(); ++i) {
        if (id == _installedPlugins.at(i).id) { return _installedPlugins.at(i); }
    }
    return PluginSpecs();
}

std::vector<Plugins::PluginSpecs> Plugins::getAvailablePlugins()
{
    return _availablePlugins;
}

std::vector<Plugins::PluginSpecs> Plugins::getInstalledPlugins()
{
    return _installedPlugins;
}

const QStringList Plugins::getPluginGroups(Plugins::PluginType type)
{
    QStringList result;
    std::vector<PluginSpecs> plugins;
    switch (type) {
    case NATRON_PLUGIN_TYPE_AVAILABLE:
        plugins = _availablePlugins;
        break;
    case NATRON_PLUGIN_TYPE_INSTALLED:
        plugins = _installedPlugins;
        break;
    case NATRON_PLUGIN_TYPE_UPDATE:
        break;
    default:;
    }
    for (unsigned long i = 0; i < plugins.size(); ++i) {
        PluginSpecs plugin = plugins.at(i);
        if (!result.contains(plugin.group)) { result << plugin.group; }
    }
    result.sort();
    return result;
}

const std::vector<Plugins::PluginSpecs> Plugins::getPluginsInGroup(Plugins::PluginType type,
                                                                   const QString &group)
{
    std::vector<PluginSpecs> result, plugins;
    switch (type) {
    case NATRON_PLUGIN_TYPE_AVAILABLE:
        plugins = _availablePlugins;
        break;
    case NATRON_PLUGIN_TYPE_INSTALLED:
        plugins = _installedPlugins;
        break;
    case NATRON_PLUGIN_TYPE_UPDATE:
        break;
    default:;
    }
    for (unsigned long i = 0; i < plugins.size(); ++i) {
        PluginSpecs plugin = plugins.at(i);
        if (group == plugin.group) { result.push_back(plugin); }
    }
    std::sort(result.begin(), result.end(), comparePluginsOrder);
    return result;
}

const QString Plugins::getValueFromFile(const QString &key,
                                        const QString &filename,
                                        bool toHtml)
{
    QString value;
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        bool getValue = false;
        while(!in.atEnd()) {
            QString line = in.readLine();
            if (!getValue && value.isEmpty() && line.contains(key)) {
                getValue = true;
                continue;
            }
            if (getValue && value.isEmpty()) {
                QString tmp = line.replace(line.indexOf("return"),
                                           QString("return").size(),
                                           "").trimmed();
                if (tmp.startsWith("\"") && tmp.endsWith("\"")) {
                    tmp = tmp.remove(0, 1);
                    tmp = tmp.remove(tmp.size() - 1, 1);
                }
                value = toHtml? tmp.toHtmlEscaped() : tmp;
                break;
            }
        }
        file.close();
    }
    return value;
}

Plugins::PluginSpecs Plugins::getPluginSpecs(const QString &path)
{
    PluginSpecs specs;
    if (!QFile::exists(path)) { return specs; }

    QDir dir(path);
    QString folder = dir.dirName();
    if (folder.isEmpty()) { return specs; }

    QString pyFile = QString("%1/%2.py").arg(path, folder);
    if (QFile::exists(pyFile)) {
        specs.id = getValueFromFile("getPluginID():", pyFile);
        specs.label = QString(getValueFromFile("getLabel():", pyFile)).replace("_", " ");
        specs.version = getValueFromFile("getVersion():", pyFile).toDouble();
        specs.icon = getValueFromFile("getIconPath():", pyFile);
        specs.group = QString(getValueFromFile("getGrouping():", pyFile)).replace("Community/", "");
        specs.desc = getValueFromFile("getPluginDescription():", pyFile, true);
        specs.path = path;
        specs.folder = folder;
    }
    return specs;
}

bool Plugins::isValidPlugin(const Plugins::PluginSpecs &plugin)
{
    if (!plugin.id.isEmpty() &&
        !plugin.group.isEmpty() &&
        !plugin.label.isEmpty() &&
        !plugin.path.isEmpty() &&
        !plugin.folder.isEmpty() &&
        plugin.version > 0)
    { return true; }
    return false;
}

bool Plugins::folderHasPlugin(const QString &path)
{
    if (!QFile::exists(path)) { return false; }
    PluginSpecs specs = getPluginSpecs(path);
    if (!specs.id.isEmpty()) { return true; }
    return false;
}

const QString Plugins::getUserPluginPath()
{
    QSettings settings;
    QString folder = settings.value("UserPluginPath",
                                    QString("%1/.Natron/plugins")
                                    .arg(QDir::homePath())).toString();
    if (!QFile::exists(folder)) {
        QDir dir;
        dir.mkpath(folder);
    }
    return folder;
}

const QString Plugins::getCachePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
}

Plugins::PluginStatus Plugins::installPlugin(const QString &id)
{
    PluginStatus status;
    if (hasInstalledPlugin(id)) {
        status.message = tr("Plug-in already installed");
        return status;
    }
    if (!hasAvailablePlugin(id)) {
        status.message = tr("Plug-in not available");
        return  status;
    }
    PluginSpecs plugin = getAvailablePlugin(id);
    if (!isValidPlugin(plugin)) {
        status.message = tr("Not a valid plug-in");
        return  status;
    }

    QString destPath = QString("%1/%2").arg(getUserPluginPath(), plugin.folder);
    if (QFile::exists(destPath)) {
        status.message = tr("Plug-in directory (%1) already exists").arg(destPath);
        return status;
    }

    QDir pluginDir(plugin.path);
    QStringList files = pluginDir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks);
    if (files.size() < 1) {
        status.message = tr("No files to install");
        return status;
    }

    QDir destDir;
    if (!destDir.mkpath(destPath)) {
        status.message = tr("Unable to create directory %1").arg(destPath);
        return status;
    }
    for (int i = 0; i < files.size(); ++i) {
        QString fileSrc = QString("%1/%2").arg(plugin.path, files.at(i));
        QString fileDst = QString("%1/%2").arg(destPath, files.at(i));
        QFile file(fileSrc);
        QFileInfo info(fileSrc);
        if (info.isDir()) { continue; }
        if (!file.copy(fileDst)) {
            status.message = tr("Unable to copy file %1 to %2").arg(files.at(i), destPath);
            status.success = false;
            return status;
        }
    }
    status.success = true;
    return status;
}
