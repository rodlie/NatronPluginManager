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

#ifndef STATUSWIDGET_H
#define STATUSWIDGET_H

#include <QWidget>
#include <QProgressBar>
#include <QLabel>
#include <QIcon>
#include <QPixmap>
#include <QTimer>

class StatusWidget : public QWidget
{
    Q_OBJECT

public:

    explicit StatusWidget(QWidget *parent = nullptr);

public slots:

    void showMessage(const QString &message, int timeout = 1000);
    void showProgress(const QString &message,
                      qint64 value,
                      qint64 total);
    void clear();

private:

    QTimer *_timer;
    QLabel *_label;
    QProgressBar *_prog;

private slots:

    void handleLabelTimeout();
};

#endif // STATUSWIDGET_H
