# 简介

 这是Qt下一个轻量级的Ribbon控件(Office样式UI),界面截图：

![](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/doc/screenshot/001.gif)

MIT协议，欢迎大家使用并提出意见

[gitee(码云) - https://gitee.com/czyt1988/SARibbon](https://gitee.com/czyt1988/SARibbon)

[github - https://github.com/czyt1988/SARibbon](https://github.com/czyt1988/SARibbon)

# 编译方法

 使用Qt Creator直接打开SARibbonBar.pro，并编译即可，会编译出SARibbonBar库和一个（目前只有一个例子）例子，lib和例子的目录位于bin目录下

# 使用方法

可以把编译好的整个目录移动到需要的工程目录下，然后在项目的pro文件中,`include SARibbonBar.pri`即可，示例如下：

```shell
include($$PWD/SARibbonBar/SARibbon/SARibbonBar.pri)
```

使用cmakefile的话自己手撸

# 题外

这个Ribbon主要靠`QSS`实现而不是重写样式(`QStyle`)，主要是重写样式工作量巨大，一些`QSS`无法实现的，重写了界面，如RibbonToolButton，

目前只完成了最基本的一些功能如:Ribbon的整体界面，Ribbon的ToolButton等基本功能，像RibbonGallery这样的功能仍然需要完善

更多界面截图可看：

[github - https://github.com/czyt1988/sa](https://github.com/czyt1988/sa)

[gitee - https://gitee.com/czyt1988/SA](https://gitee.com/czyt1988/SA)

SARibbon项目是SA项目的子项

具体Ribbon的生成代码可见：
[https://github.com/czyt1988/sa/blob/master/src/signA/MainWindowPrivate.cpp](https://github.com/czyt1988/sa/blob/master/src/signA/MainWindowPrivate.cpp)

这个ribbon主要通过QTabbar和QStackWidget实现，按照微软MFCRibbbon的接口命名方式来写，主要通过qss来控制，默认qss在资源文件中，理论上能满足各种样式，可以实现样式的切换(换肤)

若有空会把这个`SARibbon`和`SA`一起打包讲讲如何开发，其中遇到的一些坑啊和注意的问题。


# 更多演示

    office模式是最常见的ribbon模式了，就是我们经常看到的word模式，但office模式有个缺点，就是非常占位置

![](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/doc/screenshot/office-mode.png)

    于是就有wps模式，wps模式是office模式的改良版，具体可看wps的软件，它为了减小ribbon的高度，甚至把pannel的按钮数目默认变为2行

![](https://cdn.jsdelivr.net/gh/czyt1988/SARibbon/doc/screenshot/wps-mode.png)
