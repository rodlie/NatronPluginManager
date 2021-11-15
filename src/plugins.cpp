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
#include <QRandomGenerator>
#include <QHash>
#include <QHashIterator>
#include <QNetworkRequest>

#include <zip.h>

#define ZIP_BUF_SIZE 2048
#define NATRON_COMMUNITY_PLUGINS_URL "https://github.com/NatronGitHub/natron-plugins/archive/master.zip"

Plugins::Plugins(QObject *parent)
    : QObject(parent)
    , _isWorking(false)
    , _isDownloading(false)
    , _nam(nullptr)
{
    _nam = new QNetworkAccessManager(this);
    connect(_nam,
            SIGNAL(finished(QNetworkReply*)),
            this,
            SLOT(handleFileDownloaded(QNetworkReply*)));
    connect(this,
            SIGNAL(downloadRequired()),
            this,
            SLOT(startDownloads()));
}

Plugins::~Plugins()
{
    saveRepositories(_availableRepositories);
}

void Plugins::scanForAvailablePlugins(const QString &path,
                                      bool append)
{
    if (!QFile::exists(path)) { return; }
    if (!append) { _availablePlugins.clear(); }
    QDirIterator it(path,
                    QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString item = it.next();
        if (!folderHasPlugin(item)) { continue; }
        PluginSpecs plugin = getPluginSpecs(item);
        if (!hasAvailablePlugin(plugin.id) &&
            !hasInstalledPlugin(plugin.id))
        {
            emit statusMessage(tr("Found available plug-in %1").arg(plugin.label));
            _availablePlugins.push_back(plugin);
        }
    }
    emit statusMessage(tr("Done"));
    if (_availablePlugins.size() > 0) { emit updatedPlugins(); }
}

void Plugins::scanForInstalledPlugins(const QString &path,
                                      bool append)
{
    if (!QFile::exists(path)) { return; }
    if (!append) { _installedPlugins.clear(); }
    QDirIterator it(path,
                    QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString item = it.next();
        if (!folderHasPlugin(item)) { continue; }
        PluginSpecs plugin = getPluginSpecs(item);
        if (!hasInstalledPlugin(plugin.id)) {
            emit statusMessage(tr("Found installed plug-in %1").arg(plugin.label));
            _installedPlugins.push_back(plugin);
        }
    }
    emit statusMessage(tr("Done"));
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

Plugins::PluginSpecs Plugins::getPlugin(const QString &id)
{
    PluginSpecs plugin = getAvailablePlugin(id);
    if (!isValidPlugin(plugin)) { plugin = getInstalledPlugin(id); }
    return plugin;
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

int Plugins::folderHasPlugins(const QString &path)
{
    int plugins = 0;
    if (!QFile::exists(path)) { return plugins; }
    QDirIterator it(path,
                    QDir::AllEntries | QDir::NoDotAndDotDot | QDir::NoSymLinks,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        QString item = it.next();
        if (!folderHasPlugin(item)) { continue; }
        PluginSpecs plugin = getPluginSpecs(item);
        if (isValidPlugin(plugin)) { plugins++; }
    }
    return plugins;
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
    QString folder = QStandardPaths::writableLocation(QStandardPaths::CacheLocation);
    if (!QFile::exists(folder) && !folder.isEmpty()) {
        QDir dir;
        dir.mkpath(folder);
    }
    return folder;
}

const QString Plugins::getRepoPath()
{
    QString cache = getCachePath();
    if (cache.isEmpty()) { return cache; }
    cache.append("/Repositories");
    if (!QFile::exists(cache)) {
        QDir dir;
        dir.mkpath(cache);
    }
    return cache;
}

const QString Plugins::getRepoPath(const QString &uid)
{
    QString cache = getRepoPath();
    if (cache.isEmpty() || uid.isEmpty()) { return QString(); }
    cache.append(QString("/%1").arg(uid));
    /*if (!QFile::exists(cache)) {
        QDir dir;
        dir.mkpath(cache);
    }*/
    return cache;
}

const QString Plugins::getRandom()
{
    return QString::number(QRandomGenerator::global()->generate64());
}

const QString Plugins::getTempPath()
{
    QString cache = getCachePath();
    if (cache.isEmpty()) { return cache; }
    cache.append("/Temp");
    if (!QFile::exists(cache)) {
        QDir dir;
        dir.mkpath(cache);
    }
    return cache;
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

Plugins::PluginStatus Plugins::removePlugin(const QString &id)
{
    PluginStatus status;
    if (!hasInstalledPlugin(id)) {
        status.message = tr("Plug-in is not installed");
        status.success = false;
        return status;
    }

    PluginSpecs plugin = getInstalledPlugin(id);
    if (!isValidPlugin(plugin)) {
        status.message = tr("Plug-in not found or is invalid");
        status.success = false;
        return  status;
    }

    QString destPath = QString("%1/%2").arg(getUserPluginPath(), plugin.folder);
    if (QFile::exists(destPath)) {
        QDir dir(destPath);
        if (!dir.removeRecursively()) {
            status.message = tr("Unable to remove directory %1").arg(destPath);
            status.success = false;
        } else { status.success = true; }
    } else {
        status.message = tr("Unable to find directory %1").arg(destPath);
        status.success = false;
    }
    return  status;
}

Plugins::PluginStatus Plugins::extractPluginArchive(const QString &filename,
                                                    const QString &folder)
{
    PluginStatus status;
    status.success = true;

    if (!QFile::exists(filename)) {
        status.message = tr("File %1 does not exists").arg(filename);
        status.success = false;
        return status;
    }

    QFileInfo dir(folder);
    if (!dir.isWritable()) {
        status.message = tr("Directory %1 is not writable").arg(folder);
        status.success = false;
        return status;
    }

    struct zip* p_zip = NULL;
    zip_int64_t n_entries;
    struct zip_file* p_file = NULL;
    int bytes_read;
    char buffer[ZIP_BUF_SIZE];

    int error;
    p_zip = zip_open(filename.toStdString().c_str(), 0, &error);
    if (p_zip == NULL) {
      status.message = tr("Failed to open %1").arg(filename);
      status.success = false;
      return status;
    }

    n_entries = zip_get_num_entries(p_zip, 0);
    for (zip_int64_t entry_idx=0; entry_idx < n_entries; entry_idx++) {
        struct zip_stat file_stat;
        if (zip_stat_index(p_zip, entry_idx, 0, &file_stat)) {
            status.message = tr("Failed to read file from %1").arg(filename);
            status.success = false;
            break;
        }
        if (!(file_stat.valid & ZIP_STAT_NAME)) { continue; }
        QString filePath = QString("%1/%2").arg(folder, file_stat.name);

        //qDebug() << "EXTRACT" << filePath;

        if ((file_stat.name[0] != '\0') && (file_stat.name[strlen(file_stat.name)-1] == '/')) {
            QDir newDir;
            if (!newDir.mkpath(filePath)) {
                status.message = tr("Unable to create directory %1").arg(filePath);
                status.success = false;
                break;
            }
            continue;
        }

        if ((p_file = zip_fopen_index(p_zip, entry_idx, 0)) == NULL) {
            status.message = tr("Failed to extract file %1").arg(filePath);
            status.success = false;
            break;
        }

        QFile output(filePath);
        if (!output.open(QIODevice::WriteOnly)) {
            status.message = tr("Unable to write to file %1").arg(filePath);
            status.success = false;
            break;
        }

        do {
            if ((bytes_read = zip_fread(p_file, buffer, ZIP_BUF_SIZE)) == -1) {
                status.message = tr("Failed to extract file %1").arg(filePath);
                status.success = false;
                break;
            }
            if (bytes_read > 0) { output.write(buffer, bytes_read); }
        } while(bytes_read > 0);

        output.close();
        zip_fclose(p_file);
        p_file = NULL;
    }

    if (p_file) {
        zip_fclose(p_file);
        p_file = NULL;
    }
    if (p_zip) { zip_close(p_zip); }

    return status;
}

bool Plugins::isValidRepository(const Plugins::RepoSpecs &repo)
{
    if (repo.label.isEmpty() || repo.archive.isEmpty()) { return false; }
    return true;
}

void Plugins::loadRepositories()
{
    emit statusMessage(tr("Loading repositories"));
    _availableRepositories.clear();
    QSettings settings;
    if (settings.value("repos").isValid()) {
        QHashIterator<QString, QVariant> i(settings.value("repos").toHash());
        while (i.hasNext()) {
            i.next();
            qDebug() << "repo" << i.key() << i.value();
            QString label = i.key();
            QStringList options = i.value().toStringList();
            if (label.isEmpty() || options.size() < 1) { continue; }
            RepoSpecs repo;
            repo.label = label;
            repo.archive = QUrl::fromUserInput(options.at(0));
            if (options.size() > 1) { repo.folder = options.at(1); }
            if (isValidRepository(repo)) {
                emit statusMessage(tr("Found repository %1").arg(repo.label));
                _availableRepositories.push_back(repo);
            }
        }
    } else {
        RepoSpecs repo;
        repo.label = "Community";
        repo.archive = QUrl::fromUserInput(NATRON_COMMUNITY_PLUGINS_URL);
        _availableRepositories.push_back(repo);
    }

    checkRepositories();
}

void Plugins::saveRepositories(const std::vector<Plugins::RepoSpecs> &repos)
{
    emit statusMessage(tr("Saving repositories"));
    if (repos.size() < 1) { return; }
    QSettings settings;
    QHash<QString,QVariant> list;
    for (unsigned long i = 0; i < repos.size(); ++i) {
        if (list.contains(repos.at(i).label)) { continue; }
        list.insert(repos.at(i).label,
                    QStringList() << repos.at(i).archive.toString() << repos.at(i).folder);
    }
    settings.setValue("repos", list);
    settings.sync();
}

void Plugins::checkRepositories()
{
    emit statusMessage(tr("Checking repositories"));
    scanForInstalledPlugins(getUserPluginPath());
    _availablePlugins.clear();
    _downloadQueue.clear();
    for (unsigned long i = 0; i < _availableRepositories.size(); ++i) {
        RepoSpecs repo = _availableRepositories.at(i);
        if (!isValidRepository(repo)) { continue; }
        bool hasFolder = false;
        if (repo.folder.isEmpty()) {
            repo.folder = getRandom();
            _availableRepositories.at(i).folder = repo.folder;
        }
        QString repoPath = getRepoPath(repo.folder);
        if (folderHasPlugins(repoPath) > 0) { hasFolder = true; }
        if (!hasFolder) {
            emit statusMessage(tr("Need to download %1 repository").arg(repo.label));
            _downloadQueue.push_back(repo.archive);
        } else {
            scanForAvailablePlugins(repoPath, true);
        }
    }
    if (_downloadQueue.size() > 0) { emit downloadRequired(); }
}

std::vector<Plugins::RepoSpecs> Plugins::getAvailableRepositories()
{
    return _availableRepositories;
}

Plugins::RepoSpecs Plugins::getRepoFromUrl(const QUrl &url)
{
    for (unsigned long i = 0; i < _availableRepositories.size(); ++i) {
        QUrl repoUrl = _availableRepositories.at(i).archive;
        if (!url.isEmpty() && url == repoUrl) { return _availableRepositories.at(i); }
    }
    return RepoSpecs();
}

bool Plugins::isBusy()
{
    return _isWorking || _isDownloading;
}

void Plugins::startDownloads()
{
    if (_isDownloading || _downloadQueue.size() < 1) { return; }
    QUrl url = _downloadQueue.front();
    if (url.isEmpty()) { return; }
    qDebug() << "download" << url;
    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::FollowRedirectsAttribute, true);
    QNetworkReply *reply = _nam->get(request);
    reply->setObjectName(url.toString());
    connect(reply,
            SIGNAL(readyRead()),
            this,
            SLOT(handleDownloadReadyRead()));
    connect(reply,
            SIGNAL(error(QNetworkReply::NetworkError)),
            this,
            SLOT(handleDownloadError(QNetworkReply::NetworkError)));
    connect(reply,
            SIGNAL(downloadProgress(qint64, qint64)),
            this,
            SLOT(handleDownloadProgress(qint64, qint64)));
}

void Plugins::removeFromDownloadQueue(const QUrl &url)
{
    int pos = -1;
    for (unsigned long i = 0; i < _downloadQueue.size(); ++i) {
        if (_downloadQueue.at(i) == url) {
            pos = i;
            break;
        }
    }
    if (pos > -1) { _downloadQueue.erase(_downloadQueue.begin()+pos); }
}

void Plugins::handleFileDownloaded(QNetworkReply *reply)
{
    emit statusMessage(tr("Done"));
    if (!reply) { return; }
    QUrl url = reply->objectName().isEmpty() ? reply->url() : QUrl::fromUserInput(reply->objectName());
    QByteArray fileData = reply->readAll();
    reply->deleteLater();
    _isDownloading = false;
    removeFromDownloadQueue(url);

    RepoSpecs repo = getRepoFromUrl(url);
    if (fileData.size() > 0 && isValidRepository(repo)) {
        QFile tempFile(QString("%1/%2.zip").arg(getTempPath(), getRandom()));
        if (tempFile.open(QIODevice::WriteOnly)) {
            tempFile.write(fileData);
            tempFile.close();
        }
        QString destFolder = getRepoPath(repo.folder);
        if (!destFolder.isEmpty() && !QFile::exists(destFolder)) {
            QDir dir;
            dir.mkpath(destFolder);
        }
        if (tempFile.exists() && QFile::exists(destFolder)) {
            emit statusMessage(tr("Extracting repository %1 ...").arg(repo.label));
            PluginStatus res = extractPluginArchive(tempFile.fileName(), destFolder);
            if (res.success) {
                emit statusMessage(tr("Done"));
                scanForAvailablePlugins(destFolder, true);
            } else {
                emit statusError(res.message);
            }
            tempFile.remove();
        } else {
            emit statusError(tr("Failed to read/extract repository %1 archive").arg(repo.label));
        }
    }
    if (_downloadQueue.size() > 0) {
        qDebug() << "more to download";
        emit downloadRequired();
    }
}

void Plugins::handleDownloadError(QNetworkReply::NetworkError /*error*/)
{
    emit statusError(tr("Failed to download repository"));
    _isDownloading = false;
    QNetworkReply *reply = qobject_cast<QNetworkReply*>(QObject::sender());
    if (!reply) { return; }
    QUrl url = reply->objectName().isEmpty() ? reply->url() : QUrl::fromUserInput(reply->objectName());
    removeFromDownloadQueue(url);
    reply->deleteLater();
}

void Plugins::handleDownloadProgress(qint64 value, qint64 total)
{
    emit statusDownload(tr("Downloading repository, please wait ..."), value, total);
}

void Plugins::handleDownloadReadyRead()
{
    if (!_isDownloading) { _isDownloading = true; }
}