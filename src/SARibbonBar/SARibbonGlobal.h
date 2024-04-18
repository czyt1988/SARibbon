#ifndef SARIBBONGLOBAL_H
#define SARIBBONGLOBAL_H
#include <memory>
#include <QtGlobal>
#include <QObject>

/**
  @file  SARibbonGlobal.h
 */

/**
 * @def ribbon的数字版本 {MAJ}.MIN.PAT
 */
#ifndef SA_RIBBON_BAR_VERSION_MAJ
#define SA_RIBBON_BAR_VERSION_MAJ 2
#endif
/**
 * @def ribbon的数字版本 MAJ.{MIN}.PAT
 */
#ifndef SA_RIBBON_BAR_VERSION_MIN
#define SA_RIBBON_BAR_VERSION_MIN 0
#endif
/**
 * @def ribbon的数字版本 MAJ.MIN.{PAT}
 */
#ifndef SA_RIBBON_BAR_VERSION_PAT
#define SA_RIBBON_BAR_VERSION_PAT 4
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
 * @brief 定义Ribbon的对其方式，目前支持左对齐和居中对其
 */
enum class SARibbonAlignment
{
	AlignLeft,   ///< 左对齐，tab栏左对齐，同时category也是左对齐
	AlignCenter  ///< 居中对其，tab栏居中对齐，同时category也是居中对齐
};

/**
 * @brief ribbon主题
 * 
 * 注意，由于有些qss的尺寸，在C++代码中无法获取到，因此针对用户自定义的qss主题，有些尺寸是需要手动设置进去的
 *
 * 例如ribbon tab的margin信息，在QTabBar是无法获取到，而这个影响了SARibbonContextCategory的绘制，
 * 因此，在设置qss后需要针对margin信息重新设置进SARibbonTabBar中
 */
enum class SARibbonTheme
{
	RibbonThemeOffice2013,      ///< office2013主题
	RibbonThemeOffice2016Blue,  ///< office2016-蓝色主题
	RibbonThemeOffice2021Blue,  ///< office2021-蓝色主题
	RibbonThemeWindows7,        ///< win7主题
	RibbonThemeDark,            ///< 暗色主题
	RibbonThemeDark2
};

/**
 * @def 属性，用于标记是否可以进行自定义，用于动态设置到@ref SARibbonCategory 和@ref SARibbonPannel
 * 值为bool，在为true时，可以通过@ref SARibbonCustomizeWidget 改变这个SARibbonCategory和SARibbonPannel的布局，
 * 默认不会有此属性，仅在有此属性且为true时才会在SARibbonCustomizeWidget中能显示为可设置
 */
#ifndef SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE
#define SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE "_sa_isCanCustomize"
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5, 11, 0))
#ifndef SA_FONTMETRICS_WIDTH
#define SA_FONTMETRICS_WIDTH(fm, str) fm.horizontalAdvance(str)
#endif
#else
#ifndef SA_FONTMETRICS_WIDTH
#define SA_FONTMETRICS_WIDTH(fm, str) fm.width(str)
#endif
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#ifndef SA_MOUSEEVENT_GLOBALPOS_POINT
#define SA_MOUSEEVENT_GLOBALPOS_POINT(MouseEventPtr) MouseEventPtr->globalPosition().toPoint()
#endif
#else
#ifndef SA_MOUSEEVENT_GLOBALPOS_POINT
#define SA_MOUSEEVENT_GLOBALPOS_POINT(MouseEventPtr) MouseEventPtr->globalPos()
#endif
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#ifndef SA_HOVEREVENT_POS_POINT
#define SA_HOVEREVENT_POS_POINT(HoverEventPtr) HoverEventPtr->position().toPoint()
#endif
#else
#ifndef SA_HOVEREVENT_POS_POINT
#define SA_HOVEREVENT_POS_POINT(HoverEventPtr) HoverEventPtr->pos()
#endif
#endif

/**
  @def 定义此宏，将打印和尺寸刷新相关的信息

    仅用于调试
 */
#ifndef SA_DEBUG_PRINT_SIZE_HINT
#define SA_DEBUG_PRINT_SIZE_HINT 0
#endif

/**
  @def 定义此宏，将打印事件

    仅用于调试
 */
#ifndef SA_DEBUG_PRINT_EVENT
#define SA_DEBUG_PRINT_EVENT 0
#endif

/**
  @def 定义此宏，qDebug将支持SARibbonBar的属性打印

    仅用于调试
 */
#ifndef SA_DEBUG_PRINT_SARIBBONBAR
#define SA_DEBUG_PRINT_SARIBBONBAR 1
#endif

#endif  // SARIBBONGLOBAL_H
