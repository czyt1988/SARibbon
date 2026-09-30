#ifndef SARIBBONENUMS_H
#define SARIBBONENUMS_H
#include <QMetaType>
#include <QFlags>
// 计划 02 S1：公共枚举与属性名常量下沉 core（原 src/widgets/SARibbonGlobal.h 与 SARibbonPanelItem.h 的内容，
// 值序与字符串值一字不改）。三个自由枚举维持全局命名空间（2.x 用户零改名）；
// core 新增类型名放 namespace SARibbon::Core（v2 §3.5）。

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

namespace SARibbon
{
namespace Core
{
// 提升自 SARibbonPanelItem::RowProportion（SARibbonPanelItem.h L36-42），保持 unscoped：
// SARibbonCustomizeWidget.cpp 依赖枚举到 int 的隐式转换；放命名空间避免 None/Large 泄漏到全局（X11 None 宏）。
// widgets 侧 SARibbonPanelItem 以类型别名 + static constexpr 成员保持 SARibbonPanelItem::Large 等拼写兼容（NOTES B21）。
enum SARibbonRowProportion
{
    None,   ///< Undefined proportion, at this time it will be judged based on expandingDirections
    Large,  ///< Large proportion, the height of a widget will fill the entire panel
    Medium, ///< Medium proportion, only works in ThreeRowMode
    Small   ///< Small proportion, occupies one row of SARibbonPanel
};
}  // namespace Core
}  // namespace SARibbon

#ifndef SA_ActionPropertyName_RowProportion
#define SA_ActionPropertyName_RowProportion "_sa_RowProportion"
#endif
#ifndef SA_ActionPropertyName_ToolButtonPopupMode
#define SA_ActionPropertyName_ToolButtonPopupMode "_sa_ToolButtonPopupMode"
#endif
#ifndef SA_ActionPropertyName_ToolButtonStyle
#define SA_ActionPropertyName_ToolButtonStyle "_sa_ToolButtonStyle"
#endif

#endif  // SARIBBONENUMS_H
