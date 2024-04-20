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

#include "app.h"

#include <QApplication>
#ifdef Q_OS_WIN
#include <QSettings>
#endif

int main(int argc, char *argv[])
{
#ifdef Q_OS_WIN
#if QT_VERSION < QT_VERSION_CHECK(6, 5, 0)
    // Set window title bar color based on dark/light theme
    // https://www.qt.io/blog/dark-mode-on-windows-11-with-qt-6.5
    // https://learn.microsoft.com/en-us/answers/questions/1161597/how-to-detect-windows-application-dark-mode
    QSettings registry(QString::fromUtf8("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize"),
                       QSettings::NativeFormat);
    if (registry.value(QString::fromUtf8("AppsUseLightTheme"), 0).toInt() == 0) {
        qputenv("QT_QPA_PLATFORM", "windows:darkmode=1");
    }
#endif
#endif

    QApplication::setApplicationName(APP_NAME);
    QApplication::setApplicationVersion(APP_VERSION);
    QApplication::setApplicationDisplayName(QObject::tr("Natron Plug-in Manager"));
    QApplication::setOrganizationName(APP_ORG);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication a(argc, argv);
    NatronPluginManager app;
    app.show();

    return a.exec();
}
