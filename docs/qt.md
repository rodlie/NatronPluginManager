# Build Qt 5.15(.2)

The recommended Qt version for Natron Plug-in Manager is 5.15.2.

## Patches from KDE

Qt 5.15 Open Source is maintained by KDE. They offer several fixes that should be added to 5.15.2. There is however no known issues with 5.15.2 and Natron Plug-in Manager, so this is optional.

```
git clone https://invent.kde.org/qt/qt/qtbase.git
git format-patch origin/5.15.2..origin/kde/5.15
```

# Ubuntu
```
./configure \
-prefix /opt/qtbase-5.15 \
-opensource \
-release \
-confirm-license \
-no-glib \
-no-dbus \
-static \
-nomake examples \
-nomake tests \
-no-icu \
-xcb \
-xkbcommon \
-opengl desktop
```

# Windows

Natron Plug-in Manager for Windows is built using MXE (MinGW) on Linux. See [mingw.md](mingw.md) for more information.

# macOS

```
./configure \
-prefix /opt/qtbase-5.15 \
-opensource \
-release \
-confirm-license \
-no-glib \
-no-dbus \
-static \
-nomake examples \
-nomake tests \
-no-icu \
-no-sql-{db2,ibase,mysql,oci,odbc,psql,sqlite2,sqlite,tds} \
-qt-zlib \
-qt-libpng \
-qt-libjpeg \
-no-fontconfig \
-qt-freetype \
-qt-harfbuzz \
-qt-pcre \
-no-cups \
-securetransport \
-no-avx \
-no-avx2 \
-no-avx512
```

