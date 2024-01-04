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

#include "SAWindowButtonGroup.h"
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
#include "FramelessHelper/Widgets/standardsystembutton.h"
#include "FramelessHelper/Widgets/framelesswidgetshelper.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonStackedWidget.h"
FRAMELESSHELPER_USE_NAMESPACE
using namespace Global;
#else
#include "SAFramelessHelper.h"
#include "SAWindowButtonGroup.h"
#endif

/**
 * @brief The SARibbonMainWindowPrivate class
 */
class SARibbonMainWindow::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonMainWindow)
public:
    PrivateData(SARibbonMainWindow* p);
    void init();

public:
    SARibbonMainWindow::RibbonTheme mCurrentRibbonTheme { SARibbonMainWindow::RibbonThemeOffice2013 };
    SARibbonBar* mRibbonBar { nullptr };
    SAWindowButtonGroup* mWindowButtonGroup { nullptr };
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
    FRAMELESSHELPER_PREPEND_NAMESPACE(StandardTitleBar) * mTitleBar = nullptr;
#else
    SAFramelessHelper* mFramelessHelper { nullptr };
#endif
};

SARibbonMainWindow::PrivateData::PrivateData(SARibbonMainWindow* p) : q_ptr(p)
{
}

void SARibbonMainWindow::PrivateData::init()
{
}

//===================================================
// SARibbonMainWindow
//===================================================
SARibbonMainWindow::SARibbonMainWindow(QWidget* parent, bool useRibbon, const Qt::WindowFlags flags)
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
    : FramelessMainWindow(parent, flags)
#else
    : QMainWindow(parent, flags)
#endif
    , d_ptr(new SARibbonMainWindow::PrivateData(this))
{
    d_ptr->init();
    connect(qApp, &QApplication::primaryScreenChanged, this, &SARibbonMainWindow::onPrimaryScreenChanged);
    if (useRibbon) {
        setRibbonBar(createRibbonBar());
        setRibbonTheme(ribbonTheme());
    } else {
        setupNormalWindow();
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
void SARibbonMainWindow::setRibbonBar(SARibbonBar* bar)
{
    QWidget* old = QMainWindow::menuWidget();
    if (old) {
        // 如果之前已经设置了menubar，要把之前的删除
        old->deleteLater();
    }
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
    auto helper = FramelessWidgetsHelper::get(this);
    QMainWindow::setMenuWidget(bar);
    helper->setTitleBarWidget(bar);

    // 设置window按钮
    if (nullptr == d_ptr->mWindowButtonGroup) {
        d_ptr->mWindowButtonGroup = new SAWindowButtonGroup(this);
    }
    d_ptr->mWindowButtonGroup->setWindowStates(windowState());
    d_ptr->mWindowButtonGroup->show();
    helper->setHitTestVisible(d_ptr->mWindowButtonGroup);   // IMPORTANT!
    helper->setHitTestVisible(bar->ribbonTabBar());         // IMPORTANT!
    helper->setHitTestVisible(bar->rightButtonGroup());     // IMPORTANT!
    helper->setHitTestVisible(bar->applicationButton());    // IMPORTANT!
    helper->setHitTestVisible(bar->quickAccessBar());       // IMPORTANT!
    helper->setHitTestVisible(bar->ribbonStackedWidget());  // IMPORTANT!
#else

    QMainWindow::setMenuWidget(bar);
    bar->installEventFilter(this);
    // 设置窗体的标题栏高度
    if (nullptr == d_ptr->mFramelessHelper) {
        d_ptr->mFramelessHelper = new SAFramelessHelper(this);
    }
    d_ptr->mFramelessHelper->setTitleHeight(bar->titleBarHeight());
    // 设置window按钮
    if (nullptr == d_ptr->mWindowButtonGroup) {
        d_ptr->mWindowButtonGroup = new SAWindowButtonGroup(this);
    }
    QSize s = d_ptr->mWindowButtonGroup->sizeHint();
    s.setHeight(bar->titleBarHeight());
    d_ptr->mWindowButtonGroup->setFixedSize(s);
    d_ptr->mWindowButtonGroup->setWindowStates(windowState());
    d_ptr->mWindowButtonGroup->show();

#endif
}

#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
FRAMELESSHELPER_PREPEND_NAMESPACE(FramelessWidgetsHelper*) SARibbonMainWindow::framelessHelper()
{
    return FramelessWidgetsHelper::get(this);
}
#else

SAFramelessHelper* SARibbonMainWindow::framelessHelper()
{
    return (d_ptr->mFramelessHelper);
}

bool SARibbonMainWindow::eventFilter(QObject* obj, QEvent* e)
{
    // 这个过滤是为了把ribbonBar上的动作传递到mainwindow，再传递到frameless，
    // 由于ribbonbar会遮挡掉frameless的区域，导致frameless无法捕获这些消息
    if (obj == ribbonBar()) {
        switch (e->type()) {
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonRelease:
        case QEvent::MouseMove:
        case QEvent::Leave:
        case QEvent::HoverMove:
        case QEvent::MouseButtonDblClick:
            QApplication::sendEvent(this, e);

        default:
            break;
        }
    }
    return (QMainWindow::eventFilter(obj, e));
}
#endif
/**
 * @brief 此函数仅用于控制最小最大化和关闭按钮的显示
 */
void SARibbonMainWindow::updateWindowFlag(Qt::WindowFlags flags)
{
    if (isUseRibbon()) {
        d_ptr->mWindowButtonGroup->updateWindowFlag(flags);
    }
    repaint();
}

/**
 * @brief 此函数返回的flags仅包括 Qt::WindowCloseButtonHint，Qt::WindowMaximizeButtonHint，Qt::WindowMinimizeButtonHint
 * 三个
 * @return
 */
Qt::WindowFlags SARibbonMainWindow::windowButtonFlags() const
{
    if (isUseRibbon()) {
        return (d_ptr->mWindowButtonGroup->windowButtonFlags());
    }
    return (windowFlags());
}

/**
 * @brief SARibbonMainWindow::setRibbonTheme
 *
 * 注意主题在构造函数设置主题会不完全生效，使用QTimer投放到队列最后执行即可
 * @code
 * QTimer::singleShot(0, this, [ this ]() { this->setRibbonTheme(SARibbonMainWindow::RibbonThemeDark); });
 * @endcode
 * @param theme
 */
void SARibbonMainWindow::setRibbonTheme(SARibbonMainWindow::RibbonTheme theme)
{
    sa_set_ribbon_theme(this, theme);
    d_ptr->mCurrentRibbonTheme = theme;
    if (SARibbonBar* bar = ribbonBar()) {
        // 尺寸修正
        switch (ribbonTheme()) {
        case RibbonThemeWindows7:
            break;
        case RibbonThemeOffice2013:
        case RibbonThemeOffice2016Blue:
        case RibbonThemeDark: {
            //! 在设置qss后需要针对margin信息重新设置进SARibbonTabBar中
            //! office2013.qss的margin信息如下设置，em是字符M所对应的宽度的长度
            //! margin-top: 0em;
            //! margin-right: 0em;
            //! margin-left: 0.2em;
            //! margin-bottom: 0em;

            SARibbonTabBar* tab = bar->ribbonTabBar();
            if (!tab) {
                break;
            }
            QFontMetrics fm = tab->fontMetrics();
            int emWidth     = SA_FONTMETRICS_WIDTH(fm, "M");
            tab->setTabMargin(QMargins(0.2 * emWidth, 0, 0, 0));
        } break;
        default:
            break;
        }
        // 上下文标签颜色设置
        switch (ribbonTheme()) {
        case RibbonThemeWindows7:
        case RibbonThemeOffice2013:
        case RibbonThemeDark:
            bar->setContextCategoryColorList(QList< QColor >());  //< 设置空颜色列表会重置为默认色系
            break;
        case RibbonThemeOffice2016Blue:
            bar->setContextCategoryColorList(QList< QColor >() << QColor(18, 64, 120));  //< 设置空颜色列表会重置为默认色系
            break;
        case RibbonThemeOffice2021Blue:
            bar->setContextCategoryColorList(QList< QColor >() << QColor(0, 0, 0, 0));  //< 设置空颜色列表会重置为默认色系
            break;
        default:
            break;
        }
    }
}

SARibbonMainWindow::RibbonTheme SARibbonMainWindow::ribbonTheme() const
{
    return (d_ptr->mCurrentRibbonTheme);
}

bool SARibbonMainWindow::isUseRibbon() const
{
    return (nullptr != ribbonBar());
}

/**
   @brief 获取左上角按钮组（最大化，最小化，关闭）
   @return
 */
SAWindowButtonGroup* SARibbonMainWindow::windowButtonGroup() const
{
    return d_ptr->mWindowButtonGroup;
}

/**
 * @brief 创建ribbonbar的工厂函数
 * @return
 */
SARibbonBar* SARibbonMainWindow::createRibbonBar()
{
    SARibbonBar* bar = new SARibbonBar(this);
    bar->setContentsMargins(3, 0, 3, 0);
    return bar;
}

void SARibbonMainWindow::resizeEvent(QResizeEvent* event)
{
    SARibbonBar* bar = ribbonBar();
    if (bar) {
        if (bar->size().width() != (this->size().width())) {
            bar->setFixedWidth(this->size().width());
        }
        if (d_ptr->mWindowButtonGroup) {
            bar->setWindowButtonSize(d_ptr->mWindowButtonGroup->size());
        }
    }
    QMainWindow::resizeEvent(event);
}

bool SARibbonMainWindow::event(QEvent* e)
{
    if (e) {
        switch (e->type()) {
        case QEvent::WindowStateChange: {
            if (isUseRibbon()) {
                d_ptr->mWindowButtonGroup->setWindowStates(windowState());
            }
        } break;

        default:
            break;
        }
    }
    return (QMainWindow::event(e));
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

/**
   @brief 构建为普通窗口
 */
void SARibbonMainWindow::setupNormalWindow()
{
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
    auto helper = FramelessWidgetsHelper::get(this);
    // 设置window按钮
    if (nullptr == d_ptr->mWindowButtonGroup) {
        d_ptr->mWindowButtonGroup = new SAWindowButtonGroup(this);
    }
    d_ptr->mWindowButtonGroup->setWindowStates(windowState());
    d_ptr->mWindowButtonGroup->resize(60, 30);
    d_ptr->mWindowButtonGroup->show();
    d_ptr->mWindowButtonGroup->raise();
#endif
}

void sa_set_ribbon_theme(QWidget* w, SARibbonMainWindow::RibbonTheme theme)
{
    QFile file;
    switch (theme) {
    case SARibbonMainWindow::RibbonThemeWindows7:
        file.setFileName(":/theme/resource/theme-win7.qss");
        break;
    case SARibbonMainWindow::RibbonThemeOffice2013:
        file.setFileName(":/theme/resource/theme-office2013.qss");
        break;
    case SARibbonMainWindow::RibbonThemeOffice2016Blue:
        file.setFileName(":/theme/resource/theme-office2016-blue.qss");
        break;
    case SARibbonMainWindow::RibbonThemeOffice2021Blue:
        file.setFileName(":/theme/resource/theme-office2021-blue.qss");
        break;
    case SARibbonMainWindow::RibbonThemeDark:
        file.setFileName(":/theme/resource/theme-dark.qss");
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
