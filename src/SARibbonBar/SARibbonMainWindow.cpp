#include "SARibbonMainWindow.h"
#include "SARibbonUtil.h"
#include "SARibbonBar.h"
#include "SARibbonElementManager.h"
#include "SARibbonTabBar.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QHash>
#include <QWindowStateChangeEvent>
#include <QScreen>

#include "SARibbonSystemButtonBar.h"
#include "SARibbonWidget.h"
#include "SARibbonTitleIconWidget.h"
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
#include <QWKWidgets/widgetwindowagent.h>
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonStackedWidget.h"
#else
#include "SAFramelessHelper.h"
#endif

/**
 * @brief The SARibbonMainWindowPrivate class
 */
class SARibbonMainWindow::PrivateData
{
	SA_RIBBON_DECLARE_PUBLIC(SARibbonMainWindow)
public:
	PrivateData(SARibbonMainWindow* p);
	void installFrameless(SARibbonMainWindow* p);
	bool isUseRibbonBar() const;
	bool isUseRibbonFrame() const;
	bool isUseNativeFrame() const;
	void checkMainWindowFlag();
	static void updateTabBarMargins(SARibbonTabBar* tab, SARibbonTheme theme);
	static void updateContextColors(SARibbonBar* bar, SARibbonTheme theme);
	static void updateTabBarBaseLineColor(SARibbonBar* bar, SARibbonTheme theme);

public:
	SARibbonMainWindowStyles mRibbonMainWindowStyle;
	SARibbonTheme mCurrentRibbonTheme { SARibbonTheme::RibbonThemeOffice2021Blue };
	SARibbonSystemButtonBar* mWindowButtonGroup { nullptr };
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
	QWK::WidgetWindowAgent* mFramelessHelper { nullptr };
#else
	SAFramelessHelper* mFramelessHelper { nullptr };
#endif
	SARibbonMainWindowEventFilter* mEventFilter { nullptr };
};

SARibbonMainWindow::PrivateData::PrivateData(SARibbonMainWindow* p) : q_ptr(p)
{
}

void SARibbonMainWindow::PrivateData::installFrameless(SARibbonMainWindow* p)
{
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
	mFramelessHelper = new QWK::WidgetWindowAgent(p);
	mFramelessHelper->setup(p);
#else
	mFramelessHelper = new SAFramelessHelper(p);
#endif
}

bool SARibbonMainWindow::PrivateData::isUseRibbonBar() const
{
	return mRibbonMainWindowStyle.testFlag(SARibbonMainWindowStyleFlag::UseRibbonMenuBar);
}

bool SARibbonMainWindow::PrivateData::isUseRibbonFrame() const
{
	return mRibbonMainWindowStyle.testFlag(SARibbonMainWindowStyleFlag::UseRibbonFrame);
}

bool SARibbonMainWindow::PrivateData::isUseNativeFrame() const
{
    return mRibbonMainWindowStyle.testFlag(SARibbonMainWindowStyleFlag::UseNativeFrame);
}

/**
 * @brief 检查flag的设置合理性
 */
void SARibbonMainWindow::PrivateData::checkMainWindowFlag()
{
	// 如果都没有设置边框样式，默认设置为ribbon边框
	if (!mRibbonMainWindowStyle.testFlag(SARibbonMainWindowStyleFlag::UseRibbonFrame)
        && !mRibbonMainWindowStyle.testFlag(SARibbonMainWindowStyleFlag::UseNativeFrame)) {
		mRibbonMainWindowStyle.setFlag(SARibbonMainWindowStyleFlag::UseRibbonFrame, true);
	}

	// 如果都没有设置MenuBar，默认设置为ribbonbar
	if (!mRibbonMainWindowStyle.testFlag(SARibbonMainWindowStyleFlag::UseRibbonMenuBar)
        && !mRibbonMainWindowStyle.testFlag(SARibbonMainWindowStyleFlag::UseNativeMenuBar)) {
		mRibbonMainWindowStyle.setFlag(SARibbonMainWindowStyleFlag::UseRibbonMenuBar, true);
	}
}

void SARibbonMainWindow::PrivateData::updateTabBarMargins(SARibbonTabBar* tab, SARibbonTheme theme)
{
	static const std::map< SARibbonTheme, QMargins > themeMargins = {
		{ SARibbonTheme::RibbonThemeWindows7, { 5, 0, 0, 0 } },
		{ SARibbonTheme::RibbonThemeOffice2013, { 5, 0, 0, 0 } },
		{ SARibbonTheme::RibbonThemeOffice2016Blue, { 5, 0, 0, 0 } },
		{ SARibbonTheme::RibbonThemeDark, { 5, 0, 0, 0 } },
		{ SARibbonTheme::RibbonThemeDark2, { 5, 0, 0, 0 } },
		{ SARibbonTheme::RibbonThemeOffice2021Blue, { 5, 0, 5, 0 } }
	};
	auto it = themeMargins.find(theme);
	if (it != themeMargins.end()) {
		tab->setTabMargin(it->second);
	}
}

void SARibbonMainWindow::PrivateData::updateContextColors(SARibbonBar* bar, SARibbonTheme theme)
{
	static const SARibbonBar::FpContextCategoryHighlight cs_darkerHighlight = [](const QColor& c) -> QColor {
		return c.darker();
	};
	static const SARibbonBar::FpContextCategoryHighlight cs_vibrantHighlight = [](const QColor& c) -> QColor {
		return SA::makeColorVibrant(c);
	};

	switch (theme) {
	case SARibbonTheme::RibbonThemeWindows7:
	case SARibbonTheme::RibbonThemeOffice2013:
	case SARibbonTheme::RibbonThemeDark:
		bar->setContextCategoryColorList({});  // 重置为默认色系
		bar->setContextCategoryColorHighLight(cs_vibrantHighlight);
		break;
	case SARibbonTheme::RibbonThemeOffice2016Blue:
		bar->setContextCategoryColorList({ QColor(18, 64, 120) });
		bar->setContextCategoryColorHighLight(cs_darkerHighlight);
		break;
	case SARibbonTheme::RibbonThemeOffice2021Blue:
		bar->setContextCategoryColorList({ QColor(209, 207, 209) });
		bar->setContextCategoryColorHighLight([](const QColor& c) -> QColor { return QColor(39, 96, 167); });
		break;
	default:
		break;
	}
}

void SARibbonMainWindow::PrivateData::updateTabBarBaseLineColor(SARibbonBar* bar, SARibbonTheme theme)
{
	if (theme == SARibbonTheme::RibbonThemeOffice2013) {
		bar->setTabBarBaseLineColor(QColor(186, 201, 219));
	} else {
		bar->setTabBarBaseLineColor(QColor());
	}
}

//===================================================
// SARibbonMainWindow
//===================================================

/**
 * @brief 构造一个 SARibbonMainWindow 实例。
 *
 * 此构造函数初始化一个带有 Ribbon 界面风格的主窗口，支持自定义窗口样式（如是否使用 Ribbon 边框、菜单栏等），
 * 并根据样式自动配置窗口行为（如无边框、RibbonBar 安装等）。
 *
 * @param parent 父窗口部件
 * @param style 窗口样式标志，控制窗口外观和行为。支持以下标志组合：
 *        - @c SARibbonMainWindowStyleFlag::UseRibbonFrame：使用Ribbon自定义边框（会启用无边框窗口）（默认启用）。
 *        - @c SARibbonMainWindowStyleFlag::UseNativeFrame：使用系统原生窗口边框。
 *        - @c SARibbonMainWindowStyleFlag::UseRibbonMenuBar：使用 Ribbon 风格菜单栏（默认启用）。
 *        - @c SARibbonMainWindowStyleFlag::UseNativeMenuBar：使用系统原生菜单栏（非ribbon）。
 *        标志可通过位或（|）组合使用，例如：@c SARibbonMainWindowStyleFlag::UseRibbonFrame | SARibbonMainWindowStyleFlag::UseRibbonMenuBar
 *
 * @param flags 标准 Qt 窗口标志
 *
 * @note 如果启用了 @c UseRibbonFrame，则窗口将自动安装无边框支持。
 *
 * @sa SARibbonMainWindowStyleFlag, setRibbonBar(), ribbonTheme()
 */
SARibbonMainWindow::SARibbonMainWindow(QWidget* parent, SARibbonMainWindowStyles style, const Qt::WindowFlags flags)
    : QMainWindow(parent, flags), d_ptr(new SARibbonMainWindow::PrivateData(this))
{
	SA_D(d);
	d->mRibbonMainWindowStyle = style;
	d->checkMainWindowFlag();
	if (d->isUseRibbonBar()) {
		if (d->isUseRibbonFrame()) {
			d->installFrameless(this);
		}
		setRibbonBar(createRibbonBar());
		setRibbonTheme(ribbonTheme());
        setContentsMargins(2, 0, 2, 0);
		if (d->isUseNativeFrame()) {
			// 在ribbon模式下使用本地边框，将隐藏icon，同时默认设置为紧凑模式
			if (SARibbonBar* bar = ribbonBar()) {
                // 隐藏icon
                bar->setTitleIconVisible(false);
				// 设置为紧凑模式
				bar->setRibbonStyle(SARibbonBar::RibbonStyleCompactThreeRow);
			}
		}
	}
	connect(qApp, &QApplication::primaryScreenChanged, this, &SARibbonMainWindow::onPrimaryScreenChanged);
}

SARibbonMainWindow::~SARibbonMainWindow()
{
}

/**
 * @brief 返回ribbonbar，如果不是使用ribbon模式，返回nullptr
 * @return
 */
SARibbonBar* SARibbonMainWindow::ribbonBar() const
{
    return qobject_cast< SARibbonBar* >(menuWidget());
}

/**
 * @brief 设置ribbonbar
 * @param bar
 */
void SARibbonMainWindow::setRibbonBar(SARibbonBar* ribbon)
{
	QWidget* old = QMainWindow::menuWidget();
	if (old) {
		// 如果之前已经设置了menubar，要把之前的删除
		old->deleteLater();
	}
	QMainWindow::setMenuWidget(ribbon);
	ribbon->setMainWindowStyles(d_ptr->mRibbonMainWindowStyle);
	const int th = ribbon->titleBarHeight();

	if (d_ptr->isUseRibbonFrame()) {
		// 设置window按钮
		if (nullptr == d_ptr->mWindowButtonGroup) {
			d_ptr->mWindowButtonGroup = RibbonSubElementFactory->createWindowButtonGroup(this);
			d_ptr->mWindowButtonGroup->setObjectName(QStringLiteral("objSARibbonSystemButtonBar"));
			d_ptr->mWindowButtonGroup->setIconSize(QSize(18, 18));
			// SARibbonSystemButtonBar的eventfilter捕获mainwindow的事件
			// 通过eventerfilter来处理mainwindow的事件，避免用户错误的继承resizeEvent导致systembar的位置异常
			installEventFilter(d_ptr->mWindowButtonGroup);
		}
		SARibbonSystemButtonBar* sysBar = d_ptr->mWindowButtonGroup;
		sysBar->setWindowStates(windowState());
		sysBar->setWindowTitleHeight(th);
		sysBar->raise();  // 确保sysbar在最顶层，避免第二次设置ribbonbar的时候，被ribbonbar覆盖了sysbar
		sysBar->show();

		// 图标
		ribbon->titleIconWidget()->setIcon(windowIcon());
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
		auto helper = d_ptr->mFramelessHelper;
		helper->setTitleBar(ribbon);
		// 以下这些窗口，需要允许点击
		helper->setHitTestVisible(sysBar);                         // IMPORTANT!
		helper->setHitTestVisible(ribbon->ribbonTabBar());         // IMPORTANT!
		helper->setHitTestVisible(ribbon->rightButtonGroup());     // IMPORTANT!
		helper->setHitTestVisible(ribbon->applicationButton());    // IMPORTANT!
		helper->setHitTestVisible(ribbon->quickAccessBar());       // IMPORTANT!
		helper->setHitTestVisible(ribbon->ribbonStackedWidget());  // IMPORTANT!
		helper->setHitTestVisible(ribbon->titleIconWidget());      // IMPORTANT!
#if SARIBBON_ENABLE_SNAP_LAYOUT
		if (sysBar->closeButton()) {
			helper->setSystemButton(QWK::WindowAgentBase::Close, sysBar->closeButton());
		}
		if (sysBar->minimizeButton()) {
			helper->setSystemButton(QWK::WindowAgentBase::Minimize, sysBar->minimizeButton());
		}
		if (sysBar->maximizeButton()) {
			helper->setSystemButton(QWK::WindowAgentBase::Maximize, sysBar->maximizeButton());
		}
#endif
#else
		// 捕获ribbonbar的事件
		ribbon->installEventFilter(this);
		// 设置窗体的标题栏高度
		d_ptr->mFramelessHelper->setTitleHeight(th);
		d_ptr->mFramelessHelper->setRubberBandOnResize(false);
#endif
		// 最后要提升，否则新加入的会被覆盖
		if (d_ptr->mWindowButtonGroup) {
			d_ptr->mWindowButtonGroup->raise();
		}
	}
	if (!d_ptr->mEventFilter) {
		d_ptr->mEventFilter = new SARibbonMainWindowEventFilter(this);
		installEventFilter(d_ptr->mEventFilter);
	}
}

#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER

/**
 * @brief 如果ribbon中有自定义的窗口在标题栏等非点击区域加入后，想能点击，需要调用此接口告知可点击
 * @param w
 * @param visible
 */
void SARibbonMainWindow::setFramelessHitTestVisible(QWidget* w, bool visible)
{
	auto helper = d_ptr->mFramelessHelper;
	helper->setHitTestVisible(const_cast< QWidget* >(w), visible);
}
#else

/**
 * @brief 无边框辅助对象
 * @return
 */
SAFramelessHelper* SARibbonMainWindow::framelessHelper() const
{
	return (d_ptr->mFramelessHelper);
}

/**
 * @brief 设置在缩放时是否启用“橡皮筋”示意模式。
 *
 * 当启用时，窗口在拖拽缩放过程中不会立即重绘内容，而是先用一个半透明矩形框（橡皮筋）
 * 显示目标尺寸，待用户释放鼠标后才一次性完成resize。此模式可显著降低
 * CAD、三维渲染、大型图表等重负载应用在高频resize时的CPU/GPU消耗。
 *
 * @param on  true  启用橡皮筋缩放示意；
 *             false 禁用橡皮筋，采用实时重绘（默认行为）。
 *
 * @see isRubberBandOnResize(), SAFramelessHelper::setRubberBandOnResize()
 */
void SARibbonMainWindow::setRubberBandOnResize(bool on)
{
    if (SAFramelessHelper* fl = framelessHelper()) {
        fl->setRubberBandOnResize(on);
    }
}

/**
 * @brief 返回当前是否启用了橡皮筋缩放示意模式。
 *
 * @return true  橡皮筋模式已启用；
 *         false 橡皮筋模式未启用
 *
 * @see setRubberBandOnResize(), SAFramelessHelper::rubberBandOnResize()
 */
bool SARibbonMainWindow::isRubberBandOnResize() const
{
    if (SAFramelessHelper* fl = framelessHelper()) {
        return fl->rubberBandOnResisze();
    }
    return false;
}
#endif

bool SARibbonMainWindow::eventFilter(QObject* obj, QEvent* e)
{
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
#else
	// 这个过滤是为了把ribbonBar上的动作传递到mainwindow，再传递到frameless，
	// 由于ribbonbar会遮挡掉frameless的区域，导致frameless无法捕获这些消息
	// 因此必须ribbonBar()->installEventFilter(this);

	if (obj == ribbonBar()) {
		switch (e->type()) {
		case QEvent::MouseButtonPress:
		case QEvent::MouseButtonRelease:
		case QEvent::MouseMove:
		case QEvent::Leave:
		case QEvent::HoverMove:
		case QEvent::MouseButtonDblClick: {
			QApplication::sendEvent(this, e);
		} break;
		default:
			break;
		}
	}
#endif
	return (QMainWindow::eventFilter(obj, e));
}

/**
 * @brief 获取系统按钮组，可以在此基础上添加其他按钮
 * @return
 */
SARibbonSystemButtonBar* SARibbonMainWindow::windowButtonBar() const
{
    return d_ptr->mWindowButtonGroup;
}

/**
 * @brief 获取当前ribbonMainWidow的样式
 * @return
 */
SARibbonMainWindowStyles SARibbonMainWindow::ribbonMainwindowStyle() const
{
    return d_ptr->mRibbonMainWindowStyle;
}

/**
 * @brief 此函数仅用于控制最小最大化和关闭按钮的显示
 */
void SARibbonMainWindow::updateWindowFlag(Qt::WindowFlags flags)
{
	if (d_ptr->isUseRibbonFrame()) {
		if (SARibbonSystemButtonBar* g = d_ptr->mWindowButtonGroup) {
			g->updateWindowFlag(flags);
		}
	}
}

/**
 * @brief SARibbonMainWindow::setRibbonTheme
 *
 * 注意某些版本的qt，在构造函数设置主题会不完全生效，可以使用QTimer投放到队列最后执行，如：
 * @code
 * QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); });
 * @endcode
 *
 * @param theme
 */
void SARibbonMainWindow::setRibbonTheme(SARibbonTheme theme)
{
    SA::setBuiltInRibbonTheme(this, theme);
	d_ptr->mCurrentRibbonTheme = theme;
	if (SARibbonBar* bar = ribbonBar()) {
		// 1. tab bar的间距
		if (SARibbonTabBar* tab = bar->ribbonTabBar()) {
			SARibbonMainWindow::PrivateData::updateTabBarMargins(tab, theme);
		}
		// 2. 上下文颜色设置
		SARibbonMainWindow::PrivateData::updateContextColors(bar, theme);
		// 3. tabbar的基线颜色
		SARibbonMainWindow::PrivateData::updateTabBarBaseLineColor(bar, theme);
	}
}

SARibbonTheme SARibbonMainWindow::ribbonTheme() const
{
	return (d_ptr->mCurrentRibbonTheme);
}

bool SARibbonMainWindow::isUseRibbon() const
{
    return (nullptr != ribbonBar());
}

/**
 * @brief 创建ribbonbar的工厂函数
 *
 * 用户如果重写了SARibbonBar，可以通过重新此虚函数返回自己的Ribbon实例
 * @return
 */
SARibbonBar* SARibbonMainWindow::createRibbonBar()
{
	SARibbonBar* bar = RibbonSubElementFactory->createRibbonBar(this);
	return bar;
}

/**
 * @brief 主屏幕切换触发的信号
 * @param screen
 */
void SARibbonMainWindow::onPrimaryScreenChanged(QScreen* screen)
{
	Q_UNUSED(screen);
	// 主屏幕切换后，从新计算所有尺寸
	if (SARibbonBar* bar = ribbonBar()) {
		qDebug() << "Primary Screen Changed";
		bar->updateRibbonGeometry();
	}
}

//----------------------------------------------------
// SARibbonMainWindowEventFilter
//----------------------------------------------------
SARibbonMainWindowEventFilter::SARibbonMainWindowEventFilter(QObject* par) : QObject(par)
{
}

SARibbonMainWindowEventFilter::~SARibbonMainWindowEventFilter()
{
}

bool SARibbonMainWindowEventFilter::eventFilter(QObject* obj, QEvent* e)
{
	if (e && obj) {
		if (e->type() == QEvent::Resize) {
			if (SARibbonMainWindow* m = qobject_cast< SARibbonMainWindow* >(obj)) {
				if (SARibbonBar* ribbon = m->ribbonBar()) {
                    QMargins mg = m->contentsMargins();
                    ribbon->setFixedWidth(m->size().width() - mg.left() - mg.right());
				}
			}
		}
	}
	return QObject::eventFilter(obj, e);
}
