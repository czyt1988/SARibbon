# PyPI Publishing Guide

This document explains how to publish SARibbon Python bindings to PyPI so users can install them via `pip install`.

## Package Structure

SARibbon has three Python bindings, each published as a separate PyPI package:

| PyPI Package | Binding | Build Backend | Dependency | Install Command |
|--------------|---------|---------------|------------|-----------------|
| `PyQtSARibbon` | PyQt5 (SIP) | `sipbuild.api` | `PyQt5>=5.15` | `pip install PyQtSARibbon` |
| `PyQt6SARibbon` | PyQt6 (SIP) | `sipbuild.api` | `PyQt6>=6.2` | `pip install PyQt6SARibbon` |
| `PySideSARibbon` | PySide6 (Shiboken6) | `scikit-build-core` | `PySide6>=6.5` | `pip install PySideSARibbon` |

### Project Directory Structure

```
SARibbon/
├── pyproject.toml          # PyQt5: SIP build config (package PyQtSARibbon)
├── project.py              # PyQt5: PyQtBuilder project class
├── sip/                    # PyQt5: SIP binding description files
│   ├── SARibbon.sip        #   Module entry (name=PyQtSARibbon.saribbon)
│   └── *.sip               #   Per-class SIP descriptions
├── pyqt6/                  # PyQt6: standalone SIP project directory
│   ├── pyproject.toml      #   SIP build config (package PyQt6SARibbon)
│   └── sip/                #   SIP binding files (name=PyQt6SARibbon.saribbon)
├── pyside6/                # PySide6: Shiboken6 + CMake project directory
│   ├── pyproject.toml      #   scikit-build-core build config (package PySideSARibbon)
│   ├── CMakeLists.txt      #   CMake build script
│   ├── typesystem_saribbon.xml  # Shiboken type system
│   ├── saribbon_python_glue.h   # C++ glue code
│   └── PySideSARibbon/
│       └── __init__.py     #   Python package entry
└── .github/workflows/
    └── publish-python-bindings.yml  # CI auto build + publish
```

## Build Backends

### PyQt5 / PyQt6 (SIP)

Uses `sipbuild.api` as the PEP 517 build backend. `sip-build` / `pip wheel` automatically:
1. Parses `.sip` files to generate C++ binding code
2. Compiles C++ sources and binding code into an extension module
3. Packages into a standard wheel

```bash
# PyQt5 (from repo root)
pip wheel . --no-build-isolation

# PyQt6 (from pyqt6/ directory)
pip wheel pyqt6/ --no-build-isolation
```

### PySide6 (scikit-build-core)

Uses `scikit-build-core` as the PEP 517 build backend, bridging `pyproject.toml` and CMake. `pip wheel` automatically:
1. Calls CMake to configure and build
2. Runs shiboken6 to generate binding code
3. Compiles into an extension module
4. Packages via CMake install targets into a wheel

```bash
# PySide6 (from pyside6/ directory)
pip wheel pyside6/
```

> **Note**: Building PySide6 bindings requires Qt6 C++ development package (headers), which is not included in the PySide6 pip package. The build environment needs Qt6 dev installed, but end users only need `pip install PySideSARibbon`.

## CI/CD Automation

GitHub Actions workflow: `.github/workflows/publish-python-bindings.yml`

### Triggers
- Automatic on GitHub Release publication
- Manual trigger supported (`workflow_dispatch`)

### Build Matrix

| Platform | Python | Qt Version |
|----------|--------|------------|
| Windows x64 | 3.10, 3.11, 3.12 | Qt 6.8.3 (MSVC 2022) / Qt 5.15.2 (MSVC 2019) |
| Linux x64 | 3.10, 3.11, 3.12 | Qt 6 (apt) / Qt 5 (apt) |
| macOS | 3.10, 3.11, 3.12 | Qt 6 (brew) / Qt 5 (brew) |

### Publishing Method

Uses PyPI Trusted Publishing (OIDC) — no API token needed:
1. Configure Trusted Publisher at [pypi.org](https://pypi.org/manage/account/publishing/)
2. GitHub Actions builds wheels for all platforms
3. Uses `pypa/gh-action-pypi-publish` to upload to PyPI

## Version Synchronization

Version numbers must be kept in sync across these locations:

| File | Field |
|------|-------|
| `pyproject.toml` (PyQt5) | `version = "x.y.z"` |
| `pyqt6/pyproject.toml` (PyQt6) | `version = "x.y.z"` |
| `pyside6/pyproject.toml` (PySide6) | `version = "x.y.z"` |
| `pyside6/CMakeLists.txt` | `project(PySideSARibbon VERSION x.y.z)` |
| `pyside6/PySideSARibbon/__init__.py` | `__version__ = "x.y.z"` |

## First-Time Publish Steps

1. **Register PyPI package names**: The following names are auto-registered on first upload:
   - `PyQtSARibbon`
   - `PyQt6SARibbon`
   - `PySideSARibbon`

2. **Configure Trusted Publishing**:
   - Log in to PyPI → Account settings → Publishing → Add a publisher
   - Select GitHub, enter repo `czyt1988/SARibbon`
   - Workflow: `publish-python-bindings.yml`
   - Environment: `pypi`

3. **Create a Release**:
   ```bash
   git tag v2.8.0
   git push origin v2.8.0
   # Create a Release on GitHub
   ```

4. CI automatically builds and publishes to PyPI

5. **Verify installation**:
   ```bash
   pip install PyQtSARibbon      # PyQt5
   pip install PyQt6SARibbon     # PyQt6
   pip install PySideSARibbon    # PySide6
   ```

## References

- [scikit-build-core Documentation](https://scikit-build-core.readthedocs.io/)
- [PyPI Trusted Publishing](https://docs.pypi.org/trusted-publishers/)
- [SIP / PyQt-builder](https://www.riverbankcomputing.com/static/Docs/sip/)
