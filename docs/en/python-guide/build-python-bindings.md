# Python Bindings Build Guide

This document describes how to build the SARibbon Python bindings (PyQtSARibbon) from source.

## Documentation Navigation

| Document | Content |
|----------|---------|
| [Build Python Bindings](./build-python-bindings.md) | Compile and install PyQtSARibbon |
| [Use Python Bindings](./use-python-bindings.md) | Use SARibbon in Python |

## Prerequisites

Before building, ensure the following software is installed:

| Software | Version | Description |
|----------|---------|-------------|
| Python | 3.8 or higher | 3.10+ recommended |
| PyQt5 | 5.15.4 or higher | Python Qt bindings |
| Qt | 5.15 or higher | C++ Qt framework |
| sip | 6.0.2 or higher | PyQt build tools |
| PyQt-builder | 1.6 or higher | PyQt build system |
| C++ Compiler | MSVC 2019+ / GCC 9+ | Native code compiler |

## Installing Dependencies

### 1. Install Python Dependencies

Use pip to install the required Python packages:

`ash
pip install PyQt5 PyQt5-sip PyQt-builder
`

Verify the installation:

`ash
python -c "import PyQt5; print(PyQt5.__version__)"
python -c "import sipbuild; print(sipbuild.version.SIP_VERSION_STR)"
`

### 2. Verify Qt Installation

Ensure Qt is properly installed and configured in your PATH:

`ash
qmake --version
`

If the command is not found, add Qt's bin directory to your system PATH.

### 3. Configure Compiler

#### Windows (MSVC)

Use the Visual Studio Developer Command Prompt:

`ash
# Search for "x64 Native Tools Command Prompt for VS 2019" in Start Menu
# Or run:
"C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvarsall.bat" x64
`

#### Linux (GCC)

Ensure development tools are installed:

`ash
sudo apt-get install build-essential
`

#### macOS (Clang)

Ensure Xcode Command Line Tools are installed:

`ash
xcode-select --install
`

## Build Steps

### 1. Clone Source Code

If you haven't cloned the SARibbon repository yet:

`ash
git clone https://github.com/czyt1988/SARibbon.git
cd SARibbon
`

### 2. Build Python Bindings

From the project root directory, run:

`ash
sip-build --build-dir build-python
`

This command will:

1. Generate C++ binding code
2. Compile the native extension module
3. Generate PyQtSARibbon.pyd (Windows) or PyQtSARibbon.so (Linux/macOS)

!!! tip
    If you encounter Qt path errors during the build, you can explicitly specify the qmake path:
    `ash
    sip-build --qmake /path/to/qmake --build-dir build-python
    `

### 3. Install Module

After a successful build, copy the compiled module to Python's site-packages directory:

#### Windows

`ash
copy build-python\PyQtSARibbon\PyQtSARibbon.pyd <Python_path>\Lib\site-packages\
`

#### Linux/macOS

`ash
cp build-python/PyQtSARibbon/PyQtSARibbon.so <Python_path>/lib/python3.x/site-packages/
`

Or use pip to install:

`ash
pip install .
`

## Verify Installation

After installation, verify the module is available:

`ash
python -c "from PyQtSARibbon import SARibbonBar; print('PyQtSARibbon installed successfully!')"
`

If you see "PyQtSARibbon installed successfully!", the installation is correct.

## Troubleshooting

### Issue 1: Qt headers not found

**Error**: atal error: QtWidgets: No such file or directory

**Solution**: Set the Qt header path

`ash
sip-build --qmake /path/to/qmake --build-dir build-python
`

### Issue 2: Compiler not found

**Error**: Cannot run compiler 'cl' (Windows) or compiler not found

**Solution**: Make sure you run the build command in the correct development environment. Windows users should use the Visual Studio Developer Command Prompt.

### Issue 3: DLL load failed

**Error**: ImportError: DLL load failed while importing PyQtSARibbon

**Solution**: Copy Qt DLLs to Python's site-packages directory:

`ash
copy <Qt_path>\bin\Qt5Widgets.dll <Python_path>\Lib\site-packages\
copy <Qt_path>\bin\Qt5Gui.dll <Python_path>\Lib\site-packages\
copy <Qt_path>\bin\Qt5Core.dll <Python_path>\Lib\site-packages\
`

### Issue 4: Version mismatch

**Error**: ImportError: Module use of python3x.dll conflicts with this version of Python

**Solution**: Ensure PyQt5, sip, and Python versions are compatible. It's recommended to use PyQt5 and Python from the same source.

## Uninstallation

To uninstall PyQtSARibbon:

`ash
pip uninstall PyQtSARibbon
`

Or manually delete:

`ash
# Windows
del <Python_path>\Lib\site-packages\PyQtSARibbon.pyd

# Linux/macOS
rm <Python_path>/lib/python3.x/site-packages/PyQtSARibbon.so
`

## Next Steps

After building, see [Use Python Bindings](./use-python-bindings.md) to learn how to use SARibbon in Python.
