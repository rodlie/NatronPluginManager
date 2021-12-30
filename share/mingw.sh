#!/bin/bash
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

set -e -x

CWD=`pwd`
MXE=/opt/mxe-NatronPluginManager
TOOL=x86_64-w64-mingw32.static
PATH=$MXE/usr/bin:$PATH
PKG_CONFIG_PATH=$MXE/usr/$TOOL/lib/pkgconfig

rm -r build || true
mkdir build && cd build

$TOOL-cmake -DCMAKE_BUILD_TYPE=Release ..
make

$TOOL-strip -s NatronPluginManager.exe

mkdir -p NatronPluginManager/third-party
cp $CWD/LICENSE NatronPluginManager/
cp $CWD/README.md NatronPluginManager/
cp -a $MXE/licenses/* NatronPluginManager/third-party/
mv NatronPluginManager.exe NatronPluginManager/
zip -9 -r NatronPluginManager.zip NatronPluginManager
