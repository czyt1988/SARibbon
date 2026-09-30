#ifndef SARIBBONGLOBAL_H
#define SARIBBONGLOBAL_H
// 3.0 兼容转发头：原内容拆分至 SARibbonCore/SARibbonCoreGlobal.h（PIMPL/导出宏基座）
// 与 SARibbonWidgetsGlobal.h（widgets 导出宏），本文件保留以兼容既有 include。
#include <SARibbonCore/SARibbonCoreGlobal.h>
#include "SARibbonWidgetsGlobal.h"
#include "SARibbonBarVersionInfo.h"   // 原 Global.h:6 行为保持：版本宏随全局头可见
class QWidget;                        // 原 Global.h:7 前置声明保留（widgets 侧需要）

// ==== 三个枚举（SARibbonAlignment/SARibbonTheme/SARibbonMainWindowStyleFlag，
//      含注释与 Q_DECLARE_FLAGS/Q_DECLARE_OPERATORS_FOR_FLAGS）与
//      SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE 原样保留在本文件，计划 02 再下沉 core ====

/**
 * \if ENGLISH
 * @brief Define the alignment mode of Ribbon, supports left alignment, center alignment and right alignment
 * @note If your compiler reports: the qualified name of the member declaration is illegal, then check if the file line
 * break is LF, if so, change the file line break to CRLF \endif
 *
 * \if CHINESE
 * @brief 定义 Ribbon 的对其方式，支持左对齐、居中对其和右对齐
 * @note 如果你编译器提示：成员声明的限定名称非法，那么留意一下文件换行是否为 LF，如果是把文件换行改为 CRLF
 * \endif
 */
enum class SARibbonAlignment
{
    AlignLeft,    ///< Left alignment, tab bar left aligned, category also left aligned
    AlignCenter,  ///< Center alignment, tab bar center aligned, category also center aligned
    AlignRight    ///< Right alignment, tab bar right aligned, category also right aligned
};
Q_DECLARE_METATYPE(SARibbonAlignment)

/**
 * \if ENGLISH
 * @brief Ribbon theme
 * @note Some QSS sizes cannot be obtained in C++ code, so for user-defined QSS themes, some sizes need to be set manually
 * @note For example, ribbon tab margin information cannot be obtained from QTabBar, which affects the drawing of SARibbonContextCategory
 * @note Therefore, after setting QSS, you need to reset the margin information into SARibbonTabBar
 * \endif
 *
 * \if CHINESE
 * @brief ribbon主题
 * @note 由于有些qss的尺寸，在C++代码中无法获取到，因此针对用户自定义的qss主题，有些尺寸是需要手动设置进去的
 * @note 例如ribbon tab的margin信息，在QTabBar是无法获取到，而这个影响了SARibbonContextCategory的绘制，
 * @note 因此，在设置qss后需要针对margin信息重新设置进SARibbonTabBar中
 * \endif
 */
enum class SARibbonTheme
{
    RibbonThemeWindows7 = 0,     ///< Windows 7 theme
    RibbonThemeOffice2013,       ///< Office 2013 theme
    RibbonThemeOffice2016Blue,   ///< Office 2016 - Blue theme
    RibbonThemeOffice2016Green,  ///< Office 2016 - Green theme
    RibbonThemeOffice2016Dark,   ///< Office 2016 - Dark theme
    RibbonThemeOffice2021Blue,   ///< Office 2021 - Blue theme
    RibbonThemeOffice2021Green,  ///< Office 2021 - Green theme
    RibbonThemeOffice2021Dark,   ///< Office 2021 - Dark theme
    RibbonThemeDark,             ///< Dark theme
    RibbonThemeDark2,            ///< Dark theme 2
    RibbonThemeUserDefine = 1000
};
Q_DECLARE_METATYPE(SARibbonTheme)

/**
 * \if ENGLISH
 * @brief RibbonMainWindow style
 * \endif
 *
 * \if CHINESE
 * @brief RibbonMainWindow的样式
 * \endif
 */
enum class SARibbonMainWindowStyleFlag : int
{
    UseRibbonFrame   = 1,  ///< Use ribbon frame, which is more compact
    UseNativeFrame   = 2,  ///< Use operating system frame
    UseRibbonMenuBar = 4,  ///< Use ribbon menu bar
    UseNativeMenuBar = 8   ///< Use native menu bar
};
Q_DECLARE_FLAGS(SARibbonMainWindowStyles, SARibbonMainWindowStyleFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(SARibbonMainWindowStyles)

/**
 * \if ENGLISH
 * @def SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE
 * @brief Property used to mark if customization is allowed, dynamically set to @ref SARibbonCategory and @ref SARibbonPanel
 * @details Value is bool, when true, the layout of SARibbonCategory and SARibbonPanel can be changed through @ref SARibbonCustomizeWidget
 * @details By default, this property does not exist, only when this property exists and is true will it be displayed as configurable in SARibbonCustomizeWidget
 * \endif
 *
 * \if CHINESE
 * @def SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE
 * @brief 属性，用于标记是否可以进行自定义，用于动态设置到@ref SARibbonCategory 和@ref SARibbonPanel
 * @details 值为bool，在为true时，可以通过@ref SARibbonCustomizeWidget 改变这个SARibbonCategory和SARibbonPanel的布局，
 * @details 默认不会有此属性，仅在有此属性且为true时才会在SARibbonCustomizeWidget中能显示为可设置
 * \endif
 */
#ifndef SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE
#define SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE "_sa_isCanCustomize"
#endif

#endif  // SARIBBONGLOBAL_H
