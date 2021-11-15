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

cmake_minimum_required(VERSION 3.5)

project(NatronPluginManager VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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
endif()

include_directories("src")
include_directories(${ZIP_INCLUDE_DIRS})

add_executable(${PROJECT_NAME}
    src/main.cpp
    src/plugins.cpp
    src/plugins.h
    src/app.cpp
    src/app.h
    docs/assets.qrc
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    Qt${QTV}::Concurrent
    Qt${QTV}::Network
    Qt${QTV}::Widgets
    ${ZIP_LDFLAGS}
)

if(UNIX AND NOT APPLE)
    include(GNUInstallDirs)
    install(TARGETS
        ${PROJECT_NAME}
        DESTINATION
        ${CMAKE_INSTALL_BINDIR}
    )
    install(FILES
        docs/${PROJECT_NAME}.desktop
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
        docs/hicolor
        DESTINATION
        ${CMAKE_INSTALL_DATAROOTDIR}/icons/
    )
endif()

