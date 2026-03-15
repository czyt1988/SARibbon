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
 * @def color-widgets的数字版本 MAJ.MIN.PAT
 */
#ifndef SA_COLOR_WIDGETS_VERSION_MAJ
#define SA_COLOR_WIDGETS_VERSION_MAJ 0
#endif
/**
 * @def color-widgets的数字版本 MAJ.MIN.PAT
 */
#ifndef SA_COLOR_WIDGETS_VERSION_MIN
#define SA_COLOR_WIDGETS_VERSION_MIN 1
#endif
/**
 * @def color-widgets的数字版本 MAJ.MIN.PAT
 */
#ifndef SA_COLOR_WIDGETS_VERSION_PAT
#define SA_COLOR_WIDGETS_VERSION_PAT 0
#endif

/**
 * @def   模仿Q_DECLARE_PRIVATE，但不用前置声明而是作为一个内部类
 */
#ifndef SA_COLOR_WIDGETS_DECLARE_PRIVATE
#define SA_COLOR_WIDGETS_DECLARE_PRIVATE(classname)                                                                    \
	class PrivateData;                                                                                                 \
	friend class classname::PrivateData;                                                                               \
	QScopedPointer< PrivateData > d_ptr;
#endif
/**
 * @def   模仿Q_DECLARE_PUBLIC
 */
#ifndef SA_COLOR_WIDGETS_DECLARE_PUBLIC
#define SA_COLOR_WIDGETS_DECLARE_PUBLIC(classname)                                                                     \
	friend class classname;                                                                                            \
	classname* q_ptr { nullptr };
#endif

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
#define SA_RIBBON_BAR_VERSION_MIN 6
#endif
/**
 * @def ribbon的数字版本 MAJ.MIN.{PAT}
 */
#ifndef SA_RIBBON_BAR_VERSION_PAT
#define SA_RIBBON_BAR_VERSION_PAT 1
#endif

/**
 * @def 版本号（字符串）
 */
#ifndef SARIBBON_VERSION
#define SARIBBON_VERSION "2.6.1"
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
 * @def SA_RIBBON_DECLARE_PRIVATE
 * @brief 模仿Q_DECLARE_PRIVATE，但不用前置声明而是作为一个内部类
 *
 * 例如:
 *
 * @code
 * //header
 * class A
 * {
 *  SA_RIBBON_DECLARE_PRIVATE(A)
 * };
 * @endcode
 *
 * 其展开效果为：
 *
 * @code
 * class A{
 *  class PrivateData;
 *  friend class A::PrivateData;
 *  std::unique_ptr< PrivateData > d_ptr;
 * }
 * @endcode
 *
 * 这样前置声明了一个内部类PrivateData，在cpp文件中建立这个内部类的实现
 *
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
 *
 */
#ifndef SA_RIBBON_DECLARE_PRIVATE
#define SA_RIBBON_DECLARE_PRIVATE(classname)                                                                           \
	class PrivateData;                                                                                                 \
	friend class classname::PrivateData;                                                                               \
	std::unique_ptr< PrivateData > d_ptr;
#endif

/**
 * @def 模仿Q_DECLARE_PUBLIC
 *
 * 配套SA_RIBBON_DECLARE_PRIVATE使用
 */
#ifndef SA_RIBBON_DECLARE_PUBLIC
#define SA_RIBBON_DECLARE_PUBLIC(classname)                                                                            \
	friend class classname;                                                                                            \
	classname* q_ptr { nullptr };                                                                                      \
	PrivateData(const PrivateData&)            = delete;                                                               \
	PrivateData& operator=(const PrivateData&) = delete;
#endif

/**
 * @def SA_RIBBON_IMPL_CONSTRUCT
 *
 * 配套SA_RIBBON_DECLARE_PRIVATE使用,在构造函数中构建PrivateData
 */
#ifndef SA_RIBBON_IMPL_CONSTRUCT
#define SA_RIBBON_IMPL_CONSTRUCT d_ptr(std::make_unique< PrivateData >(this))
#endif

/**
 *@def SA_D
 *@brief impl获取指针，参考Q_D
 */
#ifndef SA_D
#define SA_D(pointerName) PrivateData* pointerName = d_ptr.get()
#endif

/**
 *@def SA_DC
 *@brief impl获取指针，参考Q_DC
 */
#ifndef SA_DC
#define SA_DC(pointerName) const PrivateData* pointerName = d_ptr.get()
#endif

/**
 *@def SA_Q
 *@brief impl获取指针，参考Q_Q
 */
#ifndef SA_Q
#define SA_Q(pointerName) auto* pointerName = q_ptr
#endif

/**
 *@def SA_QC
 *@brief impl获取指针，参考Q_QC
 */
#ifndef SA_QC
#define SA_QC(pointerName) const auto* pointerName = q_ptr
#endif

/**
 * @brief 定义Ribbon的对其方式，目前支持左对齐和居中对其
 *
 * @note 如果你编译器提示：成员声明的限定名称非法，那么留意一下文件换行是否为LF，如果是把文件换行改为CRLF
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
	RibbonThemeDark2            ///< 暗色主题2
};

/**
 * @brief RibbonMainWindow的样式
 */
enum class SARibbonMainWindowStyleFlag : int
{
	UseRibbonFrame   = 1,  ///< 代表使用ribbon边框，ribbon边框会更紧凑
	UseNativeFrame   = 2,  ///< 代表使用操作系统的边框
	UseRibbonMenuBar = 4,
	UseNativeMenuBar = 8
};
Q_DECLARE_FLAGS(SARibbonMainWindowStyles, SARibbonMainWindowStyleFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(SARibbonMainWindowStyles)

/**
 * @def 属性，用于标记是否可以进行自定义，用于动态设置到@ref SARibbonCategory 和@ref SARibbonPanel
 * 值为bool，在为true时，可以通过@ref SARibbonCustomizeWidget 改变这个SARibbonCategory和SARibbonPanel的布局，
 * 默认不会有此属性，仅在有此属性且为true时才会在SARibbonCustomizeWidget中能显示为可设置
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
 * @brief 标准颜色菜单
 */
class SA_COLOR_WIDGETS_API SAColorMenu : public QMenu
{
	Q_OBJECT
	SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorMenu)
public:
	explicit SAColorMenu(QWidget* parent = nullptr);
	explicit SAColorMenu(const QString& title, QWidget* parent = nullptr);
	~SAColorMenu();
	// 快速绑定colorbtn
	void bindToColorToolButton(SAColorToolButton* btn);
	// ThemeColorsPalette对应的action
	QWidgetAction* themeColorsPaletteAction() const;
	// CustomColorsWidget对应的action
	QWidgetAction* getCustomColorsWidgetAction() const;
	// 自定义颜色action
	QAction* customColorAction() const;
	// 获取ThemeColorsPalette
	SAColorPaletteGridWidget* colorPaletteGridWidget() const;
	// 获取自定义颜色grid
	SAColorGridWidget* customColorsWidget() const;
	// 构建无颜色action，默认无颜色action是没有的
	void enableNoneColorAction(bool on = true);
	// 获取None Color Action,注意，enableNoneColorAction(true),之后才不是nullptr
	QAction* noneColorAction() const;
public Q_SLOTS:
	// 这是一个辅助槽函数，为了让用户自定义的其他action也能关联menu，可以调用此槽函数，实现selectedColor信号以及menu的隐藏
	void emitSelectedColor(const QColor& c);
Q_SIGNALS:
	/**
	 * @brief 选择了颜色触发的信号
	 * @param c
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
 * @brief 一个grid的Color布局
 *
 * 一个形如下面的颜色grid：
 *
 * □□□□□□□□□
 *
 * □□□□□□□□□
 *
 * □□□□□□□□□
 */
class SA_COLOR_WIDGETS_API SAColorGridWidget : public QWidget
{
	Q_OBJECT
	SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorGridWidget)
	Q_PROPERTY(int spacing READ spacing WRITE setSpacing)
public:
	using FunColorBtn = std::function< void(SAColorToolButton*) >;

public:
	SAColorGridWidget(QWidget* par = nullptr);
	~SAColorGridWidget();
	// 设置列数，行数量会根据列数量来匹配,如果设置-1或者0，说明不限定列数量，这样会只有一行
	void setColumnCount(int c);
	int columnCount() const;
	// 设置当前的颜色列表
	void setColorList(const QList< QColor >& cls);
	QList< QColor > getColorList() const;
	// 间隔
	int spacing() const;
	void setSpacing(int v);
	// 获取颜色的数量
	int colorCount() const;
	// 图标的尺寸
	void setColorIconSize(const QSize& s);
	QSize colorIconSize() const;
	// 设置颜色是否是checkable
	void setColorCheckable(bool on = true);
	bool isColorCheckable() const;
	// 获取当前选中的颜色
	QColor currentCheckedColor() const;
	// 获取index对于的colorbutton
	SAColorToolButton* colorButton(int index) const;
	// 垂直间距
	void setVerticalSpacing(int v);
	int verticalSpacing() const;
	// 水平间距
	void setHorizontalSpacing(int v);
	int horizontalSpacing() const;
	// 清除当前选中状态，这时没有颜色是选中的
	void clearCheckedState();
	// 对所有的colorbtn就行遍历处理，可以通过此函数进行tooltip设置等操作
	void iterationColorBtns(FunColorBtn fn);
	// 设置行最小高度
	void setRowMinimumHeight(int row, int minSize);
	// 让颜色块左对齐
	void setHorizontalSpacerToRight(bool on = true);
private Q_SLOTS:
	void onButtonClicked(QAbstractButton* btn);
	void onButtonPressed(QAbstractButton* btn);
	void onButtonReleased(QAbstractButton* btn);
	void onButtonToggled(QAbstractButton* btn, bool on);

Q_SIGNALS:
	/**
	 * @brief 对于check模式，check的颜色触发的信号
	 * @param c
	 * @param on
	 */
	void colorClicked(const QColor& c);
	void colorPressed(const QColor& c);
	void colorReleased(const QColor& c);
	void colorToggled(const QColor& c, bool on);

public:
	virtual QSize sizeHint() const Q_DECL_OVERRIDE;
};
namespace SA
{
/**
 * @brief 获取标准色列表（一共10种颜色）
 * @return
 */
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
 * @brief 类似office的颜色选择窗口，有一排标准色，下面有一个颜色板，有3行浅色，有2行深色
 */
class SA_COLOR_WIDGETS_API SAColorPaletteGridWidget : public QWidget
{
	Q_OBJECT
	SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorPaletteGridWidget)
public:
	SAColorPaletteGridWidget(QWidget* par = nullptr);
	SAColorPaletteGridWidget(const QList< QColor >& cls, QWidget* par = nullptr);
	~SAColorPaletteGridWidget();
	// 设置窗口维护的colorList
	void setColorList(const QList< QColor >& cls);
	QList< QColor > colorList() const;
	// 设置颜色深浅比例factor，默认为{ 180, 160, 140, 75, 50 }
	void setFactor(const QList< int >& factor);
	QList< int > factor() const;
	// 设置iconsize
	void setColorIconSize(const QSize& s);
	QSize colorIconSize() const;
	// 设置颜色是否是checkable
	void setColorCheckable(bool on = true);
	bool isColorCheckable() const;
private Q_SLOTS:
	void onMainColorClicked(const QColor& c);
	void onPaletteColorClicked(const QColor& c);

private:
	void init();
Q_SIGNALS:
	/**
	 * @brief 对于check模式，check的颜色触发的信号
	 * @param c
	 * @param on
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
 * @brief 这是一个只显示颜色的toolbutton
 *
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
 *
 */
class SA_COLOR_WIDGETS_API SAColorToolButton : public QToolButton
{
	Q_OBJECT
	SA_COLOR_WIDGETS_DECLARE_PRIVATE(SAColorToolButton)
public:
	/**
	 * @brief 颜色按钮的内置样式
	 * @param parent
	 * @return
	 */
	enum ColorToolButtonStyle
	{
		WithColorMenu,  ///< 默认会构建一个SAColorMenu
		NoColorMenu     ///< 没有ColorMenu
	};

public:
	explicit SAColorToolButton(QWidget* parent = nullptr);
	explicit SAColorToolButton(ColorToolButtonStyle style, QWidget* parent = nullptr);
	~SAColorToolButton();
	// 获取颜色
	QColor color() const;
	// 设置Margins
	void setMargins(const QMargins& mg);
	QMargins margins() const;
	// 绘制无颜色
	static void paintNoneColor(QPainter* p, const QRect& colorRect);
	// 设置颜色按钮的样式
	void setColorToolButtonStyle(ColorToolButtonStyle s);
	ColorToolButtonStyle colorToolButtonStyle() const;
	// 获取colorMenu，注意，这个函数很有可能会返回nullptr，如果ColorToolButtonStyle设置为NoColorMenu或者自己设置了菜单，此函数返回nullptr
	SAColorMenu* colorMenu() const;
	// 建立标准的颜色菜单
	SAColorMenu* createColorMenu();
public Q_SLOTS:
	// 设置颜色,会发射colorChanged信号
	void setColor(const QColor& c);

protected:
	// 获取关键的三个rect位置
	virtual void calcRect(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
	virtual void paintButton(QStylePainter* p, const QStyleOptionToolButton& opt);
	virtual void paintIcon(QStylePainter* p, const QRect& iconRect, const QStyleOptionToolButton& opt);
	virtual void paintText(QStylePainter* p, const QRect& textRect, const QStyleOptionToolButton& opt);
	virtual void paintColor(QStylePainter* p, const QRect& colorRect, const QColor& color, const QStyleOptionToolButton& opt);

protected:
	virtual void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
	virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
	virtual QSize sizeHint() const Q_DECL_OVERRIDE;
private Q_SLOTS:
	void onButtonClicked(bool checked = false);
Q_SIGNALS:
	/**
	 * @brief 颜色被点击的响应
	 * @param color
	 */
	void colorClicked(const QColor& color, bool checked = false);
	/**
	 * @brief 颜色改变信号
	 * @param color
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
 * @brief 窗口的最大最小化按钮
 * @note 内部有个SARibbonButtonGroupWidget，其ObjectName = SASystemButtonGroup
 * 如果需要qss，可以进行特化处理:
 *
 * @code
 * SARibbonButtonGroupWidget#SASystemButtonGroup
 * @endcode
 *
 * SARibbonSystemButtonBar的eventfilter捕获mainwindow的事件,通过eventerfilter来处理mainwindow的事件，避免用户错误的继承resizeEvent导致systembar的位置异常
 *
 * 因此，主窗口需要安装事件过滤器
 *
 * @code
 * MainWindow::MainWindow(){
 *    installEventFilter(mWindowButtonGroup);
 * }
 * @endcode
 */
class SA_RIBBON_EXPORT SARibbonSystemButtonBar : public QFrame
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonSystemButtonBar)
public:
	explicit SARibbonSystemButtonBar(QWidget* parent);
	explicit SARibbonSystemButtonBar(QWidget* parent, Qt::WindowFlags flags);
	~SARibbonSystemButtonBar();
	void setupMinimizeButton(bool on);
	void setupMaximizeButton(bool on);
	void setupCloseButton(bool on);
	// 以最顶层的widget的WindowFlag作为WindowFlag
	void updateWindowFlag();
	void updateWindowFlag(Qt::WindowFlags flags);
	// 设置按钮的宽度比例,最终按钮宽度将按照此比例进行设置
	void setButtonWidthStretch(int close = 4, int max = 3, int min = 3);
	// 标题栏高度
	void setWindowTitleHeight(int h);
	int windowTitleHeight() const;
	// 设置标准系统按钮的宽度
	void setWindowButtonWidth(int w);
	int windowButtonWidth() const;
	// 设置Qt::WindowStates
	void setWindowStates(Qt::WindowStates s);
	// 仅获取按钮的状态
	Qt::WindowFlags windowButtonFlags() const;
	// 三个标准系统窗口按钮
	QAbstractButton* minimizeButton() const;
	QAbstractButton* maximizeButton() const;
	QAbstractButton* closeButton() const;

	// 图标尺寸
	void setIconSize(const QSize& ic);
	QSize iconSize() const;
	// 生成并添加一个action
	void addAction(QAction* a);
	void addMenuAction(QAction* menuAction, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
	QAction* addMenuAction(QMenu* menu, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
	QAction* addSeparator();
	QAction* addWidget(QWidget* w);

	virtual QSize sizeHint() const Q_DECL_OVERRIDE;
	// 捕获mainwindow的事件
	bool eventFilter(QObject* obj, QEvent* event) override;

protected:
	virtual void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;

protected Q_SLOTS:
	Q_SLOT void closeWindow();
	Q_SLOT void minimizeWindow();
	Q_SLOT void maximizeWindow();
};

/**
 * @brief The SARibbonSystemToolButton class/系统最大最小化按钮
 */
class SARibbonSystemToolButton : public QToolButton
{
	Q_OBJECT
public:
	SARibbonSystemToolButton(QWidget* p = nullptr);
};

#endif  // SARIBBONSYSTEMBUTTONBAR_H

/*** End of inlined file: SARibbonSystemButtonBar.h ***/


/*** Start of inlined file: SARibbonToolButton.h ***/
#ifndef SARIBBONTOOLBUTTON_H
#define SARIBBONTOOLBUTTON_H

#include <QToolButton>
#include <QDebug>
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
		qreal twoLineHeightFactor { 2.05 };

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

	/// Get the current button type (LargeButton or SmallButton)
	RibbonButtonType buttonType() const;
	/// Set the button type to LargeButton or SmallButton
	void setButtonType(const RibbonButtonType& buttonType);

	/// Check if the button is a small ribbon button
	bool isSmallRibbonButton() const;
	/// Check if the button is a large ribbon button
	bool isLargeRibbonButton() const;

	/// Get the current spacing value
	int spacing() const;
	/// Set the spacing between elements and the border
	void setSpacing(int v);

	/// Force an update of the internal layout rectangles
	void updateRect();

	/// Set the layout factor for fine-tuning the button's appearance
	void setLayoutFactor(const LayoutFactor& fac);
	/// Get the layout factor for fine-tuning the button's appearance (const version)
	const LayoutFactor& layoutFactor() const;
	/// Get the layout factor for fine-tuning the button's appearance (non-const version)
	LayoutFactor& layoutFactor();

	// Enables or disables automatic text wrapping for large buttons / 为大按钮启用或禁用自动文字换行
	void setEnableWordWrap(bool on);
	// Checks if automatic text wrapping is enabled / 检查是否启用了自动文字换行
	bool isEnableWordWrap();

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
 * @brief Refer to the color setting button in the office, which can display the color below the icon(参考office的颜色设置按钮，可以显示颜色在图标下方)
 */
class SA_RIBBON_EXPORT SARibbonColorToolButton : public SARibbonToolButton
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonColorToolButton)
public:
	/**
	 * @brief 颜色样式
	 */
	enum ColorStyle
	{
		ColorUnderIcon,  ///< 颜色在icon下方，这个要求必须设置icon
		ColorFillToIcon  ///< 颜色作为icon，这个模式下在setColor会自动生成一个颜色icon替换掉原来的icon，因此setIcon函数没有作用
	};

public:
	explicit SARibbonColorToolButton(QWidget* parent = nullptr);
	explicit SARibbonColorToolButton(QAction* defaultAction, QWidget* parent = nullptr);
	~SARibbonColorToolButton();
	// 获取颜色
	QColor color() const;
	// 设置颜色显示方案
	void setColorStyle(ColorStyle s);
	ColorStyle colorStyle() const;
	// 建立标准的颜色菜单
	SAColorMenu* setupStandardColorMenu();
public Q_SLOTS:
	// 设置颜色,会发射colorChanged信号
	void setColor(const QColor& c);
private Q_SLOTS:
	void onButtonClicked(bool checked = false);
Q_SIGNALS:
	/**
	 * @brief 颜色被点击的响应
	 * @param color
	 */
	void colorClicked(const QColor& color, bool checked = false);
	/**
	 * @brief 颜色改变信号
	 * @param color
	 */
	void colorChanged(const QColor& color);

protected:
	void paintIcon(QPainter& p, const QStyleOptionToolButton& opt, const QRect& iconDrawRect);
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
 * @brief 一个窗口容器，把窗口放置中间，前面后面都可以设置文本，主要用于放置在panel上的小窗口
 *
 * 实现如下效果：
 *
 * PrefixLabel|_Widget_|SuffixLabel
 *
 */
class SA_RIBBON_EXPORT SARibbonLineWidgetContainer : public QWidget
{
public:
	explicit SARibbonLineWidgetContainer(QWidget* par = nullptr);
	~SARibbonLineWidgetContainer();
	// 设置widget,不允许设置一个nullptr
	void setWidget(QWidget* innerWidget);

	// 设置前缀
	void setPrefix(const QString& str);

	// 设置后缀
	void setSuffix(const QString& str);

	// 前缀文本框
	QLabel* labelPrefix() const;

	// 后缀文本框
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
 * @brief 用于管理一组Action,类似于QToolBar
 */
class SA_RIBBON_EXPORT SARibbonButtonGroupWidget : public QToolBar
{
	Q_OBJECT
public:
	explicit SARibbonButtonGroupWidget(QWidget* parent = nullptr);
	~SARibbonButtonGroupWidget();

	// 在快速访问工具栏中添加一个带菜单的 QAction
	void addMenuAction(QAction* menuAction, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
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
 * @class SARibbonStackedWidget
 * @brief 具有弹出式菜单功能的堆叠窗口部件
 *
 * 此类扩展了 QStackedWidget，添加了两种工作模式：
 * 1. 正常模式：作为常规的堆叠部件，嵌入在父窗口布局中
 * 2. 弹出模式：作为独立的弹出窗口，可用于实现类似菜单的显示效果
 *
 * 在弹出模式下，窗口将显示为无边框面板，并支持模态事件循环（exec()方法），
 * 这使得它可以像菜单或对话框一样使用。
 *
 * @note 切换模式时会自动处理窗口状态和事件循环
 * @sa QStackedWidget
 */
class SA_RIBBON_EXPORT SARibbonStackedWidget : public QStackedWidget
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonStackedWidget)
public:
	Q_PROPERTY(int animationWidgetHeight READ animationWidgetHeight WRITE setAnimationWidgetHeight)  // 添加自定义属性
public:
	explicit SARibbonStackedWidget(QWidget* parent);
	~SARibbonStackedWidget();
	// 弹出模式
	void setPopupMode();
	bool isPopupMode() const;

	// 正常模式 和普通的stackwidget一样
	void setNormalMode();
	bool isNormalMode() const;

	// 在弹出模式下以模态方式运行事件循环
	void exec();

	// 类似tabbar的moveTab函数，交换两个窗口的index
	void moveWidget(int from, int to);

	// 是否启用弹出动画
	void setUseAnimation(bool on);
	bool isUseAnimation() const;

	// 动画持续时间（毫秒）
	void setAnimationDuration(int duration);
	int animationDuration() const;

	// 窗口高度
	int animationWidgetHeight() const;
	void setAnimationWidgetHeight(int h);

	// 设置窗口normalGeometry，由于此窗口会有动画，防止动画过程中设置尺寸又被动画覆盖，因此此窗口的尺寸设置使用setNormalSize
	void setNormalGeometry(const QRect& normalGeometry);
	QRect normalGeometry() const;

	// 对内部窗口发送布局请求
	void layoutRequestInnerWidgets();
Q_SIGNALS:
	/**
	 * @brief 隐藏窗口信号
	 */
	void hidWindow();

protected:
	void showEvent(QShowEvent* e) override;
	void hideEvent(QHideEvent* e) override;
	virtual void resizeEvent(QResizeEvent* e) override;
	// 同步内部窗口的尺寸
	void updateInnerWidgetGeometry();
private Q_SLOTS:
	// 动画完成槽函数
	void onAnimationFinished();
	// 建立动画
	void setupAnimation();
};

#endif  // SARIBBONSTACKEDWIDGET_H

/*** End of inlined file: SARibbonStackedWidget.h ***/


/*** Start of inlined file: SARibbonSeparatorWidget.h ***/
#ifndef SARIBBONSEPARATORWIDGET_H
#define SARIBBONSEPARATORWIDGET_H

#include <QFrame>
#include <QStyleOption>

///
/// \brief 用于显示分割线
///
class SA_RIBBON_EXPORT SARibbonSeparatorWidget : public QFrame
{
	Q_OBJECT
public:
	explicit SARibbonSeparatorWidget(QWidget* parent = nullptr);
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
 * @brief 用于装载一个窗体的容器，这个窗体会布满整个SARibbonCtrlContainer，但会预留空间显示icon或者title
 *
 * ----------------------
 * |icon|text|  widget  |
 * ----------------------
 */
class SA_RIBBON_EXPORT SARibbonCtrlContainer : public QWidget
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonCtrlContainer)
public:
	explicit SARibbonCtrlContainer(QWidget* parent = nullptr);
	~SARibbonCtrlContainer();

	void setEnableShowIcon(bool b);
	void setEnableShowTitle(bool b);
	// 判断是否存在容器窗口
	bool hasContainerWidget() const;
	// 图标
	void setIcon(const QIcon& i);
	void setIcon(const QPixmap& pixmap);
	QIcon icon() const;
	// 图标
	void setText(const QString& t);
	QString text() const;
	// 设置窗口
	void setContainerWidget(QWidget* w);
	QWidget* containerWidget();
	const QWidget* containerWidget() const;
	// 获取线索icon的窗口
	QWidget* iconWidget() const;
};

#endif  // SARIBBONCTROLCONTAINER_H

/*** End of inlined file: SARibbonCtrlContainer.h ***/


/*** Start of inlined file: SARibbonQuickAccessBar.h ***/
#ifndef SARIBBONQUICKACCESSBAR_H
#define SARIBBONQUICKACCESSBAR_H

/**
 * @brief Ribbon quick access toolbar in the top left corner/ribbon左上顶部的快速响应栏
 *
 * The SARibbonQuickAccessBar class provides a specialized toolbar that is typically placedin the top-left corner of the
 * ribbon interface. It contains frequently used commands that users can access quickly without navigating through ribbon tabs.
 *
 * This toolbar supports adding regular actions as well as menu actions with different popup modes. The menu actions can
 * be configured with various popup behaviors such as InstantPopup, MenuButtonPopup, or DelayedPopup.
 *
 * 此类提供了一个专门的工具栏，通常放置在ribbon界面的左上角。它包含常用的命令用户可以快速访问这些命令，而无需浏览ribbon选项卡。
 *
 * 此工具栏支持添加常规动作以及具有不同弹出模式的菜单动作。菜单动作可以配置各种弹出行为，如InstantPopup、MenuButtonPopup或DelayedPopup。
 *
 * @par Features:/特点:
 * - Quick access to frequently used commands/快速访问常用命令
 * - Support for menu actions with customizable popup modes/支持具有可自定义弹出模式的菜单动作
 * - Easy integration with ribbon interface/与ribbon界面轻松集成
 * - Automatic management of action lifecycles/自动管理动作生命周期
 *
 * @par Usage:/用法:
 * @code
 * // Create quick access bar/创建快速访问栏
 * SARibbonQuickAccessBar *quickAccessBar = ribbonBar()->quickAccessBar();
 *
 * // Add regular actions/添加常规动作
 * quickAccessBar->addAction(saveAction);
 * quickAccessBar->addAction(undoAction);
 *
 * // Add menu actions/添加菜单动作
 * QMenu *fileMenu = new QMenu("File");
 * fileMenu->addAction(newAction);
 * fileMenu->addAction(openAction);
 * quickAccessBar->addMenuAction(fileMenu, QToolButton::InstantPopup);
 * @endcode
 *
 * @note This class inherits from QToolBar, so all standard QToolBar features are available/此类继承自QToolBar，因此所有标准QToolBar功能都可用
 * @note Menu actions are automatically configured with appropriate tool button popup modes/菜单动作会自动配置适当的工具按钮弹出模式
 *
 * @see SARibbonBar
 */
class SA_RIBBON_EXPORT SARibbonQuickAccessBar : public SARibbonButtonGroupWidget
{
	Q_OBJECT
public:
	explicit SARibbonQuickAccessBar(QWidget* parent = nullptr);
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
 * @brief Panel右下角的操作按钮
 *
 * 此按钮和一个action关联，使用@ref SARibbonPanel::addOptionAction 函数用于生成此按钮，正常来说
 * 用户并不需要直接操作此类，仅仅用于样式设计
 * 如果一定要重载此按钮，可以通过重载@ref SARibbonElementFactory
 * 的 @ref SARibbonElementFactory::createRibbonPanelOptionButton来实现新的OptionButton
 *
 */
class SA_RIBBON_EXPORT SARibbonPanelOptionButton : public QToolButton
{
	Q_OBJECT
public:
	explicit SARibbonPanelOptionButton(QWidget* parent = nullptr);
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
 * @brief 是对panel所有子窗口的抽象，参考qt的toolbar
 *
 * 参考qt的toolbar，panel所有子窗口内容都通过QAction进行抽象，包括gallery这些窗口，也是通过QAction进行抽象
 * QAction最终会转换为SARibbonPanelItem，每个SARibbonPanelItem都含有一个widget，SARibbonPanel的布局
 * 就基于SARibbonPanelItem
 *
 * 无窗口的action会在内部生成一个SARibbonToolButton，
 */
class SA_RIBBON_EXPORT SARibbonPanelItem : public QWidgetItem
{
public:
	/**
	 * @brief 定义了行的占比，ribbon中有large，media和small三种占比
	 */
	enum RowProportion
	{
		None,  ///< 为定义占比，这时候将会依据expandingDirections来判断，如果能有Qt::Vertical，就等同于Large，否则就是Small
		Large,   ///< 大占比，一个widget的高度会充满整个panel
		Medium,  ///< 中占比，在@ref SARibbonPanel::panelLayoutMode 为 @ref SARibbonPanel::ThreeRowMode 时才会起作用，且要同一列里两个都是Medium时，会在三行中占据两行
		Small  ///< 小占比，占SARibbonPanel的一行，Medium在不满足条件时也会变为Small，但不会变为Large
	};
	explicit SARibbonPanelItem(QWidget* widget);
	~SARibbonPanelItem();

	bool isEmpty() const Q_DECL_OVERRIDE;

	short rowIndex;             ///< 记录当前item属于第几行，hide模式下为-1
	int columnIndex;            ///< 记录当前item属于第几列，hide模式下为-1
	QRect itemWillSetGeometry;  ///< 在调用SARibbonPanelLayout::updateGeomArray会更新这个此处，实际设置的时候会QWidgetItem::setGeometry设置Geometry
	QAction* action;            /// < 记录action，参考QToolBarLayoutItem
	bool customWidget;  ///< 对于没有窗口的action，实际也会有一个SARibbonToolButton，在销毁时要delete掉
	SARibbonPanelItem::RowProportion rowProportion;  ///< 行的占比，ribbon中有large，media和small三种占比,见@ref RowProportion
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
 * @brief Layout manager for SARibbonPanel / SARibbonPanel的布局管理器
 *
 * `SARibbonPanelLayout` is a custom `QLayout` subclass responsible for arranging `SARibbonToolButton`
 * and other widgets within a `SARibbonPanel`. It handles the complex Ribbon-specific layout logic,
 * including Large, Medium, and Small button proportions, and the optional title and option button.
 *
 * `SARibbonPanelLayout` 是一个自定义的 `QLayout` 子类，负责在 `SARibbonPanel` 内排列 `SARibbonToolButton`
 * 和其他窗口部件。它处理复杂的Ribbon特定布局逻辑，包括大、中、小按钮比例，以及可选的标题和选项按钮。
 */
class SA_RIBBON_EXPORT SARibbonPanelLayout : public QLayout
{
	Q_OBJECT
	friend class SARibbonPanel;

public:
	explicit SARibbonPanelLayout(QWidget* p = nullptr);
	~SARibbonPanelLayout();
	// Gets the SARibbonPanel that owns this layout / 获取拥有此布局的SARibbonPanel
	SARibbonPanel* ribbonPanel() const;

	// Inserts an action at a specific index / 在指定索引处插入一个action
	void insertAction(int index, QAction* act, SARibbonPanelItem::RowProportion rp = SARibbonPanelItem::None);

	// Sets the option action for the panel / 为面板设置选项action
	void setOptionAction(QAction* action);

	// Checks if an option action is set / 检查是否设置了选项action
	bool isHaveOptionAction() const;

	// Retrieves the SARibbonPanelItem associated with an action / 获取与action关联的SARibbonPanelItem
	SARibbonPanelItem* panelItem(QAction* action) const;

	// Gets the last item added to the layout / 获取最后添加到布局的项目
	SARibbonPanelItem* lastItem() const;

	// Gets the widget associated with the last item / 获取与最后一个项目关联的窗口部件
	QWidget* lastWidget() const;

	// Moves an item from one index to another / 将一个项目从一个索引移动到另一个索引
	void move(int from, int to);
	// 判断是否需要重新布局
	bool isDirty() const;
	// 更新尺寸
	void updateGeomArray();

	// Finds the index of an action in the layout / 在布局中查找一个action的索引
	int indexByAction(QAction* action) const;

	// Gets the height of the panel's title / 获取面板标题的高度
	int panelTitleHeight() const;

	// Sets the height of the panel's title / 设置面板标题的高度
	void setPanelTitleHeight(int newTitleHeight);

	// Checks if the panel's title is enabled for display / 检查面板标题是否启用显示
	bool isEnableShowPanelTitle() const;

	// Sets whether the panel's title is enabled for display / 设置面板标题是否启用显示
	void setEnableShowPanelTitle(bool on);

	// Gets the height of large buttons / 获取大按钮的高度
	int largeButtonHeight() const;

	// Gets the spacing between the title and the buttons / 获取标题与按钮之间的间距
	int panelTitleSpace() const;

	// Sets the spacing between the title and the buttons / 设置标题与按钮之间的间距
	void setPanelTitleSpace(int newTitleSpace);

	// Gets the panel's title label / 获取面板的标题标签
	SARibbonPanelLabel* panelTitleLabel() const;

	// Sets the default icon size for tool buttons / 设置工具按钮的默认图标尺寸
	void setToolButtonIconSize(const QSize& smallSize, const QSize& largeSize);
	// Gets the default icon size for tool buttons / 获取工具按钮的默认图标尺寸
	QPair< QSize, QSize > toolButtonIconSize() const;

	// 大按钮图标尺寸
	void setLargeIconSize(const QSize& largeSize);
	QSize largeIconSize() const;

	// 小按钮图标尺寸
	void setSmallIconSize(const QSize& largeSize);
	QSize smallIconSize() const;

	// 是否允许文字换行
	bool isEnableWordWrap() const;
	// 按钮的最大宽高比，这个系数决定按钮的最大宽度
	qreal buttonMaximumAspectRatio() const;

public:
	// Adds an item to the layout (SARibbonPanelLayout not supported) / 向布局添加一个项目（SARibbonPanelLayout不支持）
	void addItem(QLayoutItem* item) Q_DECL_OVERRIDE;

	// QLayout 所必须的重载函数
	QLayoutItem* itemAt(int index) const Q_DECL_OVERRIDE;

	// Removes and returns the item at the specified index / 移除并返回指定索引处的项目
	QLayoutItem* takeAt(int index) Q_DECL_OVERRIDE;

	// Gets the number of items in the layout / 获取布局中项目的数量
	int count() const Q_DECL_OVERRIDE;

	// Checks if the layout is empty / 检查布局是否为空
	bool isEmpty() const Q_DECL_OVERRIDE;

	// Invalidates the layout, marking it as dirty / 使布局失效，将其标记为“脏”
	void invalidate() Q_DECL_OVERRIDE;

	// Returns the directions in which the layout can expand / 返回布局可以扩展的方向
	Qt::Orientations expandingDirections() const Q_DECL_OVERRIDE;
	void setGeometry(const QRect& rect) Q_DECL_OVERRIDE;
	QSize minimumSize() const Q_DECL_OVERRIDE;
	QSize sizeHint() const Q_DECL_OVERRIDE;

protected:
	// Gets the size of the option action button / 获取选项action按钮的尺寸
	QSize optionActionButtonSize() const;
	// 布局action
	void doLayout();
	// 把action转换为item，对于纯Action，此函数会创建SARibbonToolButton,
	//  rp用于告诉Layout生成什么样的窗口，详细见SARibbonPanelItem::RowProportion
	SARibbonPanelItem* createItem(QAction* action, SARibbonPanelItem::RowProportion rp = SARibbonPanelItem::None);
	void updateGeomArray(const QRect& setrect);
	// 重新计算扩展条码，此函数必须在updateGeomArray函数之后调用
	void recalcExpandGeomArray(const QRect& setrect);
	// 设置文字允许换行
	void setEnableWordWrap(bool on);
	// 按钮的最大宽高比，这个系数决定按钮的最大宽度
	void setButtonMaximumAspectRatio(qreal fac = 1.4);

private:
	// 根据列数，计算窗口的宽度，以及最大宽度
	void columnWidthInfo(int colindex, int& width, int& maximum) const;

	//  Sets the panel's title label / 设置面板的标题标签
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
 * @brief SARibbonPanel的标题label，此类用于qss
 */
class SA_RIBBON_EXPORT SARibbonPanelLabel : public QLabel
{
	Q_OBJECT
public:
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
		TwoRowMode
	};
	Q_ENUM(PanelLayoutMode)
public:
	using FpRibbonToolButtonIterate = std::function< bool(SARibbonToolButton*) >;

public:
	explicit SARibbonPanel(QWidget* parent = nullptr);
	explicit SARibbonPanel(const QString& name, QWidget* parent = nullptr);
	~SARibbonPanel() Q_DECL_OVERRIDE;
	using QWidget::addAction;

	// 把action加入到panel
	void addAction(QAction* action, SARibbonPanelItem::RowProportion rowProportion);
	// 生成并添加一个action
	void addAction(QAction* act,
				   QToolButton::ToolButtonPopupMode popMode,
				   SARibbonPanelItem::RowProportion rowProportion = SARibbonPanelItem::Large);
	// 把action加入到panel，并以大图标显示
	void addLargeAction(QAction* action);
	// 把action加入到panel，在三行模式下会以中图标显示
	void addMediumAction(QAction* action);
	// 把action加入到panel，并以小图标显示
	void addSmallAction(QAction* action);

	// 把action加入到panel，并以小图标显示
	void addSmallAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
	// 把action加入到panel，并以大图标显示
	void addLargeAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
	// 把action加入到panel，在三行模式下会以中图标显示
	void addMediumAction(QAction* action, QToolButton::ToolButtonPopupMode popMode);
	// 生成并添加一个action
	QAction* addAction(const QString& text,
					   const QIcon& icon,
					   QToolButton::ToolButtonPopupMode popMode,
					   SARibbonPanelItem::RowProportion rowProportion = SARibbonPanelItem::Large);

	// 添加menu
	void addMenu(QMenu* menu,
				 SARibbonPanelItem::RowProportion rowProportion,
				 QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
	// 添加普通大菜单
	void addLargeMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
	// 添加普通小按钮菜单
	void addMediumMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
	// 添加普通小按钮菜单
	void addSmallMenu(QMenu* menu, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

	// 添加窗口
	QAction* addWidget(QWidget* w, SARibbonPanelItem::RowProportion rowProportion);

	// 添加窗口,占用ribbon的一行
	QAction* addSmallWidget(QWidget* w);

	// 添加窗口,占用ribbon的一行
	QAction* addMediumWidget(QWidget* w);

	// 添加窗口，占用所有行
	QAction* addLargeWidget(QWidget* w);

	// 添加一个Gallery
	SARibbonGallery* addGallery(bool expanding = true);

	// 添加分割线
	QAction* addSeparator();

	// 从panel中把action对应的button提取出来，如果action没有对应的button，就返回nullptr
	SARibbonToolButton* actionToRibbonToolButton(QAction* action);

	// 设置操作action，如果要去除，传入nullptr指针即可，SARibbonPanel不会对QAction的所有权进行管理
	// OptionAction也会触发actionTriggered信号
	void setOptionAction(QAction* action);

	// 判断是否存在OptionAction
	bool isHaveOptionAction() const;

	// 获取所有的buttons
	QList< SARibbonToolButton* > ribbonToolButtons() const;

	// 获取PanelLayoutMode
	PanelLayoutMode panelLayoutMode() const;
	void setPanelLayoutMode(PanelLayoutMode mode);

	// 更新按钮的尺寸，这个函数在panel的布局状态变换后需要调用刷新
	void resetToolButtonSize();

	// 判断是否为2行模式
	bool isTwoRow() const;

	// 把panel设置为扩展模式，此时会撑大水平区域
	void setExpanding(bool isExpanding = true);
	// 是否是扩展模式
	bool isExpanding() const;

	// 标题栏高度
	int titleHeight() const;

	// 是否显示标题，显示标题后，标题的高度需要设置，默认高度为15
	bool isEnableShowTitle() const;

	// action对应的布局index，此操作一般用于移动moveAction，其他意义不大
	int actionIndex(QAction* act) const;

	// 移动action
	void moveAction(int from, int to);

	// 判断是否可以自定义
	bool isCanCustomize() const;
	void setCanCustomize(bool b);

	// 标题
	QString panelName() const;
	void setPanelName(const QString& title);

	// 大图标的高度
	int largeButtonHeight() const;

	// 获取布局对应的item,此函数目前仅仅在自定义过程中用到
	const QList< SARibbonPanelItem* >& ribbonPanelItem() const;

	// 获取panel layout
	SARibbonPanelLayout* panelLayout() const;

	// 更新布局
	void updateItemGeometry();

	// 获取category指针，如果没有parent，或者不在category管理，返回nullptr
	SARibbonCategory* category() const;

	// 获取ribbonBar指针，如果没有返回nullptr
	SARibbonBar* ribbonBar() const;

	// 按钮之间的间隔
	int spacing() const;

	// 设置按钮的icon尺寸
	void setToolButtonIconSize(const QSize& smallSize, const QSize& largeSize);
	QPair< QSize, QSize > toolButtonIconSize() const;

	// 大按钮图标尺寸
	void setLargeIconSize(const QSize& largeSize);
	QSize largeIconSize() const;

	// 小按钮图标尺寸
	void setSmallIconSize(const QSize& smallSize);
	QSize smallIconSize() const;

	// 获取最近添加的按钮
	SARibbonToolButton* lastAddActionButton();

	// 获取panel标题的label控件
	SARibbonPanelLabel* titleLabel() const;

	// 是否允许换行
	bool isEnableWordWrap() const;

	// 按钮的最大宽高比，这个系数决定按钮的最大宽度
	qreal buttonMaximumAspectRatio() const;

	// 此函数会遍历SARibbonPanel下的所有RibbonToolButton，执行函数指针(bool(SARibbonRibbonToolButton*))，函数指针返回false则停止迭代
	bool iterateButton(FpRibbonToolButtonIterate fp) const;

public:
	virtual QSize sizeHint() const Q_DECL_OVERRIDE;
	virtual QSize minimumSizeHint() const Q_DECL_OVERRIDE;

public:
	// panel高度推荐
	static int panelHeightHint(const QFontMetrics& fm, PanelLayoutMode layMode, int panelTitleHeight);
	// 把action的行属性设置进action中，action自身携带了行属性
	static void setActionRowProportionProperty(QAction* action, SARibbonPanelItem::RowProportion rp);
	// 获取action的行属性
	static SARibbonPanelItem::RowProportion getActionRowProportionProperty(QAction* action);
	// 把action的PopupMode属性设置进action中，action自身携带了PopupMode属性
	static void setActionToolButtonPopupModeProperty(QAction* action, QToolButton::ToolButtonPopupMode popMode);
	// 获取action的PopupMode属性
	static QToolButton::ToolButtonPopupMode getActionToolButtonPopupModeProperty(QAction* action);
	// 把action的ToolButtonStyle属性设置进action中，action自身携带了ToolButtonStyle属性
	static void setActionToolButtonStyleProperty(QAction* action, Qt::ToolButtonStyle buttonStyle);
	// 获取action的ToolButtonStyle属性
	static Qt::ToolButtonStyle getActionToolButtonStyleProperty(QAction* action);

Q_SIGNALS:

	/**
	 * @brief 等同于QToolBar::actionTriggered
	 * @param action
	 */
	void actionTriggered(QAction* action);

	/**
	 * @brief Emitted when the panel's name changes / 当面板名称改变时发出
	 * @param name The new name / 新的名称
	 */
	void panelNameChanged(const QString& n);

protected:
	virtual void actionEvent(QActionEvent* e) Q_DECL_OVERRIDE;
	virtual void changeEvent(QEvent* e) Q_DECL_OVERRIDE;

protected:
	// 以下这些方法是为了接收SARibbonBar的对应接口的参数，用户在操作SARibbonPanel的时候不需要进行设置
	// SARibbonPanel在被设置到SARibbonCategory的时候，SARibbonCategory会重新把自身的这些参数同步到panel中
	// 同理，SARibbonCategory加入SARibbonBar的时候，SARibbonBar会重新把自身的这些参数同步到category

	// 设置标题栏高度
	void setTitleHeight(int h);

	// 设置按钮之间的间隔
	void setSpacing(int n);

	// 是否显示标题，显示标题后，标题的高度需要设置，默认高度为15
	void setEnableShowTitle(bool on);

	// 设置按钮允许换行，注意图标大小是由文字决定的，两行文字会让图标变小，如果想图标变大，文字不换行是最好的
	void setEnableWordWrap(bool on);

	// 按钮的最大宽高比，这个系数决定按钮的最大宽度
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
 * @brief The SARibbonCategoryLayout class
 */
class SA_RIBBON_EXPORT SARibbonCategoryLayout : public QLayout
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonCategoryLayout)
public:
	Q_PROPERTY(int scrollPosition READ scrollPosition WRITE setScrollPosition)
public:
	explicit SARibbonCategoryLayout(SARibbonCategory* parent);
	~SARibbonCategoryLayout();

	SARibbonCategory* ribbonCategory() const;

	virtual void addItem(QLayoutItem* item) override;
	virtual QLayoutItem* itemAt(int index) const override;
	virtual QLayoutItem* takeAt(int index) override;
	SARibbonCategoryLayoutItem* takePanelItem(int index);
	SARibbonCategoryLayoutItem* takePanelItem(SARibbonPanel* panel);
	bool takePanel(SARibbonPanel* panel);
	virtual int count() const override;
	void setGeometry(const QRect& rect) override;
	QSize sizeHint() const override;
	QSize minimumSize() const override;
	Qt::Orientations expandingDirections() const override;
	void invalidate() override;
	// 追加一个panel
	void addPanel(SARibbonPanel* panel);
	// 插入一个panel
	void insertPanel(int index, SARibbonPanel* panel);
	//
	QSize categoryContentSize() const;
	// 更新尺寸
	void updateGeometryArr();

	// 执行位置调整
	void doLayout();

	// 返回所有panels
	QList< SARibbonPanel* > panels() const;

	// 通过obj name获取panel
	SARibbonPanel* panelByObjectName(const QString& objname) const;
	// 通过panel name获取panel
	SARibbonPanel* panelByName(const QString& panelname) const;
	// 通过索引找到panel，如果超过索引范围，会返回nullptr
	SARibbonPanel* panelByIndex(int i) const;
	// 移动panel
	void movePanel(int from, int to);
	// panel的数量
	int panelCount() const;
	// panel的索引
	int panelIndex(SARibbonPanel* p) const;
	// 获取所有的panel
	QList< SARibbonPanel* > panelList() const;
	// 执行滚轮事件
	void scroll(int px);
	void scrollTo(int targetX);
	// 带动画的滚动
	void scrollByAnimate(int px);
	void scrollToByAnimate(int targetX);
	// 滚动后的位置
	int scrollPosition() const;
	// 设置滚动位置
	void setScrollPosition(int pos);
	// 是否在滚动动画中
	bool isAnimatingScroll() const;
	// 判断是否有滚动过
	bool isScrolled() const;
	// category的总宽度
	int categoryTotalWidth() const;
	// 设置Category的对齐方式
	void setCategoryAlignment(SARibbonAlignment al);
	SARibbonAlignment categoryAlignment() const;
	// 设置动画持续时间
	void setAnimationDuration(int duration);
	int animationDuration() const;
private Q_SLOTS:
	void onLeftScrollButtonClicked();
	void onRightScrollButtonClicked();

private:
	void setupAnimateScroll();
};

/**
 * @brief SARibbonCategoryLayoutItem，用于标识SARibbonCategoryLayout的item
 */
class SA_RIBBON_EXPORT SARibbonCategoryLayoutItem : public QWidgetItem
{
public:
	explicit SARibbonCategoryLayoutItem(SARibbonPanel* w);
	~SARibbonCategoryLayoutItem();
	SARibbonSeparatorWidget* separatorWidget;
	// 把内部的widget转换为panel
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
 * @brief 管理上下文标签的类
 */
class SA_RIBBON_EXPORT SARibbonContextCategory : public QObject
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonContextCategory)
public:
	explicit SARibbonContextCategory(QWidget* parent = nullptr);
	~SARibbonContextCategory();
	// 上下文目录添加下属目录
	SARibbonCategory* addCategoryPage(const QString& title);
	void addCategoryPage(SARibbonCategory* category);
	// 获取上下文标签下管理的标签个数
	int categoryCount() const;

	// 设置id
	void setId(const QVariant& id);
	QVariant id() const;

	// 设置上下文颜色
	void setContextColor(const QColor color);
	QColor contextColor() const;

	// 上下文标签的内容
	QString contextTitle() const;
	void setContextTitle(const QString& contextTitle);

	// 获取对应的tab页
	SARibbonCategory* categoryPage(int index);

	// 获取所有的SARibbonCategory*
	QList< SARibbonCategory* > categoryList() const;

	// 移除category
	bool takeCategory(SARibbonCategory* category);

	// 判断上下文是否维护了此SARibbonCategory
	bool isHaveCategory(SARibbonCategory* category) const;
public Q_SLOTS:
	// hide contextCategory/隐藏上下文标签
	void hide();

	// show contextCategory/显示上下文标签
	void show();
Q_SIGNALS:
	/**
	 * @brief 标签加入上下文
	 * @param category
	 */
	void categoryPageAdded(SARibbonCategory* category);

	/**
	 * @brief 标签从上下文移除
	 * @param category
	 */
	void categoryPageRemoved(SARibbonCategory* category);

	/**
	 * @brief 上下文的标题发生改变
	 * @param title
	 */
	void contextTitleChanged(const QString& title);

	/**
	 * @brief 上下文标签维护的标签页名字发生了改变
	 * @param category 发生改变的上下文标签页
	 * @param title 新名字
	 */
	void categoryTitleChanged(SARibbonCategory* category, const QString& title);
private Q_SLOTS:
	void onCategoryTitleChanged(const QString& title);

protected:
	// 获取父级窗口
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

///
/// \brief 类似QStandardItem的GalleryItem
///
class SA_RIBBON_EXPORT SARibbonGalleryItem
{
	friend class SARibbonGalleryGroupModel;

public:
	explicit SARibbonGalleryItem();
	explicit SARibbonGalleryItem(const QString& text, const QIcon& icon);
	explicit SARibbonGalleryItem(QAction* act);
	virtual ~SARibbonGalleryItem();
	// 设置角色
	void setData(int role, const QVariant& data);
	virtual QVariant data(int role) const;

	// 设置文字描述
	void setText(const QString& text);
	QString text() const;

	// 设置tooltip
	void setToolTip(const QString& text);
	QString toolTip() const;

	// 设置图标
	void setIcon(const QIcon& ico);
	QIcon icon() const;

	// 设置是否可见
	bool isSelectable() const;
	void setSelectable(bool isSelectable);

	// 设置是否可选
	bool isEnable() const;
	void setEnable(bool isEnable);

	// 设置item的flag
	void setFlags(Qt::ItemFlags flag);
	virtual Qt::ItemFlags flags() const;

	// 设置action
	void setAction(QAction* act);
	QAction* action();

	// 文字对齐方式
	void setTextAlignment(Qt::Alignment a);
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

///
/// \brief SARibbonGalleryGroup对应的显示代理
///
class SA_RIBBON_EXPORT SARibbonGalleryGroupItemDelegate : public QStyledItemDelegate
{
public:
	explicit SARibbonGalleryGroupItemDelegate(SARibbonGalleryGroup* group, QObject* parent = nullptr);
	~SARibbonGalleryGroupItemDelegate();
	virtual void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override;

	virtual QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	virtual void paintIconOnly(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	virtual void paintIconWithText(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;
	virtual void paintIconWithTextWordWrap(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const;

private:
	SARibbonGalleryGroup* mGroup;
};

///
/// \brief SARibbonGalleryGroup对应的model
///
class SA_RIBBON_EXPORT SARibbonGalleryGroupModel : public QAbstractListModel
{
	Q_OBJECT
public:
	SARibbonGalleryGroupModel(QObject* parent = Q_NULLPTR);
	~SARibbonGalleryGroupModel();
	virtual int rowCount(const QModelIndex& parent) const Q_DECL_OVERRIDE;
	virtual Qt::ItemFlags flags(const QModelIndex& index) const Q_DECL_OVERRIDE;
	virtual QVariant data(const QModelIndex& index, int role) const Q_DECL_OVERRIDE;
	virtual QModelIndex index(int row, int column, const QModelIndex& parent) const Q_DECL_OVERRIDE;
	virtual bool setData(const QModelIndex& index, const QVariant& value, int role) Q_DECL_OVERRIDE;
	void clear();
	SARibbonGalleryItem* at(int row) const;
	void insert(int row, SARibbonGalleryItem* item);
	SARibbonGalleryItem* take(int row);
	void append(SARibbonGalleryItem* item);
	bool remove(const QString& act_object_name);
	// 条目数量
	int itemSize() const;

private:
	QList< SARibbonGalleryItem* > mItems;
};

/**
 * @brief Gallery的组
 *
 * 组负责显示管理Gallery Item
 */
class SA_RIBBON_EXPORT SARibbonGalleryGroup : public QListView
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonGalleryGroup)
public:
	/**
	 * @brief GalleryGroup显示的样式
	 */
	enum GalleryGroupStyle
	{
		IconWithText,          ///< 图标带文字
		IconWithWordWrapText,  ///< 图标带文字,文字会换行显示，此模式只会对DisplayOneRow生效，如果不是DisplayOneRow，等同IconWithText
		IconOnly               ///< 只有图标
	};

	/**
	 * @brief 定义Gallery在一个panel下面显示的图标行数
	 */
	enum DisplayRow
	{
		DisplayOneRow   = 1,  ///< 显示1行，默认
		DisplayTwoRow   = 2,
		DisplayThreeRow = 3
	};

	SARibbonGalleryGroup(QWidget* w = 0);

	virtual ~SARibbonGalleryGroup();
	// 重新计算grid尺寸
	void recalcGridSize();
	void recalcGridSize(int galleryHeight);
	// 设置显示的样式
	void setGalleryGroupStyle(GalleryGroupStyle style);
	GalleryGroupStyle galleryGroupStyle() const;
	// 添加一个item
	void addItem(const QString& text, const QIcon& icon);
	void addItem(SARibbonGalleryItem* item);
	// 以一个aciton作为item添加
	void addActionItem(QAction* act);
	void addActionItemList(const QList< QAction* >& acts);
	bool removeActionItem(QAction* act);
	SARibbonGalleryGroupModel* groupModel() const;
	// 标题
	void setGroupTitle(const QString& title);
	QString groupTitle() const;
	void selectByIndex(int i);
	// 设置显示的行数
	void setDisplayRow(DisplayRow r);
	DisplayRow displayRow() const;
	// 设置grid最小的宽度，默认为0（不限制），可以限定grid的宽度
	void setGridMinimumWidth(int w);
	int gridMinimumWidth() const;
	// 设置grid最大的宽度，默认为0（不限制），可以限定grid的宽度
	void setGridMaximumWidth(int w);
	int gridMaximumWidth() const;
	// 获取SARibbonGalleryGroup管理的actiongroup
	QActionGroup* actionGroup() const;
	// 行数
	int gridRowCount() const;
	// 列数量
	int gridColumnCount() const;
	// 根据宽度计算推荐高度
	int preferredHeightForWidth(int w) const;
	// 流式listview，需要让它变为高度由宽度决定
	bool hasHeightForWidth() const override;
	//
	int heightForWidth(int w) const override;
private Q_SLOTS:
	void onItemClicked(const QModelIndex& index);
	void onItemEntered(const QModelIndex& index);
Q_SIGNALS:
	void groupTitleChanged(const QString& title);
	/**
	 * @brief 等同QActionGroup的triggered
	 * 所有加入SARibbonGalleryGroup的action都会被一个QActionGroup管理,可以通过@sa getActionGroup 获取到对应的actiongroup
	 * @param action
	 */
	void triggered(QAction* action);
	/**
	 * @brief 等同QActionGroup的triggered
	 * 所有加入SARibbonGalleryGroup的action都会被一个QActionGroup管理,可以通过@sa getActionGroup 获取到对应的actiongroup
	 * @note 此属性需要通过QAbstractItemView::entered(const QModelIndex &index)激活，因此要保证设置了setMouseTracking(true)
	 * @param action
	 */
	void hovered(QAction* action);

private:
	// 构建一个model，这个model的父类是SARibbonGalleryGroup，如果要共享model，需要手动处理model的父类
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
	Q_PROPERTY(bool tabOnTitle READ isTabOnTitle WRITE setTabOnTitle)
	Q_PROPERTY(SARibbonPanel::PanelLayoutMode panelLayoutMode READ panelLayoutMode WRITE setPanelLayoutMode)

public:
	enum RibbonStyleFlag
	{
		RibbonStyleLoose    = 0x0001,  // bit:0000 0001
		RibbonStyleCompact  = 0x0002,  // bit:0000 0010
		RibbonStyleThreeRow = 0x0010,  // bit:0001 0000
		RibbonStyleTwoRow   = 0x0020,  // bit:0010 0000

		RibbonStyleLooseThreeRow   = RibbonStyleLoose | RibbonStyleThreeRow,    ///< 宽松结构，3行模式
		RibbonStyleCompactThreeRow = RibbonStyleCompact | RibbonStyleThreeRow,  ///< 紧凑结构，3行模式
		RibbonStyleLooseTwoRow     = RibbonStyleLoose | RibbonStyleTwoRow,      ///< 宽松结构，2行模式
		RibbonStyleCompactTwoRow   = RibbonStyleCompact | RibbonStyleTwoRow     ///< 紧凑结构，2行模式
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

class SA_RIBBON_EXPORT SARibbonBarLayout : public QLayout
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonBarLayout)
public:
	explicit SARibbonBarLayout(SARibbonBar* parent);
	virtual ~SARibbonBarLayout();

	// QLayout 必须实现的接口
	void addItem(QLayoutItem* item) override;
	QLayoutItem* itemAt(int index) const override;
	QLayoutItem* takeAt(int index) override;
	int count() const override;
	QSize sizeHint() const override;
	QSize minimumSize() const override;
	void setGeometry(const QRect& rect) override;

	// 执行布局
	virtual void doLayout();

	// 样式相关
	bool isLooseStyle() const;
	bool isCompactStyle() const;

	// 标题区域
	const QRect& titleRect() const;

	// 计算最小TabBar宽度
	int calcMinTabBarWidth() const;

	// 重置尺寸，这个是在ribbon模式发生改变的时候调用，例如setMinimumMode或者setNormalMode,此函数会触发重新布局
	void resetSize();

	// 更新标题区域
	void layoutTitleRect();

	// 调整StackedContainerWidget的位置
	void layoutStackedContainerWidget();

	// 让category重新布局，这个函数在调整category的对其方式的时候调用，由于对其方式改变StackedContainerWidget的尺寸没有改变，但category要重新布局
	void layoutCategory();

	// 设置系统按钮大小
	void setSystemButtonSize(const QSize& size);

	// 设置tab在title上面，这样可以省略title区域
	void setTabOnTitle(bool on);
	bool isTabOnTitle() const;

	// 最小模式下的MainBar高度
	int minimumModeMainBarHeight() const;

	// 获取正常模式下的mainBar的高度
	int normalModeMainBarHeight() const;

	// ribbon tab的高度
	int tabBarHeight() const;
	void setTabBarHeight(int h);

	// 标题栏的高度
	int titleBarHeight() const;
	void setTitleBarHeight(int h);

	// category的高度
	int categoryHeight() const;
	void setCategoryHeight(int h);

	// 设置panel的标题栏高度
	int panelTitleHeight() const;
	void setPanelTitleHeight(int h);

	// 设置图标
	void setWindowIcon(const QIcon& icon);
	QIcon windowIcon() const;

	// 设置ApplicationButton垂直方向扩充，这样ApplicationButton能占用标题栏和tab栏两个栏的高度
	void setApplicationButtonVerticalExpansion(bool on = true);
	bool isApplicationButtonVerticalExpansion() const;

	// 获取元素
	SARibbonBar* ribbonBar() const;
	SARibbonTabBar* ribbonTabBar() const;
	SARibbonStackedWidget* stackedContainerWidget() const;
	SARibbonQuickAccessBar* quickAccessBar() const;
	SARibbonButtonGroupWidget* rightButtonGroup() const;
	QAbstractButton* applicationButton() const;
	SARibbonTitleIconWidget* titleIconWidget() const;

protected:
	void resizeInLooseStyle();
	void resizeInCompactStyle();

private:
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
///
/// \brief SARibbon的子元素创建的工厂，SARibbon内部创建子元素都通过SARibbonElementFactory来创建
///
/// 由于SARibbonBar是一个复合控件，很多子窗口组合而成，有些部件的创建如果想继承，那么就需要这个工厂类来处理
/// 如SARibbonCategory，可以重载此类的createRibbonCategory,返回重载的类的实例
///
class SA_RIBBON_EXPORT SARibbonElementFactory
{
public:
	SARibbonElementFactory();
	virtual ~SARibbonElementFactory();
	virtual SARibbonBar* createRibbonBar(QWidget* parent);
	virtual SARibbonTabBar* createRibbonTabBar(QWidget* parent);
	virtual SARibbonApplicationButton* createRibbonApplicationButton(QWidget* parent);
	virtual SARibbonCategory* createRibbonCategory(QWidget* parent);
	virtual SARibbonContextCategory* createRibbonContextCategory(QWidget* parent);
	virtual SARibbonPanel* createRibbonPanel(QWidget* parent);
	virtual SARibbonSeparatorWidget* createRibbonSeparatorWidget(QWidget* parent);
	virtual SARibbonGallery* createRibbonGallery(QWidget* parent);
	virtual SARibbonGalleryGroup* createRibbonGalleryGroup(QWidget* parent);
	virtual SARibbonToolButton* createRibbonToolButton(QWidget* parent);
	virtual SARibbonStackedWidget* createRibbonStackedWidget(SARibbonBar* parent);
	virtual SARibbonButtonGroupWidget* createButtonGroupWidget(QWidget* parent);
	virtual SARibbonQuickAccessBar* createQuickAccessBar(QWidget* parent);
	virtual SARibbonSystemButtonBar* createWindowButtonGroup(QWidget* parent);
	virtual SARibbonPanelOptionButton* createRibbonPanelOptionButton(SARibbonPanel* panel);
	virtual SARibbonTitleIconWidget* createRibbonTitleIconWidget(QWidget* parent);
	virtual SARibbonPanelLabel* createRibbonPanelLabel(QWidget* parent);
};

#endif  // SARIBBONELEMENTFACTORY_H

/*** End of inlined file: SARibbonElementFactory.h ***/


/*** Start of inlined file: SARibbonElementManager.h ***/
#ifndef SARIBBONELEMENTMANAGER_H
#define SARIBBONELEMENTMANAGER_H

/**
   @brief 此类是一个全局单例，用于管理SARibbonElementFactory

	如果你有自己的某个部件要重写，首先你需要有自己的ElementFactory
	@code
	class MyRibbonElementFactory : public SARibbonElementFactory{
	public:
		...
		virtual SARibbonPanel* createRibbonPanel(QWidget* parent){
			return new MyRibbonPanel(parent);
		}
	};
	@endcode

	然后，你需要在ribbonbar创建之前把自己的ElementFactory设置进去

	这个一般会在main函数中进行

	@code
	SARibbonElementManager::instance()->setupFactory(new MyRibbonElementFactory);
	@endcode

	这样，SARibbon创建的panel就是你自己重写的MyRibbonPanel

 */
class SA_RIBBON_EXPORT SARibbonElementManager
{
protected:
	SARibbonElementManager();

public:
	virtual ~SARibbonElementManager();
	static SARibbonElementManager* instance();
	SARibbonElementFactory* factory();
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
 * @brief 记录所有自定义操作的数据类
 * @note 此数据依赖于@ref SARibbonActionsManager 要在SARibbonActionsManager之后使用此类
 */
class SA_RIBBON_EXPORT SARibbonCustomizeData
{
public:
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
	SARibbonCustomizeData();
	SARibbonCustomizeData(ActionType type, SARibbonActionsManager* mgr = nullptr);
	// 获取CustomizeData的action type
	ActionType actionType() const;

	// 设置CustomizeData的action type
	void setActionType(ActionType a);

	// 判断是否是一个正常的CustomizeData
	bool isValid() const;

	// 应用SARibbonCustomizeData
	bool apply(SARibbonBar* bar) const;

	// 获取actionmanager指针
	SARibbonActionsManager* actionManager();

	// 设置ActionsManager
	void setActionsManager(SARibbonActionsManager* mgr);

	// 对应AddCategoryActionType
	static SARibbonCustomizeData makeAddCategoryCustomizeData(const QString& title, int index, const QString& objName);

	// 对应AddPanelActionType
	static SARibbonCustomizeData
	makeAddPanelCustomizeData(const QString& title, int index, const QString& categoryobjName, const QString& objName);

	// 对应AddActionActionType
	static SARibbonCustomizeData makeAddActionCustomizeData(const QString& key,
															SARibbonActionsManager* mgr,
															SARibbonPanelItem::RowProportion rp,
															const QString& categoryObjName,
															const QString& panelObjName);

	// 对应RenameCategoryActionType
	static SARibbonCustomizeData makeRenameCategoryCustomizeData(const QString& newname, const QString& categoryobjName);

	// 对应RenamePanelActionType
	static SARibbonCustomizeData
	makeRenamePanelCustomizeData(const QString& newname, const QString& categoryobjName, const QString& panelObjName);

	// 对应RemoveCategoryActionType
	static SARibbonCustomizeData makeRemoveCategoryCustomizeData(const QString& categoryobjName);

	// 对应ChangeCategoryOrderActionType
	static SARibbonCustomizeData makeChangeCategoryOrderCustomizeData(const QString& categoryobjName, int moveindex);

	// 对应ChangePanelOrderActionType
	static SARibbonCustomizeData
	makeChangePanelOrderCustomizeData(const QString& categoryobjName, const QString& panelObjName, int moveindex);

	// 对应ChangeActionOrderActionType
	static SARibbonCustomizeData makeChangeActionOrderCustomizeData(const QString& categoryobjName,
																	const QString& panelObjName,
																	const QString& key,
																	SARibbonActionsManager* mgr,
																	int moveindex);

	// 对应RemovePanelActionType
	static SARibbonCustomizeData makeRemovePanelCustomizeData(const QString& categoryobjName, const QString& panelObjName);

	// 对应RemoveActionActionType
	static SARibbonCustomizeData makeRemoveActionCustomizeData(const QString& categoryobjName,
															   const QString& panelObjName,
															   const QString& key,
															   SARibbonActionsManager* mgr);

	// 对应VisibleCategoryActionType
	static SARibbonCustomizeData makeVisibleCategoryCustomizeData(const QString& categoryobjName, bool isShow);

	// 判断是否可以自定义,如果某个action不想被编辑，可以通过此函数设置
	static bool isCanCustomize(QObject* obj);
	static void setCanCustomize(QObject* obj, bool canbe = true);

	// 对QList<SARibbonCustomizeData>进行简化
	static QList< SARibbonCustomizeData > simplify(const QList< SARibbonCustomizeData >& csd);

public:
	/**
	 * @brief 记录顺序的参数
	 *
	 * 在actionType==AddCategoryActionType时，此参数记录Category的insert位置,
	 * 在actionType==AddPanelActionType时，此参数记录panel的insert位置,
	 * 在actionType==AddActionActionType时，此参数记录panel的insert位置
	 */
	int indexValue;

	/**
	 * @brief 记录标题、索引等参数
	 *
	 * 在actionType==AddCategoryActionType时，key为category标题，
	 * 在actionType==AddPanelActionType时，key为panel标题，
	 * 在actionType==AddActionActionType时，key为action的查询依据，基于SARibbonActionsManager::action查询
	 */
	QString keyValue;

	/**
	 * @brief 记录categoryObjName，用于定位Category
	 */
	QString categoryObjNameValue;

	/**
	 * @brief 记录panelObjName，saribbon的Customize索引大部分基于objname
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
 * @brief 自定义界面窗口
 *
 * @note SARibbon的自定义是基于步骤的，如果在窗口生成前调用了@ref sa_apply_customize_from_xml_file 类似函数
 * 那么在对话框生成前为了保证同步需要调用@ref SARibbonCustomizeWidget::fromXml 同步配置文件，这样再次修改后的配置文件就一致
 */
class SA_RIBBON_EXPORT SARibbonCustomizeWidget : public QWidget
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonCustomizeWidget)
public:
	// 保留接口
	explicit SARibbonCustomizeWidget(SARibbonMainWindow* ribbonWindow,
									 QWidget* parent   = nullptr,
									 Qt::WindowFlags f = Qt::WindowFlags());
	// 对于不使用SARibbonMainWindow的情况，使用此构造函数
	explicit SARibbonCustomizeWidget(SARibbonBar* ribbonbar,
									 QWidget* parent   = nullptr,
									 Qt::WindowFlags f = Qt::WindowFlags());
	~SARibbonCustomizeWidget();

	/**
	 * @brief 定义ribbon树的显示类型
	 */
	enum RibbonTreeShowType
	{
		ShowAllCategory,  ///< 显示所有Category，包括contextcategory
		ShowMainCategory  ///< 显示主要的category，不包含上下文
	};

	/**
	 * @brief QStandardItem对应的role
	 */
	enum ItemRole
	{
		LevelRole        = Qt::UserRole + 1,  ///< 代表这是层级，有0：category 1：panel 2：item
		PointerRole      = Qt::UserRole + 2,  ///< 代表这是存放指针。根据LevelRole来进行转
		CanCustomizeRole = Qt::UserRole + 3,  ///< 代表个item是可以自定义的.bool
		CustomizeRole = Qt::UserRole + 4,  ///< 代表这个是自定义的item,bool,主要用于那些自己添加的标签和panel，有此角色必有CanCustomizeRole
		CustomizeObjNameRole = Qt::UserRole + 5  ///< 记录了临时的自定义内容的obj名 QString
	};

	// 设置action管理器
	void setupActionsManager(SARibbonActionsManager* mgr);

	// 判断用户是否有要存储的内容，对应save动作
	bool isApplied() const;

	// 判断用户是否有改动内容，对应apply动作
	bool isCached() const;

	// 获取model
	const QStandardItemModel* model() const;

	// 根据当前的radiobutton选项来更新model
	void updateModel();

	// 更新model
	void updateModel(RibbonTreeShowType type);

	// 应用所有的设定
	bool applys();

	// 转换为xml
	bool toXml(QXmlStreamWriter* xml) const;
	bool toXml(const QString& xmlpath) const;

	// 从xml中加载QList<SARibbonCustomizeData>，对于基于配置文件的设置，对话框显示前建议调用此函数，保证叠加设置的正确记录
	void fromXml(QXmlStreamReader* xml);
	void fromXml(const QString& xmlpath);

	// 应用xml配置，可以结合customize_datas_from_xml和customize_datas_apply函数
	static bool fromXml(QXmlStreamReader* xml, SARibbonBar* bar, SARibbonActionsManager* mgr);

	// 缓存应用的动作,这些动作不会被clear清除，用于本地存储
	void makeActionsApplied();

	// 清除applied的动作，cancel操作后需要清空已应用的动作
	void clearApplied();
	// 清除缓存动作，在执行applys函数后，如果要继续调用，应该clear，否则会导致异常
	void clearCache();
	// 清除所有动作，不包含本地读取的数据
	void clear();

protected:
	// 把QList<SARibbonCustomizeData>进行裁剪,把一些动作合并
	void simplify();

	SARibbonPanelItem::RowProportion selectedRowProportion() const;

	QAction* selectedAction() const;
	QAction* itemToAction(QStandardItem* item) const;

	QStandardItem* selectedItem() const;

	// 获取选中的ribbon tree 的level
	int selectedRibbonLevel() const;

	// 根据选中的item判断
	int itemLevel(QStandardItem* item) const;

	// 设置某个item被选中
	void setSelectItem(QStandardItem* item, bool ensureVisible = true);

	// 判断itemn能否改动，可以改动返回true
	bool isItemCanCustomize(QStandardItem* item) const;
	bool isSelectedItemCanCustomize() const;

	// 判断item是否是自定义的item
	bool isCustomizeItem(QStandardItem* item) const;
	bool isSelectedItemIsCustomize() const;

	// 删除一个item
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
 * @brief 转换为xml
 *
 * 此函数仅会写element，不会写document相关内容，因此如果需要写document，
 * 需要在此函数前调用QXmlStreamWriter::writeStartDocument(),在此函数后调用QXmlStreamWriter::writeEndDocument()
 * @param xml QXmlStreamWriter指针
 * @note 注意，在传入QXmlStreamWriter之前，需要设置编码为utf-8:xml->setCodec("utf-8");
 * @note 由于QXmlStreamWriter在QString作为io时，是不支持编码的，而此又无法保证自定义过程不出现中文字符，
 * 因此，QXmlStreamWriter不应该通过QString进行构造，如果需要用到string，也需要通过QByteArray构造，如：
 * @param cds 基于QList<SARibbonCustomizeData>生成的步骤
 * @return 如果出现异常，返回false,如果没有自定义数据也会返回false
 */
bool SA_RIBBON_EXPORT sa_customize_datas_to_xml(QXmlStreamWriter* xml, const QList< SARibbonCustomizeData >& cds);

/**
 * @brief 通过xml获取QList<SARibbonCustomizeData>
 * @param xml
 * @return QList<SARibbonCustomizeData>
 */
QList< SARibbonCustomizeData > SA_RIBBON_EXPORT sa_customize_datas_from_xml(QXmlStreamReader* xml,
																			SARibbonActionsManager* mgr);

/**
 * @brief 应用QList<SARibbonCustomizeData>
 * @param cds
 * @param w SARibbonBar指针
 * @return 成功应用的个数
 */
int SA_RIBBON_EXPORT sa_customize_datas_apply(const QList< SARibbonCustomizeData >& cds, SARibbonBar* w);

/**
 * @brief 反向取消应用
 * @param cds
 * @param w SARibbonBar指针
 * @return 成功应用的个数
 */
int SA_RIBBON_EXPORT sa_customize_datas_reverse(const QList< SARibbonCustomizeData >& cds, SARibbonBar* w);

/**
 * @brief 直接加载xml自定义ribbon配置文件用于ribbon的自定义显示
 * @param filePath xml配置文件
 * @param w 主窗体
 * @param mgr action管理器
 * @return 成功返回true
 * @note 重复加载一个配置文件会发生异常，为了避免此类事件发生，一般通过一个变量保证只加载一次，如：
 * @code
 * static bool has_call = false;
 * if (!has_call) {
 *     has_call = sa_apply_customize_from_xml_file("customize.xml", this, m_actMgr);
 * }
 * @endcode
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
 * @brief SARibbonCustomizeWidget的对话框封装
 *
 * 此功能依赖于@ref SARibbonActionsManager ，SARibbonActionsManager建议作为mianwindow的成员变量，
 * SARibbonActionsManager可以快速绑定所有QAction，详细见SARibbonActionsManager的说明
 *
 * @note SARibbon的自定义是基于步骤的，如果在窗口生成前调用了@ref sa_apply_customize_from_xml_file 类似函数
 * 那么在对话框生成前为了保证同步需要调用@ref SARibbonCustomizeDialog::fromXml 同步配置文件，这样再次修改后的配置文件就一致
 */
class SA_RIBBON_EXPORT SARibbonCustomizeDialog : public QDialog
{
	Q_OBJECT
public:
	explicit SARibbonCustomizeDialog(SARibbonMainWindow *ribbonWindow, QWidget *p = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
	~SARibbonCustomizeDialog();
	//设置action管理器
	void setupActionsManager(SARibbonActionsManager *mgr);

	//判断用户是否有要存储的内容，对应save动作
	bool isApplied() const;

	//判断用户是否有改动内容，对应apply动作
	bool isCached() const;

	//应用所有的设定
	bool applys();

	//清除所有动作
	void clear();

	//转换为xml
	bool toXml(QXmlStreamWriter *xml) const;
	bool toXml(const QString& xmlpath) const;

	//从xml中加载QList<SARibbonCustomizeData>，对于基于配置文件的设置，对话框显示前建议调用此函数，保证叠加设置的正确记录
	void fromXml(QXmlStreamReader *xml);
	void fromXml(const QString& xmlpath);

	//返回SARibbonCustomizeWidget窗口指针
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
 * @brief 如果要使用SARibbonBar，必须使用此类代替QMainWindow
 *
 * 由于ribbon的风格和传统的Toolbar风格差异较大，
 * SARibbonBar使用需要把原有的QMainWindow替换为SARibbonMainWindow,
 * SARibbonMainWindow是个无边框窗体，继承自QMainWindow（目前使用第三方的无边框方案https://github.com/wangwenx190/framelesshelper），
 * 其构造函数的参数useRibbon用于指定是否使用ribbon风格，默认为true
 *
 * @code
 * SARibbonMainWindow(QWidget* parent = nullptr,bool useRibbon = true);
 * @endcode
 *
 * 如果想换回非ribbon风格，只需要把useRibbon设置为false即可,
 * 成员函数isUseRibbon用于判断当前是否为ribbon模式，这个函数在兼容传统Toolbar风格和ribbon风格时非常有用
 *
 * 但这个不支持动态切换，因此，实际工程中，你若要进行切换，需要写配置文件，程序在构造时传入模式，
 * 并且，你的程序要做好判断，因为非Ribbon模式下，所有Ribbon相关的接口都会返回空指针
 *
 * @code
 * bool isUseRibbon() const;
 * @endcode
 *
 * @ref SARibbonMainWindow 提供了几种常用的ribbon样式
 * 通过@ref setRibbonTheme 可改变ribbon的样式，用户也可通过qss自己定义自己的样式
 *
 * 如果你自己继承了SARibbonBar，你可以通过@ref setRibbonBar 设置自己的ribbonbar进去
 *
 * 另外有个一个更加高效的方法，来实现自定义的Ribbon，就是继承一个@ref SARibbonElementFactory
 *
 * @code
 * class MyRibbonFactory:public SARibbonElementFactory{
 * ...
 * virtual SARibbonBar* createRibbonBar(QWidget* parent){
 *     return new MyRibbonBar(parent);
 * }
 * };
 * @endcode
 *
 * SARibbonMainWindow生成之前(一般在main函数），设置元件工厂：
 *
 * @code
 * SARibbonElementManager::instance()->setupFactory(new MyRibbonFactory());
 * @endcode
 *
 * 此时，Ribbon的元素会通过MyRibbonFactory的接口来生成
 */
class SA_RIBBON_EXPORT SARibbonMainWindow : public QMainWindow
{
	Q_OBJECT
	SA_RIBBON_DECLARE_PRIVATE(SARibbonMainWindow)
	friend class SARibbonBar;
	Q_PROPERTY(SARibbonTheme ribbonTheme READ ribbonTheme WRITE setRibbonTheme)

public:
	explicit SARibbonMainWindow(QWidget* parent                = nullptr,
								SARibbonMainWindowStyles style = SARibbonMainWindowStyleFlag::UseRibbonMenuBar
																 | SARibbonMainWindowStyleFlag::UseRibbonFrame,
								const Qt::WindowFlags flags = Qt::WindowFlags());
	~SARibbonMainWindow() override;
	// 返回SARibbonBar
	SARibbonBar* ribbonBar() const;
	// 设置ribbonbar
	void setRibbonBar(SARibbonBar* ribbon);
#if !SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
	// 返回SAFramelessHelper
	SAFramelessHelper* framelessHelper() const;
	// 设置缩放时，使用橡皮筋示意而不是立即缩放，这对于大渲染量的软件（如CAD、三维）比较友好
	void setRubberBandOnResize(bool on);
	bool isRubberBandOnResize() const;
#else
	// 如果ribbon中有自定义的窗口在标题栏等非点击区域加入后，想能点击，需要调用此接口告知可点击
	void setFramelessHitTestVisible(QWidget* w, bool visible = true);
#endif
	// 此函数仅用于控制最小最大化和关闭按钮的显示
	void updateWindowFlag(Qt::WindowFlags flags);
	// 注意主题在构造函数设置主题会不完全生效，使用QTimer投放到队列最后执行即可
	// QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); });
	void setRibbonTheme(SARibbonTheme theme);
	SARibbonTheme ribbonTheme() const;
	// 判断当前是否使用ribbon模式
	bool isUseRibbon() const;
	// 获取最大化，最小化，关闭按钮所在的bar。可以通过此函数在最大最小化按钮旁边设置内容
	SARibbonSystemButtonBar* windowButtonBar() const;
	// 获取当前mainwidow的样式
	SARibbonMainWindowStyles ribbonMainwindowStyle() const;

	// 把ribbonbar的事件传递到frameless
	virtual bool eventFilter(QObject* obj, QEvent* e) Q_DECL_OVERRIDE;

protected:
	// 创建ribbonbar的工厂函数
	SARibbonBar* createRibbonBar();
private Q_SLOTS:
	void onPrimaryScreenChanged(QScreen* screen);
};

/**
 * @brief 针对SARibbonMainWindow的事件处理器，主要处理systembar的位置调整
 */
class SA_RIBBON_EXPORT SARibbonMainWindowEventFilter : public QObject
{
	Q_OBJECT
public:
	explicit SARibbonMainWindowEventFilter(QObject* par);
	~SARibbonMainWindowEventFilter();
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
 * @brief 这个窗口是用于applicationButton点击后弹出类似office系列软件的文件窗口，此窗口会把整个MainWindow覆盖，
 * 同时会接管边框的一些消息
 */
class SA_RIBBON_EXPORT SARibbonApplicationWidget : public QFrame
{
	Q_OBJECT
public:
	SARibbonApplicationWidget(SARibbonMainWindow* parent);

protected:
	virtual void resizeToParent(const QSize& parentSize);

protected:
	bool eventFilter(QObject* obj, QEvent* ev) override;
	void showEvent(QShowEvent* event) override;
	void keyPressEvent(QKeyEvent* ev) override;
};

#endif  // SARIBBONAPPLICATIONWIDGET_H

/*** End of inlined file: SARibbonApplicationWidget.h ***/

/*** End of inlined file: SARibbonAmalgamTemplatePublicHeaders.h ***/

#endif
