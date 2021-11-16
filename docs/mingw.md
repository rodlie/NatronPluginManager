# Setup Windows SDK on Linux/macOS

```
git clone https://github.com/mxe/mxe
cd mxe
git checkout 02852a7b690aa411ce2a2089deea25a7292a33d6
```

```
cat << 'EOF' > settings.mk
JOBS := 8
MXE_USE_CCACHE=
MXE_TARGETS := x86_64-w64-mingw32.static
MXE_PLUGIN_DIRS := plugins/gcc7
LOCAL_PKG_LIST := cc cmake qtbase libzip
.DEFAULT_GOAL := local-pkg-list
local-pkg-list: $(LOCAL_PKG_LIST)
EOF
```

```
cat << 'EOF' > qtbase.diff
diff --git a/src/qtbase.mk b/src/qtbase.mk
index 99aaafc1..387c033d 100644
--- a/src/qtbase.mk
+++ b/src/qtbase.mk
@@ -9,7 +9,7 @@ $(PKG)_CHECKSUM := 909fad2591ee367993a75d7e2ea50ad4db332f05e1c38dd7a5a274e156a4e
 $(PKG)_SUBDIR   := $(PKG)-everywhere-src-$($(PKG)_VERSION)
 $(PKG)_FILE     := $(PKG)-everywhere-src-$($(PKG)_VERSION).tar.xz
 $(PKG)_URL      := https://download.qt.io/official_releases/qt/5.15/$($(PKG)_VERSION)/submodules/$($(PKG)_FILE)
-$(PKG)_DEPS     := cc dbus fontconfig freetds freetype harfbuzz jpeg libmysqlclient libpng mesa openssl pcre2 postgresql sqlite zlib zstd $(BUILD)~zstd
+$(PKG)_DEPS     := cc openssl
 $(PKG)_DEPS_$(BUILD) :=
 $(PKG)_TARGETS  := $(BUILD) $(MXE_TARGETS)
 
@@ -41,30 +41,25 @@ define $(PKG)_BUILD
             -force-pkg-config \
             -no-use-gold-linker \
             -release \
-            $(if $(BUILD_STATIC), -static,)$(if $(BUILD_SHARED), -shared,) \
+            -shared \
             -prefix '$(PREFIX)/$(TARGET)/qt5' \
             -no-icu \
-            -opengl dynamic \
+            -opengl desktop \
             -no-glib \
             -accessibility \
             -nomake examples \
             -nomake tests \
-            -plugin-sql-mysql \
-            -mysql_config $(PREFIX)/$(TARGET)/bin/mysql_config \
-            -plugin-sql-sqlite \
-            -plugin-sql-odbc \
-            -plugin-sql-psql \
-            -plugin-sql-tds -D Q_USE_SYBASE \
-            -system-zlib \
-            -system-libpng \
-            -system-libjpeg \
-            -system-sqlite \
-            -fontconfig \
-            -system-freetype \
-            -system-harfbuzz \
-            -system-pcre \
+            -no-sql-{db2,ibase,mysql,oci,odbc,psql,sqlite2,tds} \
+            -qt-zlib \
+            -qt-libpng \
+            -qt-libjpeg \
+            -qt-sqlite \
+            -no-fontconfig \
+            -qt-freetype \
+            -no-harfbuzz \
+            -qt-pcre \
             -openssl-linked \
-            -dbus-linked \
+            -no-dbus \
             -no-pch \
             -v \
             $($(PKG)_CONFIGURE_OPTS)
@@ -74,58 +69,58 @@ define $(PKG)_BUILD
     $(MAKE) -C '$(1)' -j 1 install
     ln -sf '$(PREFIX)/$(TARGET)/qt5/bin/qmake' '$(PREFIX)/bin/$(TARGET)'-qmake-qt5
 
-    mkdir            '$(1)/test-qt'
-    cd               '$(1)/test-qt' && '$(PREFIX)/$(TARGET)/qt5/bin/qmake' '$(PWD)/src/qt-test.pro'
-    $(MAKE)       -C '$(1)/test-qt' '$(BUILD_TYPE)' -j '$(JOBS)'
-    $(INSTALL) -m755 '$(1)/test-qt/$(BUILD_TYPE)/test-qt5.exe' '$(PREFIX)/$(TARGET)/bin/'
+#    mkdir            '$(1)/test-qt'
+#    cd               '$(1)/test-qt' && '$(PREFIX)/$(TARGET)/qt5/bin/qmake' '$(PWD)/src/qt-test.pro'
+#    $(MAKE)       -C '$(1)/test-qt' '$(BUILD_TYPE)' -j '$(JOBS)'
+#    $(INSTALL) -m755 '$(1)/test-qt/$(BUILD_TYPE)/test-qt5.exe' '$(PREFIX)/$(TARGET)/bin/'
 
-    # build test the manual way
-    mkdir '$(1)/test-$(PKG)-pkgconfig'
-    '$(PREFIX)/$(TARGET)/qt5/bin/uic' -o '$(1)/test-$(PKG)-pkgconfig/ui_qt-test.h' '$(TOP_DIR)/src/qt-test.ui'
-    '$(PREFIX)/$(TARGET)/qt5/bin/moc' \
-        -o '$(1)/test-$(PKG)-pkgconfig/moc_qt-test.cpp' \
-        -I'$(1)/test-$(PKG)-pkgconfig' \
-        '$(TOP_DIR)/src/qt-test.hpp'
-    '$(PREFIX)/$(TARGET)/qt5/bin/rcc' -name qt-test -o '$(1)/test-$(PKG)-pkgconfig/qrc_qt-test.cpp' '$(TOP_DIR)/src/qt-test.qrc'
-    '$(TARGET)-g++' \
-        -W -Wall -std=c++0x -pedantic \
-        '$(TOP_DIR)/src/qt-test.cpp' \
-        '$(1)/test-$(PKG)-pkgconfig/moc_qt-test.cpp' \
-        '$(1)/test-$(PKG)-pkgconfig/qrc_qt-test.cpp' \
-        -o '$(PREFIX)/$(TARGET)/bin/test-$(PKG)-pkgconfig.exe' \
-        -I'$(1)/test-$(PKG)-pkgconfig' \
-        `'$(TARGET)-pkg-config' Qt5Widgets$(BUILD_TYPE_SUFFIX) --cflags --libs`
+#    # build test the manual way
+#    mkdir '$(1)/test-$(PKG)-pkgconfig'
+#    '$(PREFIX)/$(TARGET)/qt5/bin/uic' -o '$(1)/test-$(PKG)-pkgconfig/ui_qt-test.h' '$(TOP_DIR)/src/qt-test.ui'
+#    '$(PREFIX)/$(TARGET)/qt5/bin/moc' \
+#        -o '$(1)/test-$(PKG)-pkgconfig/moc_qt-test.cpp' \
+#        -I'$(1)/test-$(PKG)-pkgconfig' \
+#        '$(TOP_DIR)/src/qt-test.hpp'
+#    '$(PREFIX)/$(TARGET)/qt5/bin/rcc' -name qt-test -o '$(1)/test-$(PKG)-pkgconfig/qrc_qt-test.cpp' '$(TOP_DIR)/src/qt-test.qrc'
+#    '$(TARGET)-g++' \
+#        -W -Wall -std=c++0x -pedantic \
+#        '$(TOP_DIR)/src/qt-test.cpp' \
+#        '$(1)/test-$(PKG)-pkgconfig/moc_qt-test.cpp' \
+#        '$(1)/test-$(PKG)-pkgconfig/qrc_qt-test.cpp' \
+#        -o '$(PREFIX)/$(TARGET)/bin/test-$(PKG)-pkgconfig.exe' \
+#        -I'$(1)/test-$(PKG)-pkgconfig' \
+#        `'$(TARGET)-pkg-config' Qt5Widgets$(BUILD_TYPE_SUFFIX) --cflags --libs`
 
-    # setup cmake toolchain and test
-    echo 'set(CMAKE_SYSTEM_PREFIX_PATH "$(PREFIX)/$(TARGET)/qt5" ${CMAKE_SYSTEM_PREFIX_PATH})' > '$(CMAKE_TOOLCHAIN_DIR)/$(PKG).cmake'
-    $(CMAKE_TEST)
+#    # setup cmake toolchain and test
+#    echo 'set(CMAKE_SYSTEM_PREFIX_PATH "$(PREFIX)/$(TARGET)/qt5" ${CMAKE_SYSTEM_PREFIX_PATH})' > '$(CMAKE_TOOLCHAIN_DIR)/$(PKG).cmake'
+#    $(CMAKE_TEST)
 
-    # batch file to run test programs
-    (printf 'set PATH=..\\lib;..\\qt5\\bin;..\\qt5\\lib;%%PATH%%\r\n'; \
-     printf 'set QT_QPA_PLATFORM_PLUGIN_PATH=..\\qt5\\plugins\r\n'; \
-     printf 'test-qt5.exe\r\n'; \
-     printf 'test-qtbase-pkgconfig.exe\r\n';) \
-     > '$(PREFIX)/$(TARGET)/bin/test-qt5.bat'
+#    # batch file to run test programs
+#    (printf 'set PATH=..\\lib;..\\qt5\\bin;..\\qt5\\lib;%%PATH%%\r\n'; \
+#     printf 'set QT_QPA_PLATFORM_PLUGIN_PATH=..\\qt5\\plugins\r\n'; \
+#     printf 'test-qt5.exe\r\n'; \
+#     printf 'test-qtbase-pkgconfig.exe\r\n';) \
+#     > '$(PREFIX)/$(TARGET)/bin/test-qt5.bat'
 endef
 
-define $(PKG)_BUILD_$(BUILD)
-    cd '$(BUILD_DIR)' && '$(SOURCE_DIR)/configure' \
-        -prefix '$(PREFIX)/$(TARGET)/qt5' \
-        -static \
-        -release \
-        -opensource \
-        -confirm-license \
-        -no-dbus \
-        -no-{eventfd,glib,icu,openssl} \
-        -no-sql-{db2,ibase,mysql,oci,odbc,psql,sqlite2,tds} \
-        -no-use-gold-linker \
-        -nomake examples \
-        -nomake tests \
-        -make tools \
-        -continue \
-        -verbose
-    $(MAKE) -C '$(BUILD_DIR)' -j '$(JOBS)'
-    rm -rf '$(PREFIX)/$(TARGET)/qt5'
-    $(MAKE) -C '$(BUILD_DIR)' -j 1 install
-    ln -sf '$(PREFIX)/$(TARGET)/qt5/bin/qmake' '$(PREFIX)/bin/$(TARGET)'-qmake-qt5
-endef
+#define $(PKG)_BUILD_$(BUILD)
+#    cd '$(BUILD_DIR)' && '$(SOURCE_DIR)/configure' \
+#        -prefix '$(PREFIX)/$(TARGET)/qt5' \
+#        -static \
+#        -release \
+#        -opensource \
+#        -confirm-license \
+#        -no-dbus \
+#        -no-{eventfd,glib,icu,openssl} \
+#        -no-sql-{db2,ibase,mysql,oci,odbc,psql,sqlite2,tds} \
+#        -no-use-gold-linker \
+#        -nomake examples \
+#        -nomake tests \
+#        -make tools \
+#        -continue \
+#        -verbose
+#    $(MAKE) -C '$(BUILD_DIR)' -j '$(JOBS)'
+#    rm -rf '$(PREFIX)/$(TARGET)/qt5'
+#    $(MAKE) -C '$(BUILD_DIR)' -j 1 install
+#    ln -sf '$(PREFIX)/$(TARGET)/qt5/bin/qmake' '$(PREFIX)/bin/$(TARGET)'-qmake-qt5
+#endef

EOF
```
```
patch -p1 < qtbase.diff
```
```
make
```