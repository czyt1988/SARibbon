# Python Bindings Build Guide

This document describes how to build the SARibbon Python bindings (PyQtSARibbon) from source.

## Documentation Navigation

| Document | Content |
|----------|---------|
| [Build PyQt Bindings](./build-python-bindings.md) | Compile and install PyQtSARibbon (PyQt5/6) |
| [Build PySide6 Bindings](./build-pyside6-bindings.md) | Compile and install PySideSARibbon (Shiboken6) |
| [Use Python Bindings](./use-python-bindings.md) | Use SARibbon in Python |

## Prerequisites

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

```bash
pip install PyQt5 PyQt5-sip PyQt-builder
```

Verify the installation:

```bash
python -c "import PyQt5; print(PyQt5.__version__)"
python -c "import sipbuild; print(sipbuild.version.SIP_VERSION_STR)"
```

### 2. Verify Qt Installation

```bash
qmake --version
```

### 3. Configure Compiler

#### Windows (MSVC)

```bash
# Use "x64 Native Tools Command Prompt for VS 2019"
"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat" x64
```

#### Linux (GCC)

```bash
sudo apt-get install build-essential
```

#### macOS (Clang)

```bash
xcode-select --install
```

## Build Steps

### 1. Clone Source Code

```bash
git clone https://github.com/czyt1988/SARibbon.git
cd SARibbon
```

### 2. Build Python Bindings

```bash
sip-build --build-dir build-python
```

This command will:

1. Generate C++ binding code
2. Compile the native extension module
3. Generate a PyQtSARibbon package directory containing `__init__.py` and `saribbon.pyd` (Windows) or `saribbon.so` (Linux/macOS)

!!! tip
    If you encounter Qt path errors, specify the qmake path explicitly:

    ```bash
    sip-build --qmake /path/to/qmake --build-dir build-python
    ```

### 3. Install Module

#### Windows

```bash
xcopy /s /y build-python\PyQtSARibbon <Python_path>\Lib\site-packages\PyQtSARibbon\
```

#### Linux/macOS

```bash
cp -r build-python/PyQtSARibbon <Python_path>/lib/python3.x/site-packages/
```

Or use pip:

```bash
pip install .
```

## Verify Installation

```bash
python -c "from PyQtSARibbon import saribbon; print('PyQtSARibbon installed successfully!')"
```

## Troubleshooting

### Qt headers not found

**Error**: atal error: QtWidgets: No such file or directory

```bash
sip-build --qmake /path/to/qmake --build-dir build-python
```

### Compiler not found

**Error**: Cannot run compiler 'cl'

Use the Visual Studio Developer Command Prompt.

### DLL load failed

**Error**: ImportError: DLL load failed while importing PyQtSARibbon

Copy Qt DLLs to site-packages:

```bash
copy <Qt_path>\bin\Qt5Widgets.dll <Python_path>\Lib\site-packages\
copy <Qt_path>\bin\Qt5Gui.dll <Python_path>\Lib\site-packages\
copy <Qt_path>\bin\Qt5Core.dll <Python_path>\Lib\site-packages\
```

### Version mismatch

Ensure PyQt5, sip, and Python versions are compatible.

## Next Steps

See [Use Python Bindings](./use-python-bindings.md) to learn how to use SARibbon in Python.
