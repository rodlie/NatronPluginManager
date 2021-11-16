#!/bin/bash

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

mkdir platforms
cp $MXE/usr/$TCHAIN/qt5/plugins/platforms/qwindows.dll platforms/ || exit 1

$TCHAIN-strip -s *.exe *.dll */*.dll

cd .. || exit 1

zip -9 -r NatronPluginManager.zip NatronPluginManager || exit 1
