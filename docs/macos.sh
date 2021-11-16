#!/bin/sh
set -e -x

CWD=`pwd`
SDK="/opt/Qt5.12.11/5.12.11/clang_64"
MKJOBS=${MKJOBS:-1}
CMAKE=${CMAKE:-/opt/local/bin/cmake}
OSX_MIN=10.13

rm -rf build || true
mkdir build && cd build

#export PATH="/usr/bin:/bin:/usr/sbin:/sbin"
export CC="/usr/bin/clang -stdlib=libc++ -mmacosx-version-min=$OSX_MIN"
export CXX="/usr/bin/clang++ -stdlib=libc++ -mmacosx-version-min=$OSX_MIN"

ZIP_INCLUDE_DIRS="/opt/libzip/include" \
ZIP_LDFLAGS="-L/opt/libzip/lib -lbz2 -llzma -lz -lzip" \
$CMAKE \
-DCMAKE_OSX_DEPLOYMENT_TARGET=$OSX_MIN \
-DUSE_PKGCONF=OFF \
-DCMAKE_BUILD_TYPE=Release \
-DCMAKE_INSTALL_PREFIX=/ \
-DCMAKE_PREFIX_PATH=${SDK} ..
make -j${MKJOBS}

$SDK/bin/macdeployqt NatronPluginManager.app

#rm -rf NatronPluginManager.app/Contents/Frameworks/QtDBus.framework
rm -rf NatronPluginManager.app/Contents/Frameworks/QtSvg.framework

rm -rf NatronPluginManager.app/Contents/PlugIns/bearer
rm -rf NatronPluginManager.app/Contents/PlugIns/iconengines
rm -rf NatronPluginManager.app/Contents/PlugIns/imageformats

strip -S -x NatronPluginManager.app/Contents/MacOS/NatronPluginManager

mkdir NatronPluginManager
mv NatronPluginManager.app NatronPluginManager
hdiutil create -volname "Natron Plug-in Manager" -srcfolder NatronPluginManager -ov -format UDBZ NatronPluginManager.dmg
