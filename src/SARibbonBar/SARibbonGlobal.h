﻿#ifndef SARIBBONGLOBAL_H
#define SARIBBONGLOBAL_H
#include <qglobal.h>
#include <memory>

/**
  @file  SARibbonGlobal.h

  @note My native language is not English, and most of the translation of documents is machine translation

  版本记录(change log):
  - 2023-12-29 -> 1.1.2
  合并了lixinchang的修改(https://gitee.com/lixinchang)
  添加了SARibbonControlToolButton，ControlBar不再使用QToolButton，避免和QToolButton的qss冲突
  ControlBar支持按钮菜单样式的指定
  调整了默认样式的内容

  - 2023-12-25 -> 1.1.1
  修正了SARibbonBar的布局过程对窗体隐藏的判断方式，使得在窗口没显示时也能正确布局

  - 2023-12-25 -> 1.1.0
  修正了尺寸刷新的问题，在首次显示不会出现控件跳动的状态
  修正了一些问题
  调整了创建RibbonButton的方式
  调整了SARibbonPannel一些接口，使得创建更加规范
  调整了ToolButton的渲染方式

  ------------------------

  - 2023-11-19 -> 1.0.6
  添加Office2016主题
  Category可以居中对齐
  QuickAccessBar的图标调小一像素，且用户可设置

  - 2023-11-10 -> 1.0.5
  自动根据Qt版本来适配是否加载frameless库

  - 2023-11-02 -> 1.0.4

  cn:因为引入了framelss库，导致很多版本的qt无法编译，为了兼容不同版本的qt编译问题，进行了版本自适应，
  不符合framelss的qt版本自动使用原来的framelss方案，从而实现了qt5.9到qt6的完全支持
  en:Because of the introduction of the framelss library, many versions of qt cannot be compiled.
  In order to be compatible with different versions of qt compilation, version adaptation is carried out.
  The qt version that does not conform to the framelss automatically uses the original framelss scheme, thus realizing full support for qt5.9 to qt6

  - 2023-10-26 -> 1.0.2

  cn:
  添加了SARibbonBar在QWidget窗口上使用的例子
  整理了SARibbon.h和SARibbon.cpp,调整了静态使用的例子
  frameless库更新到c4a7bc8版本（20231022）

  - 2023-10-14 -> 1.0.1

  cn:
  引入第三方库frameless
  支持ubuntu和macos
  解决windwos系统的多屏幕移动问题
  依赖frameless，实现windows系统的一些界面操作，如边缘触发半屏
  添加深色主题
  优化了一些界面刷新逻辑
  最低C++版本要求为c++17
  一些接口的调整和一些枚举名称的调整
  添加了QWidget使用SARibbonBar的例子
  同步把自定义ribbon的SARibbonMainWindow的依赖去除
  完善了文档包括高分屏问题和linux编译的方法

  en(machine translation):

  import third-party library:frameless
  Supports ubuntu and macos
  Solving the problem of multi screen movement in Windwos system
  Relying on frameless library to implement some interface operations in Windows systems, such as edge triggered half screen
  Add a dark theme
  Optimized some interface refresh logic
  The minimum C++version requirement is c++17
  Some interface adjustments and some enumeration name adjustments
  Added an example of using SARibbonBar for QWidgets
  Synchronize the removal of dependencies on SARibbonMainWindow for custom ribbons
  Improved documentation including high resolution issues and methods for compiling Linux

  - 2023-10-09 -> 0.7.1

  增加了深色主题
  调整了枚举的命名方式
  增加了一些重绘方式

  - 2023-10-09 -> 0.7.0

  修正了category的布局问题

  - 2023-09-27 -> 0.6.0

  添加了Amalgamate，修正了一些显示的bug，修正cmake的异常
  添加了上下文标签中category标题名字改变的信号

  - 2023-05-28 -> 0.5.0

  调整了大按钮模式下的显示方案，去除了原来SARibbonToolButton的Lite和Normal模式，以WordWrap来表征
  支持文字自定义换行
  调整了RibbonPannel的标题栏的高度计算方案

  - 0.5.1

  不使用QString::simplified,而是简单的仅仅替换\n的simplified，这样中文换行不会多出空格

  - 0.5.2

  SARibbonColorToolButton\SARibbonToolButton修正&操作在三项表达式未加括号问题
  SARibbonStyleOption添加虚析构函数
  原来SARibbonElementCreateDelegate类改名为SARibbonElementFactory
 */

/**
 * @def ribbon的数字版本 {MAJ}.MIN.PAT
 */
#ifndef SA_RIBBON_BAR_VERSION_MAJ
#define SA_RIBBON_BAR_VERSION_MAJ 1
#endif
/**
 * @def ribbon的数字版本 MAJ.{MIN}.PAT
 */
#ifndef SA_RIBBON_BAR_VERSION_MIN
#define SA_RIBBON_BAR_VERSION_MIN 1
#endif
/**
 * @def ribbon的数字版本 MAJ.MIN.{PAT}
 */
#ifndef SA_RIBBON_BAR_VERSION_PAT
#define SA_RIBBON_BAR_VERSION_PAT 2
#endif

#ifndef SA_RIBBON_BAR_NO_EXPORT
#if defined(SA_RIBBON_BAR_MAKE_LIB)  // 定义此宏将构建library
#define SA_RIBBON_EXPORT Q_DECL_EXPORT
#else
#define SA_RIBBON_EXPORT Q_DECL_IMPORT
#endif
#endif

#ifndef SA_RIBBON_EXPORT
#define SA_RIBBON_EXPORT
#endif

/**
 * @def   模仿Q_DECLARE_PRIVATE，但不用前置声明而是作为一个内部类
 */
#ifndef SA_RIBBON_DECLARE_PRIVATE
#define SA_RIBBON_DECLARE_PRIVATE(classname)                                                                           \
    class PrivateData;                                                                                                 \
    friend class classname::PrivateData;                                                                               \
    std::unique_ptr< PrivateData > d_ptr;
#endif
/**
 * @def   模仿Q_DECLARE_PUBLIC
 */
#ifndef SA_RIBBON_DECLARE_PUBLIC
#define SA_RIBBON_DECLARE_PUBLIC(classname)                                                                            \
    friend class classname;                                                                                            \
    classname* q_ptr { nullptr };                                                                                      \
    PrivateData(const PrivateData&)            = delete;                                                               \
    PrivateData& operator=(const PrivateData&) = delete;
#endif

/**
   @brief 定义Ribbon的对其方式，目前支持左对齐和居中对其
 */
enum class SARibbonAlignment
{
    AlignLeft,   ///< 左对齐，tab栏左对齐，同时category也是左对齐
    AlignCenter  ///< 居中对其，tab栏居中对齐，同时category也是居中对齐
};

/**
 * @def 属性，用于标记是否可以进行自定义，用于动态设置到@ref SARibbonCategory 和@ref SARibbonPannel
 * 值为bool，在为true时，可以通过@ref SARibbonCustomizeWidget 改变这个SARibbonCategory和SARibbonPannel的布局，
 * 默认不会有此属性，仅在有此属性且为true时才会在SARibbonCustomizeWidget中能显示为可设置
 */
#ifndef SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE
#define SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE "_sa_isCanCustomize"
#endif

/**
 * @def 定义此宏用第三方的frameless作为无边框方案
 * 此宏在qmake或在cmake中定义，不需要在此显示定义
 */
// #define SARIBBON_USE_3RDPARTY_FRAMELESSHELPER 0

#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
#ifndef SA_FONTMETRICS_WIDTH
#define SA_FONTMETRICS_WIDTH(fm, str) fm.horizontalAdvance(str)
#endif
#else
#ifndef SA_FONTMETRICS_WIDTH
#define SA_FONTMETRICS_WIDTH(fm, str) fm.width(str)
#endif
#endif

#ifndef SA_DEBUG_PRINT_SIZE_HINT
/**
  @def 定义此宏，将打印和尺寸刷新相关的信息

    仅用于调试
 */
#define SA_DEBUG_PRINT_SIZE_HINT 0
#endif

#ifndef SA_DEBUG_PRINT_EVENT
/**
  @def 定义此宏，将打印事件

    仅用于调试
 */
#define SA_DEBUG_PRINT_EVENT 0
#endif
#endif  // SARIBBONGLOBAL_H
