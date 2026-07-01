# Python Bindings Build Guide (PyQt5 / PyQt6)

This document describes how to build the SARibbon Python bindings (PyQtSARibbon) from source, for both PyQt5 and PyQt6.

## Documentation Navigation

| Document | Content |
|----------|---------|
| [Build PyQt Bindings](./build-python-bindings.md) | Compile and install PyQtSARibbon (PyQt5/6) |
| [Build PySide6 Bindings](./build-pyside6-bindings.md) | Compile and install PySideSARibbon (Shiboken6) |
| [Use Python Bindings](./use-python-bindings.md) | Use SARibbon in Python |

## Prerequisites

### PyQt5

| Software | Version | Description |
|----------|---------|-------------|
| Python | 3.8 or higher | 3.10+ recommended |
| PyQt5 | 5.15.4 or higher | Python Qt bindings |
| Qt | 5.15 or higher | C++ Qt framework |
| sip | 6.0.2 or higher | PyQt build tools |
| PyQt-builder | 1.6 or higher | PyQt build system |
| C++ Compiler | MSVC 2019+ / GCC 9+ | Native code compiler |

### PyQt6

| Software | Version | Description |
|----------|---------|-------------|
| Python | 3.8 or higher | 3.10+ recommended |
| PyQt6 | 6.2 or higher | Python Qt bindings |
| PyQt6-sip | 13.4 or higher | SIP support for PyQt6 |
| Qt | 6.2 or higher | C++ Qt framework |
| sip | 6.0.2 or higher | PyQt build tools |
| PyQt-builder | 1.6 or higher | PyQt build system |
| C++ Compiler | MSVC 2019+ / GCC 9+ | Native code compiler |

## Installing Dependencies

### PyQt5

```bash
pip install PyQt5 PyQt5-sip PyQt-builder
```

### PyQt6

```bash
pip install PyQt6 PyQt6-sip PyQt-builder
```

### Verify Qt Installation

```bash
qmake --version
```

### Configure Compiler

#### Windows (MSVC)

```bash
# Use "x64 Native Tools Command Prompt for VS 2019"
"C:\\Program Files (x86)\\Microsoft Visual Studio\\2019\\Community\\VC\\Auxiliary\\Build\\vcvarsall.bat" x64
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

### 2. Build Python Bindings

#### PyQt5

```bash
sip-build --build-dir build-python
```

#### PyQt6

The project includes `pyproject-pyqt6.toml` for PyQt6 builds. Use the build script with the `--pyqt6` flag:

```bash
tools\build_python_bindings.bat --pyqt6
```

Or manually swap the config file and build:

```bash
copy pyproject-pyqt6.toml pyproject.toml
sip-build --build-dir build-python6
copy pyproject.toml pyproject-pyqt6.toml
```

!!! tip
    If you encounter Qt path errors, specify the qmake path explicitly:

    ```bash
    sip-build --qmake /path/to/qmake --build-dir build-python
    ```

### 3. Install Module

#### Windows

```bash
# PyQt5
xcopy /s /y build-python\PyQtSARibbon <Python_path>\Lib\site-packages\PyQtSARibbon\

# PyQt6
xcopy /s /y build-python6\PyQtSARibbon <Python_path>\Lib\site-packages\PyQtSARibbon\
```

#### Linux/macOS

```bash
# PyQt5
cp -r build-python/PyQtSARibbon <Python_path>/lib/python3.x/site-packages/

# PyQt6
cp -r build-python6/PyQtSARibbon <Python_path>/lib/python3.x/site-packages/
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

**Error**: `fatal error: QtWidgets: No such file or directory`

```bash
sip-build --qmake /path/to/qmake --build-dir build-python
```

### Compiler not found

**Error**: Cannot run compiler 'cl'

Use the Visual Studio Developer Command Prompt.

### DLL load failed

**Error**: `ImportError: DLL load failed while importing PyQtSARibbon`

Copy Qt DLLs to site-packages, or add Qt bin directory to PATH.

### PyQt5 / PyQt6 conflict

Use a virtual environment to isolate PyQt5 and PyQt6:

```bash
python -m venv venv-pyqt5
venv-pyqt5\Scripts\activate
pip install PyQt5 PyQt5-sip PyQt-builder
```

### Version mismatch

Ensure PyQt, sip, and Python versions are compatible.

## Next Steps

See [Use Python Bindings](./use-python-bindings.md) to learn how to use SARibbon in Python.
