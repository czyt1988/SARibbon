# SARibbon Introduction

[点击此处查看中文版本](../zh/index.md)

<div align="center">
<p>
<img src="https://img.shields.io/badge/C++-17-blue"/>
<img src="https://img.shields.io/badge/Qt-5.14+-green"/>
<img src="https://img.shields.io/badge/Qt-6-green"/>
<img src="https://img.shields.io/badge/license-MIT-yellow"/>
</p>
<p>
<img src="https://img.shields.io/badge/windows-0077d6"/>
<img src="https://img.shields.io/badge/ubuntu-ed6432"/>
<img src="https://img.shields.io/badge/macos-000"/>
</p>
</div>


📚 **Documentation**：[https://czyt1988.github.io/SARibbon/en](https://czyt1988.github.io/SARibbon/en)

---

||Windows(latest)|Linux ubuntu(latest)|Mac(latest)|
|:-|:-|:-|:-|
|Qt5.12 LTS|[![cmake-win-qt5.12](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.12.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.12.yml)|[![CMake-Linux-Qt5.12](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.12.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.12.yml)|[![cmake-mac-qt5.12](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.12.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.12.yml)|
|Qt5.14|[![cmake-win-qt5.14](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.14.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.14.yml)|[![CMake-Linux-Qt5.14](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.14.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.14.yml)|[![cmake-mac-qt5.14](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.14.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.14.yml)|
|Qt5.15 LTS|[![cmake-win-qt5.15](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.15.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.15.yml)|[![CMake-Linux-Qt5.15](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.15.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.15.yml)|[![cmake-mac-qt5.15](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.15.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.15.yml)|
|Qt6.2 LTS|[![cmake-win-qt6.2](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.2.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.2.yml)|[![CMake-Linux-Qt6.2](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.2.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.2.yml)|[![cmake-mac-qt6.2](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.2.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.2.yml)|
|Qt6.5 LTS|[![cmake-win-qt6.5](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.5.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.5.yml)|[![CMake-Linux-Qt6.5](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.5.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.5.yml)|[![cmake-mac-qt6.5](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.5.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.5.yml)|
|Qt6.8 LTS|[![cmake-win-qt6.8](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.8.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.8.yml)|[![CMake-Linux-Qt6.8](https://github.com/czyt8/SARibbon/actions/workflows/cmake-linux-qt6.8.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.8.yml)|[![cmake-mac-qt6.8](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.8.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.8.yml)|

---

## Project Overview

**SARibbon** is a Qt-based Ribbon UI control library designed to provide modern, Microsoft Office-like interfaces for desktop applications.

### Applicable Scenarios
- Large-scale software
- Industrial-grade software
- Complex desktop applications

### Design Philosophy
- API naming style references **MFC Ribbon**
- UI style combines the strengths of **Microsoft Office** and **WPS Office**
- Supports rapid theme customization via **QSS (Qt Style Sheets)**
- Provides rich built-in control encapsulations (e.g., [color picker button and palette](https://github.com/czyt1988/SAColorWidgets))

## Features

- Ribbon layout and display

![Ribbon layout and display](../assets/screenshot/SARibbonBar-overview.png)

- Supports minimize mode where only tabs are shown (double-click a tab to toggle); supports contextual tabs

![SARibbon minimize mode](../assets/screenshot/SARibbonBar-minMode.gif)

- Supports quick access bar and right button group, with adaptive display under different layout modes
- Supports multiple ribbon button styles: normal button, delayed popup menu button, menu button, action menu button (action menu button is one of the key problems this library solves)

![SARibbon multiple ribbon button styles](../assets/screenshot/SARibbonBar-ribbonbutton.gif)

- Supports multiple layout styles

![SARibbon multiple layout styles](../assets/screenshot/SARibbonBar-style.gif)

- Supports QSS-based ribbon customization and real-time theme switching; includes 6 built-in themes

Win7 theme:
![SARibbon theme win7](../assets/screenshot/SARibbon-theme-win7.png)
Office 2013 theme:
![SARibbon theme office2013](../assets/screenshot/SARibbon-theme-office2013.png)
Office 2016 theme:
![SARibbon theme office2016](../assets/screenshot/SARibbon-theme-office2016.png)
Office 2021 theme:
![SARibbon theme office2021](../assets/screenshot/SARibbon-theme-office2021.png)
Dark theme:
![SARibbon theme dark](../assets/screenshot/SARibbon-theme-dark.png)
Dark2 theme:
![SARibbon theme dark2](../assets/screenshot/SARibbon-theme-dark2.png)

- Provides Gallery control

![SARibbonBar gallery](../assets/screenshot/SARibbonBar-gallery.png)

- Supports long content scrolling and Option Action

![SARibbonBar option action](../assets/screenshot/SARibbonBar-option-action.gif)

- Provides center alignment mode

![SARibbon alignment center](../assets/screenshot/SARibbon-aligment-center.png)

- Supports 4K screens and multi-monitor setups
- Supports Linux and macOS (UI not deeply adapted)

- **License**: MIT (free to use, modify, and distribute)
- **Contributions welcome**: Feel free to submit issues, PRs, or join the discussion group!

[Gitee - https://gitee.com/czyt1988/SARibbon](https://gitee.com/czyt1988/SARibbon)

[GitHub - https://github.com/czyt1988/SARibbon](https://github.com/czyt1988/SARibbon)

## Build and Usage

**Documentation is located in the docs/en folder, or visit:** [https://czyt1988.github.io/SARibbon/en](https://czyt1988.github.io/SARibbon/en)

**Doxygen documentation is deployed at:** [https://czyt1988.github.io/SARibbon/doxygen/index.html](https://czyt1988.github.io/SARibbon/doxygen/index.html)

**It is recommended to start learning SARibbon from the `example/MainWindowExample`, which you can also run directly to experience the features.**

## More Screenshots

- Screenshots of software built with SARibbon

![data-workbench-screenshot1-en](../assets/screenshot/data-workbench-screenshot1-cn.gif)
![data-workbench-screenshot01-en](../assets/screenshot/data-workbench-screenshot01-en.png)

[GitHub - https://github.com/czyt1988/data-workbench](https://github.com/czyt1988/data-workbench)

[Gitee - https://gitee.com/czyt1988/data-workbench](https://gitee.com/czyt1988/data-workbench)

Sample Ribbon generation code:

[https://github.com/czyt1988/data-workbench/blob/master/src/APP/DAAppRibbonArea.cpp](https://github.com/czyt1988/data-workbench/blob/master/src/APP/DAAppRibbonArea.cpp)

## Documentation Generation

- Use `doxygen` to generate HTML or `.qch` documentation:
  - `docs/Doxyfile-wiki-en` → Generate web documentation
  - `docs/Doxyfile-qch-en` → Generate Qt Creator help documentation
- Static documentation is built with `mkdocs` and deployed to GitHub Pages: [https://czyt1988.github.io/SARibbon/en](https://czyt1988.github.io/SARibbon/en)
