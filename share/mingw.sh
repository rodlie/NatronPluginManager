#!/bin/bash
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

MXE=/opt/mxe-NatronPluginManager
TCHAIN=x86_64-w64-mingw32.static
PATH=$MXE/usr/bin:$PATH
PKG_CONFIG_PATH=$MXE/usr/$TCHAIN/lib/pkgconfig
QTLIBS="Qt5Concurrent Qt5Core Qt5Gui Qt5Network Qt5Widgets"

rm -r build || true

mkdir build && cd build
$TCHAIN-cmake -DCMAKE_BUILD_TYPE=Release .. && make
mkdir NatronPluginManager && cd NatronPluginManager
mv ../*.exe .

for i in $QTLIBS; do
    cp $MXE/usr/$TCHAIN/qt5/bin/${i}.dll . || exit 1
done

mkdir platforms imageformats
cp $MXE/usr/$TCHAIN/qt5/plugins/platforms/qwindows.dll platforms/ || exit 1
cp $MXE/usr/$TCHAIN/qt5/plugins/imageformats/qjpeg.dll imageformats/ || exit 1

$TCHAIN-strip -s *.exe *.dll */*.dll

cd .. || exit 1

zip -9 -r NatronPluginManager.zip NatronPluginManager || exit 1
