# SARibbon的布局

> `SARibbonBar`是Qt的开源Ribbon控件，代码托管于[github](https://github.com/czyt1988/SARibbon)和[gitee](https://gitee.com/czyt1988/SARibbon)
> - `SARibbon`介绍[[github]](https://github.com/czyt1988/SARibbon/blob/master/readme.md),[[gitee]](https://gitee.com/czyt1988/SARibbon/blob/master/readme.md)
> - `SARibbon`布局入门[[github]](https://github.com/czyt1988/SARibbon/blob/master/doc/saribbon-guide.md),[[gitee]](https://gitee.com/czyt1988/SARibbon/blob/master/doc/saribbon-guide.md)

---

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

此模式和Office的Ribbon样式有区别，参考了WPS的ribbon界面做法，把office ribbon较占空间的标题栏进行利用，实现了界面的最大利用，这个模式下可以减少一个标题栏的高度，WPS模式截图如下：

这个图时WPS对ribbon的第一次更改，wps对ribbon的第二次改动，在`行数`这一节会有介绍

![Wps 改动了ribbon的布局方式，第1次改动](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/src/SARibbonBar/doc/wps-change-ribbon-1.jpg)

wps模式布局：

![WpsLiteStyle 布局](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/src/SARibbonBar/doc/ribbonbar-geometry-wps3.png)



## pannel的布局

在标准的pannel中，一个action（按钮）有3种布局，以office word为例，pannel的三种布局其实是所占行数:

- 第一种，占满整个pannel，只有一行，称之为`large`
- 第二种，一个pannel下放置两行，称之为`medium`
- 第三种，一个pannel放置3行内容，称之为`samll`



![word pannel 布局示例](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/src/SARibbonBar/doc/pannelLayout3row-example.png)

因此，pannel的布局其实归根结底就是行数，SARibbon一开始是用`QGridLayout`来对pannel进行布局的，把grid分割为6行，`large`模式下占全部6行，`medium`模式下占据3行，`small`模式下占据2行，后续参考QToolBar的源码，重新实现了pannel的自定义布局。


### 行数

上一节讲到了pannel的布局，pannel布局一个很关键的点就是`行`的概念，`行`的不同定义，会影响整个显示效果。
在高版本的wps中，为了进一步缩减ribbon bar的高度，把ribbon的3行按钮改为了2个，这样使得ribbon bar进一步缩减，对应action不多的category显得会更饱满，相当于只有medium，没有small模式。这是WPS对ribbon的第二次布局更改，新版WPS的截图如下：

![Wps 改动了ribbon的布局方式，第2次改动](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/src/SARibbonBar/doc/wps-change-ribbon-2.png)

通过这个改动，机智挖掘了ribbon的高度，`SARibbon`支持这几种ribbon样式，引入了行数的概念，在`SARibbonBar::RibbonStyle`的枚举中带`TwoRow`结尾的都是指代2行模式

```cpp
/**
* @brief 定义ribbon的风格,第一字节代表样式，第二字节代表是否是2行
*/
enum RibbonStyle {
    OfficeStyle		= 0x0000        ///< 类似office 的ribbon风格
    , WpsLiteStyle		= 0x0001        ///< 类似wps的紧凑风格
    , OfficeStyleTwoRow	= 0x0100        ///< 类似office 的ribbon风格 2行工具栏 三行布局模式，office就是三行布局模式，pannel能布置3行小toolbutton，默认模式
    , WpsLiteStyleTwoRow	= 0x0101        ///< 类似wps的紧凑风格  2行工具栏
};
```

枚举`SARibbonPannelItem::RowProportion`是为了表征每个窗体在pannel所占行数的情况，在pannel布局中会常用到，这个枚举定义如下：

```cpp
/**
    * @brief 定义了行的占比，ribbon中有large，media和small三种占比
    */
enum RowProportion {
    None            ///< 为定义占比，这时候将会依据expandingDirections来判断，如果能有Qt::Vertical，就等同于Large，否则就是Small
    , Large         ///< 大占比，一个widget的高度会充满整个pannel
    , Medium        ///< 中占比，在@ref SARibbonPannel::pannelLayoutMode 为 @ref SARibbonPannel::ThreeRowMode 时才会起作用，且要同一列里两个都是Medium时，会在三行中占据两行
    , Small         ///< 小占比，占SARibbonPannel的一行，Medium在不满足条件时也会变为Small，但不会变为Large
};
```

SARibbonPannel里管理的每个action都会带有一个占位的属性（`SARibbonPannelItem::RowProportion`），这个占位属性决定了这个action在pannel里的布局。

下面针对不同行数进行详细介绍。

### 3行模式

三行模式是传统的pannel布局方式，如下图所示：

![3行模式ribbon布局示例](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/src/SARibbonBar/doc/pannelLayout3row.png)

3行模式下有三种占位(`SARibbonPannelItem::RowProportion`)，分别为large、medium和small

- large大占比，一个widget的高度会充满整个pannel
- medium中占比，pannel里一列放置2个窗体，前提是这一列2个都是medium，否则会显示异常（暂时还未做medium条件降级到small的处理，后续会实现）
- small小占比，pannel里一列放置3个窗体

3行模式下的pannel会显示pannel的标题在`Pannel Title`区域，另外还有一个`OptionAction`的区域，这个是给这个action添加特殊触发使用的，如果没有设置`OptionAction`，这个区域是隐藏。

三行模式最经典的布局就是word：

![word pannel 布局示例](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/src/SARibbonBar/doc/pannelLayout3row-example.png)

### 2行模式

两行模式是传统的WPS的改进布局法（具体是否是WPS首先这样做的不清楚，我是按照WPS的布局进行参考的），如下图所示：

![2行模式ribbon布局示例](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/src/SARibbonBar/doc/pannelLayout2row.png)

2行模式下medium和small占位(`SARibbonPannelItem::RowProportion`)是一样的，不做区分。

另外两行模式下pannel是不显示标题的。

2行模式是按照WPS的2020进行参考编写的，WPS2020的截图如下：

![WPS pannel 2行ribbon布局示例](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/src/SARibbonBar/doc/pannelLayout2row-example.png)

