# SARibbon Build Instructions

This document explains in detail how to build SARibbon dynamically.  
If you are not familiar with building, we recommend the static approach: simply add `SARibbon.h` and `SARibbon.cpp` to your project and you are ready to go.

SARibbon uses [QWindowkit](https://github.com/stdware/qwindowkit) as its borderless-window solution, but also supports a minimal built-in borderless mode.  
If you need native OS window features—such as Windows 7+ snap-to-edge or Windows 11 Snap Layouts—it is strongly advised to enable the [QWindowkit](https://github.com/stdware/qwindowkit) library, which also fixes multi-monitor relocation issues.

With QWindowkit enabled you will get effects like:

![set-qwindowkit-on-snap](../../assets/pic/set-qwindowkit-on-snap.gif)

To enable [QWindowkit](https://github.com/stdware/qwindowkit) you must first build that library.

!!! warning "Note"
    QWindowkit is a submodule of SARibbon.  
    If you cloned without `--recursive`, run:
    ```shell
    git submodule update --init --recursive
    ```

## Installation Location

After CMake finishes building, the `install` target will deploy all dependencies.  
Consumers can simply call `find_package` to pull in libraries, dependencies and predefined macros—this is the recommended workflow.

During development, however, you often need to switch between compilers (MSVC, MinGW, …) and Qt versions.  
Using the default global location (e.g. `C:\Program Files` on Windows) allows only one variant to be installed at a time.

To keep builds isolated, SARibbon defaults to **local installation**.  
A folder is created whose name encodes the Qt version and compiler, e.g.  
`bin_qt{version}_[MSVC|GNU]_x[64|86]`.

You can control this behaviour with the CMake option  
`SARIBBON_INSTALL_IN_CURRENT_DIR` (default `ON`).  
When `ON`, the above local folder is used; when `OFF`, the standard system location is used.