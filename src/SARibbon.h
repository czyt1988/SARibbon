#ifndef SA_RIBBON_H
#define SA_RIBBON_H
//定义此宏，将SA_RIBBON_EXPORT定义为空
#ifndef SA_RIBBON_BAR_NO_EXPORT
#define SA_RIBBON_BAR_NO_EXPORT
#endif
//定义此宏，将SA_COLOR_WIDGETS_API定义为空
#ifndef SA_COLOR_WIDGETS_NO_DLL
#define SA_COLOR_WIDGETS_NO_DLL
#endif


/*** Start of inlined file: SARibbonAmalgamTemplatePublicHeaders.h ***/
// Global

/*** Start of inlined file: SAColorWidgetsGlobal.h ***/
#ifndef SACOLORWIDGETSGLOBAL_H
#define SACOLORWIDGETSGLOBAL_H
#include <QScopedPointer>

/**
 * \if ENGLISH
 * @def color-widgets version MAJ.MIN.PAT
 * \endif
 *
 * \if CHINESE
 * @def color-widgets的数字版本 MAJ.MIN.PAT
 * \endif
 */
#ifndef SA_COLOR_WIDGETS_VERSION_MAJ
#define SA_COLOR_WIDGETS_VERSION_MAJ 0
#endif

/**
 * \if ENGLISH
 * @def color-widgets version MAJ.MIN.PAT
 * \endif
 *
 * \if CHINESE
 * @def color-widgets的数字版本 MAJ.MIN.PAT
 * \endif
 */
#ifndef SA_COLOR_WIDGETS_VERSION_MIN
#define SA_COLOR_WIDGETS_VERSION_MIN 1
#endif

/**
 * \if ENGLISH
 * @def color-widgets version MAJ.MIN.PAT
 * \endif
 *
 * \if CHINESE
 * @def color-widgets的数字版本 MAJ.MIN.PAT
 * \endif
 */
#ifndef SA_COLOR_WIDGETS_VERSION_PAT
#define SA_COLOR_WIDGETS_VERSION_PAT 0
#endif

/**
 * \if ENGLISH
 * @def Imitates Q_DECLARE_PRIVATE but uses an internal class instead of forward declaration
 * \endif
 *
 * \if CHINESE
 * @def 模仿Q_DECLARE_PRIVATE，但不用前置声明而是作为一个内部类
 * \endif
 */
#ifndef SA_COLOR_WIDGETS_DECLARE_PRIVATE
#define SA_COLOR_WIDGETS_DECLARE_PRIVATE(classname)                                                                    \
	class PrivateData;                                                                                                 \
	friend class classname::PrivateData;                                                                               \
	QScopedPointer< PrivateData > d_ptr;
#endif

/**
 * \if ENGLISH
 * @def Imitates Q_DECLARE_PUBLIC
 * \endif
 *
 * \if CHINESE
 * @def 模仿Q_DECLARE_PUBLIC
 * \endif
 */
#ifndef SA_COLOR_WIDGETS_DECLARE_PUBLIC
#define SA_COLOR_WIDGETS_DECLARE_PUBLIC(classname)                                                                     \
	friend class classname;                                                                                            \
	classname* q_ptr { nullptr };
#endif

/**
 * \if ENGLISH
 * @def API declaration for color widgets library
 * \endif
 *
 * \if CHINESE
 * @def 颜色组件库的API声明
 * \endif
 */
#ifndef SA_COLOR_WIDGETS_NO_DLL
#if defined(SA_COLOR_WIDGETS_MAKE_LIB)  // 定义此宏将构建library
#ifndef SA_COLOR_WIDGETS_API
#define SA_COLOR_WIDGETS_API Q_DECL_EXPORT
#endif
#else
#ifndef SA_COLOR_WIDGETS_API
#define SA_COLOR_WIDGETS_API Q_DECL_IMPORT
#endif
#endif
#else
#ifndef SA_COLOR_WIDGETS_API
#define SA_COLOR_WIDGETS_API
#endif
#endif

/**
 * \if ENGLISH
 * @def const helper for different C++ standards
 * \endif
 *
 * \if CHINESE
 * @def 不同C++标准的const辅助宏
 * \endif
 */
#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#ifndef sacolor_as_const
#define sacolor_as_const std::as_const
#endif
#else
// C++14 及以下版本使用 Qt 的 qwt_as_const
#ifndef sacolor_as_const
#define sacolor_as_const qAsConst
#endif
#endif

#endif  // SACOLORWIDGETSGLOBAL_H

/*** End of inlined file: SAColorWidgetsGlobal.h ***/



/*** Start of inlined file: SARibbonBarVersionInfo.h ***/
#ifndef SARIBBONVERSIONINFO_H
#define SARIBBONVERSIONINFO_H
/**
 * @file 此文档由cmake根据SARibbonBarVersionInfo.h.in自动生成，任何在此文件上的改动都将覆写
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
#define SA_RIBBON_BAR_VERSION_MIN 8
#endif
/**
 * @def ribbon的数字版本 MAJ.MIN.{PAT}
 */
#ifndef SA_RIBBON_BAR_VERSION_PAT
#define SA_RIBBON_BAR_VERSION_PAT 0
#endif

/**
 * @def 版本号（字符串）
 */
#ifndef SARIBBON_VERSION
#define SARIBBON_VERSION "2.7.0"
#endif

#endif // SARIBBONVERSIONINFO_H

/*** End of inlined file: SARibbonBarVersionInfo.h ***/


/*** Start of inlined file: SARibbonGlobal.h ***/
#ifndef SARIBBONGLOBAL_H
#define SARIBBONGLOBAL_H
#include <memory>
#include <QtGlobal>
#include <QObject>

class QWidget;

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
 * \if ENGLISH
 * @def SA_RIBBON_DECLARE_PRIVATE
 * @brief Similar to Q_DECLARE_PRIVATE, but uses an internal class instead of forward declaration
 * @code
 * //header
 * class A
 * {
 *  SA_RIBBON_DECLARE_PRIVATE(A)
 * };
 * @endcode
 * @code
 * // Expanded result:
 * class A{
 *  class PrivateData;
 *  friend class A::PrivateData;
 *  std::unique_ptr< PrivateData > d_ptr;
 * }
 * @endcode
 * @code
 * //cpp
 * class A::PrivateData{
 *  DA_DECLARE_PUBLIC(A)
 *  PrivateData(A* p):q_ptr(p){
 *  }
 * };
 *
 * A::A():d_ptr(new PrivateData(this)){
 * }
 * @endcode
 * \endif
 *
 * \if CHINESE
 * @def SA_RIBBON_DECLARE_PRIVATE
 * @brief 模仿Q_DECLARE_PRIVATE，但不用前置声明而是作为一个内部类
 * @code
 * //header
 * class A
 * {
 *  SA_RIBBON_DECLARE_PRIVATE(A)
 * };
 * @endcode
 * @code
 * // 其展开效果为：
 * class A{
 *  class PrivateData;
 *  friend class A::PrivateData;
 *  std::unique_ptr< PrivateData > d_ptr;
 * }
 * @endcode
 * @code
 * //cpp
 * class A::PrivateData{
 *  DA_DECLARE_PUBLIC(A)
 *  PrivateData(A* p):q_ptr(p){
 *  }
 * };
 *
 * A::A():d_ptr(new PrivateData(this)){
 * }
 * @endcode
 * \endif
 */
#ifndef SA_RIBBON_DECLARE_PRIVATE
#define SA_RIBBON_DECLARE_PRIVATE(classname)                                                                           \
	class PrivateData;                                                                                                 \
	friend class classname::PrivateData;                                                                               \
	std::unique_ptr< PrivateData > d_ptr;
#endif

/**
 * \if ENGLISH
 * @def SA_RIBBON_DECLARE_PUBLIC
 * @brief Similar to Q_DECLARE_PUBLIC
 * @details Used with SA_RIBBON_DECLARE_PRIVATE
 * \endif
 *
 * \if CHINESE
 * @def SA_RIBBON_DECLARE_PUBLIC
 * @brief 模仿Q_DECLARE_PUBLIC
 * @details 配套SA_RIBBON_DECLARE_PRIVATE使用
 * \endif
 */
#ifndef SA_RIBBON_DECLARE_PUBLIC
#define SA_RIBBON_DECLARE_PUBLIC(classname)                                                                            \
	friend class classname;                                                                                            \
	classname* q_ptr { nullptr };                                                                                      \
	PrivateData(const PrivateData&)            = delete;                                                               \
	PrivateData& operator=(const PrivateData&) = delete;
#endif

/**
 * \if ENGLISH
 * @def SA_RIBBON_IMPL_CONSTRUCT
 * @brief Used with SA_RIBBON_DECLARE_PRIVATE to construct PrivateData in constructor
 * \endif
 *
 * \if CHINESE
 * @def SA_RIBBON_IMPL_CONSTRUCT
 * @brief 配套SA_RIBBON_DECLARE_PRIVATE使用,在构造函数中构建PrivateData
 * \endif
 */
#ifndef SA_RIBBON_IMPL_CONSTRUCT
#define SA_RIBBON_IMPL_CONSTRUCT d_ptr(std::make_unique< PrivateData >(this))
#endif

/**
 * \if ENGLISH
 * @def SA_D
 * @brief Get impl pointer, similar to Q_D
 * \endif
 *
 * \if CHINESE
 * @def SA_D
 * @brief impl获取指针，参考Q_D
 * \endif
 */
#ifndef SA_D
#define SA_D(pointerName) PrivateData* pointerName = d_ptr.get()
#endif

/**
 * \if ENGLISH
 * @def SA_DC
 * @brief Get const impl pointer, similar to Q_DC
 * \endif
 *
 * \if CHINESE
 * @def SA_DC
 * @brief impl获取指针，参考Q_DC
 * \endif
 */
#ifndef SA_DC
#define SA_DC(pointerName) const PrivateData* pointerName = d_ptr.get()
#endif

/**
 * \if ENGLISH
 * @def SA_Q
 * @brief Get pointer in impl, similar to Q_Q
 * \endif
 *
 * \if CHINESE
 * @def SA_Q
 * @brief impl获取指针，参考Q_Q
 * \endif
 */
#ifndef SA_Q
#define SA_Q(pointerName) auto* pointerName = q_ptr
#endif

/**
 * \if ENGLISH
 * @def SA_QC
 * @brief Get const pointer in impl, similar to Q_QC
 * \endif
 *
 * \if CHINESE
 * @def SA_QC
 * @brief impl获取指针，参考Q_QC
 * \endif
 */
#ifndef SA_QC
#define SA_QC(pointerName) const auto* pointerName = q_ptr
#endif

/**
 * \if ENGLISH
 * @brief Define the alignment mode of Ribbon, supports left alignment, center alignment and right alignment
 * @note If your compiler reports: the qualified name of the member declaration is illegal, then check if the file line break is LF, if so, change the file line break to CRLF
 * \endif
 *
 * \if CHINESE
 * @brief 定义 Ribbon 的对其方式，支持左对齐、居中对其和右对齐
 * @note 如果你编译器提示：成员声明的限定名称非法，那么留意一下文件换行是否为 LF，如果是把文件换行改为 CRLF
 * \endif
 */
enum class SARibbonAlignment
{
	AlignLeft,   ///< Left alignment, tab bar left aligned, category also left aligned
	AlignCenter, ///< Center alignment, tab bar center aligned, category also center aligned
	AlignRight   ///< Right alignment, tab bar right aligned, category also right aligned
};

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
	RibbonThemeOffice2013,      ///< Office 2013 theme
	RibbonThemeOffice2016Blue,  ///< Office 2016 - Blue theme
	RibbonThemeOffice2021Blue,  ///< Office 2021 - Blue theme
	RibbonThemeWindows7,        ///< Windows 7 theme
	RibbonThemeDark,            ///< Dark theme
	RibbonThemeDark2            ///< Dark theme 2
};

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

#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#ifndef sa_as_const
#define sa_as_const std::as_const
#endif
#else
// C++14 及以下版本使用 Qt 的 qwt_as_const
#ifndef sa_as_const
#define sa_as_const qAsConst
#endif
#endif

#endif  // SARIBBONGLOBAL_H

/*** End of inlined file: SARibbonGlobal.h ***/


/*** Start of inlined file: SARibbonQt5Compat.hpp ***/
#ifndef SARIBBONQT5COMPAT_HPP
#define SARIBBONQT5COMPAT_HPP
#include <QtCore/QtGlobal>
#include <QtCore/QObject>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtGui/QWheelEvent>
#include <QtGui/QFontMetrics>
#include <QtGui/QFontMetricsF>

namespace SA
{

/**
 * @brief 处理Qt5与Qt6中的差异
 *
 */
namespace compat
{

/**
 * @brief 获取事件的位置（QPoint）
 * @tparam EventType 事件类型（需支持pos()或position()方法）
 * @param event 事件指针
 * @return 事件位置的QPoint表示
 */
template< typename EventType >
inline QPoint eventGlobalPos(EventType* event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	return event->globalPos();
#else
	return event->globalPosition().toPoint();
#endif
}

/**
 * @brief 获取事件的位置（QPoint）
 * @tparam EventType 事件类型（需支持pos()或position()方法）
 * @param event 事件指针
 * @return 事件位置的QPoint表示
 */
template< typename EventType >
inline QPoint eventPos(EventType* event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	return event->pos();
#else
	return event->position().toPoint();
#endif
}

/**
 * @brief 获取事件的x坐标
 * @tparam EventType 事件类型
 * @param event 事件指针
 * @return x坐标（整数）
 */
template< typename EventType >
inline int eventPosX(EventType* event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	return event->pos().x();
#else
	return static_cast< int >(event->position().x());
#endif
}

/**
 * @brief 获取事件的y坐标
 * @tparam EventType 事件类型
 * @param event 事件指针
 * @return y坐标（整数）
 */
template< typename EventType >
inline int eventPosY(EventType* event)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	return event->pos().y();
#else
	return static_cast< int >(event->position().y());
#endif
}

/**
 * @brief 计算字符串的水平宽度（整数版本）
 * @param fm QFontMetrics对象
 * @param str 目标字符串
 * @return 宽度（整数）
 */
template< typename strType >
inline int horizontalAdvance(const QFontMetrics& fm, const strType& str)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
	return fm.width(str);
#else
	return fm.horizontalAdvance(str);
#endif
}

/**
 * @brief 计算字符串的水平宽度（浮点数版本）
 * @param fm QFontMetricsF对象
 * @param str 目标字符串
 * @return 宽度（浮点数）
 */
template< typename strType >
inline qreal horizontalAdvanceF(const QFontMetricsF& fm, const strType& str)
{
#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
	return fm.width(str);
#else
	return fm.horizontalAdvance(str);
#endif
}

/**
 * @brief Get vertical wheel delta value compatible with Qt5 and Qt6
 *
 * This function provides a unified interface to retrieve the vertical scroll delta
 * from a QWheelEvent, supporting both Qt5 (using delta()) and Qt6 (using angleDelta().y())
 * without changing the calling code.
 *
 * The return value represents the vertical scroll amount:
 * - Positive value: Wheel scrolled up
 * - Negative value: Wheel scrolled down
 * - The magnitude follows the standard wheel step (typically ±120 per notch)
 *
 * @param e Pointer to the QWheelEvent object (must not be nullptr)
 * @return Integer delta value of vertical wheel movement
 * @note The function only returns vertical wheel delta (ignores horizontal scroll via angleDelta().x())
 * @warning Ensure the input QWheelEvent pointer is valid to avoid null pointer dereference
 */
inline int wheelEventDelta(QWheelEvent* e)
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	return e->delta();
#else
	return e->angleDelta().y();
#endif
}
}  // namespace   compat
}  // namespace   qwt
#endif  // SARIBBONQT5COMPAT_HPP

/*** End of inlined file: SARibbonQt5Compat.hpp ***/

// color widget

/*** Start of inlined file: SAColorMenu.h ***/
#ifndef SACOLORMENU_H
#define SACOLORMENU_H
#include <QMenu>

class QWidgetAction;
class SAColorPaletteGridWidget;
class SAColorGridWidget;
class SAColorToolButton;
/**
 * \if ENGLISH
 * @brief Standard color menu
 * \endif
 *
 * \if CHINESE
 * @brief 标准颜色菜单
 * \endif
 */
class SA_COLOR_WIDGETS_API SAColorMenu : public QMenu
{
	Q_OBJECT
	SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorMenu)
public:
	/// Constructor for SAColorMenu
	explicit SAColorMenu(QWidget* parent = nullptr);
	/// Constructor for SAColorMenu with title
	explicit SAColorMenu(const QString& title, QWidget* parent = nullptr);
	/// Destructor for SAColorMenu
	~SAColorMenu();
	/// Quick bind to color button
	void bindToColorToolButton(SAColorToolButton* btn);
	/// Get theme colors palette action
	QWidgetAction* themeColorsPaletteAction() const;
	/// Get custom colors widget action
	QWidgetAction* getCustomColorsWidgetAction() const;
	/// Get custom color action
	QAction* customColorAction() const;
	/// Get theme colors palette widget
	SAColorPaletteGridWidget* colorPaletteGridWidget() const;
	/// Get custom colors widget
	SAColorGridWidget* customColorsWidget() const;
	/// Enable none color action, disabled by default
	void enableNoneColorAction(bool on = true);
	/// Get none color action, returns nullptr until enableNoneColorAction(true) is called
	QAction* noneColorAction() const;
public Q_SLOTS:
	/// Helper slot function to emit selectedColor signal and hide menu
	void emitSelectedColor(const QColor& c);
Q_SIGNALS:
	/**
	 * \if ENGLISH
	 * @brief Signal emitted when a color is selected
	 * @param c Selected color
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 选择了颜色触发的信号
	 * @param c 选中的颜色
	 * \endif
	 */
	void selectedColor(const QColor& c);
private Q_SLOTS:
	void onCustomColorActionTriggered(bool on);
	void onNoneColorActionTriggered(bool on);

private:
	void init(const QList< QColor >& themeCls);
};

#endif  // SACOLORMENU_H

/*** End of inlined file: SAColorMenu.h ***/



/*** Start of inlined file: SAColorGridWidget.h ***/
#ifndef SACOLORGRIDWIDGET_H
#define SACOLORGRIDWIDGET_H
#include <QWidget>
#include <functional>

class QAbstractButton;
class SAColorToolButton;
/**
 * \if ENGLISH
 * @brief A grid-based color layout widget
 * @details This widget displays a grid of color buttons like:
 * @details □□□□□□□□□
 * @details □□□□□□□□□
 * @details □□□□□□□□□
 * \endif
 *
 * \if CHINESE
 * @brief 一个grid的Color布局
 * @details 一个形如下面的颜色grid：
 * @details □□□□□□□□□
 * @details □□□□□□□□□
 * @details □□□□□□□□□
 * \endif
 */
class SA_COLOR_WIDGETS_API SAColorGridWidget : public QWidget
{
	Q_OBJECT
	SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorGridWidget)
	Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
public:
	using FunColorBtn = std::function< void(SAColorToolButton*) >;

public:
	/// Constructor for SAColorGridWidget
	SAColorGridWidget(QWidget* par = nullptr);
	/// Destructor for SAColorGridWidget
	~SAColorGridWidget();
	/// Set column count, row count will be determined by column count
	void setColumnCount(int c);
	/// Get column count
	int columnCount() const;
	/// Set current color list
	void setColorList(const QList< QColor >& cls);
	/// Get color list
	QList< QColor > getColorList() const;
	/// Get spacing
	int spacing() const;
	/// Set spacing
	void setSpacing(int v);
	/// Get color count
	int colorCount() const;
	/// Set color icon size
	void setColorIconSize(const QSize& s);
	/// Get color icon size
	QSize colorIconSize() const;
	/// Set whether colors are checkable
	void setColorCheckable(bool on = true);
	/// Check if colors are checkable
	bool isColorCheckable() const;
	/// Get currently checked color
	QColor currentCheckedColor() const;
	/// Get color button by index
	SAColorToolButton* colorButton(int index) const;
	/// Set vertical spacing
	void setVerticalSpacing(int v);
	/// Get vertical spacing
	int verticalSpacing() const;
	/// Set horizontal spacing
	void setHorizontalSpacing(int v);
	/// Get horizontal spacing
	int horizontalSpacing() const;
	/// Clear checked state, no color will be selected
	void clearCheckedState();
	/// Iterate through all color buttons
	void iterationColorBtns(FunColorBtn fn);
	/// Set row minimum height
	void setRowMinimumHeight(int row, int minSize);
	/// Set horizontal spacer to right
	void setHorizontalSpacerToRight(bool on = true);
private Q_SLOTS:
	void onButtonClicked(QAbstractButton* btn);
	void onButtonPressed(QAbstractButton* btn);
	void onButtonReleased(QAbstractButton* btn);
	void onButtonToggled(QAbstractButton* btn, bool on);

Q_SIGNALS:
	/**
	 * \if ENGLISH
	 * @brief Signal emitted when a color is clicked
	 * @param c Clicked color
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 颜色被点击时发出的信号
	 * @param c 被点击的颜色
	 * \endif
	 */
	void colorClicked(const QColor& c);

	/**
	 * \if ENGLISH
	 * @brief Signal emitted when a color is pressed
	 * @param c Pressed color
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 颜色被按下时发出的信号
	 * @param c 被按下的颜色
	 * \endif
	 */
	void colorPressed(const QColor& c);

	/**
	 * \if ENGLISH
	 * @brief Signal emitted when a color is released
	 * @param c Released color
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 颜色被释放时发出的信号
	 * @param c 被释放的颜色
	 * \endif
	 */
	void colorReleased(const QColor& c);

	/**
	 * \if ENGLISH
	 * @brief Signal emitted when a color is toggled (for checkable mode)
	 * @param c Toggled color
	 * @param on Toggle state
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 对于check模式，check的颜色触发的信号
	 * @param c 被切换的颜色
	 * @param on 切换状态
	 * \endif
	 */
	void colorToggled(const QColor& c, bool on);

public:
	/// Get size hint
	virtual QSize sizeHint() const Q_DECL_OVERRIDE;
};
namespace SA
{
/// Get standard color list (10 colors in total)
SA_COLOR_WIDGETS_API QList< QColor > getStandardColorList();
}
#endif  // SACOLORGRIDWIDGET_H

/*** End of inlined file: SAColorGridWidget.h ***/


/*** Start of inlined file: SAColorPaletteGridWidget.h ***/
#ifndef SACOLORPALETTEGRIDWIDGET_H
#define SACOLORPALETTEGRIDWIDGET_H
#include <QWidget>

class QMenu;
class SAColorToolButton;
/**
 * \if ENGLISH
 * @brief Office-like color selection widget with standard colors and color palette
 * @details Has a row of standard colors, below is a color palette with 3 rows of light colors and 2 rows of dark colors
 * \endif
 *
 * \if CHINESE
 * @brief 类似office的颜色选择窗口，有一排标准色，下面有一个颜色板，有3行浅色，有2行深色
 * \endif
 */
class SA_COLOR_WIDGETS_API SAColorPaletteGridWidget : public QWidget
{
	Q_OBJECT
	SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorPaletteGridWidget)
public:
	/// Constructor for SAColorPaletteGridWidget
	SAColorPaletteGridWidget(QWidget* par = nullptr);
	/// Constructor for SAColorPaletteGridWidget with specified colors
	SAColorPaletteGridWidget(const QList< QColor >& cls, QWidget* par = nullptr);
	/// Destructor for SAColorPaletteGridWidget
	~SAColorPaletteGridWidget();
	/// Set color list maintained by the widget
	void setColorList(const QList< QColor >& cls);
	/// Get color list
	QList< QColor > colorList() const;
	/// Set color shade factor, default is { 180, 160, 140, 75, 50 }
	void setFactor(const QList< int >& factor);
	/// Get factor list
	QList< int > factor() const;
	/// Set color icon size
	void setColorIconSize(const QSize& s);
	/// Get color icon size
	QSize colorIconSize() const;
	/// Set whether colors are checkable
	void setColorCheckable(bool on = true);
	/// Check if colors are checkable
	bool isColorCheckable() const;
private Q_SLOTS:
	void onMainColorClicked(const QColor& c);
	void onPaletteColorClicked(const QColor& c);

private:
	void init();
Q_SIGNALS:
	/**
	 * \if ENGLISH
	 * @brief Signal emitted when a color is clicked
	 * @param c Clicked color
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 对于check模式，check的颜色触发的信号
	 * @param c 点击的颜色
	 * \endif
	 */
	void colorClicked(const QColor& c);
};

#endif  // SACOLORPALETTEGRIDWIDGET_H

/*** End of inlined file: SAColorPaletteGridWidget.h ***/


/*** Start of inlined file: SAColorToolButton.h ***/
#ifndef SACOLORTOOLBUTTON_H
#define SACOLORTOOLBUTTON_H
#include <QToolButton>

class QPaintEvent;
class QResizeEvent;
class QPainter;
class QStylePainter;
class SAColorMenu;
/**
 * \if ENGLISH
 * @brief A tool button that displays color
 *
 * In ToolButtonIconOnly mode, if no icon is set, the color occupies the entire area:
 *
 * ┌─────┐
 * │color│
 * └─────┘
 *
 * If an icon is set in ToolButtonIconOnly mode, the icon is displayed above and the color below:
 *
 * ┌─────┐
 * │icon │
 * │color│
 * └─────┘
 *
 * In ToolButtonTextBesideIcon and ToolButtonTextUnderIcon modes, setIconSize can specify the color size,
 * but it only works in ToolButtonTextBesideIcon and ToolButtonTextUnderIcon modes.
 *
 * If no icon is set (setIcon(QIcon())), iconSize is used as the color block size:
 *
 * ┌─────────┐
 * │┌─┐      │
 * │└─┘      │
 * └─────────┘
 *
 * If there is an icon, the color bar will be below the icon, 1/4 of the icon height and the same width as the icon.
 * If it exceeds the size of the control, it will automatically shrink.
 * \endif
 *
 * \if CHINESE
 * @brief 这是一个只显示颜色的toolbutton
 *
 * 在ToolButtonIconOnly模式下，如果没有setIcon,则颜色占用所有区域，如下图所示
 *
 * ┌─────┐
 * │color│
 * └─────┘
 *
 * 如果在ToolButtonIconOnly模式下有图标，图标在上面显示，下面显示颜色，如下图所示
 *
 * ┌─────┐
 * │icon │
 * │color│
 * └─────┘
 *
 * 在ToolButtonTextBesideIcon，ToolButtonTextUnderIcon下，setIconSize 可以指定颜色的大小，
 * 但只在ToolButtonTextBesideIcon，ToolButtonTextUnderIcon下有效
 *
 * 如果没有设置图标，也就是setIcon(QIcon()),iconSize作为颜色块的大小
 *
 * ┌─────────┐
 * │┌─┐      │
 * │└─┘      │
 * └─────────┘
 *
 * 如果有图标，颜色条会在图标下方，为图标高度的1/4 为图标宽度一致，如若超过控件的大小，会自动缩小体积
 * \endif
 */
class SA_COLOR_WIDGETS_API SAColorToolButton : public QToolButton
{
	Q_OBJECT
	SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorToolButton)
public:
	/**
	 * @brief Color button built-in styles
	 */
	enum ColorToolButtonStyle
	{
		WithColorMenu,  ///< Default will build a SAColorMenu
		NoColorMenu     ///< No ColorMenu
	};

public:
	/// Constructor for SAColorToolButton
	explicit SAColorToolButton(QWidget* parent = nullptr);
	/// Constructor for SAColorToolButton with style
	explicit SAColorToolButton(ColorToolButtonStyle style, QWidget* parent = nullptr);
	/// Destructor for SAColorToolButton
	~SAColorToolButton();
	/// Get color
	QColor color() const;
	/// Set margins
	void setMargins(const QMargins& mg);
	/// Get margins
	QMargins margins() const;
	/// Paint none color
	static void paintNoneColor(QPainter* p, const QRect& colorRect);
	/// Set color tool button style
	void setColorToolButtonStyle(ColorToolButtonStyle s);
	/// Get color tool button style
	ColorToolButtonStyle colorToolButtonStyle() const;
	/// Get colorMenu, note that this function may return nullptr if ColorToolButtonStyle is set to NoColorMenu or if a custom menu is set
	SAColorMenu* colorMenu() const;
	/// Create standard color menu
	SAColorMenu* createColorMenu();
public Q_SLOTS:
	/// Set color, will emit colorChanged signal
	void setColor(const QColor& c);

protected:
	/// Calculate positions of basic components
	virtual void calcRect(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
	/// Paint button
	virtual void paintButton(QStylePainter* p, const QStyleOptionToolButton& opt);
	/// Paint icon
	virtual void paintIcon(QStylePainter* p, const QRect& iconRect, const QStyleOptionToolButton& opt);
	/// Paint text
	virtual void paintText(QStylePainter* p, const QRect& textRect, const QStyleOptionToolButton& opt);
	/// Paint color
	virtual void paintColor(QStylePainter* p, const QRect& colorRect, const QColor& color, const QStyleOptionToolButton& opt);

protected:
	/// Handle paint event
	virtual void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
	/// Handle resize event
	virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
	/// Override sizeHint
	virtual QSize sizeHint() const Q_DECL_OVERRIDE;
private Q_SLOTS:
	/// Handle button clicked
	void onButtonClicked(bool checked = false);
Q_SIGNALS:
	/**
	 * \if ENGLISH
	 * @brief Signal emitted when color is clicked
	 * @param color The clicked color
	 * @param checked Checked state
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 颜色被点击的响应
	 * @param color 颜色
	 * @param checked 选中状态
	 * \endif
	 */
	void colorClicked(const QColor& color, bool checked = false);
	/**
	 * \if ENGLISH
	 * @brief Signal emitted when color changes
	 * @param color The new color
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 颜色改变信号
	 * @param color 颜色
	 * \endif
	 */
	void colorChanged(const QColor& color);
};

#endif  // SACOLORTOOLBUTTON_H

/*** End of inlined file: SAColorToolButton.h ***/

// sa ribbon

/*** Start of inlined file: SARibbonUtil.h ***/
#ifndef SARIBBONUTIL_H
#define SARIBBONUTIL_H

#include <QColor>
#include <QSize>
#include <QIcon>
namespace SA
{

// 让颜色鲜艳
QColor SA_RIBBON_EXPORT makeColorVibrant(const QColor& c, int saturationDelta = 150, int valueDelta = 30);

// 按照指定的新高度，保持宽高比缩放 QSize
QSize SA_RIBBON_EXPORT scaleSizeByHeight(const QSize& originalSize, int newHeight);

// 按照指定的新高度，宽高比为1:factor缩放 QSize。
QSize SA_RIBBON_EXPORT scaleSizeByHeight(const QSize& originalSize, int newHeight, qreal factor);

// 按照指定的新宽度，保持宽高比缩放 QSize
QSize SA_RIBBON_EXPORT scaleSizeByWidth(const QSize& originalSize, int newWidth);

// 获取内置的ribbon主题对应的qss内容
QString SA_RIBBON_EXPORT getBuiltInRibbonThemeQss(SARibbonTheme theme);

// 给窗口设置内置的ribbon主题
void SA_RIBBON_EXPORT setBuiltInRibbonTheme(QWidget* w, SARibbonTheme theme);

// 提供类似QIcon::pixmap(const QSize &size, qreal devicePixelRatio, Mode mode, State state) const（Qt6新增）的兼容函数
QPixmap iconToPixmap(const QIcon& icon,
					 const QSize& size,
					 qreal devicePixelRatio,
					 QIcon::Mode mode   = QIcon::Normal,
					 QIcon::State state = QIcon::Off);

// 获取窗口当前所在屏幕的dpr
qreal widgetDevicePixelRatio(QWidget* w);

/**
 * \if ENGLISH
 * @brief Check if the application layout direction is Right-to-Left (RTL)
 * @return true if layout direction is Qt::RightToLeft, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 检查应用程序布局方向是否为从右到左（RTL）
 * @return 如果布局方向为 Qt::RightToLeft 返回 true，否则返回 false
 * \endif
 */
bool SA_RIBBON_EXPORT saIsRTL();

/**
 * \if ENGLISH
 * @brief Mirror X coordinate for RTL layout support
 * @param x The original X coordinate
 * @param containerWidth The width of the container
 * @param elementWidth The width of the element
 * @return containerWidth - x - elementWidth when RTL, x unchanged when LTR
 * \endif
 *
 * \if CHINESE
 * @brief 为 RTL 布局支持镜像 X 坐标
 * @param x 原始 X 坐标
 * @param containerWidth 容器宽度
 * @param elementWidth 元素宽度
 * @return RTL 时返回 containerWidth - x - elementWidth，LTR 时返回 x 不变
 * \endif
 */
int SA_RIBBON_EXPORT saMirrorX(int x, int containerWidth, int elementWidth);
}
#endif  // SARIBBONUTIL_H

/*** End of inlined file: SARibbonUtil.h ***/



/*** Start of inlined file: SAFramelessHelper.h ***/
#ifndef SAFRAMELESSHELPER_H
#define SAFRAMELESSHELPER_H

#include <QObject>

class QWidget;

/**
 * @class SAFramelessHelper
 * @brief 该类用于帮助实现无边框窗体的移动和缩放功能。
 *
 * SAFramelessHelper 提供了一系列方法来激活、移除无边框窗体，设置窗体的移动、缩放属性，
 * 以及橡皮筋效果等。通过事件过滤机制，它能够处理鼠标事件，从而实现窗体的移动和缩放操作。
 *
 * 示例：
 *
 * @code
 * SAFramelessHelper* helper = new SAFramelessHelper(this);
 * helper->activateOn(myWidget);
 * helper->setWidgetMovable(true);
 * helper->setWidgetResizable(true);
 * @endcode
 */
class SA_RIBBON_EXPORT SAFramelessHelper : public QObject
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SAFramelessHelper)
	friend class SAPrivateFramelessWidgetData;

public:
	explicit SAFramelessHelper(QObject* parent);
	~SAFramelessHelper();
	// 激活窗体
	void activateOn(QWidget* topLevelWidget);

	// 移除窗体
	void removeFrom(QWidget* topLevelWidget);

	// 设置窗体移动
	void setWidgetMovable(bool movable);

	// 设置窗体缩放
	void setWidgetResizable(bool resizable);

	// 设置橡皮筋移动
	void setRubberBandOnMove(bool movable);

	// 设置橡皮筋缩放
	void setRubberBandOnResize(bool resizable);

	// 设置边框的宽度
	void setBorderWidth(int width);

	// 设置标题栏高度
	void setTitleHeight(int height);
	bool widgetResizable();
	bool widgetMovable();
	bool rubberBandOnMove();
	bool rubberBandOnResisze();
	uint borderWidth();
	uint titleHeight();
	// 获取当前屏幕的 DPI 缩放因子
	static qreal getScreenDpiScale(const QWidget* widget);

protected:
	// 事件过滤，进行移动、缩放等
	virtual bool eventFilter(QObject* obj, QEvent* event);
};

#endif  // FRAMELESSHELPER_H

/*** End of inlined file: SAFramelessHelper.h ***/


/*** Start of inlined file: SARibbonApplicationButton.h ***/
#ifndef SARIBBONAPPLICATIONBUTTON_H
#define SARIBBONAPPLICATIONBUTTON_H
#include <QToolButton>

/**
 * @brief The SARibbonApplicationButton class
 *
 * 默认的ApplicationButton,可以通过样式指定不一样的ApplicationButton
 */
class SA_RIBBON_EXPORT SARibbonApplicationButton : public QToolButton
{
	Q_OBJECT
public:
	explicit SARibbonApplicationButton(QWidget* parent = nullptr);
	explicit SARibbonApplicationButton(const QString& text, QWidget* parent = nullptr);
	explicit SARibbonApplicationButton(const QIcon& icon, const QString& text, QWidget* parent = nullptr);
	~SARibbonApplicationButton();
};

#endif  // SARIBBONAPPLICATIONBUTTON_H

/*** End of inlined file: SARibbonApplicationButton.h ***/


/*** Start of inlined file: SARibbonSystemButtonBar.h ***/
#ifndef SARIBBONSYSTEMBUTTONBAR_H
#define SARIBBONSYSTEMBUTTONBAR_H

#include <QFrame>
#include <QToolButton>

/**
 * \if ENGLISH
 * @brief Window maximize/minimize buttons
 * @note There is a SARibbonButtonGroupWidget inside, its ObjectName = SASystemButtonGroup
 * @details If you need qss, you can specialize:
 * @details @code
 * @details SARibbonButtonGroupWidget#SASystemButtonGroup
 * @details @endcode
 * @details SARibbonSystemButtonBar's eventfilter captures mainwindow events, handles mainwindow events through eventfilter to avoid abnormal systembar position caused by user incorrectly inheriting resizeEvent
 * @details Therefore, the main window needs to install event filter
 * @details @code
 * @details MainWindow::MainWindow(){
 * @details    installEventFilter(mWindowButtonGroup);
 * @details }
 * @details @endcode
 * \endif
 *
 * \if CHINESE
 * @brief 窗口的最大最小化按钮
 * @note 内部有个SARibbonButtonGroupWidget，其ObjectName = SASystemButtonGroup
 * @details 如果需要qss，可以进行特化处理:
 * @details @code
 * @details SARibbonButtonGroupWidget#SASystemButtonGroup
 * @details @endcode
 * @details SARibbonSystemButtonBar的eventfilter捕获mainwindow的事件,通过eventerfilter来处理mainwindow的事件，避免用户错误的继承resizeEvent导致systembar的位置异常
 * @details 因此，主窗口需要安装事件过滤器
 * @details @code
 * @details MainWindow::MainWindow(){
 * @details    installEventFilter(mWindowButtonGroup);
 * @details }
 * @details @endcode
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonSystemButtonBar : public QFrame
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonSystemButtonBar)
public:
	/// Constructor for SARibbonSystemButtonBar
	explicit SARibbonSystemButtonBar(QWidget* parent);
	/// Constructor with window flags
	explicit SARibbonSystemButtonBar(QWidget* parent, Qt::WindowFlags flags);
	/// Destructor for SARibbonSystemButtonBar
	~SARibbonSystemButtonBar();
	/// Setup minimize button
	void setupMinimizeButton(bool on);
	/// Setup maximize button
	void setupMaximizeButton(bool on);
	/// Setup close button
	void setupCloseButton(bool on);
	/// Use the WindowFlag of the top-level widget as WindowFlag
	void updateWindowFlag();
	/// Update window flag with specified flags
	void updateWindowFlag(Qt::WindowFlags flags);
	/// Set button width ratio, final button width will be set according to this ratio
	void setButtonWidthStretch(int close = 4, int max = 3, int min = 3);
	/// Set title bar height
	void setWindowTitleHeight(int h);
	/// Get title bar height
	int windowTitleHeight() const;
	/// Set standard system button width
	void setWindowButtonWidth(int w);
	/// Get standard system button width
	int windowButtonWidth() const;
	/// Set Qt::WindowStates
	void setWindowStates(Qt::WindowStates s);
	/// Get button states only
	Qt::WindowFlags windowButtonFlags() const;
	/// Get minimize button
	QAbstractButton* minimizeButton() const;
	/// Get maximize button
	QAbstractButton* maximizeButton() const;
	/// Get close button
	QAbstractButton* closeButton() const;

	/// Set icon size
	void setIconSize(const QSize& ic);
	/// Get icon size
	QSize iconSize() const;
	/// Generate and add an action
	void addAction(QAction* a);
	/// Add menu action
	void addMenuAction(QAction* menuAction, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
	/// Add menu action and return the action
	QAction* addMenuAction(QMenu* menu, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
	/// Add separator
	QAction* addSeparator();
	/// Add widget
	QAction* addWidget(QWidget* w);

	/// Get size hint
	virtual QSize sizeHint() const Q_DECL_OVERRIDE;
	/// Capture mainwindow events
	bool eventFilter(QObject* obj, QEvent* event) override;

protected:
	/// Handle resize event
	virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
	/// Handle change event for layout direction changes
	virtual void changeEvent(QEvent* e) override;

protected Q_SLOTS:
	/// Close window slot
	Q_SLOT void closeWindow();
	/// Minimize window slot
	Q_SLOT void minimizeWindow();
	/// Maximize window slot
	Q_SLOT void maximizeWindow();
};

/**
 * \if ENGLISH
 * @brief System maximize/minimize button
 * \endif
 *
 * \if CHINESE
 * @brief 系统最大最小化按钮
 * \endif
 */
class SARibbonSystemToolButton : public QToolButton
{
	Q_OBJECT
public:
	/// Constructor for SARibbonSystemToolButton
	SARibbonSystemToolButton(QWidget* p = nullptr);
};

#endif  // SARIBBONSYSTEMBUTTONBAR_H

/*** End of inlined file: SARibbonSystemButtonBar.h ***/


/*** Start of inlined file: SARibbonToolButton.h ***/
#ifndef SARIBBONTOOLBUTTON_H
#define SARIBBONTOOLBUTTON_H

#include <QToolButton>
#include <QDebug>

// 前向声明
class SARibbonButtonLayoutStrategy;
struct SARibbonButtonLayoutContext;
/**
 * \if ENGLISH
 * @brief Ribbon interface adapted tool button
 *
 * This button is designed specifically for the Ribbon interface, supporting two display modes: large buttons and small
 * buttons. It also supports automatic text wrapping for large buttons to optimize space usage.
 *
 * @note The icon size is dynamically adjusted according to the button size and cannot be set manually via `setIconSize`
 * \endif
 *
 * \if CHINESE
 * @brief Ribbon界面适用的toolButton
 *
 * 该按钮专为Ribbon界面设计，支持大按钮和小按钮两种显示模式。
 * 图标尺寸会根据按钮尺寸动态调整，无法通过 `setIconSize` 手动设置。
 * 大按钮模式下还支持文字自动换行，以优化空间利用。
 *
 * @note `setIconSize` 函数不在起作用，iconsize是根据当前尺寸动态调整的
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonToolButton : public QToolButton
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonToolButton)
public:
	/**
	 * @brief Button type enumeration / 按钮样式枚举
	 */
	enum RibbonButtonType
	{
		/**
		 * @brief Large button type, corresponding to the large button in SARibbonBar / 大按钮类型，此类型对应SARibbonBar的大按钮
		 */
		LargeButton,

		/**
		 * @brief Small button type, corresponding to the small button in SARibbonBar, equivalent to a normal toolbar
		 * button / 小按钮类型，此类型对应SARibbonBar的小按钮，等同于普通工具条的按钮
		 */
		SmallButton
	};
	Q_ENUM(RibbonButtonType)

	/**
	 * @brief Layout factor structure for fine-tuning button appearance / 用于微调按钮外观的布局系数结构体
	 */
	struct LayoutFactor
	{
		/**
		 * @brief Coefficient for the height of two-line text in large button mode / 大按钮模式下，两行文本高度系数
		 *
		 * This coefficient determines the height of the rectangle for two-line text when word wrapping is enabled.
		 * Two-line text area height = fontMetrics.lineSpacing * coefficient.
		 *
		 * 这个系数决定了文字换行时2行文本的矩形高度。
		 * 两行文本区域高度 = fontMetrics.lineSpacing*系数
		 *
		 * @note This value should be greater than 2. / 此值应该大于2
		 * @default 2.05
		 */
		qreal twoLineHeightFactor { 2.05 };  // Note: Kept as literal for header inline initialization compatibility

		/**
		 * @brief Coefficient for the height of single-line text in large button mode / 大按钮模式下，单行文本高度系数
		 *
		 * This coefficient determines the line height for single-line text.
		 * Single-line text area height = fontMetrics.lineSpacing * coefficient.
		 *
		 * 这个系数决定了单行文本的行高度。
		 * 单行文本区域高度 = fontMetrics.lineSpacing*系数
		 *
		 * @note This value should be greater than 1. / 此值应该大于1
		 * @default 1.2
		 */
		qreal oneLineHeightFactor { 1.2 };

		/**
		 * @brief Maximum aspect ratio (width/height) for the button / 按钮最大宽高比
		 *
		 * The maximum width of the button is determined by its height multiplied by this coefficient.
		 * For example, if the button height is `h`, then the maximum width is `maxw = h * buttonMaximumAspectRatio`.
		 * If the text cannot be fully displayed within this width, the button will not expand further horizontally,
		 * and ellipsis (...) will be used to indicate truncated text.
		 *
		 * 按钮的最大宽度为按钮高度*此系数，例如按钮高度为h，那么按钮最大宽度maxw=h*buttonMaximumAspectRatio
		 * 如果在此宽度下文字还无法完全显示，那么按钮将不会继续横向扩展，将使用...替代未完全显示的文字
		 *
		 * @default 1.4
		 */
		qreal buttonMaximumAspectRatio { 1.4 };
	};

public:
	explicit SARibbonToolButton(QWidget* parent = nullptr);
	explicit SARibbonToolButton(QAction* defaultAction, QWidget* parent = nullptr);
	~SARibbonToolButton();

	// Get the current button type (LargeButton or SmallButton)
	RibbonButtonType buttonType() const;
	// Set the button type to LargeButton or SmallButton
	void setButtonType(const RibbonButtonType& buttonType);

	/// Set whether text is displayed to the right of the icon
	void setEnableIconRightText(bool on);
	/// Check if icon-right-text mode is enabled
	bool isEnableIconRightText() const;

	// Check if the button is a small ribbon button
	bool isSmallRibbonButton() const;
	// Check if the button is a large ribbon button
	bool isLargeRibbonButton() const;

	// Get the current spacing value
	int spacing() const;
	// Set the spacing between elements and the border
	void setSpacing(int v);

	// Force an update of the internal layout rectangles
	void updateRect();

	// Set the layout factor for fine-tuning the button's appearance
	void setLayoutFactor(const LayoutFactor& fac);
	// Get the layout factor for fine-tuning the button's appearance (const version)
	const LayoutFactor& layoutFactor() const;
	// Get the layout factor for fine-tuning the button's appearance (non-const version)
	LayoutFactor& layoutFactor();

	// Enables or disables automatic text wrapping for large buttons / 为大按钮启用或禁用自动文字换行
	void setEnableWordWrap(bool on);
	// Checks if automatic text wrapping is enabled / 检查是否启用了自动文字换行
	bool isEnableWordWrap() const;

	// Sets the button's maximum aspect ratio (width/height) / 设置按钮的最大宽高比
	void setButtonMaximumAspectRatio(qreal v = 1.4);
	// Gets the button's maximum aspect ratio (width/height) / 获取按钮的最大宽高比
	qreal buttonMaximumAspectRatio() const;

	// Invalidates the cached size hint / 使缓存的size hint失效
	void invalidateSizeHint();

	// 大按钮尺寸
	void setLargeIconSize(const QSize& largeSize);
	QSize largeIconSize() const;

	// 小按钮尺寸
	void setSmallIconSize(const QSize& smallSize);
	QSize smallIconSize() const;

public:
	virtual QSize sizeHint() const Q_DECL_OVERRIDE;
	virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

protected:
	virtual void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
	virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
	virtual void mouseMoveEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
	virtual void mousePressEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QMouseEvent* e) Q_DECL_OVERRIDE;
	virtual void focusOutEvent(QFocusEvent* e) Q_DECL_OVERRIDE;
	virtual void leaveEvent(QEvent* e) Q_DECL_OVERRIDE;
	virtual bool hitButton(const QPoint& pos) const Q_DECL_OVERRIDE;
	virtual bool event(QEvent* e) Q_DECL_OVERRIDE;
	virtual void changeEvent(QEvent* e) Q_DECL_OVERRIDE;
	virtual void actionEvent(QActionEvent* e) Q_DECL_OVERRIDE;

	// Paints the button's background and frame / 绘制按钮的背景和边框
	virtual void paintButton(QPainter& p, const QStyleOptionToolButton& opt);
	// Paints the button's icon / 绘制按钮的图标
	virtual void paintIcon(QPainter& p, const QStyleOptionToolButton& opt, const QRect& iconDrawRect);
	// Paints the button's text / 绘制按钮的文字
	virtual void paintText(QPainter& p, const QStyleOptionToolButton& opt, const QRect& textDrawRect);
	// Paints the button's indicator (e.g., dropdown arrow) / 绘制按钮的指示器（例如下拉箭头）
	virtual void paintIndicator(QPainter& p, const QStyleOptionToolButton& opt, const QRect& indicatorDrawRect);

	/**
	 * @brief 创建图标pixmap，子类可以重写此函数以自定义图标绘制
	 * @param opt 样式选项
	 * @param iconSize 图标尺寸
	 * @return 图标pixmap
	 * @note 此函数在paintIcon中被调用，提供扩展点给子类
	 */
	virtual QPixmap createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconSize) const;

private:
	static void drawArrow(const QStyle* style,
						  const QStyleOptionToolButton* toolbutton,
						  const QRect& rect,
						  QPainter* painter,
						  const QWidget* widget = 0);
};

namespace SA
{
QDebug operator<<(QDebug debug, const QStyleOptionToolButton& opt);
}
#endif  // SARIBBONTOOLBUTTON_H

/*** End of inlined file: SARibbonToolButton.h ***/


/*** Start of inlined file: SARibbonColorToolButton.h ***/
#ifndef SARIBBONCOLORTOOLBUTTON_H
#define SARIBBONCOLORTOOLBUTTON_H

class SAColorMenu;
/**
 * \if ENGLISH
 * @brief Color tool button similar to Office color setting buttons
 * @details This button can display color below the icon or use color as the icon itself
 * \endif
 *
 * \if CHINESE
 * @brief 参考office的颜色设置按钮，可以显示颜色在图标下方
 * @details 此按钮可以在图标下方显示颜色，或使用颜色作为图标本身
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonColorToolButton : public SARibbonToolButton
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonColorToolButton)
public:
	/**
	 * \if ENGLISH
	 * @brief Color display style
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 颜色样式
	 * \endif
	 */
	enum ColorStyle
	{
		ColorUnderIcon,  ///< 颜色在icon下方，这个要求必须设置icon
		ColorFillToIcon  ///< 颜色作为icon，这个模式下在setColor会自动生成一个颜色icon替换掉原来的icon，因此setIcon函数没有作用
	};

public:
	// Constructor for SARibbonColorToolButton
	explicit SARibbonColorToolButton(QWidget* parent = nullptr);
	// Constructor with default action
	explicit SARibbonColorToolButton(QAction* defaultAction, QWidget* parent = nullptr);
	// Destructor for SARibbonColorToolButton
	~SARibbonColorToolButton();
	// Get the current color
	QColor color() const;
	// Set the color display style
	void setColorStyle(ColorStyle s);
	// Get the current color display style
	ColorStyle colorStyle() const;
	// Set up a standard color menu
	SAColorMenu* setupStandardColorMenu();
public Q_SLOTS:
	// Set the color, emits colorChanged signal
	void setColor(const QColor& c);
private Q_SLOTS:
	void onButtonClicked(bool checked = false);
Q_SIGNALS:
	/**
	 * \if ENGLISH
	 * @brief Signal emitted when color is clicked
	 * @param color The clicked color
	 * @param checked Whether the button is checked
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 颜色被点击的响应
	 * @param color 点击的颜色
	 * @param checked 按钮是否被选中
	 * \endif
	 */
	void colorClicked(const QColor& color, bool checked = false);
	/**
	 * \if ENGLISH
	 * @brief Signal emitted when color is changed
	 * @param color The new color
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 颜色改变信号
	 * @param color 新的颜色
	 * \endif
	 */
	void colorChanged(const QColor& color);

protected:
	/// Override createIconPixmap to add color under the icon
	QPixmap createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconSize) const override;
};

#endif  // SARIBBONCOLORTOOLBUTTON_H

/*** End of inlined file: SARibbonColorToolButton.h ***/


/*** Start of inlined file: SARibbonLineWidgetContainer.h ***/
#ifndef SARIBBONLINEWIDGETCONTAINER_H
#define SARIBBONLINEWIDGETCONTAINER_H

#include <QtCore/qglobal.h>
#include <QWidget>
#include <QLabel>

/**
 * \if ENGLISH
 * @brief A widget container that places the widget in the middle, with text before and after, mainly used for small widgets on the panel
 * @details Implements the following effect:
 * @details PrefixLabel|_Widget_|SuffixLabel
 * \endif
 *
 * \if CHINESE
 * @brief 一个窗口容器，把窗口放置中间，前面后面都可以设置文本，主要用于放置在panel上的小窗口
 * @details 实现如下效果：
 * @details PrefixLabel|_Widget_|SuffixLabel
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonLineWidgetContainer : public QWidget
{
public:
	/// Constructor for SARibbonLineWidgetContainer
	explicit SARibbonLineWidgetContainer(QWidget* par = nullptr);
	/// Destructor for SARibbonLineWidgetContainer
	~SARibbonLineWidgetContainer();
	/// Set widget (nullptr is not allowed)
	void setWidget(QWidget* innerWidget);

	/// Set prefix
	void setPrefix(const QString& str);

	/// Set suffix
	void setSuffix(const QString& str);

	/// Prefix text box
	QLabel* labelPrefix() const;

	/// Suffix text box
	QLabel* labelSuffix() const;

private:
	// 两个文本
	QLabel* m_labelPrefix;
	QLabel* m_labelSuffix;
	QWidget* m_innerWidget;
};

#endif  // SARIBBONWIDGETCONTAINER_H

/*** End of inlined file: SARibbonLineWidgetContainer.h ***/


/*** Start of inlined file: SARibbonActionsManager.h ***/
#ifndef SARIBBONACTIONSMANAGER_H
#define SARIBBONACTIONSMANAGER_H

#include <QObject>
#include <QAbstractListModel>
#include <QAction>
#include <QMap>
#include <QString>
#include <QSet>
class SARibbonBar;
class SARibbonCategory;

/**
 * \if ENGLISH
 * @brief Manager for all SARibbon actions
 *
 * SARibbonActionsManager maintains two tables: one for tag-to-action list mapping,
 * and another for all actions under management.
 * \endif
 *
 * \if CHINESE
 * @brief SARibbon动作管理器
 *
 * SARibbonActionsManager维护两个表：一个用于标签到动作列表的映射，
 * 另一个用于管理下的所有动作。
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonActionsManager : public QObject
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonActionsManager)
	friend class SARibbonActionsManagerModel;

public:
	/// Action tag definition
	enum ActionTag
	{
		UnknowActionTag              = 0,     ///< Unknown tag
		CommonlyUsedActionTag        = 0x01,  ///< Preset tag - commonly used commands
		NotInFunctionalAreaActionTag = 0x02,  ///< Preset tag - commands not in functional area
		AutoCategoryDistinguishBeginTag = 0x1000,  ///< Auto category distinguish begin tag
		AutoCategoryDistinguishEndTag = 0x2000,  ///< Auto category distinguish end tag
		NotInRibbonCategoryTag = 0x2001,  ///< Tag for actions not in ribbon category
		UserDefineActionTag = 0x8000  ///< User defined tag, all custom tags should be greater than this
	};
	/// Constructor
	explicit SARibbonActionsManager(SARibbonBar* bar);
	/// Destructor
	~SARibbonActionsManager();
	/// Set tag name
	void setTagName(int tag, const QString& name);

	/// Get tag name
	QString tagName(int tag) const;

	/// Remove tag, note this function is time-consuming
	void removeTag(int tag);

	/// Register action
	bool registeAction(QAction* act, int tag, const QString& key = QString(), bool enableEmit = true);

	/// Unregister action
	void unregisteAction(QAction* act, bool enableEmit = true);

	/// Filter actions by tag, returns a reference
	QList< QAction* >& filter(int tag);

	/// Get actions by tag
	QList< QAction* >& actions(int tag);
	/// Get actions by tag (const version)
	const QList< QAction* > actions(int tag) const;

	/// Get all tags
	QList< int > actionTags() const;

	/// Get action by key
	QAction* action(const QString& key) const;

	/// Get key by action
	QString key(QAction* act) const;

	/// Get count of all managed actions
	int count() const;

	/// Get all managed actions
	QList< QAction* > allActions() const;

	/// Auto register actions from SARibbonBar, returns tag-to-category mapping
	QMap< int, SARibbonCategory* > autoRegisteActions(SARibbonBar* bar);

	/// Auto register widget actions
	QSet< QAction* > autoRegisteWidgetActions(QWidget* w, int tag, bool enableEmit = false);

	/// Search actions by text
	QList< QAction* > search(const QString& text);

	/// Clear all
	void clear();

Q_SIGNALS:

	/**
	 * \if ENGLISH
	 * @brief Signal emitted when action tag changed
	 * @param tag Changed tag
	 * @param isdelete Whether the tag is deleted (true) or added (false)
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 当action标签发生变化时发射的信号
	 * @param tag 发生变化的标签
	 * @param isdelete 标签是否被删除（true表示删除，false表示添加）
	 * \endif
	 */
	void actionTagChanged(int tag, bool isdelete);

private Q_SLOTS:
	void onActionDestroyed(QObject* o);
	void onCategoryTitleChanged(const QString& title);

private:
	void removeAction(QAction* act, bool enableEmit = true);
};

/**
 * \if ENGLISH
 * @brief Model for SARibbonActionsManager
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonActionsManager的模型类
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonActionsManagerModel : public QAbstractListModel
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonActionsManagerModel)
public:
	/// Constructor
	explicit SARibbonActionsManagerModel(QObject* p = nullptr);
	/// Constructor with SARibbonActionsManager
	explicit SARibbonActionsManagerModel(SARibbonActionsManager* m, QObject* p = nullptr);
	/// Destructor
	~SARibbonActionsManagerModel();
	/// Get row count
	virtual int rowCount(const QModelIndex& parent) const override;
	/// Get header data
	virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	/// Get item flags
	virtual Qt::ItemFlags flags(const QModelIndex& index) const override;
	/// Get data
	virtual QVariant data(const QModelIndex& index, int role) const override;
	/// Set filter tag
	void setFilter(int tag);
	/// Update model
	void update();
	/// Setup actions manager
	void setupActionsManager(SARibbonActionsManager* m);
	/// Uninstall actions manager
	void uninstallActionsManager();
	/// Get action from model index
	QAction* indexToAction(QModelIndex index) const;
	/// Search actions
	void search(const QString& text);

private Q_SLOTS:
	void onActionTagChanged(int tag, bool isdelete);
};

#endif  // SARIBBONACTIONSMANAGER_H

/*** End of inlined file: SARibbonActionsManager.h ***/


/*** Start of inlined file: SARibbonButtonGroupWidget.h ***/
#ifndef SARIBBONBUTTONGROUPWIDGET_H
#define SARIBBONBUTTONGROUPWIDGET_H

#include <QToolButton>
#include <QMenu>
#include <QToolBar>
/**
 * \if ENGLISH
 * @brief Widget for managing a group of actions, similar to QToolBar
 * @details This class provides a widget to manage a group of actions, similar to QToolBar, but with additional functionality for menu actions.
 * \endif
 *
 * \if CHINESE
 * @brief 用于管理一组Action,类似于QToolBar
 * @details 此类提供一个小部件来管理一组动作，类似于QToolBar，但具有菜单动作的附加功能。
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonButtonGroupWidget : public QToolBar
{
	Q_OBJECT
public:
	/// Constructor for SARibbonButtonGroupWidget
	explicit SARibbonButtonGroupWidget(QWidget* parent = nullptr);
	/// Destructor for SARibbonButtonGroupWidget
	~SARibbonButtonGroupWidget();

	/// Add a menu QAction to the button group widget
	void addMenuAction(QAction* menuAction, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
	/// Create and add a menu action to the button group widget
	QAction* addMenuAction(QMenu* menu, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
};

#endif  // SARIBBONBUTTONGROUPWIDGET_H

/*** End of inlined file: SARibbonButtonGroupWidget.h ***/


/*** Start of inlined file: SARibbonStackedWidget.h ***/
#ifndef SARIBBONSTACKEDWIDGET_H
#define SARIBBONSTACKEDWIDGET_H
#include <QStackedWidget>

class QHideEvent;
class QResizeEvent;

/**
 * \if ENGLISH
 * @class SARibbonStackedWidget
 * @brief Stacked widget with popup menu functionality
 * @details This class extends QStackedWidget and adds two working modes:
 * @details 1. Normal mode: As a regular stacked widget, embedded in the parent window layout
 * @details 2. Popup mode: As an independent popup window, can be used to implement menu-like display effects
 * @details In popup mode, the window will be displayed as a frameless panel and supports modal event loop (exec() method),
 * @details which makes it usable like a menu or dialog.
 * @note Switching modes will automatically handle window state and event loop
 * @sa QStackedWidget
 * \endif
 *
 * \if CHINESE
 * @class SARibbonStackedWidget
 * @brief 具有弹出式菜单功能的堆叠窗口部件
 * @details 此类扩展了 QStackedWidget，添加了两种工作模式：
 * @details 1. 正常模式：作为常规的堆叠部件，嵌入在父窗口布局中
 * @details 2. 弹出模式：作为独立的弹出窗口，可用于实现类似菜单的显示效果
 * @details 在弹出模式下，窗口将显示为无边框面板，并支持模态事件循环（exec()方法），
 * @details 这使得它可以像菜单或对话框一样使用。
 * @note 切换模式时会自动处理窗口状态和事件循环
 * @sa QStackedWidget
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonStackedWidget : public QStackedWidget
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonStackedWidget)
public:
	Q_PROPERTY(int animationWidgetHeight READ animationWidgetHeight WRITE setAnimationWidgetHeight)  // Add custom property
public:
	/// Constructor for SARibbonStackedWidget
	explicit SARibbonStackedWidget(QWidget* parent);

	/// Destructor for SARibbonStackedWidget
	~SARibbonStackedWidget();

	/// Set popup mode
	void setPopupMode();

	/// Check if currently in popup mode
	bool isPopupMode() const;

	/// Set normal mode, same as normal stackwidget
	void setNormalMode();

	/// Check if currently in normal mode
	bool isNormalMode() const;

	/// Run event loop modally in popup mode
	void exec();

	/// Similar to tabbar's moveTab function, swap the index of two widgets
	void moveWidget(int from, int to);

	/// Set whether to enable popup animation
	void setUseAnimation(bool on);

	/// Check if animation is enabled
	bool isUseAnimation() const;

	/// Set animation duration (milliseconds)
	void setAnimationDuration(int duration);

	/// Get animation duration
	int animationDuration() const;

	/// Get animation widget height
	int animationWidgetHeight() const;

	/// Set animation widget height
	void setAnimationWidgetHeight(int h);

	/// Set window normalGeometry, since this window has animation, to prevent size setting from being overwritten by animation during animation process
	void setNormalGeometry(const QRect& normalGeometry);

	/// Get normal geometry
	QRect normalGeometry() const;

	/// Send layout request to inner widgets
	void layoutRequestInnerWidgets();
Q_SIGNALS:
	/**
	 * \if ENGLISH
	 * @brief Hide window signal
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 隐藏窗口信号
	 * \endif
	 */
	void hidWindow();

protected:
	/// Handle show event
	void showEvent(QShowEvent* e) override;

	/// Handle hide event
	void hideEvent(QHideEvent* e) override;

	/// Handle resize event
	virtual void resizeEvent(QResizeEvent* e) override;

	/// Update inner widget geometry
	void updateInnerWidgetGeometry();
private Q_SLOTS:
	/// Animation finished slot function
	void onAnimationFinished();

	/// Setup animation
	void setupAnimation();
};

#endif  // SARIBBONSTACKEDWIDGET_H

/*** End of inlined file: SARibbonStackedWidget.h ***/


/*** Start of inlined file: SARibbonSeparatorWidget.h ***/
#ifndef SARIBBONSEPARATORWIDGET_H
#define SARIBBONSEPARATORWIDGET_H

#include <QFrame>
#include <QStyleOption>

/**
 * \if ENGLISH
 * @brief Used to display separator lines
 * \endif
 *
 * \if CHINESE
 * @brief 用于显示分割线
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonSeparatorWidget : public QFrame
{
	Q_OBJECT
public:
	/// Constructor for SARibbonSeparatorWidget
	explicit SARibbonSeparatorWidget(QWidget* parent = nullptr);
	/// Destructor for SARibbonSeparatorWidget
	~SARibbonSeparatorWidget();
};

#endif  // SARIBBONSEPARATORWIDGET_H

/*** End of inlined file: SARibbonSeparatorWidget.h ***/


/*** Start of inlined file: SARibbonCtrlContainer.h ***/
#ifndef SARIBBONCTROLCONTAINER_H
#define SARIBBONCTROLCONTAINER_H

#include <QWidget>
#include <QScopedPointer>
class QStyleOption;

/**
 * \if ENGLISH
 * @brief Container for holding a widget with optional icon and title
 * @details This container fills the entire SARibbonCtrlContainer but reserves space for displaying an icon or title
 * @par Layout:
 * ----------------------
 * |icon|text|  widget  |
 * ----------------------
 * \endif
 *
 * \if CHINESE
 * @brief 用于装载一个窗体的容器，这个窗体会布满整个SARibbonCtrlContainer，但会预留空间显示icon或者title
 * @par 布局:
 * ----------------------
 * |icon|text|  widget  |
 * ----------------------
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonCtrlContainer : public QWidget
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonCtrlContainer)
public:
	/// Constructor for SARibbonCtrlContainer
	explicit SARibbonCtrlContainer(QWidget* parent = nullptr);
	/// Destructor for SARibbonCtrlContainer
	~SARibbonCtrlContainer();

	/// Enable or disable icon display
	void setEnableShowIcon(bool b);
	/// Enable or disable title display
	void setEnableShowTitle(bool b);
	/// Check if container widget exists
	bool hasContainerWidget() const;
	/// Set the icon
	void setIcon(const QIcon& i);
	/// Set the icon from pixmap
	void setIcon(const QPixmap& pixmap);
	/// Get the icon
	QIcon icon() const;
	/// Set the text
	void setText(const QString& t);
	/// Get the text
	QString text() const;
	/// Set the container widget
	void setContainerWidget(QWidget* w);
	/// Get the container widget
	QWidget* containerWidget();
	/// Get the container widget (const)
	const QWidget* containerWidget() const;
	/// Get the widget displaying the icon
	QWidget* iconWidget() const;
};

#endif  // SARIBBONCTROLCONTAINER_H

/*** End of inlined file: SARibbonCtrlContainer.h ***/


/*** Start of inlined file: SARibbonQuickAccessBar.h ***/
#ifndef SARIBBONQUICKACCESSBAR_H
#define SARIBBONQUICKACCESSBAR_H

/**
 * \if ENGLISH
 * @brief Ribbon quick access toolbar in the top left corner
 * @details The SARibbonQuickAccessBar class provides a specialized toolbar that is typically placed in the top-left corner of the
 * @details ribbon interface. It contains frequently used commands that users can access quickly without navigating through ribbon tabs.
 * @details This toolbar supports adding regular actions as well as menu actions with different popup modes. The menu actions can
 * @details be configured with various popup behaviors such as InstantPopup, MenuButtonPopup, or DelayedPopup.
 * @details @par Features:
 * @details - Quick access to frequently used commands
 * @details - Support for menu actions with customizable popup modes
 * @details - Easy integration with ribbon interface
 * @details - Automatic management of action lifecycles
 * @details @par Usage:
 * @details @code
 * @details // Create quick access bar
 * @details SARibbonQuickAccessBar *quickAccessBar = ribbonBar()->quickAccessBar();
 * @details
 * @details // Add regular actions
 * @details quickAccessBar->addAction(saveAction);
 * @details quickAccessBar->addAction(undoAction);
 * @details
 * @details // Add menu actions
 * @details QMenu *fileMenu = new QMenu("File");
 * @details fileMenu->addAction(newAction);
 * @details fileMenu->addAction(openAction);
 * @details quickAccessBar->addMenuAction(fileMenu, QToolButton::InstantPopup);
 * @details @endcode
 * @details @note This class inherits from QToolBar, so all standard QToolBar features are available
 * @details @note Menu actions are automatically configured with appropriate tool button popup modes
 * @details @see SARibbonBar
 * \endif
 *
 * \if CHINESE
 * @brief ribbon左上顶部的快速响应栏
 * @details 此类提供了一个专门的工具栏，通常放置在ribbon界面的左上角。它包含常用的命令用户可以快速访问这些命令，而无需浏览ribbon选项卡。
 * @details 此工具栏支持添加常规动作以及具有不同弹出模式的菜单动作。菜单动作可以配置各种弹出行为，如InstantPopup、MenuButtonPopup或DelayedPopup。
 * @details @par 特点:
 * @details - 快速访问常用命令
 * @details - 支持具有可自定义弹出模式的菜单动作
 * @details - 与ribbon界面轻松集成
 * @details - 自动管理动作生命周期
 * @details @par 用法:
 * @details @code
 * @details // 创建快速访问栏
 * @details SARibbonQuickAccessBar *quickAccessBar = ribbonBar()->quickAccessBar();
 * @details
 * @details // 添加常规动作
 * @details quickAccessBar->addAction(saveAction);
 * @details quickAccessBar->addAction(undoAction);
 * @details
 * @details // 添加菜单动作
 * @details QMenu *fileMenu = new QMenu("File");
 * @details fileMenu->addAction(newAction);
 * @details fileMenu->addAction(openAction);
 * @details quickAccessBar->addMenuAction(fileMenu, QToolButton::InstantPopup);
 * @details @endcode
 * @details @note 此类继承自QToolBar，因此所有标准QToolBar功能都可用
 * @details @note 菜单动作会自动配置适当的工具按钮弹出模式
 * @details @see SARibbonBar
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonQuickAccessBar : public SARibbonButtonGroupWidget
{
	Q_OBJECT
public:
	/// Constructor for SARibbonQuickAccessBar
	explicit SARibbonQuickAccessBar(QWidget* parent = nullptr);
	/// Destructor for SARibbonQuickAccessBar
	~SARibbonQuickAccessBar();
};

#endif  // SARIBBONQUICKACCESSBAR_H

/*** End of inlined file: SARibbonQuickAccessBar.h ***/


/*** Start of inlined file: SARibbonTabBar.h ***/
#ifndef SARIBBONTABBAR_H
#define SARIBBONTABBAR_H

#include <QTabBar>
#include <QMargins>

/**
 * \if ENGLISH
 * @brief Ribbon tab bar widget for SARibbon framework
 * \endif
 *
 * \if CHINESE
 * @brief SARibbon框架的Ribbon标签栏部件
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonTabBar : public QTabBar
{
	Q_OBJECT
public:
	/// Constructs a SARibbonTabBar instance
	explicit SARibbonTabBar(QWidget* parent = nullptr);
	/// Destructor
	~SARibbonTabBar();
	/// Gets the tab margin
	const QMargins& tabMargin() const;
	/// Sets the tab margin
	void setTabMargin(const QMargins& tabMargin);

protected:
	/// Estimates the size of a tab
	QSize tabSizeHint(int index) const;

private:
	QMargins m_tabMargin;
};

#endif  // SARIBBONTABBAR_H

/*** End of inlined file: SARibbonTabBar.h ***/


/*** Start of inlined file: SARibbonMenu.h ***/
#ifndef SARIBBONMENU_H
#define SARIBBONMENU_H

#include <QMenu>

/**
 * \if ENGLISH
 * @brief Menu used in Ribbon interface
 * Can be styled via StyleSheet without affecting QMenu
 * \endif
 *
 * \if CHINESE
 * @brief 用在ribbon的menu
 * 可以通过StyleSheet设置样式而不影响QMenu
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonMenu : public QMenu
{
	Q_OBJECT
public:
	/// Constructor for SARibbonMenu
	explicit SARibbonMenu(QWidget* parent = nullptr);
	/// Constructor for SARibbonMenu with title
	explicit SARibbonMenu(const QString& title, QWidget* parent = nullptr);
	/// Destructor for SARibbonMenu
	~SARibbonMenu();
	/// Add a submenu to this menu
	QAction* addRibbonMenu(SARibbonMenu* menu);
	/// Create and add a submenu with the given title
	SARibbonMenu* addRibbonMenu(const QString& title);
	/// Create and add a submenu with the given icon and title
	SARibbonMenu* addRibbonMenu(const QIcon& icon, const QString& title);
	/// Add a custom widget to the menu
	QAction* addWidget(QWidget* w);
};

#endif  // SARIBBONMENU_H

/*** End of inlined file: SARibbonMenu.h ***/


/*** Start of inlined file: SARibbonTitleIconWidget.h ***/
#ifndef SARIBBONTITLEICONWIDGET_H
#define SARIBBONTITLEICONWIDGET_H

#include <QWidget>
#include <QMenu>
#include <QIcon>
#include <QPointer>
/**
 * @brief Ribbon风格的标题栏图标控件
 * 模拟Windows原生窗口左上角的程序图标功能，支持右键菜单和窗口控制操作
 */
class SA_RIBBON_EXPORT SARibbonTitleIconWidget : public QWidget
{
	Q_OBJECT
public:
	// 构造函数
	explicit SARibbonTitleIconWidget(QWidget* parent = nullptr);

	// 关联的窗口
	void setWidget(QWidget* window);
	QWidget* widget() const;

	// 图标
	void setIcon(const QIcon& icon);
	QIcon icon() const;

	// 图标大小
	void setIconSize(const QSize& size);
	QSize iconSize() const;

	// 内边距
	void setPadding(int v);
	int padding() const;

public:
	virtual QSize sizeHint() const override;

protected:
	// 绘制事件处理
	void paintEvent(QPaintEvent* event) override;

	// 鼠标按下事件处理
	void mousePressEvent(QMouseEvent* event) override;

	// 右键菜单事件处理
	void contextMenuEvent(QContextMenuEvent* event) override;

private:
	// 创建上下文菜单
	void createContextMenu();

	// 设置菜单动作
	void setupMenuActions();

private Q_SLOTS:
	// 还原窗口
	void onRestore();

	// 移动窗口
	void onMove();

	// 调整窗口大小
	void onSize();

	// 最小化窗口
	void onMinimize();

	// 最大化窗口
	void onMaximize();

	// 关闭窗口
	void onClose();

private:
	QPointer< QWidget > m_widget;  ///< 关联的窗口对象
	QIcon m_icon;                  ///< 显示的图标
	QSize m_iconSize;              ///< 图标大小
	QMenu* m_contextMenu;          ///< 上下文菜单
	int mPadding;
};

#endif  // SARIBBONTITLEICONWIDGET_H

/*** End of inlined file: SARibbonTitleIconWidget.h ***/


/*** Start of inlined file: SARibbonPanelOptionButton.h ***/
#ifndef SARIBBONPANELOPTIONBUTTON_H
#define SARIBBONPANELOPTIONBUTTON_H
#include <QToolButton>

class QAction;

/**
 * \if ENGLISH
 * @brief Operation button at the bottom right corner of the panel
 * @details This button is associated with an action, and the @ref SARibbonPanel::addOptionAction function is used to generate this button. Normally,
 * @details users do not need to directly operate this class, it is only used for style design
 * @details If you must override this button, you can implement a new OptionButton by overriding
 * @details @ref SARibbonElementFactory::createRibbonPanelOptionButton of @ref SARibbonElementFactory
 * \endif
 *
 * \if CHINESE
 * @brief Panel右下角的操作按钮
 * @details 此按钮和一个action关联，使用@ref SARibbonPanel::addOptionAction 函数用于生成此按钮，正常来说
 * @details 用户并不需要直接操作此类，仅仅用于样式设计
 * @details 如果一定要重载此按钮，可以通过重载@ref SARibbonElementFactory
 * @details 的 @ref SARibbonElementFactory::createRibbonPanelOptionButton来实现新的OptionButton
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonPanelOptionButton : public QToolButton
{
	Q_OBJECT
public:
	/// Constructor for SARibbonPanelOptionButton
	explicit SARibbonPanelOptionButton(QWidget* parent = nullptr);
	/// Destructor for SARibbonPanelOptionButton
	~SARibbonPanelOptionButton();
};

#endif  // SAROBBONPANELOPTIONBUTTON_H

/*** End of inlined file: SARibbonPanelOptionButton.h ***/


/*** Start of inlined file: SARibbonPanelItem.h ***/
#ifndef SARIBBONPANELITEM_H
#define SARIBBONPANELITEM_H

#include <QWidgetItem>
#include <QAction>
class SARibbonToolButton;
/**
 * \if ENGLISH
 * @brief Abstraction for all child windows of the panel, reference Qt's toolbar
 * @details Reference Qt's toolbar, all child window content of the panel is abstracted through QAction, including gallery windows, which are also abstracted through QAction
 * @details QAction will eventually be converted to SARibbonPanelItem, each SARibbonPanelItem contains a widget, and the layout of SARibbonPanel
 * @details is based on SARibbonPanelItem
 * @details Action without window will generate a SARibbonToolButton internally
 * \endif
 *
 * \if CHINESE
 * @brief 是对panel所有子窗口的抽象，参考qt的toolbar
 * @details 参考qt的toolbar，panel所有子窗口内容都通过QAction进行抽象，包括gallery这些窗口，也是通过QAction进行抽象
 * @details QAction最终会转换为SARibbonPanelItem，每个SARibbonPanelItem都含有一个widget，SARibbonPanel的布局
 * @details 就基于SARibbonPanelItem
 * @details 无窗口的action会在内部生成一个SARibbonToolButton
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonPanelItem : public QWidgetItem
{
public:
	/**
	 * \if ENGLISH
	 * @brief Defines the row proportion, there are three types of proportions in ribbon: large, medium and small
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 定义了行的占比，ribbon中有large，media和small三种占比
	 * \endif
	 */
	enum RowProportion
	{
		None,  ///< Undefined proportion, at this time it will be judged based on expandingDirections, if there is Qt::Vertical, it is equivalent to Large, otherwise it is Small
		Large,   ///< Large proportion, the height of a widget will fill the entire panel
		Medium,  ///< Medium proportion, only works when @ref SARibbonPanel::panelLayoutMode is @ref SARibbonPanel::ThreeRowMode, and when both in the same column are Medium, they will occupy two rows in three rows
		Small  ///< Small proportion, occupies one row of SARibbonPanel, Medium will also become Small when conditions are not met, but will not become Large
	};
	/// Constructor for SARibbonPanelItem
	explicit SARibbonPanelItem(QWidget* widget);
	/// Destructor for SARibbonPanelItem
	~SARibbonPanelItem();

	/// Check if the item is empty
	bool isEmpty() const Q_DECL_OVERRIDE;

	short rowIndex;             ///< Record which row the current item belongs to, -1 in hide mode
	int columnIndex;            ///< Record which column the current item belongs to, -1 in hide mode
	QRect itemWillSetGeometry;  ///< This will be updated when calling SARibbonPanelLayout::updateGeomArray, the actual setting will use QWidgetItem::setGeometry to set Geometry
	QAction* action;            /// < Record action, reference QToolBarLayoutItem
	bool customWidget;  ///< For action without window, there will actually be a SARibbonToolButton, which needs to be deleted during destruction
	SARibbonPanelItem::RowProportion rowProportion;  ///< Row proportion, there are three types of proportions in ribbon: large, medium and small, see @ref RowProportion
};
#ifndef SA_ActionPropertyName_RowProportion
#define SA_ActionPropertyName_RowProportion "_sa_RowProportion"
#endif
#ifndef SA_ActionPropertyName_ToolButtonPopupMode
#define SA_ActionPropertyName_ToolButtonPopupMode "_sa_ToolButtonPopupMode"
#endif
#ifndef SA_ActionPropertyName_ToolButtonStyle
#define SA_ActionPropertyName_ToolButtonStyle "_sa_ToolButtonStyle"
#endif
#endif  // SARIBBONPANELITEM_H

/*** End of inlined file: SARibbonPanelItem.h ***/


/*** Start of inlined file: SARibbonPanelLayout.h ***/
#ifndef SARIBBONPANELLAYOUT_H
#define SARIBBONPANELLAYOUT_H

#include <QLayout>

class QToolButton;
class SARibbonPanel;
class SARibbonPanelLabel;

/**
 * \if ENGLISH
 * @brief Layout manager for SARibbonPanel
 * @details `SARibbonPanelLayout` is a custom `QLayout` subclass responsible for arranging `SARibbonToolButton`
 * @details and other widgets within a `SARibbonPanel`. It handles the complex Ribbon-specific layout logic,
 * @details including Large, Medium, and Small button proportions, and the optional title and option button.
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonPanel的布局管理器
 * @details `SARibbonPanelLayout` 是一个自定义的 `QLayout` 子类，负责在 `SARibbonPanel` 内排列 `SARibbonToolButton`
 * @details 和其他窗口部件。它处理复杂的Ribbon特定布局逻辑，包括大、中、小按钮比例，以及可选的标题和选项按钮。
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonPanelLayout : public QLayout
{
	Q_OBJECT
	friend class SARibbonPanel;

public:
	/// Constructor for SARibbonPanelLayout
	explicit SARibbonPanelLayout(QWidget* p = nullptr);
	/// Destructor for SARibbonPanelLayout
	~SARibbonPanelLayout();
	/// Gets the SARibbonPanel that owns this layout
	SARibbonPanel* ribbonPanel() const;

	/// Inserts an action at a specific index
	void insertAction(int index, QAction* act, SARibbonPanelItem::RowProportion rp = SARibbonPanelItem::None);

	/// Sets the option action for the panel
	void setOptionAction(QAction* action);

	/// Checks if an option action is set
	bool isHaveOptionAction() const;

	/// Retrieves the SARibbonPanelItem associated with an action
	SARibbonPanelItem* panelItem(QAction* action) const;

	/// Gets the last item added to the layout
	SARibbonPanelItem* lastItem() const;

	/// Gets the widget associated with the last item
	QWidget* lastWidget() const;

	/// Moves an item from one index to another
	void move(int from, int to);
	/// Checks if layout needs to be reorganized
	bool isDirty() const;
	/// Updates size
	void updateGeomArray();

	/// Finds the index of an action in the layout
	int indexByAction(QAction* action) const;

	/// Gets the height of the panel's title
	int panelTitleHeight() const;

	/// Sets the height of the panel's title
	void setPanelTitleHeight(int newTitleHeight);

	/// Checks if the panel's title is enabled for display
	bool isEnableShowPanelTitle() const;

	/// Sets whether the panel's title is enabled for display
	void setEnableShowPanelTitle(bool on);

	/// Gets the height of large buttons
	int largeButtonHeight() const;

	/// Gets the spacing between the title and the buttons
	int panelTitleSpace() const;

	/// Sets the spacing between the title and the buttons
	void setPanelTitleSpace(int newTitleSpace);

	/// Gets the panel's title label
	SARibbonPanelLabel* panelTitleLabel() const;

	/// Sets the default icon size for tool buttons
	void setToolButtonIconSize(const QSize& smallSize, const QSize& largeSize);
	/// Gets the default icon size for tool buttons
	QPair< QSize, QSize > toolButtonIconSize() const;

	/// Large button icon size
	void setLargeIconSize(const QSize& largeSize);
	/// Get large icon size
	QSize largeIconSize() const;

	/// Small button icon size
	void setSmallIconSize(const QSize& largeSize);
	/// Get small icon size
	QSize smallIconSize() const;

	/// Check if word wrap is enabled
	bool isEnableWordWrap() const;
	/// Maximum aspect ratio of buttons, this coefficient determines the maximum width of buttons
	qreal buttonMaximumAspectRatio() const;

public:
	/// Adds an item to the layout (SARibbonPanelLayout not supported)
	void addItem(QLayoutItem* item) Q_DECL_OVERRIDE;

	/// QLayout required override functions
	QLayoutItem* itemAt(int index) const Q_DECL_OVERRIDE;

	/// Removes and returns the item at the specified index
	QLayoutItem* takeAt(int index) Q_DECL_OVERRIDE;

	/// Gets the number of items in the layout
	int count() const Q_DECL_OVERRIDE;

	/// Checks if the layout is empty
	bool isEmpty() const Q_DECL_OVERRIDE;

	/// Invalidates the layout, marking it as dirty
	void invalidate() Q_DECL_OVERRIDE;

	/// Returns the directions in which the layout can expand
	Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;
	/// Set geometry
	void setGeometry(const QRect& rect) Q_DECL_OVERRIDE;
	/// Get minimum size
	QSize minimumSize() const Q_DECL_OVERRIDE;
	/// Get size hint
	QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
	/// Gets the size of the option action button
	QSize optionActionButtonSize() const;
	/// Layout action
	void doLayout();
	/// Convert action to item, for pure Action, this function will create SARibbonToolButton
	/// rp is used to tell Layout what kind of window to generate, see SARibbonPanelItem::RowProportion for details
	SARibbonPanelItem* createItem(QAction* action, SARibbonPanelItem::RowProportion rp = SARibbonPanelItem::None);
	/// Update geometry array
	void updateGeomArray(const QRect& setrect);
	/// Recalculate expansion bar code, this function must be called after updateGeomArray function
	void recalcExpandGeomArray(const QRect& setrect);
	/// Set text wrap enabled
	void setEnableWordWrap(bool on);
	/// Set maximum aspect ratio of buttons, this coefficient determines the maximum width of buttons
	void setButtonMaximumAspectRatio(qreal fac = 1.4);

private:
	/// Calculate window width and maximum width based on column count
	void columnWidthInfo(int colindex, int& width, int& maximum) const;

	/// Sets the panel's title label
	void setPanelTitleLabel(SARibbonPanelLabel* newTitleLabel);

private:
	QList< SARibbonPanelItem* > mItems;
	int mColumnCount { 0 };                       ///< 记录有多少列
	bool mExpandFlag { false };                   ///< 标记是否是会扩展的
	QSize mSizeHint;                              ///< sizeHint返回的尺寸
	QSize mSmallToolButtonIconSize { 22, 22 };    ///< 记录小按钮图标尺寸
	QSize mLargeToolButtonIconSize { 32, 32 };    ///< 记录大按钮图标尺寸
	bool mDirty { true };                         ///< 用于标记是否需要刷新元素，参考QToolBarLayout源码
	int mLargeHeight { 0 };                       ///< 记录大图标的高度
	int mTitleHeight { 15 };                      ///< 标题区域高度
	int mTitleSpace { 2 };                        ///< 标题区域和按钮的间隔
	bool mEnableShowTitle { true };               ///< 是否运行显示panel标题
	SARibbonPanelLabel* mTitleLabel { nullptr };  ///< titlelabel指针
	QRect mTitleLabelGeometry;                    ///< titlelabel的位置
	QToolButton* mOptionActionBtn { nullptr };    ///< optionAction对应的button
	QRect mOptionActionBtnGeometry;               ///< optionAction的位置
	bool mEnableWordWrap { true };                ///< 是否允许文字换行
	qreal mButtonMaximumAspectRatio { 1.4 };      ///< 按钮的宽高比
};

#endif  // SARIBBONPANELLAYOUT_H

/*** End of inlined file: SARibbonPanelLayout.h ***/


/*** Start of inlined file: SARibbonPanel.h ***/
#ifndef SARIBBONPANEL_H
#define SARIBBONPANEL_H

#include <QLayout>
#include <QWidget>
#include <QLabel>
class SARibbonMenu;
class SARibbonGallery;
class QGridLayout;
class SARibbonPanelOptionButton;
class SARibbonPanelLayout;
class SARibbonCategory;
class SARibbonBar;

/**
 * \if ENGLISH
 * @brief Title label for SARibbonPanel, used for qss styling
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonPanel的标题label，此类用于qss
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonPanelLabel : public QLabel
{
	Q_OBJECT
public:
	/// Constructor for SARibbonPanelLabel
	SARibbonPanelLabel(QWidget* parent = nullptr);
};

/**
 * @brief panel页窗口，panel是ribbon的面板用于承放控件 / Ribbon panel container, used to hold controls
 *
 * The `SARibbonPanel` is a core component of the Ribbon interface. It acts as a container for `QAction` objects,
 * automatically creating and managing `SARibbonToolButton` instances based on the specified layout proportion (Large, Medium, Small).
 * It supports dynamic layout modes (ThreeRowMode, TwoRowMode) and can display an optional title and an option button.
 *
 * `SARibbonPanel` 是 Ribbon 界面的核心组件之一。它作为 `QAction` 对象的容器，
 * 根据指定的布局比例（大、中、小）自动创建和管理 `SARibbonToolButton` 实例。
 * 它支持动态布局模式（三行模式、两行模式），并可显示可选的标题和选项按钮。
 *
 * ---
 *
 * ### 布局模式 / Layout Modes
 * The panel has two layout modes: two-row and three-row.
 * - **ThreeRowMode**: The Ribbon in Microsoft Office is a representative.
 * - **TwoRowMode**: The "compact style" represented by WPS.
 * You can specify the mode via @ref SARibbonBar::RibbonStyle (using @ref SARibbonBar::setRibbonStyle).
 *
 * ribbon的panel分为两行模式和三行模式。
 * - **三行模式**: 以 Microsoft Office 为代表。
 * - **两行模式**: 以 WPS 为代表的“紧凑派”。
 * 可通过 @ref SARibbonBar::RibbonStyle (使用 @ref SARibbonBar::setRibbonStyle 函数) 来指定模式。
 *
 * ---
 *
 * ### 扩展模式 / Expanding Mode
 * Within a panel, you can use @ref setExpanding to specify horizontal expansion.
 * **Recommendation**: Only use this if the panel contains expandable widgets (e.g., @ref SARibbonGallery).
 * Otherwise, blank space will be left.
 *
 * 在panel中，可以通过@ref setExpanding 函数指定panel水平扩展。
 * **建议**: 仅在面板内包含可扩展控件（如 @ref SARibbonGallery）时使用此函数。
 * 否则，将会留白。
 *
 * ---
 *
 * ### 自定义布局 / Custom Layout
 * The panel's layout is implemented by @ref SARibbonPanelLayout.
 * To use a custom layout, inherit from @ref SARibbonElementCreateDelegate::createRibbonPanel.
 *
 * panel的布局通过@ref SARibbonPanelLayout 来实现。
 * 如需自定义布局，请继承 @ref SARibbonElementCreateDelegate::createRibbonPanel 函数。
 *
 * ---
 *
 * ### 全局属性设置 / Global Property Settings
 * `SARibbonPanel` is a child of `SARibbonCategory`, which belongs to `SARibbonBar`.
 * **Important**: Setting properties directly on a standalone `SARibbonPanel` is often meaningless.
 * Global properties (like title height, spacing) are managed by `SARibbonBar`.
 * Use these functions in `SARibbonBar` instead:
 * - `panelTitleHeight`
 * - `isEnableShowPanelTitle`
 * - `panelSpacing`
 * - `panelToolButtonIconSize`
 *
 * `SARibbonPanel`属于`SARibbonCategory`的子对象，`SARibbonCategory`又属于`SARibbonBar`的对象。
 * **重要**: 单独设置一个`SARibbonPanel`的属性很多情况是没有意义的。
 * 全局性属性（如标题高度、间距）由`SARibbonBar`统一管理。
 * 请改用`SARibbonBar`中的以下函数：
 * - `panelTitleHeight`
 * - `isEnableShowPanelTitle`
 * - `panelSpacing`
 * - `panelToolButtonIconSize`
 */
class SA_RIBBON_EXPORT SARibbonPanel : public QFrame
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonPanel)
	friend class SARibbonBar;
	friend class SARibbonCategory;
	friend class SARibbonCategoryPrivate;
	friend class SARibbonCustomizeWidgetPrivate;
	friend class SARibbonPanelLayout;
	Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
	Q_PROPERTY(bool isExpanding READ isExpanding WRITE setExpanding)
	Q_PROPERTY(QString panelName READ panelName WRITE setPanelName)
public:
	/**
	 * @brief Layout mode for the panel / 面板的布局模式
	 */
	enum PanelLayoutMode
	{
		/**
		 * @brief Three-row layout mode / 三行布局模式
		 *
		 * In this mode, small buttons are arranged in three rows, while medium buttons occupy two rows.
		 * Large buttons always occupy the full height.
		 *
		 * 在此模式下，小按钮排列成三行，中等按钮占据两行。
		 * 大按钮始终占据整个高度。
		 */
		ThreeRowMode,

		/**
		 * @brief Two-row layout mode / 两行布局模式
		 *
		 * In this mode, both small and medium buttons are arranged in two rows.
		 * Large buttons always occupy the full height.
		 *
		 * 在此模式下，小按钮和中等按钮都排列成两行。
		 * 大按钮始终占据整个高度。
		 */
		TwoRowMode,

		/**
		 * \if ENGLISH
		 * @brief Single-row layout mode
		 *
		 * In this mode, all buttons are arranged in a single horizontal row.
		 * Large, Medium and Small row proportions all behave as Small.
		 * Panel titles are hidden by default.
		 * \endif
		 *
		 * \if CHINESE
		 * @brief 单行布局模式
		 *
		 * 在此模式下，所有按钮排列在单行水平行中。
		 * Large、Medium 和 Small 行占比都等效为 Small。
		 * Panel 标题默认隐藏。
		 * \endif
		 */
		SingleRowMode
	};
	Q_ENUM(PanelLayoutMode)
public:
	using FpRibbonToolButtonIterate = std::function< bool(SARibbonToolButton*) >;

public:
	/// Constructor for SARibbonPanel
	explicit SARibbonPanel(QWidget* parent = nullptr);
	/// Constructor for SARibbonPanel with name
	explicit SARibbonPanel(const QString& name, QWidget* parent = nullptr);
	/// Destructor for SARibbonPanel
	~SARibbonPanel() Q_DECL_OVERRIDE;
	using QWidget::addAction;

	/// Add action to panel with specified row proportion
	void addAction(QAction* action, SARibbonPanelItem::RowProportion rowProportion);
	/// Generate and add an action with specified popup mode and row proportion
	void addAction(QAction* act,
				   QToolButton::ToolButtonPopupMode popMode,
				   SARibbonPanelItem::RowProportion rowProportion = SARibbonPanelItem::Large);
	/// Add action to panel with large icon display
	void addLargeAction(QAction* action);
	/// Add action to panel with medium icon display in three-row mode
	void addMediumAction(QAction* action);
	/// Add action to panel with small icon display
	void addSmallAction(QAction* action);

	/// Add action to panel with small icon display and specified popup mode
	void addSmallAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
	/// Add action to panel with large icon display and specified popup mode
	void addLargeAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
	/// Add action to panel with medium icon display in three-row mode and specified popup mode
	void addMediumAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
	/// Generate and add an action with text, icon, popup mode, and row proportion
	QAction* addAction(const QString& text,
					   const QIcon& icon,
					   QToolButton::ToolButtonPopupMode popMode,
					   SARibbonPanelItem::RowProportion rowProportion = SARibbonPanelItem::Large);

	/// Add menu with specified row proportion and popup mode
	void addMenu(QMenu* menu,
				 SARibbonPanelItem::RowProportion rowProportion,
				 QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
	/// Add large menu with specified popup mode
	void addLargeMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
	/// Add medium menu with specified popup mode
	void addMediumMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
	/// Add small menu with specified popup mode
	void addSmallMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

	/// Add widget with specified row proportion
	QAction* addWidget(QWidget* w, SARibbonPanelItem::RowProportion rowProportion);

	/// Add small widget (occupies one row of ribbon)
	QAction* addSmallWidget(QWidget* w);

	/// Add medium widget (occupies one row of ribbon)
	QAction* addMediumWidget(QWidget* w);

	/// Add large widget (occupies all rows)
	QAction* addLargeWidget(QWidget* w);

	/// Add a gallery
	SARibbonGallery* addGallery(bool expanding = true);

	/// Add separator
	QAction* addSeparator();

	/// Extract the button corresponding to the action from the panel, returns nullptr if no corresponding button
	SARibbonToolButton* actionToRibbonToolButton(QAction* action);

	/// Set option action, pass nullptr to remove, SARibbonPanel does not manage QAction ownership
	/// OptionAction also triggers actionTriggered signal
	void setOptionAction(QAction* action);

	/// Check if option action exists
	bool isHaveOptionAction() const;

	/// Get all buttons
	QList< SARibbonToolButton* > ribbonToolButtons() const;

	/// Get PanelLayoutMode
	PanelLayoutMode panelLayoutMode() const;
	/// Set PanelLayoutMode
	void setPanelLayoutMode(PanelLayoutMode mode);

	/// Update button sizes, this function needs to be called after panel layout state changes
	void resetToolButtonSize();

	/// Check if it is two-row mode
	bool isTwoRow() const;

	/// Set panel to expanding mode, which will expand the horizontal area
	void setExpanding(bool isExpanding = true);
	/// Check if it is expanding mode
	bool isExpanding() const;

	/// Title bar height
	int titleHeight() const;

	/// Check if title is enabled, after enabling title, the title height needs to be set, default height is 15
	bool isEnableShowTitle() const;

	/// Layout index corresponding to action, this operation is generally used for moveAction, other meanings are not significant
	int actionIndex(QAction* act) const;

	/// Move action
	void moveAction(int from, int to);

	/// Check if customization is allowed
	bool isCanCustomize() const;
	/// Set customization allowed
	void setCanCustomize(bool b);

	/// Panel name
	QString panelName() const;
	/// Set panel name
	void setPanelName(const QString& title);

	/// Large button height
	int largeButtonHeight() const;

	/// Get layout corresponding items, this function is currently only used in the customization process
	const QList< SARibbonPanelItem* >& ribbonPanelItem() const;

	/// Get panel layout
	SARibbonPanelLayout* panelLayout() const;

	/// Update layout
	void updateItemGeometry();

	/// Get category pointer, returns nullptr if no parent or not managed by category
	SARibbonCategory* category() const;

	/// Get ribbonBar pointer, returns nullptr if none
	SARibbonBar* ribbonBar() const;

	/// Spacing between buttons
	int spacing() const;

	/// Set button icon sizes
	void setToolButtonIconSize(const QSize& smallSize, const QSize& largeSize);
	/// Get button icon sizes
	QPair< QSize, QSize > toolButtonIconSize() const;

	/// Large button icon size
	void setLargeIconSize(const QSize& largeSize);
	/// Get large icon size
	QSize largeIconSize() const;

	/// Small button icon size
	void setSmallIconSize(const QSize& smallSize);
	/// Get small icon size
	QSize smallIconSize() const;

	/// Get last added button
	SARibbonToolButton* lastAddActionButton();

	/// Get panel title label widget
	SARibbonPanelLabel* titleLabel() const;

	/// Check if word wrap is enabled
	bool isEnableWordWrap() const;

	/// Set whether button text is displayed to the right of the icon
	void setEnableIconRightText(bool on);
	/// Check if icon-right-text mode is enabled
	bool isEnableIconRightText() const;

	/// Maximum aspect ratio of buttons, this coefficient determines the maximum width of buttons
	qreal buttonMaximumAspectRatio() const;

	/// This function will iterate through all RibbonToolButton under SARibbonPanel, execute function pointer
	/// (bool(SARibbonRibbonToolButton*)), function pointer returns false to stop iteration
	bool iterateButton(FpRibbonToolButtonIterate fp) const;

public:
	/// Get size hint
	virtual QSize sizeHint() const Q_DECL_OVERRIDE;
	/// Get minimum size hint
	virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

public:
	/// Recommended panel height
	static int panelHeightHint(const QFontMetrics& fm, PanelLayoutMode layMode, int panelTitleHeight);
	/// Set action row proportion property into action, action itself carries row property
	static void setActionRowProportionProperty(QAction* action, SARibbonPanelItem::RowProportion rp);
	/// Get action row proportion property
	static SARibbonPanelItem::RowProportion getActionRowProportionProperty(QAction* action);
	/// Set action PopupMode property into action, action itself carries PopupMode property
	static void setActionToolButtonPopupModeProperty(QAction* action, QToolButton::ToolButtonPopupMode popMode);
	/// Get action PopupMode property
	static QToolButton::ToolButtonPopupMode getActionToolButtonPopupModeProperty(QAction* action);
	/// Set action ToolButtonStyle property into action, action itself carries ToolButtonStyle property
	static void setActionToolButtonStyleProperty(QAction* action, Qt::ToolButtonStyle buttonStyle);
	/// Get action ToolButtonStyle property
	static Qt::ToolButtonStyle getActionToolButtonStyleProperty(QAction* action);

Q_SIGNALS:

	/**
	 * \if ENGLISH
	 * @brief Equivalent to QToolBar::actionTriggered
	 * @param action
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 等同于QToolBar::actionTriggered
	 * @param action
	 * \endif
	 */
	void actionTriggered(QAction* action);

	/**
	 * \if ENGLISH
	 * @brief Emitted when the panel's name changes
	 * @param n The new name
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 当面板名称改变时发出
	 * @param n 新的名称
	 * \endif
	 */
	void panelNameChanged(const QString& n);

protected:
	/// Handle action events
	virtual void actionEvent(QActionEvent* e) Q_DECL_OVERRIDE;
	/// Handle change events
	virtual void changeEvent(QEvent* e) Q_DECL_OVERRIDE;

protected:
	// The following methods are for receiving parameters from the corresponding interfaces of SARibbonBar, users do not
	// need to set them when operating SARibbonPanel When SARibbonPanel is set to SARibbonCategory, SARibbonCategory
	// will resynchronize its own parameters to the panel Similarly, when SARibbonCategory is added to SARibbonBar,
	// SARibbonBar will resynchronize its own parameters to the category

	/// Set title bar height
	void setTitleHeight(int h);

	/// Set spacing between buttons
	void setSpacing(int n);

	/// Set whether to show title, after showing title, the title height needs to be set, default height is 15
	void setEnableShowTitle(bool on);

	/// Set whether buttons are allowed to wrap, note that the icon size is determined by the text, two lines of text
	/// will make the icon smaller, if you want the icon to be larger, it is best not to wrap the text
	void setEnableWordWrap(bool on);

	/// Set the maximum aspect ratio of buttons, this coefficient determines the maximum width of buttons
	void setButtonMaximumAspectRatio(qreal fac = 1.4);
};

#endif  // SARIBBONPANEL_H

/*** End of inlined file: SARibbonPanel.h ***/


/*** Start of inlined file: SARibbonCategory.h ***/
#ifndef SARIBBONCATEGORY_H
#define SARIBBONCATEGORY_H

#include <QFrame>

#include <QScopedPointer>
#include <QPushButton>
#include <QWheelEvent>

class QAction;
class QHBoxLayout;
class QWheelEvent;
class SARibbonBar;
class SARibbonCategoryLayout;

/**
 * \if ENGLISH
 * @brief Ribbon category page containing multiple panels
 *
 * Each Category represents a tab page in the Ribbon, containing multiple panels (SARibbonPanel).
 * It acts as a container for organizing related actions and controls into logical groups.
 * \endif
 *
 * \if CHINESE
 * @brief 包含多个面板的Ribbon类别页面
 *
 * 每个Category代表Ribbon中的一个标签页，包含多个面板（SARibbonPanel）。
 * 它作为一个容器，用于将相关的操作和控件组织成逻辑组。
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonCategory : public QFrame
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonCategory)
	friend class SARibbonBar;
	friend class SARibbonContextCategory;
	Q_PROPERTY(bool isCanCustomize READ isCanCustomize WRITE setCanCustomize)
	Q_PROPERTY(QString categoryName READ categoryName WRITE setCategoryName)
public:
	using FpPanelIterate = std::function< bool(SARibbonPanel*) >;

public:
	/// Constructor
	explicit SARibbonCategory(QWidget* p = nullptr);
	/// Constructor with name
	explicit SARibbonCategory(const QString& name, QWidget* p = nullptr);
	/// Destructor
	~SARibbonCategory();

	/// Get the category name
	QString categoryName() const;

	/// Set the category name
	void setCategoryName(const QString& title);

	/// Get the panel layout mode
	SARibbonPanel::PanelLayoutMode panelLayoutMode() const;

	/// Set the panel layout mode
	void setPanelLayoutMode(SARibbonPanel::PanelLayoutMode m);

	/// Add a panel with title
	SARibbonPanel* addPanel(const QString& title);

	/// Add an existing panel
	void addPanel(SARibbonPanel* panel);

	/// Add panel for Qt Designer
	Q_INVOKABLE void addPanel(QWidget* panel);

	/// Create and insert a new panel at index position
	SARibbonPanel* insertPanel(const QString& title, int index);

	/// Insert an existing panel at index position
	void insertPanel(SARibbonPanel* panel, int index);

	/// Find panel by name
	SARibbonPanel* panelByName(const QString& title) const;

	/// Find panel by ObjectName
	SARibbonPanel* panelByObjectName(const QString& objname) const;

	/// Find panel by index, returns nullptr if out of range
	SARibbonPanel* panelByIndex(int index) const;

	/// Get the index of a panel
	int panelIndex(SARibbonPanel* p) const;

	/// Move a panel from one index to another
	void movePanel(int from, int to);

	/// Detach panel from SARibbonCategory management
	bool takePanel(SARibbonPanel* panel);

	/// Remove panel and delete it
	bool removePanel(SARibbonPanel* panel);

	/// Remove panel by index
	bool removePanel(int index);

	/// Get all panels
	QList< SARibbonPanel* > panelList() const;

	/// Check if this is a context category
	bool isContextCategory() const;

	/// Get the number of panels
	int panelCount() const;

	/// Check if customization is allowed
	bool isCanCustomize() const;
	/// Set whether customization is allowed
	void setCanCustomize(bool b);

	/// Get panel title bar height
	int panelTitleHeight() const;
	/// Set panel title bar height
	void setPanelTitleHeight(int h);

	/// Check if panel title bar is displayed
	bool isEnableShowPanelTitle() const;
	/// Set whether to display panel title
	void setEnableShowPanelTitle(bool on);

	/// Set category alignment
	void setCategoryAlignment(SARibbonAlignment al);
	/// Get category alignment
	SARibbonAlignment categoryAlignment() const;

	/// Set panel spacing
	void setPanelSpacing(int n);
	/// Get panel spacing
	int panelSpacing() const;

	/// Set panel large icon size
	void setPanelLargeIconSize(const QSize& largeSize);
	/// Get panel large icon size
	QSize panelLargeIconSize() const;

	/// Set panel small icon size
	void setPanelSmallIconSize(const QSize& smallSize);
	/// Get panel small icon size
	QSize panelSmallIconSize() const;

	/// Set panel tool button icon sizes
	void setPanelToolButtonIconSize(const QSize& smallSize, const QSize& largeSize);
	/// Get panel tool button icon sizes
	QPair< QSize, QSize > panelToolButtonIconSize() const;

	/// Get the parent ribbonbar, returns null if not managed
	SARibbonBar* ribbonBar() const;

	/// Refresh category layout, call after changing ribbon mode
	void updateItemGeometry();

	/// Set whether to use animation when scrolling
	void setUseAnimatingScroll(bool useAnimating);
	/// Check if animation is used when scrolling
	bool isUseAnimatingScroll() const;

	/// Set wheel scroll step in pixels
	void setWheelScrollStep(int step);
	/// Get wheel scroll step
	int wheelScrollStep() const;

	/// Set animation duration in milliseconds
	void setAnimationDuration(int duration);
	/// Get animation duration in milliseconds
	int animationDuration() const;

	/// Check if panel text word wrap is enabled
	bool isEnableWordWrap() const;

	/// Check if icon-right-text mode is enabled
	bool isEnableIconRightText() const;

	/// Get button maximum aspect ratio
	qreal buttonMaximumAspectRatio() const;

	/// Iterate through all panels
	bool iteratePanel(FpPanelIterate fp) const;

	QSize sizeHint() const Q_DECL_OVERRIDE;
Q_SIGNALS:
	/// Emitted when category name changes
	void categoryNameChanged(const QString& n);

	/// Emitted when an action is triggered
	void actionTriggered(QAction* action);

protected:
	virtual bool event(QEvent* e) override;
	/// Handle wheel event
	void wheelEvent(QWheelEvent* event) override;
	/// Handle change event
	void changeEvent(QEvent* event) override;

	/// Mark this as a context category
	void markIsContextCategory(bool isContextCategory = true);

	/// Get the category layout
	SARibbonCategoryLayout* categoryLayout() const;

	/// Set whether panel button text word wrap is enabled
	void setEnableWordWrap(bool on);

	/// Set whether button text is displayed to the right of the icon
	void setEnableIconRightText(bool on);

	/// Set button maximum aspect ratio
	void setButtonMaximumAspectRatio(qreal fac = 1.4);
};

/// Scroll button for SARibbonCategory when content exceeds width
class SA_RIBBON_EXPORT SARibbonCategoryScrollButton : public QToolButton
{
	Q_OBJECT
public:
	explicit SARibbonCategoryScrollButton(Qt::ArrowType arr, QWidget* p = nullptr);
	~SARibbonCategoryScrollButton();
};

#endif  // SARIBBONCATEGORY_H

/*** End of inlined file: SARibbonCategory.h ***/


/*** Start of inlined file: SARibbonCategoryLayout.h ***/
#ifndef SARIBBONCATEGORYLAYOUT_H
#define SARIBBONCATEGORYLAYOUT_H

#include <QLayout>
#include <QList>
#include <QMap>

class SARibbonPanel;
class SARibbonCategoryLayoutItem;
class SARibbonSeparatorWidget;

/**
 * \if ENGLISH
 * @brief Layout class for SARibbonCategory
 * @details This class handles the layout of panels in a SARibbonCategory, including scrolling and animation support
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonCategory 的布局类
 * @details 此类处理 SARibbonCategory 中面板的布局，包括滚动和动画支持
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonCategoryLayout : public QLayout
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonCategoryLayout)
public:
	Q_PROPERTY(int scrollPosition READ scrollPosition WRITE setScrollPosition)
public:
	/// Constructor for SARibbonCategoryLayout
	explicit SARibbonCategoryLayout(SARibbonCategory* parent);
	/// Destructor for SARibbonCategoryLayout
	~SARibbonCategoryLayout();

	/// Get the parent SARibbonCategory
	SARibbonCategory* ribbonCategory() const;

	/// Add a layout item (overridden from QLayout)
	virtual void addItem(QLayoutItem* item) override;
	/// Get the layout item at the specified index (overridden from QLayout)
	virtual QLayoutItem* itemAt(int index) const override;
	/// Take the layout item at the specified index (overridden from QLayout)
	virtual QLayoutItem* takeAt(int index) override;
	/// Take the panel item at the specified index
	SARibbonCategoryLayoutItem* takePanelItem(int index);
	/// Take the panel item for the specified panel
	SARibbonCategoryLayoutItem* takePanelItem(SARibbonPanel* panel);
	/// Remove a panel and its separator
	bool takePanel(SARibbonPanel* panel);
	/// Get the number of layout items (overridden from QLayout)
	virtual int count() const override;
	/// Set the geometry of the layout (overridden from QLayout)
	void setGeometry(const QRect& rect) override;
	/// Get the size hint of the layout (overridden from QLayout)
	QSize sizeHint() const override;
	/// Get the minimum size of the layout (overridden from QLayout)
	QSize minimumSize() const override;
	/// Get the expanding directions of the layout (overridden from QLayout)
	Qt::Orientations expandingDirections() const override;
	/// Invalidate the layout (overridden from QLayout)
	void invalidate() override;
	/// Add a panel to the layout
	void addPanel(SARibbonPanel* panel);
	/// Insert a panel at the specified index
	void insertPanel(int index, SARibbonPanel* panel);
	/// Get the content size of the category
	QSize categoryContentSize() const;
	/// Update geometry of the layout
	void updateGeometryArr();

	/// Execute layout adjustment
	void doLayout();

	/// Get all panels in the layout
	QList< SARibbonPanel* > panels() const;

	/// Find a panel by its object name
	SARibbonPanel* panelByObjectName(const QString& objname) const;
	/// Find a panel by its name
	SARibbonPanel* panelByName(const QString& panelname) const;
	/// Find a panel by its index
	SARibbonPanel* panelByIndex(int i) const;
	/// Move a panel from one position to another
	void movePanel(int from, int to);
	/// Get the number of panels
	int panelCount() const;
	/// Find the index of a panel
	int panelIndex(SARibbonPanel* p) const;
	/// Get all panels in the layout
	QList< SARibbonPanel* > panelList() const;
	/// Execute scrolling
	void scroll(int px);
	/// Scroll to a specified position
	void scrollTo(int targetX);
	/// Animate scrolling by a specified distance
	void scrollByAnimate(int px);
	/// Animate scrolling to a specified position
	void scrollToByAnimate(int targetX);
	/// Get the current scroll position
	int scrollPosition() const;
	/// Set the scroll position
	void setScrollPosition(int pos);
	/// Check if scrolling animation is in progress
	bool isAnimatingScroll() const;
	/// Check if the layout has been scrolled
	bool isScrolled() const;
	/// Get the total width of the content
	int categoryTotalWidth() const;
	/// Set the alignment of the category
	void setCategoryAlignment(SARibbonAlignment al);
	/// Get the alignment of the category
	SARibbonAlignment categoryAlignment() const;
	/// Set the duration of the animation
	void setAnimationDuration(int duration);
	/// Get the duration of the animation
	int animationDuration() const;
private Q_SLOTS:
	void onLeftScrollButtonClicked();
	void onRightScrollButtonClicked();

private:
	void setupAnimateScroll();
};

/**
 * \if ENGLISH
 * @brief Layout item for SARibbonCategoryLayout
 * @details This class is used to identify items in SARibbonCategoryLayout, including panels and their associated separators
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonCategoryLayoutItem，用于标识SARibbonCategoryLayout的item
 * @details 此类用于标识 SARibbonCategoryLayout 中的项目，包括面板及其关联的分隔符
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonCategoryLayoutItem : public QWidgetItem
{
public:
	/// Constructor for SARibbonCategoryLayoutItem
	explicit SARibbonCategoryLayoutItem(SARibbonPanel* w);
	/// Destructor for SARibbonCategoryLayoutItem
	~SARibbonCategoryLayoutItem();
	SARibbonSeparatorWidget* separatorWidget;
	/// Convert the internal widget to a SARibbonPanel
	SARibbonPanel* toPanelWidget();
	QRect mWillSetGeometry;           ///< panel将要设置的Geometry
	QRect mWillSetSeparatorGeometry;  ///< panel将要设置的Separator的Geometry
};
#endif  // SARIBBONCATEGORYLAYOUT_H

/*** End of inlined file: SARibbonCategoryLayout.h ***/


/*** Start of inlined file: SARibbonContextCategory.h ***/
#ifndef SARIBBONCONTEXTCATEGORY_H
#define SARIBBONCONTEXTCATEGORY_H

#include <QWidget>

/**
 * \if ENGLISH
 * @brief Class for managing context categories
 * @details This class manages context categories in the ribbon bar, allowing grouping of related category pages
 * \endif
 *
 * \if CHINESE
 * @brief 管理上下文标签的类
 * @details 此类管理 Ribbon 栏中的上下文标签，允许对相关标签页进行分组
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonContextCategory : public QObject
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonContextCategory)
public:
	/// Constructor for SARibbonContextCategory
	explicit SARibbonContextCategory(QWidget* parent = nullptr);
	/// Destructor for SARibbonContextCategory
	~SARibbonContextCategory();
	/// Add a category page with title
	SARibbonCategory* addCategoryPage(const QString& title);
	/// Add an existing category page
	void addCategoryPage(SARibbonCategory* category);
	/// Get the number of managed category pages
	int categoryCount() const;

	/// Set the context ID
	void setId(const QVariant& id);
	/// Get the context ID
	QVariant id() const;

	/// Set the context color
	void setContextColor(const QColor color);
	/// Get the context color
	QColor contextColor() const;

	/// Get the context title
	QString contextTitle() const;
	/// Set the context title
	void setContextTitle(const QString& contextTitle);

	/// Get the category page at the specified index
	SARibbonCategory* categoryPage(int index);

	/// Get all SARibbonCategory objects
	QList< SARibbonCategory* > categoryList() const;

	/// Remove a category from management
	bool takeCategory(SARibbonCategory* category);

	/// Check if the context manages this category
	bool isHaveCategory(SARibbonCategory* category) const;
public Q_SLOTS:
	/// Hide the context category
	void hide();

	/// Show the context category
	void show();
Q_SIGNALS:
	/**
	 * \if ENGLISH
	 * @brief Signal emitted when a category page is added to the context
	 * @param category The added category page
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 标签加入上下文
	 * @param category 添加的标签页
	 * \endif
	 */
	void categoryPageAdded(SARibbonCategory* category);

	/**
	 * \if ENGLISH
	 * @brief Signal emitted when a category page is removed from the context
	 * @param category The removed category page
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 标签从上下文移除
	 * @param category 移除的标签页
	 * \endif
	 */
	void categoryPageRemoved(SARibbonCategory* category);

	/**
	 * \if ENGLISH
	 * @brief Signal emitted when the context title changes
	 * @param title The new context title
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 上下文的标题发生改变
	 * @param title 新的上下文标题
	 * \endif
	 */
	void contextTitleChanged(const QString& title);

	/**
	 * \if ENGLISH
	 * @brief Signal emitted when a category page title changes
	 * @param category The category page that changed
	 * @param title The new title
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 上下文标签维护的标签页名字发生了改变
	 * @param category 发生改变的上下文标签页
	 * @param title 新名字
	 * \endif
	 */
	void categoryTitleChanged(SARibbonCategory* category, const QString& title);
private Q_SLOTS:
	void onCategoryTitleChanged(const QString& title);

protected:
	/// Get the parent widget
	QWidget* parentWidget() const;
	virtual bool eventFilter(QObject* watched, QEvent* e) override;
};

#endif  // SARIBBONCONTEXTCATEGORY_H

/*** End of inlined file: SARibbonContextCategory.h ***/


/*** Start of inlined file: SARibbonGalleryItem.h ***/
#ifndef SARIBBONGALLERYITEM_H
#define SARIBBONGALLERYITEM_H

#include <QIcon>
#include <QVariant>
#include <QMap>
#include <QAction>
class SARibbonGalleryGroup;

/**
 * \if ENGLISH
 * @brief GalleryItem similar to QStandardItem
 * \endif
 *
 * \if CHINESE
 * @brief 类似QStandardItem的GalleryItem
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonGalleryItem
{
	friend class SARibbonGalleryGroupModel;

public:
	/// Default constructor
	explicit SARibbonGalleryItem();
	/// Constructor with text and icon
	explicit SARibbonGalleryItem(const QString& text, const QIcon& icon);
	/// Constructor with QAction
	explicit SARibbonGalleryItem(QAction* act);
	/// Destructor
	virtual ~SARibbonGalleryItem();
	/// Set data for role
	void setData(int role, const QVariant& data);
	/// Get data for role
	virtual QVariant data(int role) const;

	/// Set text
	void setText(const QString& text);
	/// Get text
	QString text() const;

	/// Set tool tip
	void setToolTip(const QString& text);
	/// Get tool tip
	QString toolTip() const;

	/// Set icon
	void setIcon(const QIcon& ico);
	/// Get icon
	QIcon icon() const;

	/// Check if selectable
	bool isSelectable() const;
	/// Set selectable
	void setSelectable(bool isSelectable);

	/// Check if enabled
	bool isEnable() const;
	/// Set enable
	void setEnable(bool isEnable);

	/// Set item flags
	void setFlags(Qt::ItemFlags flag);
	/// Get item flags
	virtual Qt::ItemFlags flags() const;

	/// Set action
	void setAction(QAction* act);
	/// Get action
	QAction* action();

	/// Set text alignment
	void setTextAlignment(Qt::Alignment a);
	/// Get text alignment
	Qt::Alignment textAlignment() const;

private:
	QMap< int, QVariant > mDatas;
	Qt::ItemFlags mFlags;
	QAction* mAction;
};

#endif  // SARIBBONGALLERYITEM_H

/*** End of inlined file: SARibbonGalleryItem.h ***/


/*** Start of inlined file: SARibbonGalleryGroup.h ***/
#ifndef SARIBBONGALLERYGROUP_H
#define SARIBBONGALLERYGROUP_H

#include <QList>
#include <QListView>
#include <QStyledItemDelegate>

/**
 * \if ENGLISH
 * @brief Delegate for SARibbonGalleryGroup display
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGalleryGroup对应的显示代理
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonGalleryGroupItemDelegate : public QStyledItemDelegate
{
public:
	/// Constructor for SARibbonGalleryGroupItemDelegate
	explicit SARibbonGalleryGroupItemDelegate(SARibbonGalleryGroup* group, QObject* parent = nullptr);
	/// Destructor for SARibbonGalleryGroupItemDelegate
	~SARibbonGalleryGroupItemDelegate();
	/// Paint the item
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	/// Get size hint
	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	/// Paint only icon
	virtual void paintIconOnly(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	/// Paint icon with text
	virtual void paintIconWithText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	/// Paint icon with word wrap text
	virtual void
	paintIconWithTextWordWrap(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
	SARibbonGalleryGroup* mGroup;
};

/**
 * \if ENGLISH
 * @brief Model for SARibbonGalleryGroup
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGalleryGroup对应的model
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonGalleryGroupModel : public QAbstractListModel
{
	Q_OBJECT
public:
	/// Constructor for SARibbonGalleryGroupModel
	SARibbonGalleryGroupModel(QObject* parent = Q_NULLPTR);
	/// Destructor for SARibbonGalleryGroupModel
	~SARibbonGalleryGroupModel();
	/// Get row count
	virtual int rowCount(const QModelIndex& parent) const Q_DECL_OVERRIDE;
	/// Get item flags
	virtual Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
	/// Get data at index
	virtual QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
	/// Create model index
	virtual QModelIndex index(int row, int column, const QModelIndex& parent) const Q_DECL_OVERRIDE;
	/// Set data at index
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role) Q_DECL_OVERRIDE;
	/// Clear all items
	void clear();
	/// Get item at row
	SARibbonGalleryItem* at(int row) const;
	/// Insert item at row
	void insert(int row, SARibbonGalleryItem* item);
	/// Take item at row
	SARibbonGalleryItem* take(int row);
	/// Append item
	void append(SARibbonGalleryItem* item);
	/// Remove item by action object name
	bool remove(const QString& act_object_name);
	/// Get item count
	int itemSize() const;

private:
	QList< SARibbonGalleryItem* > mItems;
};

/**
 * \if ENGLISH
 * @brief Gallery group
 * @details The group is responsible for displaying and managing Gallery Items
 * \endif
 *
 * \if CHINESE
 * @brief Gallery的组
 * @details 组负责显示管理Gallery Item
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonGalleryGroup : public QListView
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonGalleryGroup)
public:
	/**
	 * @brief GalleryGroup display style
	 */
	enum GalleryGroupStyle
	{
		IconWithText,          ///< Icon with text
		IconWithWordWrapText,  ///< Icon with text that wraps, only works for DisplayOneRow
		IconOnly               ///< Icon only
	};

	/**
	 * @brief Define the number of icon rows displayed under a panel
	 */
	enum DisplayRow
	{
		DisplayOneRow   = 1,  ///< Display 1 row (default)
		DisplayTwoRow   = 2,  ///< Display 2 rows
		DisplayThreeRow = 3   ///< Display 3 rows
	};

	/// Constructor
	SARibbonGalleryGroup(QWidget* w = 0);

	/// Destructor
	virtual ~SARibbonGalleryGroup();
	/// Recalculate grid size
	void recalcGridSize();
	/// Recalculate grid size
	void recalcGridSize(int galleryHeight);
	/// Set display style
	void setGalleryGroupStyle(GalleryGroupStyle style);
	/// Get gallery group style
	GalleryGroupStyle galleryGroupStyle() const;
	/// Add item
	void addItem(const QString& text, const QIcon& icon);
	/// Add item
	void addItem(SARibbonGalleryItem* item);
	/// Add action as item
	void addActionItem(QAction* act);
	/// Add action item list
	void addActionItemList(const QList< QAction* >& acts);
	/// Remove action item
	bool removeActionItem(QAction* act);
	/// Get group model
	SARibbonGalleryGroupModel* groupModel() const;
	/// Set group title
	void setGroupTitle(const QString& title);
	/// Get group title
	QString groupTitle() const;
	/// Select by index
	void selectByIndex(int i);
	/// Set display row
	void setDisplayRow(DisplayRow r);
	/// Get display row
	DisplayRow displayRow() const;
	/// Set grid minimum width (default 0: no limit)
	void setGridMinimumWidth(int w);
	/// Get grid minimum width
	int gridMinimumWidth() const;
	/// Set grid maximum width (default 0: no limit)
	void setGridMaximumWidth(int w);
	/// Get grid maximum width
	int gridMaximumWidth() const;
	/// Get action group
	QActionGroup* actionGroup() const;
	/// Get grid row count
	int gridRowCount() const;
	/// Get grid column count
	int gridColumnCount() const;
	/// Calculate preferred height for width
	int preferredHeightForWidth(int w) const;
	/// Check if has height for width
	bool hasHeightForWidth() const override;
	/// Get height for width
	int heightForWidth(int w) const override;
private Q_SLOTS:
	void onItemClicked(const QModelIndex& index);
	void onItemEntered(const QModelIndex& index);
Q_SIGNALS:
	/**
	 * \if ENGLISH
	 * @brief Signal when group title changed
	 * @param title New title
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 组标题改变时的信号
	 * @param title 新标题
	 * \endif
	 */
	void groupTitleChanged(const QString& title);
	/**
	 * \if ENGLISH
	 * @brief Equivalent to QActionGroup::triggered
	 * @details All actions added to SARibbonGalleryGroup are managed by a QActionGroup
	 * @param action Triggered action
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 等同QActionGroup的triggered
	 * @details 所有加入SARibbonGalleryGroup的action都会被一个QActionGroup管理
	 * @param action 触发的动作
	 * \endif
	 */
	void triggered(QAction* action);
	/**
	 * \if ENGLISH
	 * @brief Equivalent to QActionGroup::hovered
	 * @details All actions added to SARibbonGalleryGroup are managed by a QActionGroup
	 * @note This property needs to be activated by QAbstractItemView::entered(const QModelIndex &index), so ensure setMouseTracking(true) is set
	 * @param action Hovered action
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 等同QActionGroup的hovered
	 * @details 所有加入SARibbonGalleryGroup的action都会被一个QActionGroup管理
	 * @note 此属性需要通过QAbstractItemView::entered(const QModelIndex &index)激活，因此要保证设置了setMouseTracking(true)
	 * @param action 悬停的动作
	 * \endif
	 */
	void hovered(QAction* action);

private:
	/// Setup group model
	SARibbonGalleryGroupModel* setupGroupModel();
};

#endif  // SARIBBONGALLERYGROUP_H

/*** End of inlined file: SARibbonGalleryGroup.h ***/


/*** Start of inlined file: SARibbonGallery.h ***/
#ifndef SARIBBONGALLERY_H
#define SARIBBONGALLERY_H

#include <QFrame>
#include <QToolButton>
#include <QScrollArea>

class QLabel;
class QVBoxLayout;
class QSizeGrip;
class QShowEvent;
class QHideEvent;
class SARibbonGalleryViewport;

/**
 * \if ENGLISH
 * @brief Button for SARibbonGallery control
 * \endif
 *
 * \if CHINESE
 * @brief 针对SARibbonGallery控件的按钮
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonGalleryButton : public QToolButton
{
	Q_OBJECT
public:
	explicit SARibbonGalleryButton(QWidget* parent = nullptr);
	~SARibbonGalleryButton();
};

/**
 * \if ENGLISH
 * @brief Gallery control widget
 *
 * The Gallery control consists of a currently active @sa SARibbonGalleryGroup and a popup @sa SARibbonGalleryViewport.
 *
 * @sa SARibbonGalleryGroup inherits from @sa QListView and displays actions through icons.
 *     Related properties can be set according to QListView.
 *
 * @sa SARibbonGalleryViewport is a window with an internal vertical layout that displays
 *     all managed SARibbonGalleryGroups when popped up.
 *
 * Example usage:
 * @code
 * SARibbonGallery* gallery = panel1->addGallery();
 * QList< QAction* > galleryActions;
 * ...create many actions ...
 * SARibbonGalleryGroup* group1 = gallery->addCategoryActions(tr("Files"), galleryActions);
 * galleryActions.clear();
 * ...create many actions ...
 * gallery->addCategoryActions(tr("Apps"), galleryActions);
 * gallery->setCurrentViewGroup(group1);
 * @endcode
 * \endif
 *
 * \if CHINESE
 * @brief Gallery控件
 *
 * Gallery控件是由一个当前激活的@sa SARibbonGalleryGroup 和弹出的 @sa SARibbonGalleryViewport 组成
 *
 * @sa SARibbonGalleryGroup 是继承@sa QListView actions通过icon展示出来，相关的属性可以按照QListView设置
 *
 * @sa SARibbonGalleryViewport 是一个内部有垂直布局的窗体，在弹出激活时，把管理的SARibbonGalleryGroup都展示出来
 *
 * 示例如下：
 * @code
 * SARibbonGallery* gallery = panel1->addGallery();
 * QList< QAction* > galleryActions;
 * ...create many actions ...
 * SARibbonGalleryGroup* group1 = gallery->addCategoryActions(tr("Files"), galleryActions);
 * galleryActions.clear();
 * ...create many actions ...
 * gallery->addCategoryActions(tr("Apps"), galleryActions);
 * gallery->setCurrentViewGroup(group1);
 * @endcode
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonGallery : public QFrame
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonGallery)
public:
	/// Constructor for SARibbonGallery
	explicit SARibbonGallery(QWidget* parent = nullptr);
	/// Destructor for SARibbonGallery
	virtual ~SARibbonGallery();
	/// Get a blank SARibbonGalleryGroup
	SARibbonGalleryGroup* addGalleryGroup();
	/// Add an existing SARibbonGalleryGroup to the gallery
	void addGalleryGroup(SARibbonGalleryGroup* group);
	/// Quickly add a set of actions as a gallery group
	SARibbonGalleryGroup* addCategoryActions(const QString& title, QList< QAction* > actions);
	/// Set the currently displayed gallery group
	void setCurrentViewGroup(SARibbonGalleryGroup* group);
	/// Get the currently displayed gallery group
	SARibbonGalleryGroup* currentViewGroup() const;
	/// Get the popup viewport widget
	SARibbonGalleryViewport* getPopupViewPort() const;

public:
	/// Set the maximum width for gallery control buttons (default 15)
	static void setGalleryButtonMaximumWidth(int w);
	/// Set single-row display mode for gallery
	void setSingleRowMode(bool on);
	/// Check if gallery is in single-row mode
	bool isSingleRowMode() const;
Q_SIGNALS:
	/**
	 * \if ENGLISH
	 * @brief Forwards SARibbonGalleryGroup::triggered signal
	 * All actions added to SARibbonGallery are managed by a QActionGroup
	 * @param action Triggered action
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 转发管理的SARibbonGalleryGroup::triggered信号
	 * 所有加入SARibbonGallery的action都会被一个QActionGroup管理
	 * @param action 被触发的动作
	 * \endif
	 */
	void triggered(QAction* action);
	/**
	 * \if ENGLISH
	 * @brief Forwards SARibbonGalleryGroup::hovered signal
	 * @note This requires SARibbonGalleryGroup::setMouseTracking(true)
	 * @param action Hovered action
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 转发管理的SARibbonGalleryGroup::hovered信号
	 * @note 此属性需要确保SARibbonGalleryGroup::setMouseTracking(true)
	 * @param action 悬停的动作
	 * \endif
	 */
	void hovered(QAction* action);

public Q_SLOTS:
	/// Scroll up one page in the current gallery group
	virtual void pageUp();
	/// Scroll down one page in the current gallery group
	virtual void pageDown();
	/// Show the popup viewport with all gallery groups
	virtual void showMoreDetail();
protected Q_SLOTS:
	/// Slot called when an item in a gallery group is clicked
	void onItemClicked(const QModelIndex& index);
	/// Slot called when an action is triggered from the gallery
	virtual void onTriggered(QAction* action);

private:
	SARibbonGalleryViewport* ensureGetPopupViewPort();

protected:
	/// Handle resize events for the gallery
	void resizeEvent(QResizeEvent* event) override;
	/// Handle change events for the gallery
	void changeEvent(QEvent* event) override;
};

/**
 * \if ENGLISH
 * @brief Viewport class for SARibbonGallery
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonGallery的Viewport类
 * \endif
 */
class SARibbonGalleryViewport : public QScrollArea
{
	Q_OBJECT
public:
	explicit SARibbonGalleryViewport(QWidget* parent);
	// 添加窗口不带标题
	void addWidget(QWidget* w);
	// 添加窗口，带标题
	void addWidget(QWidget* w, const QString& title);
	// 通过SARibbonGalleryGroup获取对应的标题，用户可以通过此函数设置QLabel的属性
	QLabel* titleLabel(QWidget* w);
	// 移除窗口
	void removeWidget(QWidget* w);
	// 获取此窗口下所有的SARibbonGalleryGroup
	QList< SARibbonGalleryGroup* > galleryGroupList() const;
	// 获取gallery的高度
	int galleryHeight() const;
	// 根据宽度计算高度推荐值
	int heightHintForWidth(int w) const;
public Q_SLOTS:
	void onTitleChanged(QWidget* w, const QString& title);

protected:  // 新增
	bool eventFilter(QObject* o, QEvent* e) override;
	void showEvent(QShowEvent* e) override;
	void hideEvent(QHideEvent* e) override;
	void resizeEvent(QResizeEvent* e) override;

private:
	QWidget* m_contentWidget { nullptr };            ///< 滚动区域的内容窗口
	QVBoxLayout* m_layout;                           ///< 内容窗口的布局
	QMap< QWidget*, QLabel* > m_widgetToTitleLabel;  ///< 窗口到标题标签的映射
	QSizeGrip* m_sizeGrip;                           ///< 尺寸调整手柄
};

#endif  // SARIBBONGALLERY_H

/*** End of inlined file: SARibbonGallery.h ***/


/*** Start of inlined file: SARibbonBar.h ***/
#ifndef SARIBBONBAR_H
#define SARIBBONBAR_H

#include <QMenuBar>
#include <QScopedPointer>
#include <QVariant>

class QAction;
class QAbstractButton;
class SARibbonElementFactory;
class SARibbonTabBar;
class SARibbonButtonGroupWidget;
class SARibbonQuickAccessBar;
class SARibbonStackedWidget;
class SARibbonTitleIconWidget;

/**
 * \if ENGLISH
 * @brief SARibbonBar inherits from QMenuBar and directly replaces the original QMenuBar in SARibbonMainWindow
 *
 * Set the ribbon style using the setRibbonStyle function:
 *
 * @code
 * void setRibbonStyle(RibbonStyles v);
 * @endcode
 *
 * SARibbonBar references Office and WPS, providing four styles of Ribbon modes, @ref SARibbonBar::RibbonStyles
 *
 * If you want the ribbon to occupy minimal space, WpsLiteStyleTwoRow mode can save 35% height space compared to OfficeStyle
 *
 * How to create a ribbon? First, let's see how a traditional Menu/ToolBar is created:
 *
 * @code
 * void MainWindow::MainWindow()
 * {
 *  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
 *  QToolBar *fileToolBar = addToolBar(tr("File"));
 *  // Create action
 *  QAction *newAct = new QAction(newIcon, tr("&New"), this);
 *  fileMenu->addAction(newAct);
 *  fileToolBar->addAction(newAct);
 *
 *  QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
 *  fileMenu->addAction(openAct);
 *  fileToolBar->addAction(openAct);
 * }
 * @endcode
 *
 * Traditional Menu/ToolBar mainly uses QMenu's addMenu to add menus, and QMainWindow::addToolBar to generate QToolBar,
 * then sets QAction into QMenu and QToolBar.
 *
 * SARibbonBar is similar to the traditional method, but compared to traditional Menu/ToolBar where QMenu and QToolBar are at the same level,
 * Ribbon has a clear hierarchical relationship: SARibbonBar is above @ref SARibbonCategory,
 * SARibbonCategory is above @ref SARibbonPanel, SARibbonPanel is above @ref SARibbonToolButton,
 * and SARibbonToolButton manages QAction.
 *
 * Therefore, creating a ribbon only requires the following functions:
 * @code
 * SARibbonCategory * SARibbonBar::addCategoryPage(const QString& title);
 * SARibbonPanel * SARibbonCategory::addPanel(const QString& title);
 * SARibbonToolButton * SARibbonPanel::addLargeAction(QAction *action);
 * SARibbonToolButton * SARibbonPanel::addSmallAction(QAction *action);
 * @endcode
 *
 * Therefore, the creation steps are as follows:
 *
 * @code
 * // Member variables
 * SARibbonCategory* categoryMain;
 * SARibbonPanel* FilePanel;
 *
 * // Build UI
 * void setupRibbonUi()
 * {
 *   ......
 *   // ribbonwindow is SARibbonMainWindow
 *   SARibbonBar* ribbon = ribbonwindow->ribbonBar();
 *   ribbon->setRibbonStyle(SARibbonBar::WpsLiteStyle);
 *   // Add a Main tab
 *   categoryMain = ribbon->addCategoryPage(QStringLiteral("Main"));
 *   // Add a File Panel under the Main tab
 *   FilePanel = categoryMain->addPanel(QStringLiteral("FilePanel"));
 *   // Start adding actions to File Panel
 *   FilePanel->addLargeAction(actionNew);
 *   FilePanel->addLargeAction(actionOpen);
 *   FilePanel->addLargeAction(actionSave);
 *   FilePanel->addSmallAction(actionImportMesh);
 *   FilePanel->addSmallAction(actionImportGeometry);
 * }
 * @endcode
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonBar继承于QMenuBar,在SARibbonMainWindow中直接替换了原来的QMenuBar
 *
 * 通过setRibbonStyle函数设置ribbon的风格:
 *
 * @code
 * void setRibbonStyle(RibbonStyles v);
 * @endcode
 *
 * SARibbonBar参考office和wps，提供了四种风格的Ribbon模式,@ref SARibbonBar::RibbonStyles
 *
 * 如果想ribbon占用的空间足够小，WpsLiteStyleTwoRow模式能比OfficeStyle节省35%的高度空间
 *
 * 如何生成ribbon?先看看一个传统的Menu/ToolBar是如何生成的：
 *
 * @code
 * void MainWindow::MainWindow()
 * {
 *  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
 *  QToolBar *fileToolBar = addToolBar(tr("File"));
 *  //生成action
 *  QAction *newAct = new QAction(newIcon, tr("&New"), this);
 *  fileMenu->addAction(newAct);
 *  fileToolBar->addAction(newAct);
 *
 *  QAction *openAct = new QAction(openIcon, tr("&Open..."), this);
 *  fileMenu->addAction(openAct);
 *  fileToolBar->addAction(openAct);
 * }
 * @endcode
 *
 * 传统的Menu/ToolBar主要通过QMenu的addMenu添加菜单,通过QMainWindow::addToolBar生成QToolBar,
 * 再把QAction设置进QMenu和QToolBar中
 *
 * SARibbonBar和传统方法相似，不过相对于传统的Menu/ToolBar QMenu和QToolBar是平级的，
 * Ribbon是有明显的层级关系，SARibbonBar下面是 @ref SARibbonCategory，
 * SARibbonCategory下面是@ref SARibbonPanel ，SARibbonPanel下面是@ref SARibbonToolButton ，
 * SARibbonToolButton管理着QAction
 *
 * 因此，生成一个ribbon只需以下几个函数：
 * @code
 * SARibbonCategory * SARibbonBar::addCategoryPage(const QString& title);
 * SARibbonPanel * SARibbonCategory::addPanel(const QString& title);
 * SARibbonToolButton * SARibbonPanel::addLargeAction(QAction *action);
 * SARibbonToolButton * SARibbonPanel::addSmallAction(QAction *action);
 * @endcode
 *
 * 因此生成步骤如下：
 *
 * @code
 * //成员变量
 * SARibbonCategory* categoryMain;
 * SARibbonPanel* FilePanel;
 *
 * //建立ui
 * void setupRibbonUi()
 * {
 *   ......
 *   //ribbonwindow为SARibbonMainWindow
 *   SARibbonBar* ribbon = ribbonwindow->ribbonBar();
 *   ribbon->setRibbonStyle(SARibbonBar::WpsLiteStyle);
 *   //添加一个Main标签
 *   categoryMain = ribbon->addCategoryPage(QStringLiteral("Main"));
 *   //Main标签下添加一个File Panel
 *   FilePanel = categoryMain->addPanel(QStringLiteral("FilePanel"));
 *   //开始为File Panel添加action
 *   FilePanel->addLargeAction(actionNew);
 *   FilePanel->addLargeAction(actionOpen);
 *   FilePanel->addLargeAction(actionSave);
 *   FilePanel->addSmallAction(actionImportMesh);
 *   FilePanel->addSmallAction(actionImportGeometry);
 * }
 * @endcode
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonBar : public QMenuBar
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonBar)
	friend class SARibbonMainWindow;
	friend class SARibbonSystemButtonBar;
	Q_PROPERTY(RibbonStyles ribbonStyle READ currentRibbonStyle WRITE setRibbonStyle)
	Q_PROPERTY(bool minimumMode READ isMinimumMode WRITE setMinimumMode)
	Q_PROPERTY(bool minimumModeButton READ haveShowMinimumModeButton WRITE showMinimumModeButton)
	Q_PROPERTY(QColor windowTitleTextColor READ windowTitleTextColor WRITE setWindowTitleTextColor)
	Q_PROPERTY(QColor tabBarBaseLineColor READ tabBarBaseLineColor WRITE setTabBarBaseLineColor)
	Q_PROPERTY(Qt::Alignment windowTitleAligment READ windowTitleAligment WRITE setWindowTitleAligment)
	Q_PROPERTY(bool enableWordWrap READ isEnableWordWrap WRITE setEnableWordWrap)
	Q_PROPERTY(bool enableShowPanelTitle READ isEnableShowPanelTitle WRITE setEnableShowPanelTitle)
	Q_PROPERTY(bool enableIconRightText READ isEnableIconRightText WRITE setEnableIconRightText)
	Q_PROPERTY(bool tabOnTitle READ isTabOnTitle WRITE setTabOnTitle)
	Q_PROPERTY(SARibbonPanel::PanelLayoutMode panelLayoutMode READ panelLayoutMode WRITE setPanelLayoutMode)

public:
enum RibbonStyleFlag
	{
		RibbonStyleLoose       = 0x0001,  // bit:0000 0001
		RibbonStyleCompact     = 0x0002,  // bit:0000 0010
		RibbonStyleThreeRow    = 0x0010,  // bit:0001 0000
		RibbonStyleTwoRow      = 0x0020,  // bit:0010 0000
		RibbonStyleSingleRow   = 0x0040,  // bit:0100 0000

		RibbonStyleLooseThreeRow   = RibbonStyleLoose | RibbonStyleThreeRow,    ///< 宽松结构，3 行模式
		RibbonStyleCompactThreeRow = RibbonStyleCompact | RibbonStyleThreeRow,  ///< 紧凑结构，3 行模式
		RibbonStyleLooseTwoRow     = RibbonStyleLoose | RibbonStyleTwoRow,      ///< 宽松结构，2 行模式
		RibbonStyleCompactTwoRow   = RibbonStyleCompact | RibbonStyleTwoRow,    ///< 紧凑结构，2 行模式
		RibbonStyleLooseSingleRow   = RibbonStyleLoose | RibbonStyleSingleRow,  ///< 宽松结构，1 行模式
		RibbonStyleCompactSingleRow = RibbonStyleCompact | RibbonStyleSingleRow ///< 紧凑结构，1 行模式
	};
	Q_ENUM(RibbonStyleFlag)
	Q_DECLARE_FLAGS(RibbonStyles, RibbonStyleFlag)
	Q_FLAG(RibbonStyles)

	/**
	 * @brief 定义当前ribbon 的状态
	 */
	enum RibbonMode
	{
		MinimumRibbonMode,  ///< 缩小模式
		NormalRibbonMode    ///< 正常模式
	};
	Q_ENUM(RibbonMode)
public:
	using FpCategoryIterate = std::function< bool(SARibbonCategory*) >;
	using FpPanelIterate    = SARibbonCategory::FpPanelIterate;
	/**
	 * @brief 这是针对上下文标签的高亮颜色绘制，用户可以设置一个函数指针，来针对上下文标签的高亮颜色进行调整
	 */
	using FpContextCategoryHighlight = std::function< QColor(const QColor&) >;

public:
	/// Check if the ribbon style is two-row mode
	static bool isTwoRowStyle(RibbonStyles s);
	/// Check if the ribbon style is three-row mode
	static bool isThreeRowStyle(RibbonStyles s);
	/// Check if the ribbon style is loose (Office) style
	static bool isLooseStyle(RibbonStyles s);
	/// Check if the ribbon style is compact (WPS) style
	static bool isCompactStyle(RibbonStyles s);
	/// Check if the ribbon style is single-row mode
	static bool isSingleRowStyle(RibbonStyles s);
	/// Get version information
	static QString versionString();

	/// Get default context category color list
	static QList< QColor > defaultContextCategoryColorList();

	/// Initialize high DPI support
	static void initHighDpi();

public:
	/// Constructor
	explicit SARibbonBar(QWidget* parent = nullptr);
	/// Destructor
	~SARibbonBar();
	/// Get application button
	QAbstractButton* applicationButton();

	/// Set application button
	void setApplicationButton(QAbstractButton* btn);

	/// Get ribbon tab bar
	SARibbonTabBar* ribbonTabBar();

	/// Add a category page
	SARibbonCategory* addCategoryPage(const QString& title);
	/// Add a category page
	void addCategoryPage(SARibbonCategory* category);

	/// Overloaded function for Qt designer support
	Q_INVOKABLE void addCategoryPage(QWidget* category);

	/// Insert a category page at specified index
	SARibbonCategory* insertCategoryPage(const QString& title, int index);
	/// Insert a category page at specified index
	void insertCategoryPage(SARibbonCategory* category, int index);

	/// Find category by name
	SARibbonCategory* categoryByName(const QString& title) const;

	/// Find category by object name
	SARibbonCategory* categoryByObjectName(const QString& objname) const;

	/// Find category by index
	SARibbonCategory* categoryByIndex(int index) const;

	/// Hide category (not delete)
	void hideCategory(SARibbonCategory* category);

	/// Show hidden category
	void showCategory(SARibbonCategory* category);

	// Batch set categories visible or hidden, only triggers one relayout at the end
	void setCategoriesVisible(const QList< SARibbonCategory* >& categories, bool visible);

	/// Check if category is visible
	bool isCategoryVisible(const SARibbonCategory* c) const;

	/// Get category index
	int categoryIndex(const SARibbonCategory* c) const;

	/// Move category from one index to another
	void moveCategory(int from, int to);

	/// Get all category pages
	QList< SARibbonCategory* > categoryPages(bool getAll = true) const;

	/// Remove category
	void removeCategory(SARibbonCategory* category);

	/// Add a context category
	SARibbonContextCategory*
	addContextCategory(const QString& title, const QColor& color = QColor(), const QVariant& id = QVariant());
	/// Add a context category
	void addContextCategory(SARibbonContextCategory* context);

	/// Show context category
	void showContextCategory(SARibbonContextCategory* context);

	/// Hide context category
	void hideContextCategory(SARibbonContextCategory* context);

	/// Check if context category is visible
	bool isContextCategoryVisible(SARibbonContextCategory* context);

	/// Set context category visibility
	void setContextCategoryVisible(SARibbonContextCategory* context, bool visible);

	/// Get all context categories
	QList< SARibbonContextCategory* > contextCategoryList() const;

	/// Destroy context category
	void destroyContextCategory(SARibbonContextCategory* context);

	/// Get current visible context category tab indexes
	QList< int > currentVisibleContextCategoryTabIndexs() const;

	/// Set minimum mode
	void setMinimumMode(bool isHide);

	/// Check if in minimum mode
	bool isMinimumMode() const;

	/// Show minimum mode button
	void showMinimumModeButton(bool isShow = true);

	/// Check if minimum mode button is shown
	bool haveShowMinimumModeButton() const;

	/// Get minimum mode action
	QAction* minimumModeAction() const;

	/// Check if tab double click enables minimum mode
	bool isEnableTabDoubleClickToMinimumMode() const;
	/// Set tab double click to enable minimum mode
	void setTabDoubleClickToMinimumMode(bool on = true) const;

	/// Get current ribbon state
	RibbonMode currentRibbonState() const;

	/// Get tab bar height
	int tabBarHeight() const;
	/// Set tab bar height
	void setTabBarHeight(int h, bool resizeByNow = true);

	/// Get title bar height
	int titleBarHeight() const;
	/// Set title bar height
	void setTitleBarHeight(int h, bool resizeByNow = true);

	/// Get category height
	int categoryHeight() const;
	/// Set category height
	void setCategoryHeight(int h, bool resizeByNow = true);

	/// Get main bar height in normal mode
	int normalModeMainBarHeight() const;

	/// Get main bar height in minimum mode
	int minimumModeMainBarHeight() const;

	/// Get active right button group
	SARibbonButtonGroupWidget* activeRightButtonGroup();

	/// Get right button group
	SARibbonButtonGroupWidget* rightButtonGroup();

	/// Get active quick access bar
	SARibbonQuickAccessBar* activeQuickAccessBar();

	/// Get quick access bar
	SARibbonQuickAccessBar* quickAccessBar();

	/// Get title icon widget
	SARibbonTitleIconWidget* titleIconWidget() const;

	/// Set title icon visibility
	void setTitleIconVisible(bool on = true);
	/// Check if title icon is visible
	bool isTitleIconVisible() const;

	/// Set ribbon style
	void setRibbonStyle(RibbonStyles v);
	/// Get current ribbon style
	RibbonStyles currentRibbonStyle() const;

	/// Set current ribbon index
	void setCurrentIndex(int index);

	/// Get current tab index
	int currentIndex();

	/// Raise category to show it
	void raiseCategory(SARibbonCategory* category);

	/// Check if current style is two-row
	bool isTwoRowStyle() const;
	/// Check if current style is three-row
	bool isThreeRowStyle() const;

	/// Check if current style is loose
	bool isLooseStyle() const;
	/// Check if current style is compact
	bool isCompactStyle() const;
	/// Check if current style is single-row
	bool isSingleRowStyle() const;

	/// Update ribbon geometry
	void updateRibbonGeometry();

	/// Get panel layout mode
	SARibbonPanel::PanelLayoutMode panelLayoutMode() const;
	/// Set panel layout mode
	void setPanelLayoutMode(SARibbonPanel::PanelLayoutMode m);

	/// Set tab on title
	void setTabOnTitle(bool on);
	/// Check if tab is on title
	bool isTabOnTitle() const;

	/// Set tab bar baseline color
	void setTabBarBaseLineColor(const QColor& clr);
	/// Get tab bar baseline color
	QColor tabBarBaseLineColor() const;

	/// Set window title text color
	void setWindowTitleTextColor(const QColor& clr);
	/// Get window title text color
	QColor windowTitleTextColor() const;

	/// Set title visibility
	void setTitleVisible(bool on = false);
	/// Check if title is visible
	bool isTitleVisible() const;

	/// Set window title background brush
	void setWindowTitleBackgroundBrush(const QBrush& bk);
	/// Get window title background brush
	QBrush windowTitleBackgroundBrush() const;

	/// Set window title alignment
	void setWindowTitleAligment(Qt::Alignment al);
	/// Get window title alignment
	Qt::Alignment windowTitleAligment() const;

	/// Set enable word wrap
	void setEnableWordWrap(bool on);
	/// Check if word wrap is enabled
	bool isEnableWordWrap() const;

	/// Set whether button text is displayed to the right of the icon
	void setEnableIconRightText(bool on);
	/// Check if icon-right-text mode is enabled
	bool isEnableIconRightText() const;

	/// Set button maximum aspect ratio
	void setButtonMaximumAspectRatio(qreal fac = 1.4);
	/// Get button maximum aspect ratio
	qreal buttonMaximumAspectRatio() const;

	/// Set panel title height
	void setPanelTitleHeight(int h);
	/// Get panel title height
	int panelTitleHeight() const;

	/// Set enable show panel title
	void setEnableShowPanelTitle(bool on);
	/// Check if panel title is shown
	bool isEnableShowPanelTitle() const;

	/// Set panel spacing
	void setPanelSpacing(int n);
	/// Get panel spacing
	int panelSpacing() const;

	/// Set panel tool button icon sizes
	void setPanelToolButtonIconSize(const QSize& smallSize, const QSize& largeSize);
	/// Get panel tool button icon sizes
	QPair< QSize, QSize > panelToolButtonIconSize() const;

	/// Set panel large icon size
	void setPanelLargeIconSize(const QSize& largeSize);
	/// Get panel large icon size
	QSize panelLargeIconSize() const;

	/// Set panel small icon size
	void setPanelSmallIconSize(const QSize& smallSize);
	/// Get panel small icon size
	QSize panelSmallIconSize() const;

	/// Get ribbon stacked widget
	SARibbonStackedWidget* ribbonStackedWidget();

	/// Set context category color list
	void setContextCategoryColorList(const QList< QColor >& cls);
	/// Get context category color list
	QList< QColor > contextCategoryColorList() const;

	/// Set context category title text color
	void setContextCategoryTitleTextColor(const QColor& clr);
	/// Get context category title text color
	QColor contextCategoryTitleTextColor() const;

	/// Set context category color highlight function
	void setContextCategoryColorHighLight(FpContextCategoryHighlight fp);

	/// Set ribbon alignment
	void setRibbonAlignment(SARibbonAlignment al);
	/// Get ribbon alignment
	SARibbonAlignment ribbonAlignment() const;

	/// Iterate through all categories
	bool iterateCategory(FpCategoryIterate fp) const;
	/// Iterate through all panels
	bool iteratePanel(FpPanelIterate fp) const;

	/// Set corner widget visibility
	void setCornerWidgetVisible(bool on, Qt::Corner c = Qt::TopLeftCorner);

	/// Set application button vertical expansion
	void setApplicationButtonVerticalExpansion(bool on = true);
	/// Check if application button is vertically expanded
	bool isApplicationButtonVerticalExpansion() const;

	/// Get all actions in panels
	QList< QAction* > allActions() const;
	/// Check if ribbon frame is used
	bool isUseRibbonFrame() const;
Q_SIGNALS:

	/**
	 * \if ENGLISH
	 * @brief Application button click response - the button in the top-left corner, connect to this signal to trigger application button click effect
	 *
	 * For example, clicking the button can pop up a full-screen window (like in Office)
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 应用按钮点击响应 - 左上角的按钮，通过关联此信号触发应用按钮点击的效果
	 *
	 * 例如想点击按钮后弹出一个全屏的窗口（如office这些）
	 * \endif
	 */
	void applicationButtonClicked();

	/**
	 * \if ENGLISH
	 * @brief Signal triggered when tab page changes
	 * @param index New tab index
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 标签页变化触发的信号
	 * @param index 新的标签页索引
	 * \endif
	 */
	void currentRibbonTabChanged(int index);

	/**
	 * \if ENGLISH
	 * @brief Signal triggered when ribbon state changes
	 * @param nowState New ribbon state after change
	 * \endif
	 *
	 * \if CHINESE
	 * @brief ribbon的状态发生了变化后触发此信号
	 * @param nowState 变更之后的ribbon状态
	 * \endif
	 */
	void ribbonModeChanged(SARibbonBar::RibbonMode nowState);

	/**
	 * \if ENGLISH
	 * @brief Signal triggered when ribbon style changes
	 * @param nowStyle New ribbon style after change
	 * \endif
	 *
	 * \if CHINESE
	 * @brief ribbon的状态发生了变化后触发此信号
	 * @param nowStyle 变更之后的ribbon样式
	 * \endif
	 */
	void ribbonStyleChanged(SARibbonBar::RibbonStyles nowStyle);

	/**
	 * \if ENGLISH
	 * @brief Signal triggered when title bar height changes
	 * @param oldHeight Old title bar height
	 * @param newHeight New title bar height
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 标题栏高度发生了变化的信号
	 * @param oldHeight 旧的标题栏高度
	 * @param newHeight 新的标题栏高度
	 * \endif
	 */
	void titleBarHeightChanged(int oldHeight, int newHeight);

	/**
	 * \if ENGLISH
	 * @brief Signal similar to QToolBar::actionTriggered
	 * @param action Triggered action
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 参考QToolBar::actionTriggered的信号
	 * @param action 触发的action
	 * \endif
	 */
	void actionTriggered(QAction* action);

protected:
	/// Event filter
	bool eventFilter(QObject* obj, QEvent* e) override;
	/// Calculate minimum tab bar width
	int calcMinTabBarWidth() const;
	/// Update category title to tab name
	void updateCategoryTitleToTabName();
	/// Set system button group size
	void setSystemButtonGroupSize(const QSize& s);
	/// Set main window styles
	void setMainWindowStyles(SARibbonMainWindowStyles s);
protected Q_SLOTS:
	/// Slot for window title changed
	void onWindowTitleChanged(const QString& title);
	/// Slot for window icon changed
	void onWindowIconChanged(const QIcon& i);
	/// Slot for category window title changed
	void onCategoryWindowTitleChanged(const QString& title);
	/// Slot for stack widget hided
	void onStackWidgetHided();
	/// Slot for current ribbon tab changed
	virtual void onCurrentRibbonTabChanged(int index);
	/// Slot for current ribbon tab clicked
	virtual void onCurrentRibbonTabClicked(int index);
	/// Slot for current ribbon tab double clicked
	virtual void onCurrentRibbonTabDoubleClicked(int index);
	/// Slot for contexts category page added
	void onContextsCategoryPageAdded(SARibbonCategory* category);
	/// Slot for contexts category name changed
	void onContextsCategoryCategoryNameChanged(SARibbonCategory* category, const QString& title);
	/// Slot for tab moved
	void onTabMoved(int from, int to);

private:
	int tabIndex(SARibbonCategory* obj);
	void paintInLooseStyle();
	void paintInCompactStyle();

	// 刷新所有ContextCategoryManagerData，这个在单独一个Category删除时调用
	void updateContextCategoryManagerData();
	void synchronousCategoryData(bool autoUpdate = true);
	// 把窗口标题文字转换为显示的文字，这里主要针对[*]占位符进行替换
	QString toDisplayTitleText(const QString& title) const;
	// 获取标题显示的区域
	QRect getWindowTitleRect() const;

protected:
	/// Set ribbon main window style
	void setRibbonMainwindowStyle();
	/// Paint event handler
	virtual void paintEvent(QPaintEvent* e) override;
	/// Move event handler
	virtual void moveEvent(QMoveEvent* e) override;
	/// Change event handler
	virtual void changeEvent(QEvent* e) override;

	/// Paint tab bar baseline
	virtual void paintTabbarBaseLine(QPainter& painter);
	/// Paint window title
	virtual void paintWindowTitle(QPainter& painter, const QString& title, const QRect& titleRegion);
	/// Paint context category tab
	virtual void
	paintContextCategoryTab(QPainter& painter, const QString& title, const QRect& contextRect, const QColor& color);
#if SA_DEBUG_PRINT_SARIBBONBAR
	SA_RIBBON_EXPORT friend QDebug operator<<(QDebug debug, const SARibbonBar& ribbon);
#endif
};
Q_DECLARE_OPERATORS_FOR_FLAGS(SARibbonBar::RibbonStyles)

#endif  // SARIBBONBAR_H

/*** End of inlined file: SARibbonBar.h ***/


/*** Start of inlined file: SARibbonBarLayout.h ***/
#ifndef SARIBBONBARLAYOUT_H
#define SARIBBONBARLAYOUT_H

#include <QLayout>
#include <QPointer>

class SARibbonTabBar;
class SARibbonStackedWidget;
class SARibbonQuickAccessBar;
class SARibbonButtonGroupWidget;
class SARibbonContextCategory;

/**
 * \if ENGLISH
 * @brief Layout class for SARibbonBar
 * @details This class handles the layout of all components in the ribbon bar, including title bar, tab bar, quick access bar, and categories.
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonBar 的布局类
 * @details 此类处理功能区栏中所有组件的布局，包括标题栏、标签栏、快速访问栏和类别。
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonBarLayout : public QLayout
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonBarLayout)
public:
	// Constructor for SARibbonBarLayout
	explicit SARibbonBarLayout(SARibbonBar* parent);
	// Destructor for SARibbonBarLayout
	virtual ~SARibbonBarLayout();

	// QLayout interface implementations
	// Add a layout item
	void addItem(QLayoutItem* item) override;
	// Get the layout item at the specified index
	QLayoutItem* itemAt(int index) const override;
	// Take and remove the layout item at the specified index
	QLayoutItem* takeAt(int index) override;
	// Get the number of layout items
	int count() const override;
	// Get the recommended size for the layout
	QSize sizeHint() const override;
	// Get the minimum size for the layout
	QSize minimumSize() const override;
	// Set the geometry of the layout
	void setGeometry(const QRect& rect) override;

	// Perform the layout
	virtual void doLayout();

	// Style related methods
	// Check if the current style is loose style
	bool isLooseStyle() const;
	// Check if the current style is compact style
	bool isCompactStyle() const;

	// Title rectangle
	// Get the title rectangle
	const QRect& titleRect() const;

	// Calculate minimum TabBar width
	int calcMinTabBarWidth() const;

	// Reset size, called when ribbon mode changes
	void resetSize();

	// Update title rectangle
	void layoutTitleRect();

	// Adjust StackedContainerWidget position
	void layoutStackedContainerWidget();

	// Re-layout category, called when category alignment changes
	void layoutCategory();

	// Set system button size
	void setSystemButtonSize(const QSize& size);

	// Set whether tab is on title bar
	void setTabOnTitle(bool on);
	// Check if tab is on title bar
	bool isTabOnTitle() const;

	// Get main bar height in minimum mode
	int minimumModeMainBarHeight() const;

	// Get main bar height in normal mode
	int normalModeMainBarHeight() const;

	// Ribbon tab bar height
	// Get tab bar height
	int tabBarHeight() const;
	// Set tab bar height
	void setTabBarHeight(int h);

	// Title bar height
	// Get title bar height
	int titleBarHeight() const;
	// Set title bar height
	void setTitleBarHeight(int h);

	// Category height
	// Get category height
	int categoryHeight() const;
	// Set category height
	void setCategoryHeight(int h);

	// Panel title height
	// Get panel title height
	int panelTitleHeight() const;
	// Set panel title height
	void setPanelTitleHeight(int h);

	// Window icon
	// Set window icon
	void setWindowIcon(const QIcon& icon);
	// Get window icon
	QIcon windowIcon() const;

	// Application button vertical expansion
	// Set whether application button is vertically expanded
	void setApplicationButtonVerticalExpansion(bool on = true);
	// Check if application button is vertically expanded
	bool isApplicationButtonVerticalExpansion() const;

	// Get elements
	// Get ribbon bar
	SARibbonBar* ribbonBar() const;
	// Get ribbon tab bar
	SARibbonTabBar* ribbonTabBar() const;
	// Get stacked container widget
	SARibbonStackedWidget* stackedContainerWidget() const;
	// Get quick access bar
	SARibbonQuickAccessBar* quickAccessBar() const;
	// Get right button group
	SARibbonButtonGroupWidget* rightButtonGroup() const;
	// Get application button
	QAbstractButton* applicationButton() const;
	// Get title icon widget
	SARibbonTitleIconWidget* titleIconWidget() const;

protected:
	// Resize in loose style
	void resizeInLooseStyle();
	// Resize in compact style
	void resizeInCompactStyle();

private:
	// Initialize the layout
	void init();
};

#endif  // SARIBBONBARLAYOUT_H

/*** End of inlined file: SARibbonBarLayout.h ***/


/*** Start of inlined file: SARibbonElementFactory.h ***/
#ifndef SARIBBONELEMENTFACTORY_H
#define SARIBBONELEMENTFACTORY_H

#include <QColor>
#include <QMargins>
#include <QSize>
#include <QScopedPointer>

class QWidget;
class SARibbonBar;
class SARibbonTabBar;
class SARibbonApplicationButton;
class SARibbonCategory;
class SARibbonContextCategory;
class SARibbonPanel;
class SARibbonSeparatorWidget;
class SARibbonGallery;
class SARibbonGalleryGroup;
class SARibbonToolButton;
class SARibbonButtonGroupWidget;
class SARibbonStackedWidget;
class SARibbonQuickAccessBar;
class SARibbonPanelOptionButton;
class SARibbonSystemButtonBar;
class SARibbonTitleIconWidget;
class SARibbonPanelLabel;
/**
 * \if ENGLISH
 * @brief Factory class for creating SARibbon sub-elements
 * @details SARibbon creates all sub-elements through SARibbonElementFactory
 * @details Since SARibbonBar is a composite control composed of many sub-windows, some parts need this factory class to handle inheritance
 * @details For example, SARibbonCategory can be overloaded by overriding createRibbonCategory to return an instance of the overloaded class
 * \endif
 *
 * \if CHINESE
 * @brief SARibbon的子元素创建的工厂，SARibbon内部创建子元素都通过SARibbonElementFactory来创建
 * @details 由于SARibbonBar是一个复合控件，很多子窗口组合而成，有些部件的创建如果想继承，那么就需要这个工厂类来处理
 * @details 如SARibbonCategory，可以重载此类的createRibbonCategory,返回重载的类的实例
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonElementFactory
{
public:
	/// Constructor for SARibbonElementFactory
	SARibbonElementFactory();
	/// Destructor for SARibbonElementFactory
	virtual ~SARibbonElementFactory();
	/// Create SARibbonBar
	virtual SARibbonBar* createRibbonBar(QWidget* parent);
	/// Create SARibbonTabBar
	virtual SARibbonTabBar* createRibbonTabBar(QWidget* parent);
	/// Create SARibbonApplicationButton
	virtual SARibbonApplicationButton* createRibbonApplicationButton(QWidget* parent);
	/// Create SARibbonCategory
	virtual SARibbonCategory* createRibbonCategory(QWidget* parent);
	/// Create SARibbonContextCategory
	virtual SARibbonContextCategory* createRibbonContextCategory(QWidget* parent);
	/// Create SARibbonPanel
	virtual SARibbonPanel* createRibbonPanel(QWidget* parent);
	/// Create SARibbonSeparatorWidget
	virtual SARibbonSeparatorWidget* createRibbonSeparatorWidget(QWidget* parent);
	/// Create SARibbonGallery
	virtual SARibbonGallery* createRibbonGallery(QWidget* parent);
	/// Create SARibbonGalleryGroup
	virtual SARibbonGalleryGroup* createRibbonGalleryGroup(QWidget* parent);
	/// Create SARibbonToolButton
	virtual SARibbonToolButton* createRibbonToolButton(QWidget* parent);
	/// Create SARibbonStackedWidget
	virtual SARibbonStackedWidget* createRibbonStackedWidget(SARibbonBar* parent);
	/// Create SARibbonButtonGroupWidget
	virtual SARibbonButtonGroupWidget* createButtonGroupWidget(QWidget* parent);
	/// Create SARibbonQuickAccessBar
	virtual SARibbonQuickAccessBar* createQuickAccessBar(QWidget* parent);
	/// Create SARibbonSystemButtonBar
	virtual SARibbonSystemButtonBar* createWindowButtonGroup(QWidget* parent);
	/// Create SARibbonPanelOptionButton
	virtual SARibbonPanelOptionButton* createRibbonPanelOptionButton(SARibbonPanel* panel);
	/// Create SARibbonTitleIconWidget
	virtual SARibbonTitleIconWidget* createRibbonTitleIconWidget(QWidget* parent);
	/// Create SARibbonPanelLabel
	virtual SARibbonPanelLabel* createRibbonPanelLabel(QWidget* parent);
};

#endif  // SARIBBONELEMENTFACTORY_H

/*** End of inlined file: SARibbonElementFactory.h ***/


/*** Start of inlined file: SARibbonElementManager.h ***/
#ifndef SARIBBONELEMENTMANAGER_H
#define SARIBBONELEMENTMANAGER_H

/**
 * \if ENGLISH
 * @brief Global singleton class for managing SARibbonElementFactory
 * @details If you want to override a component, you first need to create your own ElementFactory
 * @code
 * class MyRibbonElementFactory : public SARibbonElementFactory{
 * public:
 *     ...
 *     virtual SARibbonPanel* createRibbonPanel(QWidget* parent){
 *         return new MyRibbonPanel(parent);
 *     }
 * };
 * @endcode
 * @details Then you need to set your own ElementFactory before creating the ribbonbar
 * @details This is typically done in the main function
 * @code
 * SARibbonElementManager::instance()->setupFactory(new MyRibbonElementFactory);
 * @endcode
 * @details This way, the panel created by SARibbon will be your overridden MyRibbonPanel
 * \endif
 *
 * \if CHINESE
 * @brief 此类是一个全局单例，用于管理SARibbonElementFactory
 * @details 如果你有自己的某个部件要重写，首先你需要有自己的ElementFactory
 * @code
 * class MyRibbonElementFactory : public SARibbonElementFactory{
 * public:
 *     ...
 *     virtual SARibbonPanel* createRibbonPanel(QWidget* parent){
 *         return new MyRibbonPanel(parent);
 *     }
 * };
 * @endcode
 * @details 然后，你需要在ribbonbar创建之前把自己的ElementFactory设置进去
 * @details 这个一般会在main函数中进行
 * @code
 * SARibbonElementManager::instance()->setupFactory(new MyRibbonElementFactory);
 * @endcode
 * @details 这样，SARibbon创建的panel就是你自己重写的MyRibbonPanel
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonElementManager
{
protected:
	/// Constructor for SARibbonElementManager
	SARibbonElementManager();

public:
	/// Destructor for SARibbonElementManager
	virtual ~SARibbonElementManager();
	/// Get the singleton instance
	static SARibbonElementManager* instance();
	/// Get the current factory
	SARibbonElementFactory* factory();
	/// Set the factory
	void setupFactory(SARibbonElementFactory* fac);

private:
	QScopedPointer< SARibbonElementFactory > mFactory;
};
#ifndef RibbonSubElementMgr
#define RibbonSubElementMgr SARibbonElementManager::instance()
#endif
#ifndef RibbonSubElementFactory
#define RibbonSubElementFactory SARibbonElementManager::instance()->factory()
#endif

#endif  // SARIBBONELEMENTMANAGER_H

/*** End of inlined file: SARibbonElementManager.h ***/


/*** Start of inlined file: SARibbonCustomizeData.h ***/
#ifndef SARIBBONCUSTOMIZEDATA_H
#define SARIBBONCUSTOMIZEDATA_H

#include <QList>
class SARibbonBar;
class SARibbonMainWindow;

/**
 * \if ENGLISH
 * @brief Data class for recording all customization operations
 * @note This data depends on @ref SARibbonActionsManager, use this class after SARibbonActionsManager
 * \endif
 *
 * \if CHINESE
 * @brief 记录所有自定义操作的数据类
 * @note 此数据依赖于@ref SARibbonActionsManager 要在SARibbonActionsManager之后使用此类
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonCustomizeData
{
public:
	/**
	 * \if ENGLISH
	 * @brief Action type enumeration
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 操作类型枚举
	 * \endif
	 */
	enum ActionType
	{
		UnknowActionType = 0,           ///< 未知操作
		AddCategoryActionType,          ///< 添加category操作(1)
		AddPanelActionType,             ///< 添加panel操作(2)
		AddActionActionType,            ///< 添加action操作(3)
		RemoveCategoryActionType,       ///< 删除category操作(4)
		RemovePanelActionType,          ///< 删除panel操作(5)
		RemoveActionActionType,         ///< 删除action操作(6)
		ChangeCategoryOrderActionType,  ///< 改变category顺序的操作(7)
		ChangePanelOrderActionType,     ///< 改变panel顺序的操作(8)
		ChangeActionOrderActionType,    ///< 改变action顺序的操作(9)
		RenameCategoryActionType,       ///< 对category更名操作(10)
		RenamePanelActionType,          ///< 对Panel更名操作(11)
		VisibleCategoryActionType       ///< 对category执行隐藏/显示操作(12)
	};
	/// Default constructor
	SARibbonCustomizeData();
	/// Constructor with action type and manager
	SARibbonCustomizeData(ActionType type, SARibbonActionsManager* mgr = nullptr);
	/// Get the action type of the CustomizeData
	ActionType actionType() const;

	/// Set the action type of the CustomizeData
	void setActionType(ActionType a);

	/// Check if this is a valid CustomizeData
	bool isValid() const;

	/// Apply SARibbonCustomizeData to SARibbonBar
	bool apply(SARibbonBar* bar) const;

	/// Get the action manager pointer
	SARibbonActionsManager* actionManager();

	/// Set the ActionsManager
	void setActionsManager(SARibbonActionsManager* mgr);

	/// Create AddCategoryActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeAddCategoryCustomizeData(const QString& title, int index, const QString& objName);

	/// Create AddPanelActionType SARibbonCustomizeData
	static SARibbonCustomizeData
	makeAddPanelCustomizeData(const QString& title, int index, const QString& categoryobjName, const QString& objName);

	/// Create AddActionActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeAddActionCustomizeData(const QString& key,
															SARibbonActionsManager* mgr,
															SARibbonPanelItem::RowProportion rp,
															const QString& categoryObjName,
															const QString& panelObjName);

	/// Create RenameCategoryActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeRenameCategoryCustomizeData(const QString& newname, const QString& categoryobjName);

	/// Create RenamePanelActionType SARibbonCustomizeData
	static SARibbonCustomizeData
	makeRenamePanelCustomizeData(const QString& newname, const QString& categoryobjName, const QString& panelObjName);

	/// Create RemoveCategoryActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeRemoveCategoryCustomizeData(const QString& categoryobjName);

	/// Create ChangeCategoryOrderActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeChangeCategoryOrderCustomizeData(const QString& categoryobjName, int moveindex);

	/// Create ChangePanelOrderActionType SARibbonCustomizeData
	static SARibbonCustomizeData
	makeChangePanelOrderCustomizeData(const QString& categoryobjName, const QString& panelObjName, int moveindex);

	/// Create ChangeActionOrderActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeChangeActionOrderCustomizeData(const QString& categoryobjName,
																	const QString& panelObjName,
																	const QString& key,
																	SARibbonActionsManager* mgr,
																	int moveindex);

	/// Create RemovePanelActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeRemovePanelCustomizeData(const QString& categoryobjName, const QString& panelObjName);

	/// Create RemoveActionActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeRemoveActionCustomizeData(const QString& categoryobjName,
															   const QString& panelObjName,
															   const QString& key,
															   SARibbonActionsManager* mgr);

	/// Create VisibleCategoryActionType SARibbonCustomizeData
	static SARibbonCustomizeData makeVisibleCategoryCustomizeData(const QString& categoryobjName, bool isShow);

	/// Check if customization is allowed for the object
	static bool isCanCustomize(QObject* obj);
	/// Set whether customization is allowed for the object
	static void setCanCustomize(QObject* obj, bool canbe = true);

	/// Simplify QList<SARibbonCustomizeData>
	static QList< SARibbonCustomizeData > simplify(const QList< SARibbonCustomizeData >& csd);

public:
	/**
	 * \if ENGLISH
	 * @brief Parameter for recording order
	 * @details When actionType==AddCategoryActionType, this parameter records the insert position of Category,
	 *          When actionType==AddPanelActionType, this parameter records the insert position of panel,
	 *          When actionType==AddActionActionType, this parameter records the insert position of action
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 记录顺序的参数
	 * @details 在actionType==AddCategoryActionType时，此参数记录Category的insert位置,
	 *          在actionType==AddPanelActionType时，此参数记录panel的insert位置,
	 *          在actionType==AddActionActionType时，此参数记录panel的insert位置
	 * \endif
	 */
	int indexValue;

	/**
	 * \if ENGLISH
	 * @brief Parameter for recording title, index, etc.
	 * @details When actionType==AddCategoryActionType, key is the category title,
	 *          When actionType==AddPanelActionType, key is the panel title,
	 *          When actionType==AddActionActionType, key is the action query basis, based on SARibbonActionsManager::action query
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 记录标题、索引等参数
	 * @details 在actionType==AddCategoryActionType时，key为category标题，
	 *          在actionType==AddPanelActionType时，key为panel标题，
	 *          在actionType==AddActionActionType时，key为action的查询依据，基于SARibbonActionsManager::action查询
	 * \endif
	 */
	QString keyValue;

	/**
	 * \if ENGLISH
	 * @brief Record categoryObjName for locating Category
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 记录categoryObjName，用于定位Category
	 * \endif
	 */
	QString categoryObjNameValue;

	/**
	 * \if ENGLISH
	 * @brief Record panelObjName, SARibbon's Customize index is mostly based on objname
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 记录panelObjName，saribbon的Customize索引大部分基于objname
	 * \endif
	 */
	QString panelObjNameValue;

	SARibbonPanelItem::RowProportion actionRowProportionValue;  ///< 行的占比，ribbon中有large，media和small三种占比,见@ref RowProportion
private:
	ActionType mType;  ///< 标记这个data是category还是panel亦或是action
	SARibbonActionsManager* mActionsManagerPointer;
};
Q_DECLARE_METATYPE(SARibbonCustomizeData)

typedef QList< SARibbonCustomizeData > SARibbonCustomizeDataList;

#endif  // SARIBBONCUSTOMIZEDATA_H

/*** End of inlined file: SARibbonCustomizeData.h ***/


/*** Start of inlined file: SARibbonCustomizeWidget.h ***/
#ifndef SARIBBONCUSTOMIZEWIDGET_H
#define SARIBBONCUSTOMIZEWIDGET_H

#include <QWidget>

// SARibbonCustomizeWidget 特有
class SARibbonCustomizeWidgetUi;
class SARibbonMainWindow;
class SARibbonBar;
//
class QStandardItemModel;
class QStandardItem;
class QAbstractButton;
//
class QXmlStreamWriter;
class QXmlStreamReader;

/**
 * \if ENGLISH
 * @brief Customization interface widget
 * @note SARibbon customization is step-based. If @ref sa_apply_customize_from_xml_file or similar functions are called
 * before the window is generated, you need to call @ref SARibbonCustomizeWidget::fromXml before the dialog is generated
 * to synchronize the configuration file, so that the modified configuration file is consistent.
 * \endif
 *
 * \if CHINESE
 * @brief 自定义界面窗口
 * @note SARibbon的自定义是基于步骤的，如果在窗口生成前调用了@ref sa_apply_customize_from_xml_file 类似函数
 *       那么在对话框生成前为了保证同步需要调用@ref SARibbonCustomizeWidget::fromXml
 * 同步配置文件，这样再次修改后的配置文件就一致
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonCustomizeWidget : public QWidget
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonCustomizeWidget)
public:
	/// Constructor using SARibbonMainWindow
	explicit SARibbonCustomizeWidget(SARibbonMainWindow* ribbonWindow,
									 QWidget* parent   = nullptr,
									 Qt::WindowFlags f = Qt::WindowFlags());
	/// Constructor using SARibbonBar directly
	explicit SARibbonCustomizeWidget(SARibbonBar* ribbonbar,
									 QWidget* parent   = nullptr,
									 Qt::WindowFlags f = Qt::WindowFlags());
	~SARibbonCustomizeWidget();

	/**
	 * \if ENGLISH
	 * @brief Ribbon tree display type
	 * \endif
	 *
	 * \if CHINESE
	 * @brief 定义ribbon树的显示类型
	 * \endif
	 */
	enum RibbonTreeShowType
	{
		ShowAllCategory,  ///< Show all categories including context categories
		ShowMainCategory  ///< Show main categories only, excluding context categories
	};

	/**
	 * \if ENGLISH
	 * @brief Item roles for QStandardItem
	 * \endif
	 *
	 * \if CHINESE
	 * @brief QStandardItem对应的role
	 * \endif
	 */
	enum ItemRole
	{
		LevelRole        = Qt::UserRole + 1,  ///< Level: 0=category, 1=panel, 2=item
		PointerRole      = Qt::UserRole + 2,  ///< Pointer storage, cast based on LevelRole
		CanCustomizeRole = Qt::UserRole + 3,  ///< Whether this item can be customized (bool)
		CustomizeRole = Qt::UserRole + 4,  ///< Whether this is a custom item (bool), mainly for self-added tabs and panels
		CustomizeObjNameRole = Qt::UserRole + 5  ///< Temporary custom content object name (QString)
	};

	/// Set the action manager
	void setupActionsManager(SARibbonActionsManager* mgr);

	/// Check if there is content to store, corresponding to save action
	bool isApplied() const;

	/// Check if there are modified contents, corresponding to apply action
	bool isCached() const;

	/// Get the model
	const QStandardItemModel* model() const;

	/// Update model based on current radiobutton selection
	void updateModel();

	/// Update model with specified type
	void updateModel(RibbonTreeShowType type);

	/// Apply all settings
	bool applys();

	/// Convert to XML
	bool toXml(QXmlStreamWriter* xml) const;
	/// Convert to XML file
	bool toXml(const QString& xmlpath) const;

	/// Load from XML, for file-based settings, it is recommended to call this function before the dialog is displayed
	void fromXml(QXmlStreamReader* xml);
	/// Load from XML file
	void fromXml(const QString& xmlpath);

	/// Apply XML configuration
	static bool fromXml(QXmlStreamReader* xml, SARibbonBar* bar, SARibbonActionsManager* mgr);

	/// Cache applied actions, these actions will not be cleared by clear(), used for local storage
	void makeActionsApplied();

	/// Clear applied actions, need to clear applied actions after cancel operation
	void clearApplied();
	/// Clear cached actions, after executing applys function, if you want to continue calling, you should clear, otherwise it will cause exceptions
	void clearCache();
	/// Clear all actions, excluding locally read data
	void clear();

protected:
	/// Simplify QList<SARibbonCustomizeData>, merge some actions
	void simplify();

	/// Get the row proportion selected in the current interface
	SARibbonPanelItem::RowProportion selectedRowProportion() const;

	/// Get the action selected in listview
	QAction* selectedAction() const;
	/// Convert item to action
	QAction* itemToAction(QStandardItem* item) const;

	/// Get the selected item in ribbon tree
	QStandardItem* selectedItem() const;

	/// Get the level of selected ribbon tree item
	int selectedRibbonLevel() const;

	/// Get the level based on selected item
	int itemLevel(QStandardItem* item) const;

	/// Set an item to be selected
	void setSelectItem(QStandardItem* item, bool ensureVisible = true);

	/// Check if item can be customized
	bool isItemCanCustomize(QStandardItem* item) const;
	bool isSelectedItemCanCustomize() const;

	/// Check if item is a customize item
	bool isCustomizeItem(QStandardItem* item) const;
	bool isSelectedItemIsCustomize() const;

	/// Remove an item
	void removeItem(QStandardItem* item);

private Q_SLOTS:
	void onComboBoxActionIndexCurrentIndexChanged(int index);
	void onRadioButtonGroupButtonClicked(QAbstractButton* b);
	void onPushButtonNewCategoryClicked();
	void onPushButtonNewPanelClicked();
	void onPushButtonRenameClicked();
	void onPushButtonAddClicked();
	void onPushButtonDeleteClicked();
	void onListViewSelectClicked(const QModelIndex& index);
	void onTreeViewResultClicked(const QModelIndex& index);
	void onToolButtonUpClicked();
	void onToolButtonDownClicked();
	void onItemChanged(QStandardItem* item);
	void onLineEditSearchActionTextEdited(const QString& text);
	void onPushButtonResetClicked();

private:
	void init(SARibbonBar* ribbonbar);
	void initConnection();

private:
	SARibbonCustomizeWidgetUi* ui;
};

/**
 * \if ENGLISH
 * @brief Convert to XML
 * @details This function only writes elements, not document related content.
 * @param xml QXmlStreamWriter pointer
 * @param cds Steps generated based on QList<SARibbonCustomizeData>
 * @return Returns false if exception occurs, also returns false if there is no customization data
 * \endif
 *
 * \if CHINESE
 * @brief 转换为xml
 * @details 此函数仅会写element，不会写document相关内容
 * @param xml QXmlStreamWriter指针
 * @param cds 基于QList<SARibbonCustomizeData>生成的步骤
 * @return 如果出现异常，返回false,如果没有自定义数据也会返回false
 * \endif
 */
bool SA_RIBBON_EXPORT sa_customize_datas_to_xml(QXmlStreamWriter* xml, const QList< SARibbonCustomizeData >& cds);

/**
 * \if ENGLISH
 * @brief Get QList<SARibbonCustomizeData> from XML
 * @param xml XML stream reader
 * @param mgr Action manager
 * @return QList<SARibbonCustomizeData>
 * \endif
 *
 * \if CHINESE
 * @brief 通过xml获取QList<SARibbonCustomizeData>
 * @param xml XML流读取器
 * @param mgr Action管理器
 * @return QList<SARibbonCustomizeData>
 * \endif
 */
QList< SARibbonCustomizeData > SA_RIBBON_EXPORT sa_customize_datas_from_xml(QXmlStreamReader* xml,
																			SARibbonActionsManager* mgr);

/**
 * \if ENGLISH
 * @brief Apply QList<SARibbonCustomizeData>
 * @param cds Customize data list
 * @param w SARibbonBar pointer
 * @return Number of successfully applied items
 * \endif
 *
 * \if CHINESE
 * @brief 应用QList<SARibbonCustomizeData>
 * @param cds 自定义数据列表
 * @param w SARibbonBar指针
 * @return 成功应用的个数
 * \endif
 */
int SA_RIBBON_EXPORT sa_customize_datas_apply(const QList< SARibbonCustomizeData >& cds, SARibbonBar* w);

/**
 * \if ENGLISH
 * @brief Reverse apply QList<SARibbonCustomizeData>
 * @param cds Customize data list
 * @param w SARibbonBar pointer
 * @return Number of successfully reversed items
 * \endif
 *
 * \if CHINESE
 * @brief 反向取消应用
 * @param cds 自定义数据列表
 * @param w SARibbonBar指针
 * @return 成功应用的个数
 * \endif
 */
int SA_RIBBON_EXPORT sa_customize_datas_reverse(const QList< SARibbonCustomizeData >& cds, SARibbonBar* w);

/**
 * \if ENGLISH
 * @brief Directly load XML customization file for ribbon customization display
 * @param filePath XML configuration file path
 * @param bar SARibbonBar pointer
 * @param mgr Action manager
 * @return Returns true if successful
 * @note Repeating loading a configuration file will cause exceptions. To avoid this, generally use a variable to ensure it is only loaded once.
 * \endif
 *
 * \if CHINESE
 * @brief 直接加载xml自定义ribbon配置文件用于ribbon的自定义显示
 * @param filePath xml配置文件路径
 * @param bar SARibbonBar指针
 * @param mgr action管理器
 * @return 成功返回true
 * @note 重复加载一个配置文件会发生异常，为了避免此类事件发生，一般通过一个变量保证只加载一次
 * \endif
 */
bool SA_RIBBON_EXPORT sa_apply_customize_from_xml_file(const QString& filePath, SARibbonBar* bar, SARibbonActionsManager* mgr);

#endif  // SARIBBONCUSTOMIZEWIDGET_H

/*** End of inlined file: SARibbonCustomizeWidget.h ***/


/*** Start of inlined file: SARibbonCustomizeDialog.h ***/
#ifndef SARIBBONCUSTOMIZEDIALOG_H
#define SARIBBONCUSTOMIZEDIALOG_H

#include <QDialog>

class SARibbonActionsManager;
class SARibbonCustomizeDialogUi;
class QXmlStreamWriter;

/**
 * \if ENGLISH
 * @brief Dialog wrapper for SARibbonCustomizeWidget
 * @details This function depends on @ref SARibbonActionsManager. SARibbonActionsManager is recommended to be a member variable of mainwindow.
 *          SARibbonActionsManager can quickly bind all QActions, see SARibbonActionsManager documentation for details.
 * @note SARibbon customization is step-based. If @ref sa_apply_customize_from_xml_file or similar functions are called before the window is generated,
 *       you need to call @ref SARibbonCustomizeDialog::fromXml before the dialog is generated to synchronize the configuration file,
 *       so that the modified configuration file is consistent.
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonCustomizeWidget的对话框封装
 * @details 此功能依赖于@ref SARibbonActionsManager ，SARibbonActionsManager建议作为mianwindow的成员变量，
 *          SARibbonActionsManager可以快速绑定所有QAction，详细见SARibbonActionsManager的说明
 * @note SARibbon的自定义是基于步骤的，如果在窗口生成前调用了@ref sa_apply_customize_from_xml_file 类似函数
 *       那么在对话框生成前为了保证同步需要调用@ref SARibbonCustomizeDialog::fromXml 同步配置文件，这样再次修改后的配置文件就一致
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonCustomizeDialog : public QDialog
{
	Q_OBJECT
public:
	/// Constructor for SARibbonCustomizeDialog
	explicit SARibbonCustomizeDialog(SARibbonMainWindow *ribbonWindow, QWidget *p = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	/// Destructor for SARibbonCustomizeDialog
	~SARibbonCustomizeDialog();
	/// Set the action manager
	void setupActionsManager(SARibbonActionsManager *mgr);

	/// Check if there is content to store, corresponding to save action
	bool isApplied() const;

	/// Check if there are modified contents, corresponding to apply action
	bool isCached() const;

	/// Apply all settings
	bool applys();

	/// Clear all actions
	void clear();

	/// Convert to XML
	bool toXml(QXmlStreamWriter *xml) const;
	/// Convert to XML file
	bool toXml(const QString& xmlpath) const;

	/// Load from XML, for file-based settings, it is recommended to call this function before the dialog is displayed
	void fromXml(QXmlStreamReader *xml);
	/// Load from XML file
	void fromXml(const QString& xmlpath);

	/// Return the SARibbonCustomizeWidget pointer
	SARibbonCustomizeWidget *customizeWidget() const;

private:
	void initConnection();

	SARibbonCustomizeDialogUi *ui;
};

#endif // SARIBBONCUSTOMIZEDIALOG_H

/*** End of inlined file: SARibbonCustomizeDialog.h ***/


/*** Start of inlined file: SARibbonMainWindow.h ***/
#ifndef SARIBBONMAINWINDOW_H
#define SARIBBONMAINWINDOW_H

#include <QMainWindow>

#if !SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
class SAFramelessHelper;
#endif
class QAction;
class SARibbonBar;
class SARibbonSystemButtonBar;
class QScreen;
/**
 * \if ENGLISH
 * @brief Must use this class instead of QMainWindow to use SARibbonBar
 * @details Due to the significant difference between ribbon style and traditional Toolbar style,
 * @details SARibbonBar usage requires replacing the original QMainWindow with SARibbonMainWindow,
 * @details SARibbonMainWindow is a borderless window inherited from QMainWindow (currently using a third-party borderless solution https://github.com/wangwenx190/framelesshelper),
 * @details The useRibbon parameter in its constructor is used to specify whether to use ribbon style, defaulting to true
 * @code
 * SARibbonMainWindow(QWidget* parent = nullptr, bool useRibbon = true);
 * @endcode
 * @details If you want to switch back to non-ribbon style, just set useRibbon to false,
 * @details The isUseRibbon member function is used to determine whether the current mode is ribbon mode, which is very useful when compatible with traditional Toolbar style and ribbon style
 * @details However, this does not support dynamic switching. Therefore, in actual projects, if you want to switch, you
 * need to write a configuration file and pass the mode during program construction,
 * @details And your program must make good judgments, because in non-Ribbon mode, all Ribbon-related interfaces will return null pointers
 * @code
 * bool isUseRibbon() const;
 * @endcode
 * @details @ref SARibbonMainWindow provides several common ribbon styles
 * @details The ribbon style can be changed through @ref setRibbonTheme, and users can also define their own styles through qss
 * @details If you have inherited SARibbonBar yourself, you can set your own ribbonbar through @ref setRibbonBar
 * @details There is also a more efficient way to implement a custom Ribbon, which is to inherit a @ref SARibbonElementFactory
 * @code
 * class MyRibbonFactory:public SARibbonElementFactory{
 * ...
 * virtual SARibbonBar* createRibbonBar(QWidget* parent){
 *     return new MyRibbonBar(parent);
 * }
 * };
 * @endcode
 * @details Before SARibbonMainWindow is generated (usually in the main function), set the element factory:
 * @code
 * SARibbonElementManager::instance()->setupFactory(new MyRibbonFactory());
 * @endcode
 * @details At this point, Ribbon elements will be generated through the MyRibbonFactory interface
 * \endif
 *
 * \if CHINESE
 * @brief 如果要使用SARibbonBar，必须使用此类代替QMainWindow
 * @details 由于ribbon的风格和传统的Toolbar风格差异较大，
 * @details SARibbonBar使用需要把原有的QMainWindow替换为SARibbonMainWindow,
 * @details SARibbonMainWindow是个无边框窗体，继承自QMainWindow（目前使用第三方的无边框方案https://github.com/wangwenx190/framelesshelper），
 * @details 其构造函数的参数useRibbon用于指定是否使用ribbon风格，默认为true
 * @code
 * SARibbonMainWindow(QWidget* parent = nullptr,bool useRibbon = true);
 * @endcode
 * @details 如果想换回非ribbon风格，只需要把useRibbon设置为false即可,
 * @details 成员函数isUseRibbon用于判断当前是否为ribbon模式，这个函数在兼容传统Toolbar风格和ribbon风格时非常有用
 * @details 但这个不支持动态切换，因此，实际工程中，你若要进行切换，需要写配置文件，程序在构造时传入模式，
 * @details 并且，你的程序要做好判断，因为非Ribbon模式下，所有Ribbon相关的接口都会返回空指针
 * @code
 * bool isUseRibbon() const;
 * @endcode
 * @details @ref SARibbonMainWindow 提供了几种常用的ribbon样式
 * @details 通过@ref setRibbonTheme 可改变ribbon的样式，用户也可通过qss自己定义自己的样式
 * @details 如果你自己继承了SARibbonBar，你可以通过@ref setRibbonBar 设置自己的ribbonbar进去
 * @details 另外有个一个更加高效的方法，来实现自定义的Ribbon，就是继承一个@ref SARibbonElementFactory
 * @code
 * class MyRibbonFactory:public SARibbonElementFactory{
 * ...
 * virtual SARibbonBar* createRibbonBar(QWidget* parent){
 *     return new MyRibbonBar(parent);
 * }
 * };
 * @endcode
 * @details SARibbonMainWindow生成之前(一般在main函数），设置元件工厂：
 * @code
 * SARibbonElementManager::instance()->setupFactory(new MyRibbonFactory());
 * @endcode
 * @details 此时，Ribbon的元素会通过MyRibbonFactory的接口来生成
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonMainWindow : public QMainWindow
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonMainWindow)
	friend class SARibbonBar;
	Q_PROPERTY(SARibbonTheme ribbonTheme READ ribbonTheme WRITE setRibbonTheme)

public:
	/// Constructor for SARibbonMainWindow
	explicit SARibbonMainWindow(QWidget* parent                = nullptr,
								SARibbonMainWindowStyles style = SARibbonMainWindowStyleFlag::UseRibbonMenuBar
																 | SARibbonMainWindowStyleFlag::UseRibbonFrame,
								const Qt::WindowFlags flags = Qt::WindowFlags());
	/// Destructor for SARibbonMainWindow
	~SARibbonMainWindow() override;
	/// Return SARibbonBar
	SARibbonBar* ribbonBar() const;
	/// Set ribbonbar
	void setRibbonBar(SARibbonBar* ribbon);
#if !SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
	/// Return SAFramelessHelper
	SAFramelessHelper* framelessHelper() const;
	/// Set to use rubber band indication instead of immediate scaling during resizing, which is more friendly for software with large rendering (such as CAD, 3D)
	void setRubberBandOnResize(bool on);
	/// Check if rubber band is used on resize
	bool isRubberBandOnResize() const;
#else
	/// If there are custom windows in the ribbon added to non-clickable areas such as the title bar, and you want them
	/// to be clickable, you need to call this interface to inform them that they are clickable
	void setFramelessHitTestVisible(QWidget* w, bool visible = true);
#endif
	/// This function is only used to control the display of minimize, maximize and close buttons
	void updateWindowFlag(Qt::WindowFlags flags);
	/// Note: Setting the theme in the constructor will not take full effect, use QTimer to put it at the end of the queue to execute
	/// QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); });
	void setRibbonTheme(SARibbonTheme theme);
	/// Get ribbon theme
	SARibbonTheme ribbonTheme() const;
	/// Determine whether the current mode is ribbon mode
	bool isUseRibbon() const;
	/// Get the bar where the maximize, minimize, and close buttons are located. You can set content next to the maximize and minimize buttons through this function
	SARibbonSystemButtonBar* windowButtonBar() const;
	/// Get the current mainwindow style
	SARibbonMainWindowStyles ribbonMainwindowStyle() const;

	/// Pass ribbonbar events to frameless
	virtual bool eventFilter(QObject* obj, QEvent* e) Q_DECL_OVERRIDE;

protected:
	/// Factory function to create ribbonbar
	SARibbonBar* createRibbonBar();
private Q_SLOTS:
	/// Handle primary screen changed event
	void onPrimaryScreenChanged(QScreen* screen);
};

/**
 * \if ENGLISH
 * @brief Event handler for SARibbonMainWindow, mainly handles systembar position adjustment
 * \endif
 *
 * \if CHINESE
 * @brief 针对SARibbonMainWindow的事件处理器，主要处理systembar的位置调整
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonMainWindowEventFilter : public QObject
{
	Q_OBJECT
public:
	/// Constructor for SARibbonMainWindowEventFilter
	explicit SARibbonMainWindowEventFilter(QObject* par);
	/// Destructor for SARibbonMainWindowEventFilter
	~SARibbonMainWindowEventFilter();
	/// Event filter
	virtual bool eventFilter(QObject* obj, QEvent* e) override;
};

#endif  // SARIBBONMAINWINDOW_H

/*** End of inlined file: SARibbonMainWindow.h ***/


/*** Start of inlined file: SARibbonWidget.h ***/
#ifndef SARIBBONWIDGET_H
#define SARIBBONWIDGET_H

#include <QWidget>

class SARibbonBar;
class QScreen;
/**
 * @brief 带SARibbonBar的Widget
 *
 * @note 注意，SARibbonWidget利用了布局来把ribbonbar放到最顶部，不要给此widget添加布局，否则会导致布局异常，
 * 正确的做法是把布局的窗口通过setWidget设置进SARibbonWidget
 *
 */
class SA_RIBBON_EXPORT SARibbonWidget : public QWidget
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonWidget)
	friend class SARibbonBar;
	Q_PROPERTY(SARibbonTheme ribbonTheme READ ribbonTheme WRITE setRibbonTheme)

public:
	explicit SARibbonWidget(QWidget* parent = nullptr);
	~SARibbonWidget();
	// 返回SARibbonBar
	SARibbonBar* ribbonBar() const;
	// 设置ribbonbar
	void setRibbonBar(SARibbonBar* bar);

	// 注意主题在构造函数设置主题会不完全生效，使用QTimer投放到队列最后执行即可
	// QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonTheme::RibbonThemeDark); });
	void setRibbonTheme(SARibbonTheme theme);
	SARibbonTheme ribbonTheme() const;
	// 判断当前是否使用ribbon模式
	bool isUseRibbon() const;
	// 设置窗口
	void setWidget(QWidget* w);
	QWidget* widget() const;
	//
	QWidget* takeWidget();
private Q_SLOTS:
	void onPrimaryScreenChanged(QScreen* screen);
};

#endif  // SARIBBONWIDGET_H

/*** End of inlined file: SARibbonWidget.h ***/


/*** Start of inlined file: SARibbonApplicationWidget.h ***/
#ifndef SARIBBONAPPLICATIONWIDGET_H
#define SARIBBONAPPLICATIONWIDGET_H

#include <QFrame>
class QShowEvent;
class QKeyEvent;
class SARibbonMainWindow;
/**
 * \if ENGLISH
 * @brief Widget for application button popup
 * @details This widget is used to display a file window similar to Office software when the application button is clicked. It covers the entire MainWindow and handles some border messages.
 * \endif
 *
 * \if CHINESE
 * @brief 应用程序按钮弹出窗口
 * @details 这个窗口是用于applicationButton点击后弹出类似office系列软件的文件窗口，此窗口会把整个MainWindow覆盖，同时会接管边框的一些消息
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonApplicationWidget : public QFrame
{
	Q_OBJECT
public:
	// Constructor for SARibbonApplicationWidget
	SARibbonApplicationWidget(SARibbonMainWindow* parent);

protected:
	// Resizes the widget to match the parent's size
	virtual void resizeToParent(const QSize& parentSize);

protected:
	// Event filter to handle parent window events
	bool eventFilter(QObject* obj, QEvent* ev) override;
	// Handles the show event
	void showEvent(QShowEvent* event) override;
	// Handles key press events
	void keyPressEvent(QKeyEvent* ev) override;
};

#endif  // SARIBBONAPPLICATIONWIDGET_H

/*** End of inlined file: SARibbonApplicationWidget.h ***/

/*** End of inlined file: SARibbonAmalgamTemplatePublicHeaders.h ***/

#endif
