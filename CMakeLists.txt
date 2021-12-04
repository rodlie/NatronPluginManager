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

cmake_minimum_required(VERSION 3.5)

project(NatronPluginManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
#set(CPACK_PACKAGE_DESCRIPTION "Natron Plug-in Manager")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "NatronPluginManager")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/rodlie/NatronPluginManager")
#set(CPACK_DEBIAN_FILE_NAME "DEB-DEFAULT")
set(CPACK_DEBIAN_FILE_NAME "${PROJECT_NAME}.deb")

set(COMPANY "Natron")
set(COPYRIGHT "Copyright (c) Ole-Andre Rodlie")
set(IDENTIFIER "org.natronvfx.plugins")

set(QTV "5" CACHE STRING "Qt major version")
option(USE_PKGCONF "Use pkg-config" ON)

add_definitions(-DAPP_NAME="${PROJECT_NAME}")
add_definitions(-DAPP_VERSION="${PROJECT_VERSION}")
add_definitions(-DAPP_ORG="Natron")
add_definitions(-DAPP_DOMAIN="natronvfx.org")
add_definitions(-DAPP_URL="https://plugins.natronvfx.org")

add_compile_options(-Wall -Wextra)
if(CMAKE_BUILD_TYPE MATCHES "^(release|Release|RELEASE)$")
    add_definitions(-DQT_NO_DEBUG_OUTPUT)
else()
    add_definitions(-DQT_MESSAGELOGCONTEXT)
endif()

find_package(Qt${QTV} 5.10 COMPONENTS Concurrent Network Widgets REQUIRED)

if(${USE_PKGCONF})
    find_package(PkgConfig REQUIRED)
    pkg_search_module(ZIP REQUIRED libzip)
else()
    set(ZIP_INCLUDE_DIRS $ENV{ZIP_INCLUDE_DIRS})
    set(ZIP_LDFLAGS $ENV{ZIP_LDFLAGS})
endif()

include_directories("src")
include_directories(${ZIP_INCLUDE_DIRS})

if(WIN32)
    set(ICON_FILE "${PROJECT_NAME}.ico")
    set(WINDOWS_ICON_FILE ${CMAKE_CURRENT_BINARY_DIR}/${ICON_FILE})
    set(GUI_TYPE WIN32)
    set(PRODUCT_COMMENTS ${PROJECT_NAME})
    set(PRODUCT_COMPANY_NAME ${COMPANY})
    set(PRODUCT_FILE_DESCRIPTION "Natron Plug-in Manager")
    set(PRODUCT_VERSION "${PROJECT_VERSION}")
    set(PRODUCT_INTERNAL_NAME ${PROJECT_NAME})
    set(PRODUCT_COMPANY_COPYRIGHT ${COPYRIGHT})
    set(PRODUCT_ORIGINAL_FILENAME "${PROJECT_NAME}.exe")
    set(PRODUCT_BUNDLE ${PROJECT_NAME})
    set(WINDOWS_RESOURCE_FILE ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc)
    configure_file(share/${PROJECT_NAME}.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.rc
        @ONLY
    )
    configure_file(share/${PROJECT_NAME}.ico
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.ico
        COPYONLY
    )
endif()

if(APPLE)
    set(ICON_FILE ${CMAKE_CURRENT_SOURCE_DIR}/share/${PROJECT_NAME}.icns)
    set(MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME})
    set(MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_LONG_VERSION_STRING ${PROJECT_VERSION})
    set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}")
    set(MACOSX_BUNDLE_COPYRIGHT ${COPYRIGHT})
    set(MACOSX_BUNDLE_GUI_IDENTIFIER ${IDENTIFIER})
    set(MACOSX_BUNDLE_ICON_FILE ${PROJECT_NAME})
    set_source_files_properties(${ICON_FILE} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    set(CMAKE_INSTALL_RPATH "@executable_path/../Frameworks")
endif()

set(SRC
    src/main.cpp
    src/plugins.cpp
    src/plugins.h
    src/addrepodialog.cpp
    src/addrepodialog.h
    src/pluginlistwidget.cpp
    src/pluginlistwidget.h
    src/pluginviewwidget.cpp
    src/pluginviewwidget.h
    src/app.cpp
    src/app.h
    share/assets.qrc
)

if(WIN32)
    add_executable(${PROJECT_NAME}
        WIN32
        ${WINDOWS_ICON_FILE}
        ${WINDOWS_RESOURCE_FILE}
        ${SRC}
    )
elseif(APPLE)
    add_executable(${PROJECT_NAME}
        MACOSX_BUNDLE
        ${ICON_FILE}
        ${SRC})
else()
    add_executable(${PROJECT_NAME} ${SRC})
endif()

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt${QTV}::Concurrent
    Qt${QTV}::Network
    Qt${QTV}::Widgets
    ${ZIP_LDFLAGS}
)

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
    include(CPack)
    install(TARGETS
        ${PROJECT_NAME}
        DESTINATION
        ${CMAKE_INSTALL_BINDIR}
    )
    install(FILES
        share/${PROJECT_NAME}.desktop
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/applications
    )
    install(FILES
        LICENSE
        README.md
        DESTINATION
        ${CMAKE_INSTALL_DOCDIR}-${PROJECT_VERSION}
    )
    install(
        DIRECTORY
        share/hicolor
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/icons/
    )
endif()
