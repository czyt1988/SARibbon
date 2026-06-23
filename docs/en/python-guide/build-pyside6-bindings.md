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
| **Qt6 Dev Package** | 6.5 or higher | **Required**: Qt6 C++ headers and CMake config |
| CMake | 3.22 or higher | Build system |
| Ninja | latest | Recommended build backend |
| C++ Compiler | MSVC 2019+ / GCC 9+ | Native code compiler |

!!! warning "Important"
    The PySide6 pip package does **not** include full Qt6 C++ development headers
    (such as `qglobal.h`). You must install Qt6 development package via the
    Qt Online Installer or `aqtinstall`.

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

**Error**: `Could not find a package configuration file provided by "Shiboken6Tools"`

```bash
pip install shiboken6-generator
```

### Qt headers not found

**Error**: `fatal error: QtWidgets: No such file or directory`

```bash
cmake -S pyside6 -B build-pyside6 -DCMAKE_PREFIX_PATH="/path/to/Qt/6.7.3/msvc2019_64"
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

## Next Steps

See [Use Python Bindings](./use-python-bindings.md) for how to use SARibbon in Python.
