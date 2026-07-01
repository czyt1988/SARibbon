# SARibbon Introduction

[中文文档点击这里](./readme-cn.md)

<div align="center">
<p>
<img src="https://img.shields.io/badge/C++-17-blue"/>
<img src="https://img.shields.io/badge/Qt-5.14+-green"/>
<img src="https://img.shields.io/badge/Qt-6-green"/>
<img src="https://img.shields.io/badge/Python-PyQt5%20%7C%20PyQt6%20%7C%20PySide6-blueviolet"/>
<img src="https://img.shields.io/badge/license-MIT-yellow"/>
</p>
<p>
<img src="https://img.shields.io/badge/windows-0077d6"/>
<img src="https://img.shields.io/badge/ubuntu-ed6432"/>
<img src="https://img.shields.io/badge/macos-000"/>
</p>
</div>

📚 **Project Documentation**: [https://czyt1988.github.io/SARibbon/en](https://czyt1988.github.io/SARibbon/en)  
Detailed User Guide: [SARibbon User Guide](./docs/en/use-guide/SARibbon-user-guide.md)

||Windows (latest)|Linux Ubuntu (latest)|Mac (latest)|
|:-|:-|:-|:-|
|Qt 5.12 LTS|[![cmake-win-qt5.12](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.12.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.12.yml)|[![CMake-Linux-Qt5.12](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.12.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.12.yml)|[![cmake-mac-qt5.12](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.12.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.12.yml)|
|Qt 5.14|[![cmake-win-qt5.14](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.14.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.14.yml)|[![CMake-Linux-Qt5.14](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.14.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.14.yml)|[![cmake-mac-qt5.14](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.14.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.14.yml)|
|Qt 5.15 LTS|[![cmake-win-qt5.15](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.15.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.15.yml)|[![CMake-Linux-Qt5.15](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.15.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.15.yml)|[![cmake-mac-qt5.15](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.15.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.15.yml)|
|Qt 6.2 LTS|[![cmake-win-qt6.2](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.2.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.2.yml)|[![CMake-Linux-Qt6.2](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.2.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.2.yml)|[![cmake-mac-qt6.2](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.2.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.2.yml)|
|Qt 6.5 LTS|[![cmake-win-qt6.5](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.5.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.5.yml)|[![CMake-Linux-Qt6.5](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.5.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.5.yml)|[![cmake-mac-qt6.5](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.5.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.5.yml)|
|Qt 6.8 LTS|[![cmake-win-qt6.8](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.8.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.8.yml)|[![CMake-Linux-Qt6.8](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.8.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.8.yml)|[![cmake-mac-qt6.8](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.8.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.8.yml)|

---

## Project Overview

**SARibbon** is a **Ribbon UI control library** for **Qt**, designed to provide desktop applications with a modern interface similar to Microsoft Office applications.

### Use Cases

- Large-scale software
- Industrial software
- Complex desktop applications

### Design Philosophy

- Interface naming inspired by **MFC Ribbon**
- Visual style blends the best of **Microsoft Office** and **WPS Office** Ribbon interfaces
- Supports rapid theme customization via **QSS (Qt Style Sheets)**
- Includes encapsulated common functional widgets, such as [color picker buttons and color panels](https://github.com/czyt1988/SAColorWidgets)

---

## Features

### Ribbon Layout and Display

![Overview](./docs/assets/screenshot/SARibbonBar-overview.png)

- Supports **minimized mode**, where only tabs are shown (double-clicking a tab toggles minimization by default). Contextual tabs are also supported.  
  ![Minimized Mode](./docs/assets/screenshot/SARibbonBar-minMode.gif)

- Supports **Quick Access Bar** (like Word's quick menu) and **Right Button Group**, with adaptive layout behavior in different modes.

- Supports **four types of Ribbon buttons**: standard push button, delayed popup menu button, menu button, and action menu button. *(The action menu button addresses one of the core challenges this Ribbon control aims to solve.)*  
  ![Ribbon Buttons](./docs/assets/screenshot/SARibbonBar-ribbonbutton.gif)  
  See: [Ribbon Button Layout Guide](./docs/en/use-guide/layout-of-ribbonbutton.md)

- Offers **six distinct layout styles** — Loose 3-row, Loose 2-row, Compact 3-row, Compact 2-row, Loose Single-row, Compact Single-row — with Single-row mode ideal for space-constrained scenarios.  
  Layout styles:  
  ![Layout Styles](./docs/assets/screenshot/SARibbonBar-style.gif)  
  Single-row layout:  
  ![Single-row Compact](./docs/assets/screenshot/single-style-compact.png)  
  See: [SARibbon Layout Guide](./docs/en/use-guide/layout-of-SARibbon.md)

- Fully customizable via **QSS**, with **real-time theme switching** support. Includes **6 built-in themes**:
  - Windows 7  
    ![Win7 Theme](./docs/assets/screenshot/SARibbon-theme-win7.png)
  - Office 2013  
    ![Office 2013 Theme](./docs/assets/screenshot/SARibbon-theme-office2013.png)
  - Office 2016  
    ![Office 2016 Theme](./docs/assets/screenshot/SARibbon-theme-office2016.png)
  - Office 2021  
    ![Office 2021 Theme](./docs/assets/screenshot/SARibbon-theme-office2021.png)
  - Dark  
    ![Dark Theme](./docs/assets/screenshot/SARibbon-theme-dark.png)
  - Dark 2  
    ![Dark 2 Theme](./docs/assets/screenshot/SARibbon-theme-dark2.png)
  
  You can design any theme via QSS. See: [Design Your Theme](./docs/en/use-guide/design-your-theme.md)

- Provides a **Gallery widget**.  
  ![Gallery](./docs/assets/screenshot/SARibbonBar-gallery.png)

- Supports **horizontal scrolling** and **Option Actions**.  
  ![Option Action](./docs/assets/screenshot/SARibbonBar-option-action.gif)

- Offers **center-aligned mode**.  
  ![Center Alignment](./docs/assets/screenshot/SARibbon-aligment-center.png)

- Fully compatible with **4K displays** and **multi-monitor setups**.

- Supports **Linux** and **macOS** (*UI not deeply optimized for these platforms*).

- Provides **RTL mode** with automatic system detection.

**License**: MIT (free to use, modify, and distribute)  
**Contributions welcome**: feel free to submit Issues, PRs, or join discussions!

### Repositories

- [Gitee (Chinese Git hosting)](https://gitee.com/czyt1988/SARibbon)  
- [GitHub](https://github.com/czyt1988/SARibbon)

---

## Building and Usage

`SARibbon` provides amalgamated integration files (`SARibbon.h` and `SARibbon.cpp`) in the `src/` directory — just include them in your project to get started.

To build as a dynamic library, see: [SARibbon Build Instructions](./docs/en/build-guide/build-instructions.md)

Quick start guides:

- [Create a Ribbon-Style Window](./docs/en/use-guide/create-ribbon-style-window.md)
- [Create a Ribbon Interface](./docs/en/use-guide/create-ribbon-ui.md)

**We recommend starting with the `example/MainWindowExample` project to learn SARibbon usage. You can run it directly to explore all features interactively.**

Project documentation is in the `docs/en/` folder:  
[https://czyt1988.github.io/SARibbon/en](https://czyt1988.github.io/SARibbon/en)

Doxygen-generated API documentation is deployed at:
[https://czyt1988.github.io/SARibbon/doxygen/index.html](https://czyt1988.github.io/SARibbon/doxygen/index.html)

---

## Python Bindings

SARibbon provides Python bindings for three frameworks, allowing you to use SARibbon in Python applications:

| Binding | Framework | Build Tool |
|---------|-----------|------------|
| PyQtSARibbon | PyQt5 | SIP |
| PyQtSARibbon6 | PyQt6 | SIP |
| PySideSARibbon | PySide6 | Shiboken6 |

Install via pip:

```bash
pip install PyQtSARibbon    # PyQt5
pip install PyQtSARibbon6   # PyQt6
pip install PySideSARibbon  # PySide6
```

For building from source and detailed usage, see:
- [Build Python Bindings](./docs/en/python-guide/build-python-bindings.md)
- [Use Python Bindings](./docs/en/python-guide/use-python-bindings.md)
- [Build PySide6 Bindings](./docs/en/python-guide/build-pyside6-bindings.md)
- [Publish to PyPI](./docs/en/python-guide/publish-to-pypi.md)

Python demo examples are in the `pyexamples/` directory.

---

## More Screenshots

Screenshots of real applications built with SARibbon:

![Data Workbench GIF](./docs/assets/screenshot/data-workbench-screenshot1-cn.gif)  
![Data Workbench EN](./docs/assets/screenshot/data-workbench-screenshot01-en.png)  
![Data Workbench CN](./docs/assets/screenshot/data-workbench-screenshot01-cn.png)

- [GitHub - data-workbench](https://github.com/czyt1988/data-workbench)  
- [Gitee - data-workbench](https://gitee.com/czyt1988/data-workbench)

For actual Ribbon implementation code, see:  
[https://github.com/czyt1988/data-workbench/blob/master/src/APP/DAAppRibbonArea.cpp](https://github.com/czyt1988/data-workbench/blob/master/src/APP/DAAppRibbonArea.cpp)

---

## Documentation Generation

Project documentation is built with **MkDocs** and deployed to GitHub Pages:  
[https://czyt1988.github.io/SARibbon/en](https://czyt1988.github.io/SARibbon/en)

You can also generate documentation using **Doxygen**:

- `docs/doxygen-doc-file/Doxyfile-qch-en`: generates `.qch` files for integration into **Qt Creator** as Qt Help documentation. *(Note: the `-en` variant does not yet exist — use `Doxyfile-qch-cn` as a reference, or create the `-en` version from it.)*
- `docs/doxygen-doc-file/Doxyfile-wiki-en`: generates HTML documentation for browser viewing. *(Note: the `-en` variant does not yet exist — use `Doxyfile-wiki-cn` as a reference, or create the `-en` version from it.)*
