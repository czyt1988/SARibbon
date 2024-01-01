# 版本记录(change log):
      
## 2024-01-01 -> 2.0.0

- SARibbonPannel的文字使用SARibbonPannelLabel(QLabel)显示，可以通过qss进行自定义
- SARibbonBar的样式RibbonStyle改为QFlags
- SARibbonBar添加了相关的迭代函数
- 尺寸的计算重新进行了调整

由于接口函数有明显调整，因此版本变更为2.0

## 2023-12-29 -> 1.1.2

- 合并了lixinchang的修改(https://gitee.com/lixinchang)
- 添加了SARibbonControlToolButton，ControlBar不再使用QToolButton，避免和QToolButton的qss冲突
- ControlBar支持按钮菜单样式的指定
- 调整了默认样式的内容

## 2023-12-25 -> 1.1.1

- 修正了SARibbonBar的布局过程对窗体隐藏的判断方式，使得在窗口没显示时也能正确布局

## 2023-12-25 -> 1.1.0

- 修正了尺寸刷新的问题，在首次显示不会出现控件跳动的状态
- 修正了一些问题
- 调整了创建RibbonButton的方式
- 调整了SARibbonPannel一些接口，使得创建更加规范
- 调整了ToolButton的渲染方式

  ------------------------

## 2023-11-19 -> 1.0.6

- 添加Office2016主题
- Category可以居中对齐
- QuickAccessBar的图标调小一像素，且用户可设置

## 2023-11-10 -> 1.0.5

- 自动根据Qt版本来适配是否加载frameless库

## 2023-11-02 -> 1.0.4

cn:因为引入了framelss库，导致很多版本的qt无法编译，为了兼容不同版本的qt编译问题，进行了版本自适应，
不符合framelss的qt版本自动使用原来的framelss方案，从而实现了qt5.9到qt6的完全支持

en:Because of the introduction of the framelss library, many versions of qt cannot be compiled.
In order to be compatible with different versions of qt compilation, version adaptation is carried out.
The qt version that does not conform to the framelss automatically uses the original framelss scheme, thus realizing full support for qt5.9 to qt6

## 2023-10-26 -> 1.0.2

- 添加了SARibbonBar在QWidget窗口上使用的例子
- 整理了SARibbon.h和SARibbon.cpp,调整了静态使用的例子
- frameless库更新到c4a7bc8版本（20231022）

## 2023-10-14 -> 1.0.1

cn:
- 引入第三方库frameless
- 支持ubuntu和macos
- 解决windwos系统的多屏幕移动问题
- 依赖frameless，实现windows系统的一些界面操作，如边缘触发半屏
- 添加深色主题
- 优化了一些界面刷新逻辑
- 最低C++版本要求为c++17
- 一些接口的调整和一些枚举名称的调整
- 添加了QWidget使用SARibbonBar的例子
- 同步把自定义ribbon的SARibbonMainWindow的依赖去除
- 完善了文档包括高分屏问题和linux编译的方法

en(machine translation):
- import third-party library:frameless
- Supports ubuntu and macos
- Solving the problem of multi screen movement in Windwos system
- Relying on frameless library to implement some interface operations in Windows systems, such as edge triggered half screen
- Add a dark theme
- Optimized some interface refresh logic
- The minimum C++version requirement is c++17
- Some interface adjustments and some enumeration name adjustments
- Added an example of using SARibbonBar for QWidgets
- Synchronize the removal of dependencies on SARibbonMainWindow for custom ribbons
- Improved documentation including high resolution issues and methods for compiling Linux

## 2023-10-09 -> 0.7.1

- 增加了深色主题
- 调整了枚举的命名方式
- 增加了一些重绘方式

## 2023-10-09 -> 0.7.0

- 修正了category的布局问题

## 2023-09-27 -> 0.6.0

- 添加了Amalgamate，修正了一些显示的bug，修正cmake的异常
- 添加了上下文标签中category标题名字改变的信号

## 2023-05-28 -> 0.5.0

- 调整了大按钮模式下的显示方案，去除了原来SARibbonToolButton的Lite和Normal模式，以WordWrap来表征
- 支持文字自定义换行
- 调整了RibbonPannel的标题栏的高度计算方案

## 0.5.1

- 不使用QString::simplified,而是简单的仅仅替换\n的simplified，这样中文换行不会多出空格

## 0.5.2

- SARibbonColorToolButton\SARibbonToolButton修正&操作在三项表达式未加括号问题
- SARibbonStyleOption添加虚析构函数
- 原来SARibbonElementCreateDelegate类改名为SARibbonElementFactory