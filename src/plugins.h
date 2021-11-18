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
#include <QStringList>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>

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

    struct RepoSpecs {
        QString label;
        QString folder;
        QUrl archive;
        QUrl manifest;
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
    ~Plugins();

    void scanForAvailablePlugins(const QString &path,
                                 bool append = false);
    void scanForInstalledPlugins(const QString &path,
                                 bool append = false);

    bool hasAvailablePlugin(const QString &id);
    bool hasInstalledPlugin(const QString &id);

    Plugins::PluginSpecs getPlugin(const QString &id);
    Plugins::PluginSpecs getAvailablePlugin(const QString &id);
    Plugins::PluginSpecs getInstalledPlugin(const QString &id);

    std::vector<Plugins::PluginSpecs> getPlugins();
    std::vector<Plugins::PluginSpecs> getAvailablePlugins();
    std::vector<Plugins::PluginSpecs> getInstalledPlugins();

    const QStringList getPluginGroups();
    const QStringList getPluginGroups(Plugins::PluginType type);
    const std::vector<Plugins::PluginSpecs> getPluginsInGroup(Plugins::PluginType type,
                                                              const QString &group);

    const QString getValueFromFile(const QString &key,
                                   const QString &filename,
                                   bool toHtml = false);
    Plugins::PluginSpecs getPluginSpecs(const QString &path);
    bool isValidPlugin(const Plugins::PluginSpecs &plugin);
    bool folderHasPlugin(const QString &path);
    int folderHasPlugins(const QString &path);

    const QString getUserPluginPath();
    const QString getCachePath();
    const QString getRepoPath();
    const QString getRepoPath(const QString &uid);
    const QString getRandom();
    const QString getTempPath();

    Plugins::PluginStatus installPlugin(const QString &id);
    Plugins::PluginStatus removePlugin(const QString &id);

    Plugins::PluginStatus extractPluginArchive(const QString &filename,
                                               const QString &folder);

    bool isValidRepository(const RepoSpecs &repo);
    void loadRepositories();
    void saveRepositories(const std::vector<RepoSpecs> &repos);
    void checkRepositories();
    std::vector<Plugins::RepoSpecs> getAvailableRepositories();
    Plugins::RepoSpecs getRepoFromUrl(const QUrl &url);

    bool isBusy();

    void removeFromDownloadQueue(const QUrl &url);

signals:

    void updatedPlugins();
    void statusMessage(const QString &message);
    void statusDownload(const QString &message,
                        qint64 value,
                        qint64 total);
    void statusError(const QString &message);
    void downloadRequired();

private:

    bool _isWorking;
    bool _isDownloading;
    std::vector<Plugins::PluginSpecs> _availablePlugins;
    std::vector<Plugins::PluginSpecs> _installedPlugins;
    std::vector<Plugins::RepoSpecs> _availableRepositories;
    std::vector<QUrl> _downloadQueue;
    QNetworkAccessManager *_nam;

    static bool comparePluginsOrder(const Plugins::PluginSpecs &a,
                                    const Plugins::PluginSpecs &b)
    {
        return a.label < b.label;
    }

private slots:

    void startDownloads();
    void handleFileDownloaded(QNetworkReply *reply);
    void handleDownloadError(QNetworkReply::NetworkError error);
    void handleDownloadProgress(qint64 value,
                                qint64 total);
    void handleDownloadReadyRead();
};

#endif // PLUGINS_H
