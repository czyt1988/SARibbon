# Importing the SARibbon Library

- ✅ **Static embedding**: Just copy `SARibbon.h` + `SARibbon.cpp` to integrate
- ✅ **CMake find_package**: Build & install, then `find_package(SARibbonBar)` to link dynamic library
- ✅ **qmake pri inclusion**: One-line `importSARibbonBarLib.pri` completes integration
- ✅ **Optional QWindowKit**: Enable borderless window & Windows 11 Snap Layout via preprocessor macros
- ✅ **Static lib resource init**: `Q_INIT_RESOURCE` auto-loads resources when built as static library

---

## Direct Inclusion

Direct inclusion is the simplest integration method. Just add `src/SARibbon.h` and `src/SARibbon.cpp` to your project. By default, QWindowKit (qwk) is not enabled. If you need to enable it, do the following:

1. Add preprocessor macros

- Set `SARIBBON_USE_3RDPARTY_FRAMELESSHELPER` to 1
- Set `SARIBBON_ENABLE_SNAP_LAYOUT` to 1

If using qmake, add the following to your `.pro` file:

```shell
# Define macros with values
DEFINES += SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1
DEFINES += SARIBBON_ENABLE_SNAP_LAYOUT=1
```

If using a Visual Studio project:

- Right-click the project → Properties
- Select C/C++ → Preprocessor
- Add the macros under Preprocessor Definitions

## Bringing SARibbon into a CMake project

1. Build and install SARibbon with CMake first.
2. In your own `CMakeLists.txt`:

### 1. Tell CMake where SARibbon was installed

Point `SARibbonBar_DIR` to the folder that contains `SARibbonBarConfig.cmake`, e.g.

```cmake
set(SARibbonBar_DIR "C:/src/Qt/SARibbon/bin_qt5.14.2_MSVC_x64/lib/cmake/SARibbonBar")
```

### 2. Load the package

```cmake
find_package(SARibbonBar REQUIRED)
```

This fills the variables `SARibbonBar_INCLUDE_DIRS`, compile definitions, etc.

### 3. Link your target

```cmake
target_link_libraries(${YOUR_TARGET} PUBLIC SARibbonBar::SARibbonBar)
```

That is all that is required.

---

## Bringing SARibbon into a qmake project

!!! warning 
    Qt 6 officially deprecated qmake; SARibbon may drop qmake support in a future release.

After you built SARibbon with qmake you will find a folder  
`bin_qt{version}_{MSVC|GNU}_x{32|64}` that contains the libraries and DLLs.  
A convenience pri file, `importSARibbonBarLib.pri`, automatically adds these to your project.

Step-by-step:

### 1. Copy the source tree

Create a `3rdparty` folder inside your project and copy the whole `SARibbon` directory into it.

> The repository already contains several *.pri* files that make inclusion trivial.  
> `importSARibbonBarLib.pri` is the entry point; adapt it if you change the directory layout.

### 2. Edit `common.pri`

Open `3rdparty/SARibbon/common.pri` and uncomment the options you need:

```shell
# SA_RIBBON_CONFIG += use_frameless
#     Use the QWindowKit borderless helper (C++17, Qt ≥ 5.14 or Qt ≥ 6.4).
#     Adds native snap, better multi-screen support on Ubuntu/macOS.
#     If enabled, set the install path:
# SA_RIBBON_QWindowKit_Install_DIR = $$SARIBBON_BIN_DIR

# SA_RIBBON_CONFIG += enable_snap_layout
#     Enable Windows-11 Snap-Layout (only valid when use_frameless is set).
```

### 3. Include the pri file

In your `.pro` file add:

```shell
include($$PWD/3rdparty/SARibbon/importSARibbonBarLib.pri)
```

Typical resulting tree:

```
your-project.pro
3rdparty/
└── SARibbon/
    ├── importSARibbonBarLib.pri   (entry point)
    ├── SARibbonBar.pri            (sets libs & includes)
    ├── common.pri                 (your tweaks)
    ├── bin_qt5.14.2_MSVC_x64/   (binaries)
    └── src/
```

The three pri files above are the only ones you normally touch.

---

## Public pre-processor macros

When you consume a **pre-built** SARibbon library (especially with MSVC) you **must** define the same macros that were used while building it:

| Macro | Value | Meaning |
|-------|-------|---------|
| `SARIBBON_USE_3RDPARTY_FRAMELESSHELPER` | 1/0 | Built with/without QWindowKit |
| `SARIBBON_ENABLE_SNAP_LAYOUT` | 1/0 | Snap-layout support (only if previous = 1) |

---

## Enabling QWindowKit in a *static* inclusion

Static inclusion = add `SARibbon.h` + `SARibbon.cpp` directly to your project  
(QWindowKit is **off** by default here).

To turn it on:

1. Define the macros  
   `SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1`  
   `SARIBBON_ENABLE_SNAP_LAYOUT=1`   (optional)

2. qmake example:

   ```shell
   DEFINES += SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1 \
              SARIBBON_ENABLE_SNAP_LAYOUT=1
   ```

3. Visual Studio example:

   Project → Properties → C/C++ → Preprocessor →  
   Preprocessor Definitions → add the two macros above.

Rebuild—QWindowKit support is now active even in the single-header/static mode.

## Complete CMakeLists.txt Example for Direct Inclusion

Below is a complete CMakeLists.txt configuration for directly embedding `SARibbon.h` and `SARibbon.cpp` into your project (referenced from `example/StaticExample`):

```cmake
cmake_minimum_required(VERSION 3.5)
project(MyRibbonApp VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Qt6 requires C++17, Qt5 uses C++14
if(QT_VERSION_MAJOR EQUAL 6)
    set(CMAKE_CXX_STANDARD 17)
else()
    set(CMAKE_CXX_STANDARD 14)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find Qt
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Gui Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Widgets)

# SARibbon source file paths (adjust according to actual location)
set(SARIBBON_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/SARibbon/src)

# Add your source files and the two SARibbon files
add_executable(${PROJECT_NAME}
    main.cpp
    mainwindow.cpp
    mainwindow.h
    ${SARIBBON_DIR}/SARibbon.h
    ${SARIBBON_DIR}/SARibbon.cpp
)

# This macro must be defined for static inclusion
target_compile_definitions(${PROJECT_NAME} PRIVATE SA_RIBBON_BAR_NO_EXPORT)

# Link Qt libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
```

!!! tip "Tip"
    Direct inclusion is the simplest approach, suitable for quick start. For larger projects or when sharing SARibbon across multiple projects, it is recommended to build as a dynamic library and include via `find_package`.