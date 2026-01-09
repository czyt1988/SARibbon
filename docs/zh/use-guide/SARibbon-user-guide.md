# SARibbon使用说明

SARibbon 是一个用于创建现代化 Ribbon 界面的 Qt 库，其界面风格类似于 Microsoft Office 或 WPS。它专为复杂桌面应用程序设计，能有效组织大量功能，常见于工业软件的界面开发。

在开始编码之前，您需要将 SARibbon 库集成到您的 Qt 项目中。最简单的方式是**静态嵌入**，即直接将源文件`SARibbon.h`和`SARibbon.cpp`拷贝到您的工程即可。

## Ribbon界面和传统menubar+toolbar的异同

传统的menubar+toolbar是无法直接转化为ribbon界面的，Ribbon不仅仅是一个带`QToolBar`的工具栏，与传统菜单栏和工具栏相比它有如下特点：

- Ribbon的按钮渲染方式有一个明显的改变，导致无法直接使用ToolButton来模拟，SARibbon使用`SARibbonToolButton`针对Ribbon的按钮进行了重新布局和渲染
- Ribbon还有一种叫上下文标签页`Context Category`的特殊标签页，例如，在Office Word中选中一张图片，会自动出现一个“图片编辑”标签页，提供裁剪、旋转等图片专属功能，取消选择后该标签页自动隐藏
- Ribbon界面会带有一些特殊的控件，例如Gallery（word的样式选择就是Gallery控件）

## SARibbon接口命名

`SARibbon`在设计时参考了`MFC Ribbon`接口的命名风格，标签页称之为`Category`(种类)，每个`Category`下面有多个`panel`（面板），面板下面管理着toolbutton，`panel`有点类似传统的`Toolbar`，其层次结构如下图所示，这些命名参考了MFC的ribbon界面类

![msdn ribbon 介绍页](../../assets/pic/msdn-ribbon.png)
