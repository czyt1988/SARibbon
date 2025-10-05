# SARibbon Introduction
[中文文档点击这里](./readme-cn.md)

<div align="center">
<p>
<img src="https://img.shields.io/badge/C++-17-blue"/>
<img src="https://img.shields.io/badge/ -5.14+-green"/>
<img src="https://img.shields.io/badge/Qt-6-green"/>
<img src="https://img.shields.io/badge/license-MIT-yellow"/>
</p>
<p>
<img src="https://img.shields.io/badge/windows-0077d6"/>
<img src="https://img.shields.io/badge/ubuntu-ed6432"/>
<img src="https://img.shields.io/badge/macos-000"/>
</p>
</div>

Project Documentation: [https://czyt1988.github.io/SARibbon/en](https://czyt1988.github.io/SARibbon/en)  
Detailed User Guide: [SARibbon User Guide](./docs/SARibbon-User-Guide(en).md)

||Windows (latest)|Linux Ubuntu (latest)|Mac (latest)|
|:-|:-|:-|:-|
|Qt 5.12 LTS|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.12.yml/badge.svg)|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.12.yml/badge.svg)|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.12.yml/badge.svg)|
|Qt 5.14|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.14.yml/badge.svg)|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.14.yml/badge.svg)|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.14.yml/badge.svg)|
|Qt 5.15 LTS|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.15.yml/badge.svg)|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.15.yml/badge.svg)|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.15.yml/badge.svg)|
|Qt 6.2 LTS|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.2.yml/badge.svg)|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.2.yml/badge.svg)|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.2.yml/badge.svg)|
|Qt 6.5 LTS|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.5.yml/badge.svg)|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.5.yml/badge.svg)|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.5.yml/badge.svg)|
|Qt 6.8 LTS|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.8.yml/badge.svg)|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.8.yml/badge.svg)|![Build Status](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.8.yml/badge.svg)|

This is a **Ribbon UI control** for **Qt**, providing an interface similar to Microsoft Office applications.

- `SARibbon` is suitable for large-scale software, industrial applications, and complex UIs.
- It adopts naming conventions inspired by **MFC Ribbon** interfaces.
- The visual style draws from both **Microsoft Office** and **WPS Office** Ribbon interfaces, combining the best features of both.
- `SARibbon` supports multiple customizable themes and allows rapid styling via **Qt Style Sheets (QSS)**.
- To facilitate development of large applications, `SARibbon` includes encapsulated common functional widgets, such as [color picker buttons and color panels](https://github.com/czyt1988/SAColorWidgets).

## Features

### Ribbon Layout and Display

![Overview](./docs/assets/screenshot/SARibbonBar-overview.png)

- Supports **minimized mode**, where only tabs are shown (double-clicking a tab toggles minimization by default). Contextual tabs are also supported.  
  ![Minimized Mode](./docs/assets/screenshot/SARibbonBar-minMode.gif)

- Supports **Quick Access Bar** (like Word’s quick menu) and **Right Button Group**, with adaptive layout behavior in different modes.

- Supports **four types of Ribbon buttons**:
  - Standard push button
  - Delayed popup menu button
  - Menu button
  - Action menu button  
  *(The action menu button addresses one of the core challenges this Ribbon control aims to solve.)*  
  ![Ribbon Buttons](./docs/assets/screenshot/SARibbonBar-ribbonbutton.gif)

- Offers **four distinct layout styles**.  
  ![Layout Styles](./docs/assets/screenshot/SARibbonBar-style.gif)

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

- Provides a **Gallery widget**.  
  ![Gallery](./docs/assets/screenshot/SARibbonBar-gallery.png)

- Supports **horizontal scrolling** and **Option Actions**.  
  ![Option Action](./docs/assets/screenshot/SARibbonBar-option-action.gif)

- Offers **center-aligned mode**.  
  ![Center Alignment](./docs/assets/screenshot/SARibbon-aligment-center.png)

- Fully compatible with **4K displays** and **multi-monitor setups**.

- Supports **Linux** and **macOS** (*UI not deeply optimized for these platforms*).

- Licensed under the **MIT License** — contributions and feedback are welcome!

### Repositories

- [Gitee (Chinese Git hosting)](https://gitee.com/czyt1988/SARibbon)  
- [GitHub](https://github.com/czyt1988/SARibbon)

---

## Building and Usage

- Project documentation is located in the `docs/en/` folder. You can access it directly here:  
  [https://czyt1988.github.io/SARibbon/en](https://czyt1988.github.io/SARibbon/en)

- Doxygen-generated API documentation is deployed at:  
  [https://czyt1988.github.io/SARibbon/doxygen/index.html](https://czyt1988.github.io/SARibbon/doxygen/index.html)

- Learn how to use `SARibbon` by exploring the example project:  
  `example/MainWindowExample`. You can run it to experience the Ribbon’s features interactively.

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

You can generate documentation using **Doxygen**:

- `docs/Doxyfile-qch-en`: generates `.qch` files for integration into **Qt Creator** as Qt Help documentation.
- `docs/Doxyfile-wiki-en`: generates HTML documentation for browser viewing.

This project’s documentation is built with **MkDocs** and deployed to GitHub Pages:  
[https://czyt1988.github.io/SARibbon/en](https://czyt1988.github.io/SARibbon/en)

