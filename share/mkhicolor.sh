#!/bin/sh

set -e -x

CWD=`pwd`
SIZES="128x128 160x160 16x16 192x192 20x20 22x22 24x24 256x256 32x32 36x36 48x48 64x64 72x72 96x96"

for size in $SIZES; do
    if [ ! -d "$CWD/hicolor/$size/apps" ]; then
        mkdir -p $CWD/hicolor/$size/apps
    fi
    convert $CWD/logo.png -scale $size $CWD/hicolor/$size/apps/NatronPluginManager.png
    identify $CWD/hicolor/$size/apps/NatronPluginManager.png
done
