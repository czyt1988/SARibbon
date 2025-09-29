# SARibbon简介

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

QQ交流群:755294806、434014314(已满)

<div align="center">
<img src="../assets/SARibbon-qq交流群2.jpg" style="width:300px;"/>
</div>

此项目使用ci进行构建验证，验证结果如下表所示：

||Windows(latest)|Linux ubuntu(latest)|Mac(latest)|
|:-|:-|:-|:-|
|Qt5.12 LTS|[![cmake-win-qt5.12](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.12.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.12.yml)|[![CMake-Linux-Qt5.12](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.12.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.12.yml)|[![cmake-mac-qt5.12](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.12.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.12.yml)|
|Qt5.14|[![cmake-win-qt5.14](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.14.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.14.yml)|[![CMake-Linux-Qt5.14](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.14.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.14.yml)|[![cmake-mac-qt5.14](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.14.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.14.yml)|
|Qt5.15 LTS|[![cmake-win-qt5.15](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.15.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt5.15.yml)|[![CMake-Linux-Qt5.15](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.15.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt5.15.yml)|[![cmake-mac-qt5.15](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.15.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt5.15.yml)|
|Qt6.2 LTS|[![cmake-win-qt6.2](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.2.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.2.yml)|[![CMake-Linux-Qt6.2](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.2.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.2.yml)|[![cmake-mac-qt6.2](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.2.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.2.yml)|
|Qt6.5 LTS|[![cmake-win-qt6.5](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.5.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.5.yml)|[![CMake-Linux-Qt6.5](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.5.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.5.yml)|[![cmake-mac-qt6.5](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.5.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.5.yml)|
|Qt6.8 LTS|[![cmake-win-qt6.8](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.8.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-win-qt6.8.yml)|[![CMake-Linux-Qt6.8](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.8.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-linux-qt6.8.yml)|[![cmake-mac-qt6.8](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.8.yml/badge.svg)](https://github.com/czyt1988/SARibbon/actions/workflows/cmake-mac-qt6.8.yml)|


这是一个`Qt`下的`Ribbon`界面控件，提供了类似微软Office系列软件的操作界面。

- `SARibbon`适用于大型软件、工业软件、复杂软件的ui
- `SARibbon`在设计时参考了MFC Ribbon接口的命名风格
- `SARibbon`的界面样式参考了微软Office系列软件以及WPS软件的Ribbon界面，并结合了两者的优点
- `SARibbon`是一个可定义多种主题风格的Ribbon控件，它能通过qss快速的定义出自己想要的主题风格

为了方便大型软件的开发，`SARibbon`对常用的一些功能性控件进行了封装，例如：[颜色选择按钮和颜色选择画板](https://github.com/czyt1988/SAColorWidgets)

## 功能特点

- 针对Ribbon的布局和显示

![Ribbon的布局和显示](../assets/screenshot/SARibbonBar-overview.png)

- 支持最小化模式，ribbon只显示标签（默认双击标签会进行切换）,支持上下文标签tab

![SARibbon最小化模式](../assets/screenshot/SARibbonBar-minMode.gif)

- 支持quickAccessBar（word快速菜单）和rightButtonGroup，在不同布局模式下会有不同的显示效果
- 支持4种不同的ribbon button，普通按钮，延迟弹出菜单按钮，菜单按钮，action菜单按钮（action菜单按钮是此ribbon控件最主要解决的问题之一）

![SARibbon-4种不同的ribbon button](../assets/screenshot/SARibbonBar-ribbonbutton.gif)

- 支持4种不同风格的布局样式

![SARibbon-4种不同风格的布局样式](../assets/screenshot/SARibbonBar-style.gif)

- 支持qss对ribbon进行自定义设置，可实时切换主题,内置了6种不同风格的主题

win7主题：
![SARibbon-theme-win7](../assets/screenshot/SARibbon-theme-win7.png)
office2013主题：
![SARibbon-theme-office2013](../assets/screenshot/SARibbon-theme-office2013.png)
office2016主题：
![SARibbon-theme-office2016](../assets/screenshot/SARibbon-theme-office2016.png)
office2021主题：
![SARibbon-theme-office2021](../assets/screenshot/SARibbon-theme-office2021.png)
dark主题：
![SARibbon-theme-dark](../assets/screenshot/SARibbon-theme-dark.png)
dark2主题：
![SARibbon-theme-dark](../assets/screenshot/SARibbon-theme-dark2.png)

- 提供Gallery控件

![](../assets/screenshot/SARibbonBar-gallery.png)

- 支持超长滚动和Option Action

![](../assets/screenshot/SARibbonBar-option-action.gif)

- 提供居中对齐模式

![](../assets/screenshot/SARibbon-aligment-center.png)

- 支持4K屏和多屏幕扩展
- 支持linux和MacOS（界面未做深度适配）

MIT协议，欢迎大家使用并提出意见

[gitee(码云) - https://gitee.com/czyt1988/SARibbon](https://gitee.com/czyt1988/SARibbon)

[github - https://github.com/czyt1988/SARibbon](https://github.com/czyt1988/SARibbon)

## 更多截图

- 这是使用SARibbon构建的软件截图

![](../assets/screenshot/data-workbench-screenshot1-cn.gif)
![](../assets/screenshot/data-workbench-screenshot01-en.png)
![](../assets/screenshot/data-workbench-screenshot01-cn.png)

[github - https://github.com/czyt1988/data-workbench](https://github.com/czyt1988/data-workbench)

[gitee - https://gitee.com/czyt1988/data-workbench](https://gitee.com/czyt1988/data-workbench)

具体Ribbon的生成代码可见：

[https://github.com/czyt1988/data-workbench/blob/master/src/APP/DAAppRibbonArea.cpp](https://github.com/czyt1988/data-workbench/blob/master/src/APP/DAAppRibbonArea.cpp)

## 给我一个鼓励

如果项目对你有用，请你给我一个鼓励：

<div style="text-align:center">
    <img src="../assets/pic/赞赏码.png" alt="赞赏码" style="width:400px;" />
</div>
