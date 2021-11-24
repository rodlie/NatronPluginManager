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

#include "statuswidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

StatusWidget::StatusWidget(QWidget *parent)
    : QWidget(parent)
    , _timer(nullptr)
    , _label(nullptr)
    , _prog(nullptr)
{
    setObjectName("StatusWidget");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    _label = new QLabel(this);
    _label->setAlignment(Qt::AlignCenter);
    _label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    _prog = new QProgressBar(this);
    _prog->setObjectName("ProgressBar");
    _prog->setRange(0, 1);
    _prog->setValue(1);
    _prog->setFormat("");
    _prog->hide();

    QLabel *logoLabel = new QLabel("<h1 style=\"text-align: center;\">"
                                   "<img src=\":/NatronPluginManager.png\">"
                                   "<br>Natron<br>"
                                   "<span style=\"font-weight:normal;\">Plug-in Manager</span>"
                                   "</h1>", this);
    logoLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    mainLayout->addStretch();
    mainLayout->addWidget(logoLabel, Qt::AlignCenter);
    mainLayout->addStretch();
    mainLayout->addWidget(_label, Qt::AlignCenter);
    mainLayout->addWidget(_prog, Qt::AlignCenter);

    _timer = new QTimer(this);
    connect(_timer,
            SIGNAL(timeout()),
            this,
            SLOT(handleLabelTimeout()));
}

void StatusWidget::showMessage(const QString &message,
                               int timeout)
{
    _label->setText(message);
    _timer->start(timeout);
}

void StatusWidget::showProgress(const QString &message,
                                qint64 value,
                                qint64 total)
{
    if (_prog->isHidden()) { _prog->show(); }
    showMessage(message);
    _prog->setRange(0, total);
    _prog->setValue(value);
    if (value == total && _prog->isVisible()) { _prog->hide(); }
}

void StatusWidget::clear()
{
    _prog->setRange(0, 1);
    _prog->setValue(1);
    _prog->setFormat("");
    _prog->hide();

    _label->clear();
}

void StatusWidget::handleLabelTimeout()
{
    if (_timer->isActive()) { return; }
    _label->clear();
}
