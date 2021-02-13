# 简介

 这是Qt（最低要求Qt5，支持C++11的编译器）下一个轻量级的Ribbon控件(Office样式UI),界面截图：

![](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/doc/screenshot/001.gif)

MIT协议，欢迎大家使用并提出意见

[gitee(码云) - https://gitee.com/czyt1988/SARibbon](https://gitee.com/czyt1988/SARibbon)

[github - https://github.com/czyt1988/SARibbon](https://github.com/czyt1988/SARibbon)

 它支持4种目前常见的ribbon样式在线切换

 包括2种office模式，office模式是最常见的ribbon模式了，就是我们经常看到的word模式，office模式的tab和标题栏占用位置较多。

![](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/doc/screenshot/office-mode.png)

 另两种参考wps设计的wps模式，wps模式是office模式的改良版，它为了减小ribbon的高度，把标签和标题栏设置在一起
 
![](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/doc/screenshot/wps-mode.png)

 office模式和wps模式都支持两行和3行设计，满足不同界面需求。

# 编译方法

 使用Qt Creator直接打开SARibbonBar.pro，并编译即可，会编译出SARibbonBar库和一个（目前只有一个例子）例子，lib和例子的目录位于bin目录下

# 使用方法

可以把编译好的整个目录移动到需要的工程目录下，然后在项目的pro文件中,`include SARibbonBar.pri`即可，示例如下：

```shell
include($$PWD/SARibbonBar/SARibbon/SARibbonBar.pri)
```


# 更多截图

![](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/doc/screenshot/SARibbonBar-screenshot-01.gif)

- 支持最小化模式，ribbon只显示标签（默认双击标签会进行切换）

![](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/doc/screenshot/SARibbonBar-screenshot-minMode.gif)

- 支持quickAccessBar（word快速菜单），在wps模式和office模式下会有不同的显示效果

![](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/doc/screenshot/SARibbonBar-screenshot-quickAccessBar.gif)

- 支持4种不同的ribbon button，普通按钮，延迟弹出菜单按钮，菜单按钮，action菜单按钮（action菜单按钮是此ribbon控件最主要解决的问题之一）

![](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/doc/screenshot/SARibbonBar-screenshot-ribbonbutton.gif)

- 支持qss对ribbon进行设置

![](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/doc/screenshot/SARibbonBar-screenshot-useqss.gif)

- 这是使用此控件的本人另外一个开源软件的截图

![](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/doc/screenshot/sa-screenshot.gif)

[github - https://github.com/czyt1988/sa](https://github.com/czyt1988/sa)

[gitee - https://gitee.com/czyt1988/SA](https://gitee.com/czyt1988/SA)

# 题外

这个Ribbon主要靠`QSS`实现而不是重写样式(`QStyle`)，主要是重写样式工作量巨大，一些`QSS`无法实现的，重写了界面，如RibbonToolButton，

目前基本的布局和ToolButton功能以及完成，简单的RibbonGallery控件也有初步实现，后续还需对Gallery进行完善

更多界面截图可看：

[github - https://github.com/czyt1988/sa](https://github.com/czyt1988/sa)

[gitee - https://gitee.com/czyt1988/SA](https://gitee.com/czyt1988/SA)

SARibbon项目是SA项目的子项

具体Ribbon的生成代码可见：
[https://github.com/czyt1988/sa/blob/master/src/signA/MainWindowPrivate.cpp](https://github.com/czyt1988/sa/blob/master/src/signA/MainWindowPrivate.cpp)

这个ribbon主要通过QTabbar和QStackWidget实现，按照微软MFCRibbbon的接口命名方式来写，主要通过qss来控制，默认qss在资源文件中，理论上能满足各种样式，可以实现样式的切换(换肤)

若有空会把这个`SARibbon`和`SA`一起打包讲讲如何开发，其中遇到的一些坑啊和注意的问题。

# 计划及进度

## 计划

- 添加自定义窗口，支撑自定义，并能生成xml/json等配置文件
- context category 的背景添加一个底层窗口，使得其支撑自定义
- 可以设置title的背景，类似office未注册时的显示

## 已知bug

- 在ubuntu下会有文字换行显示不全问题

## 已解决

- 【解决】在缩小模式下tabbar无法响应鼠标经过，导致鼠标经过无法绘制
        由于stacked widget在缩小模式下时popup，弹出后即阻断了消息，需要主动给tabbar发送一个hover事件，高速tabbar鼠标已经移开
- 【解决】pannel在内容过多时，需要针对性的重新计算整个Category，以便得到最优的布局尺寸
- 【解决】pannel布局过于紧凑
- 【解决】小按钮的位置过于紧凑
- 【解决】分割线显示异常
- 【解决】在非最大化模式，pannel变为缩小时，点击不弹出stacked

# 其他

> 感谢[FastCAE](http://www.fastcae.com/product.html)项目使用了本控件，并找到了许多bug和建议，FastCAE国产CAE软件集成开发平台，免费开源，是面向求解器开发者提供CAE软件界面与通用功能模块快速研发集成框架，[其开源仓库（github）:https://github.com/DISOGitHub/FastCAE](https://github.com/DISOGitHub/FastCAE)，[gitee:https://gitee.com/DISOGitee/FastCAE](https://gitee.com/DISOGitee/FastCAE)，[官网见:http://www.fastcae.com/product.html](http://www.fastcae.com/product.html)