# PySide6 Bindings Build Guide

This document describes how to build the SARibbon PySide6 bindings (PySideSARibbon) from source, using the official Qt Shiboken6 generator.

Unlike the PyQt bindings (based on SIP), the PySide6 bindings use Shiboken6 + CMake, but share the same C++ source code.

## Comparison with PyQt Bindings

| Aspect | PyQt (SIP) | PySide6 (Shiboken6) |
|--------|-----------|-------------------|
| Binding description | `.sip` files | `typesystem_saribbon.xml` |
| Build system | pyproject.toml + sip-build | CMakeLists.txt + CMake |
| Enum handling | `%MappedType` to int | `<enum-type>` native Python Enum |
| Resource handling | qmake RESOURCES | CMake qt_add_resources |
| Module name | PyQtSARibbon.saribbon | PySideSARibbon.saribbon |

## Prerequisites

| Software | Version | Description |
|----------|---------|-------------|
| Python | 3.9 or higher | 3.10+ recommended |
| PySide6 | 6.5 or higher | Official Qt Python bindings |
| shiboken6-generator | matching PySide6 | Shiboken6 binding generator |
| **Qt6 Dev Package** | matching PySide6's Qt | **Required**: Qt6 C++ headers and CMake config |
| LLVM / Clang | 15 or higher | Linux/macOS: Shiboken6 needs Clang's builtin headers |
| CMake | 3.22 or higher | Build system |
| Ninja | latest | Recommended build backend |
| C++ Compiler | MSVC 2019+ / GCC 9+ | Native code compiler |

!!! warning "Important"
    The PySide6 pip package does **not** include full Qt6 C++ development headers
    (such as `qglobal.h`). You must install Qt6 development package via the
    Qt Online Installer or `aqtinstall`.

!!! warning "Match the Qt version to PySide6"
    The bindings are linked against the Qt you build with, but at run time they
    load the Qt bundled inside the PySide6 wheel. Install the Qt version that
    PySide6 itself reports, otherwise you get subtle run-time failures rather
    than a build error:

    ```bash
    python -c "from PySide6.QtCore import qVersion; print(qVersion())"
    ```

### Install Qt6 Development Package

#### Option A: Qt Online Installer (Recommended)

Download from [qt.io](https://www.qt.io/download) and install Qt 6.7+ for MSVC 2019 64-bit.

#### Option B: aqtinstall

```bash
pip install aqtinstall
python -m aqt install-qt windows desktop 6.7.3 win64_msvc2019_64 --outputdir D:\Qt6
```

Qt6 path will be `D:\Qt6\6.7.3\msvc2019_64`.

## Installing Dependencies

### 1. Install Python Dependencies

```bash
pip install PySide6 shiboken6-generator
```

Verify:

```bash
python -c "import PySide6; print(PySide6.__version__)"
python -c "import shiboken6_generator; print('shiboken6-generator OK')"
```

### 2. Verify Qt Installation

```bash
qmake --version
```

### 3. Install CMake and Ninja

```bash
pip install cmake ninja
```

### 4. Configure Compiler

#### Windows (MSVC)

```bash
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
```

#### Linux (GCC)

```bash
sudo apt-get install build-essential
# Qt6::Gui requires the OpenGL development files through WrapOpenGL
sudo apt-get install libgl-dev mesa-common-dev
# Shiboken6 needs Clang's builtin headers (stddef.h and friends)
sudo apt-get install clang libclang-dev
```

If Clang is installed but `llvm-config` is not on `PATH`, point Shiboken at it
explicitly:

```bash
export LLVM_INSTALL_DIR=/usr/lib/llvm-18
```

## Build Steps

### 1. Clone Source Code

```bash
git clone https://github.com/czyt1988/SARibbon.git
cd SARibbon
```

### 2. Build PySide6 Bindings

#### Option A: Build Script (Windows Recommended)

```bash
tools\build_pyside6_bindings.bat
```

With explicit paths:

```bash
tools\build_pyside6_bindings.bat "C:\Python311" "D:\Qt\6.7.3\msvc2019_64"
```

The script automatically:

1. Checks PySide6 and shiboken6-generator installation
2. Initializes MSVC environment
3. Runs CMake configure and build
4. Installs to site-packages

#### Option B: Manual CMake Build

```bash
cmake -S pyside6 -B build-pyside6 -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build-pyside6 --parallel
```

With explicit Qt path:

```bash
cmake -S pyside6 -B build-pyside6 -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="D:/Qt/6.7.3/msvc2019_64"
```

### 3. Install Module

```bash
cmake --install build-pyside6
```

## Verify Installation

```bash
python -c "from PySideSARibbon import saribbon; print('PySideSARibbon installed successfully!')"
```

## Example

```bash
python pyexamples/pyside6/ribbon_demo.py
```

## Troubleshooting

### shiboken6-generator not found

**Error**: `Could not find shiboken6 executable`

```bash
pip install shiboken6-generator
```

> **Note**: The current CMakeLists.txt does not use `find_package(Shiboken6Tools)`. Instead, it manually locates `shiboken6.exe`, library files, and include directories. PySide6 6.8.x does not include `Shiboken6ToolsConfig.cmake`, so the old `find_package(Shiboken6Tools)` approach is no longer used.

### Qt headers not found

**Error**: `fatal error: QtWidgets: No such file or directory`

```bash
cmake -S pyside6 -B build-pyside6 -DCMAKE_PREFIX_PATH="/path/to/Qt/6.7.3/msvc2019_64"
```

### Clang builtin headers not found

**Error**: `Unable to locate Clang's built-in include directory`, followed by
`/usr/include/wchar.h: fatal error: 'stddef.h' file not found`

Shiboken6 embeds libclang but does not ship Clang's builtin headers. Install
Clang and, if `llvm-config` is not on `PATH`, set `LLVM_INSTALL_DIR`:

```bash
sudo apt-get install clang libclang-dev
export LLVM_INSTALL_DIR=/usr/lib/llvm-18
```

### OpenGL development files not found

**Error**: `Could NOT find WrapOpenGL (missing: WrapOpenGL_FOUND)`, which makes
`find_package(Qt6 COMPONENTS Gui)` fail.

```bash
sudo apt-get install libgl-dev mesa-common-dev
```

### Compiler not found

**Error**: Cannot run compiler 'cl'

Use the Visual Studio Developer Command Prompt.

### DLL load failed

**Error**: `ImportError: DLL load failed while importing saribbon`

Ensure PySide6 and Qt DLLs are in PATH, or copy Qt DLLs to site-packages.

### PyQt / PySide6 conflict

Use a virtual environment to isolate PyQt and PySide6:

```bash
python -m venv venv-pyside6
venv-pyside6\Scripts\activate
pip install PySide6 shiboken6-generator
```

### Qt class constructors crash when accessed indirectly

> **Update**: After thorough investigation, this issue is NOT caused by shiboken type registration conflicts. Indirect access to Qt classes works **perfectly fine**. The original crash was caused by file encoding issues (mixed BOM/CRLF/LF). Ensure Python source files use UTF-8 without BOM and LF line endings.

### PySide6 and Qt6 version mismatch

**Symptom**: Build succeeds but crashes at runtime, or `qjsonparseerror.h: No such file or directory` error during build.

**Root cause**: The Qt header version in the PySide6 pip package must match the system-installed Qt6 C++ development package version. For example, PySide6 6.8.3 requires Qt 6.8.3 dev package.

**Solution**: Ensure version match:

```bash
# Check PySide6 version
python -c "import PySide6; print(PySide6.__version__)"

# Install matching Qt6 dev package (e.g. 6.8.3)
python -m aqt install-qt windows desktop 6.8.3 win64_msvc2022_64 -O D:\Qt
```

### Themes and icons not showing at runtime

**Symptom**: Window starts with no QSS theme styling, icons missing, console shows `can not load build in ribbon theme`.

**Root cause**: The RCC static initializer for Qt resource files (`.qrc`) is stripped by MSVC linker's `/OPT:REF` optimization. When `qt_add_resources` output is placed in the static library `saribbon_lib`, no code explicitly references the qrc-generated symbols, so the entire qrc object file is removed from the final `.pyd`.

**Solution**: Ensure `qt_add_resources` output is compiled into the final MODULE target (`.pyd`), not the static library. The current CMakeLists.txt handles this correctly:

```cmake
# NOTE: qrc resources are NOT added to saribbon_lib. They are compiled
# directly into the final .pyd module (see below) to ensure the RCC
# static initializer is included.
```

## Next Steps

See [Use Python Bindings](./use-python-bindings.md) for how to use SARibbon in Python.
