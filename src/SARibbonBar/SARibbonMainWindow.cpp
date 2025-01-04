#include "SARibbonMainWindow.h"
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

public:
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

//===================================================
// SARibbonMainWindow
//===================================================
SARibbonMainWindow::SARibbonMainWindow(QWidget* parent, bool useRibbon, const Qt::WindowFlags flags)
    : QMainWindow(parent, flags), d_ptr(new SARibbonMainWindow::PrivateData(this))
{
	connect(qApp, &QApplication::primaryScreenChanged, this, &SARibbonMainWindow::onPrimaryScreenChanged);
	if (useRibbon) {
		d_ptr->installFrameless(this);
		setRibbonBar(createRibbonBar());
		setRibbonTheme(ribbonTheme());
	}
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
	const int th = ribbon->titleBarHeight();
	// 设置window按钮
	if (nullptr == d_ptr->mWindowButtonGroup) {
		d_ptr->mWindowButtonGroup = RibbonSubElementFactory->createWindowButtonGroup(this);
		// SARibbonSystemButtonBar的eventfilter捕获mainwindow的事件
		// 通过eventerfilter来处理mainwindow的事件，避免用户错误的继承resizeEvent导致systembar的位置异常
		installEventFilter(d_ptr->mWindowButtonGroup);
	}
	SARibbonSystemButtonBar* sysBar = d_ptr->mWindowButtonGroup;
	sysBar->setWindowStates(windowState());
	sysBar->setWindowTitleHeight(th);
	sysBar->raise();  // 确保sysbar在最顶层，避免第二次设置ribbonbar的时候，被ribbonbar覆盖了sysbar
	sysBar->show();
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
	if (!d_ptr->mEventFilter) {
		d_ptr->mEventFilter = new SARibbonMainWindowEventFilter(this);
		installEventFilter(d_ptr->mEventFilter);
	}
	// 最后要提升，否则新加入的会被覆盖
	if (d_ptr->mWindowButtonGroup) {
		d_ptr->mWindowButtonGroup->raise();
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

SAFramelessHelper* SARibbonMainWindow::framelessHelper()
{
	return (d_ptr->mFramelessHelper);
}

#endif

bool SARibbonMainWindow::eventFilter(QObject* obj, QEvent* e)
{
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
#else
	// 这个过滤是为了把ribbonBar上的动作传递到mainwindow，再传递到frameless，
	// 由于ribbonbar会遮挡掉frameless的区域，导致frameless无法捕获这些消息
	// 因此必须ribbonBar()->installEventFilter(this);
	// 20240101发现installEventFilter后，SARibbonMainWindow没有执行这个回调
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
 * @brief 确保系统最大最小化按钮的事件过滤器安装成功
 *
 * 如果你清除了过滤器，需要调用此函数把最大最小化按钮的过滤器安装上去
 */
void SARibbonMainWindow::ensureSystemButtonBarEventFilter()
{
}

/**
 * @brief 此函数仅用于控制最小最大化和关闭按钮的显示
 */
void SARibbonMainWindow::updateWindowFlag(Qt::WindowFlags flags)
{
	if (SARibbonSystemButtonBar* g = d_ptr->mWindowButtonGroup) {
		g->updateWindowFlag(flags);
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
	sa_set_ribbon_theme(this, theme);
	d_ptr->mCurrentRibbonTheme = theme;
	if (SARibbonBar* bar = ribbonBar()) {
		auto theme = ribbonTheme();
		// 尺寸修正
		switch (theme) {
		case SARibbonTheme::RibbonThemeWindows7:
		case SARibbonTheme::RibbonThemeOffice2013:
		case SARibbonTheme::RibbonThemeOffice2016Blue:
		case SARibbonTheme::RibbonThemeDark:
		case SARibbonTheme::RibbonThemeDark2: {
			//! 在设置qss后需要针对margin信息重新设置进SARibbonTabBar中
			//! office2013.qss的margin信息如下设置
			//! margin-top: 0px;
			//! margin-right: 0px;
			//! margin-left: 5px;
			//! margin-bottom: 0px;
			SARibbonTabBar* tab = bar->ribbonTabBar();
			if (!tab) {
				break;
			}
			tab->setTabMargin(QMargins(5, 0, 0, 0));
		} break;
		case SARibbonTheme::RibbonThemeOffice2021Blue: {
			SARibbonTabBar* tab = bar->ribbonTabBar();
			if (!tab) {
				break;
			}
			//! 在设置qss后需要针对margin信息重新设置进SARibbonTabBar中
			//! office2021.qss的margin信息如下设置
			//! margin-top: 0px;
			//! margin-right: 5px;
			//! margin-left: 5px;
			//! margin-bottom: 0px;
			tab->setTabMargin(QMargins(5, 0, 5, 0));
		}
		default:
			break;
		}
		// 上下文标签颜色设置,以及基线颜色设置
		switch (theme) {
		case SARibbonTheme::RibbonThemeWindows7:
		case SARibbonTheme::RibbonThemeOffice2013:
		case SARibbonTheme::RibbonThemeDark:
			bar->setContextCategoryColorList(QList< QColor >());  //< 设置空颜色列表会重置为默认色系
			break;
		case SARibbonTheme::RibbonThemeOffice2016Blue:
			bar->setContextCategoryColorList(QList< QColor >() << QColor(18, 64, 120));  //< 设置空颜色列表会重置为默认色系
			break;
		case SARibbonTheme::RibbonThemeOffice2021Blue:
			bar->setContextCategoryColorList(QList< QColor >() << QColor(209, 207, 209));  //< 设置空颜色列表会重置为默认色系
			break;
		default:
			break;
		}
		// 基线颜色设置
		if (SARibbonTheme::RibbonThemeOffice2013 == theme) {
			bar->setTabBarBaseLineColor(QColor(186, 201, 219));
		} else {
			bar->setTabBarBaseLineColor(QColor());
		}
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
	bar->setContentsMargins(3, 0, 3, 0);
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

void sa_set_ribbon_theme(QWidget* w, SARibbonTheme theme)
{
	QFile file;
	switch (theme) {
	case SARibbonTheme::RibbonThemeWindows7:
		file.setFileName(":/theme/resource/theme-win7.qss");
		break;
	case SARibbonTheme::RibbonThemeOffice2013:
		file.setFileName(":/theme/resource/theme-office2013.qss");
		break;
	case SARibbonTheme::RibbonThemeOffice2016Blue:
		file.setFileName(":/theme/resource/theme-office2016-blue.qss");
		break;
	case SARibbonTheme::RibbonThemeOffice2021Blue:
		file.setFileName(":/theme/resource/theme-office2021-blue.qss");
		break;
	case SARibbonTheme::RibbonThemeDark:
		file.setFileName(":/theme/resource/theme-dark.qss");
		break;
	case SARibbonTheme::RibbonThemeDark2:
		file.setFileName(":/theme/resource/theme-dark2.qss");
		break;
	default:
		file.setFileName(":/theme/resource/theme-office2013.qss");
		break;
	}
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		return;
	}
	// 有反馈用qstring接住文件内容，再设置进去才能生效（qt5.7版本）
	QString qss = QString::fromUtf8(file.readAll());
	w->setStyleSheet(qss);
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
					ribbon->setFixedWidth(m->size().width());
				}
			}
		}
	}
	return QObject::eventFilter(obj, e);
}
