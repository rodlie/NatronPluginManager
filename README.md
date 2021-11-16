# Natron Plug-in Manager

![screenshot](docs/screenshot.png)

A plug-in ([PyPlug](https://github.com/NatronGitHub/natron-plugins)) manager for [Natron](https://github.com/NatronGitHub/Natron).

**WORK-IN-PROGRESS!**

## Build

This application requires CMake, Qt 5.10+ (Concurrent/Network/Widgets) and libzip to build.

```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr .. && make
make DESTDIR=<PACKAGE> install
```

## Downloads

### Artifacts

* [Windows 7/8/10/11 x64](https://github.com/rodlie/NatronPluginManager/actions/workflows/mingw.yml)
* [macOS High Sierra (10.13) or newer](https://github.com/rodlie/NatronPluginManager/actions/workflows/macos.yml)
