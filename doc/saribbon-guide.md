# SARibbon的布局

在介绍如何使用`SARibbon`之前，需要首先了解`SARibbon`是如何布局的，在`src/SARibbonBar/doc`下有个ppt对Ribbon的布局进行了介绍，本文内容与ppt一直，增加了一些文字说明。

由于`SARibbon`支持四种布局模式，这四种布局模式定义在SARibbonBar的枚举`SARibbonBar::RibbonStyle`,由于布局模式会影响显示效果和实际函数调用的方法，因此有必要先对`SARibbon`支持的这四种布局模式进行介绍。

## SARibbon名词定义

无论是后面介绍的wps模式还是office模式，这些专有名称都是一致的，这些命名参考了MFC的ribbon界面类

![msdn ribbon 介绍页](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/src/SARibbonBar/doc/msdn-ribbon.png)

- **Category 类别**，代表一个标签所呈现的内容，对应`SARibbonCategory`
- **Context Category 上下文类别**，这个是一种特殊的类别，它正常不显示，需要基于上下文判断是否应该显示，最常用的就是word中插入图片后，会有图片修改相关的标签出现，如果没选中图片，这个标签就消失，这个就是上下文类别，对应`SARibbonContextCategory`
- **Pannel 面板**，这个是一组菜单的集合，office里面板都会有个面板标题，显示在面板最下方，面板之后就是工具栏按钮（Action）
- **Application Button 应用按钮**，标签栏最左边的按钮（word就是对应文件按钮），这个按钮会触发一些特殊的页面或菜单，对应`SARibbonApplicationButton`,可以隐藏
- **Quick Access Bar 快速响应栏**，位于最顶部的一个简单工具栏，用于放置一些常用的action，对应`SARibbonQuickAccessBar`
- **Gallery 预览控件**,这是Ribbon最吸引眼球的控件，用直观的图像把功能显示出来，甚至有些会根据上下文进行实时渲染，典型的就是word开始标签下的样式选择，对应`SARibbonGallery`

## Office布局模式——SARibbonBar::OfficeStyle

此模式和Office的Ribbon样式一致，office的word界面截图如下

![office 界面截图](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/src/SARibbonBar/doc/office-screenshot.png)

`SARibbon OfficeStyle`的布局如下图所示：

![OfficeStyle 布局](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/src/SARibbonBar/doc/ribbonbar-geometry-office3.png)

这个布局和office的默认布局是一致的

## WPS布局模式——SARibbonBar::WpsLiteStyle

此模式和Office的Ribbon样式有区别，参考了WPS的ribbon界面做法，把office ribbon较占空间的标题栏进行利用，实现了界面的最大利用，这个模式下可以降低一定的高度

![WpsLiteStyle 布局](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/src/SARibbonBar/doc/ribbonbar-geometry-wps3.png)

## 行数

在高版本的wps中，为了进一步缩减ribbon bar的高度，把ribbon默认的3个按钮改为了2个，这样使得ribbon bar进一步缩减，对应action不多的category显得会更饱满

