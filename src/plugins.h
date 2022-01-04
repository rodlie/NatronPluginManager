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

#ifndef PLUGINS_H
#define PLUGINS_H

#include <QObject>
#include <QStringList>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDateTime>

#include <vector>

#define DEFAULT_ICON ":/NatronPluginManager.png"

#define PLUGINS_SETTINGS_KEY_REPOS "repositories"
#define PLUGINS_SETTINGS_KEY_STYLE "stylesheet"

#define PLUGINS_SETTINGS_TITLE_FONT_SIZE "pluginTitleFontSize"
#define PLUGINS_SETTINGS_GROUP_FONT_SIZE "pluginGroupFontSize"

#define PLUGINS_SETTINGS_ICON_SIZE "PluginIconSize"
#define PLUGINS_SETTINGS_ICON_SIZE_DEFAULT 48

#define PLUGINS_SETTINGS_LARGE_ICON_SIZE "PluginLargeIconSize"
#define PLUGINS_SETTINGS_LARGE_ICON_SIZE_DEFAULT 64

#define PLUGINS_SETTINGS_GRID_SIZE "PluginGridSize"
#define PLUGINS_SETTINGS_GRID_WIDTH 330
#define PLUGINS_SETTINGS_GRID_HEIGHT 160

#define PLUGINS_SETTINGS_USER_PATH "UserPluginPath"
#define ADDONS_SETTINGS_USER_PATH "UserAddonPath"

#define MANIFEST_TAG_ROOT "repo"
#define MANIFEST_TAG_VERSION "version"
#define MANIFEST_TAG_TITLE "title"
#define MANIFEST_TAG_URL "url"
#define MANIFEST_TAG_MANIFEST "manifest"
#define MANIFEST_TAG_LOGO "logo"
#define MANIFEST_TAG_ZIP "zip"
#define MANIFEST_TAG_CHECKSUM "checksum"
#define MANIFEST_TAG_MODIFIED "modified"
#define MANIFEST_MODIFIED_FORMAT "yyyy-MM-dd HH:mm"

class Plugins : public QObject
{
    Q_OBJECT

public:

    struct PluginSpecs {
        QString id;
        QString label;
        double version = 0.0;
        QString icon;
        QString group;
        QString desc;
        QString path;
        QString folder;
        bool writable = true;
        QString readme;
        QString changes;
        QString authors;
    };

    struct RepoSpecs {
        double version = 1.0;
        QString label;
        QString id;
        QUrl url;
        QUrl manifest;
        QUrl logo;
        QUrl zip;
        QString checksum;
        QDateTime modified;
        bool enabled = false;
    };

    struct PluginStatus {
        bool success = false;
        QString message;
    };

    enum PluginType {
        NATRON_PLUGIN_TYPE_NONE,
        NATRON_PLUGIN_TYPE_AVAILABLE,
        NATRON_PLUGIN_TYPE_INSTALLED,
        NATRON_PLUGIN_TYPE_UPDATE
    };

    explicit Plugins(QObject *parent = nullptr);
    ~Plugins();

    void scanForAvailablePlugins(const QString &path,
                                 bool append = false,
                                 bool emitChanges = true,
                                 bool emitCache = false);
    void scanForInstalledPlugins(const QStringList &paths);
    void scanForInstalledPlugins(const QString &path,
                                 bool append = false);

    bool hasPlugin(const QString &id);
    bool hasAvailablePlugin(const QString &id);
    bool hasInstalledPlugin(const QString &id);
    bool hasUpdatedPlugin(const QString &id);

    Plugins::PluginSpecs getPlugin(const QString &id);
    Plugins::PluginSpecs getAvailablePlugin(const QString &id);
    Plugins::PluginSpecs getInstalledPlugin(const QString &id);
    Plugins::PluginSpecs getUpdatedPlugin(const QString &id);

    std::vector<Plugins::PluginSpecs> getPlugins();
    std::vector<Plugins::PluginSpecs> getAvailablePlugins();
    std::vector<Plugins::PluginSpecs> getInstalledPlugins();
    std::vector<Plugins::PluginSpecs> getUpdatedPlugins();

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

    const QString getUserNatronPath();

    const QString getUserPluginPath();
    void setUserPluginPath(const QString &path);

    const QString getUserAddonPath();
    void setUserAddonPath(const QString &path);

    const QStringList getSystemPluginPaths();
    const QStringList getNatronCustomPaths();
    const QString getCachePath();
    qint64 getCacheSize();
    const QString getRepoPath();
    const QString getRepoPath(const QString &uid);
    const QString getRandom(const QString &path = QString(),
                            const QString &suffix = QString());
    const QString getTempPath();

    const QString genNewRepoID();

    Plugins::PluginStatus installPlugin(const QString &id,
                                        bool update = false);
    Plugins::PluginStatus removePlugin(const QString &id);
    Plugins::PluginStatus updatePlugin(const QString &id);

    Plugins::PluginStatus extractPluginArchive(const QString &filename,
                                               const QString &folder,
                                               const QString &checksum = QString());

    bool isValidRepository(const RepoSpecs &repo);
    bool addRepository(const QString &manifest);
    void loadRepositories();
    void saveRepositories(const std::vector<RepoSpecs> &repos);
    void checkRepositories(bool emitChanges = true,
                           bool emitCache = false);
    std::vector<Plugins::RepoSpecs> getAvailableRepositories();
    Plugins::RepoSpecs getRepoFromUrl(const QUrl &url);
    bool isRepoManifest(const Plugins::RepoSpecs &repo,
                        const QUrl &url);
    bool isRepoZip(const Plugins::RepoSpecs &repo,
                   const QUrl &url);
    bool isRepoLogo(const Plugins::RepoSpecs &repo,
                    const QUrl &url);

    bool isBusy();

    void removeFromDownloadQueue(const QUrl &url);

    bool isValidManifest(const QString &manifest);
    Plugins::RepoSpecs readManifest(const QString &manifest);
    Plugins::RepoSpecs openManifest(const QString &filename);

    double getManifestVersion(const QString &manifest);

    Plugins::RepoSpecs parseManifestV1(const QString &manifest);

    void addDownloadUrl(const QUrl &url);

signals:

    void updatedPlugins();
    void updatedCache();
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
    std::vector<Plugins::PluginSpecs> _availablePluginUpdates;
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
