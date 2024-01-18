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
#include <QWKWidgets/widgetwindowagent.h>
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonStackedWidget.h"
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
    void installFrameless(SARibbonMainWindow* p);

public:
    SARibbonMainWindow::RibbonTheme mCurrentRibbonTheme { SARibbonMainWindow::RibbonThemeOffice2021Blue };
    SAWindowButtonGroup* mWindowButtonGroup { nullptr };
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
    QWK::WidgetWindowAgent* mFramelessHelper { nullptr };
#else
    SAFramelessHelper* mFramelessHelper { nullptr };
#endif
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
void SARibbonMainWindow::setRibbonBar(SARibbonBar* bar)
{
    QWidget* old = QMainWindow::menuWidget();
    if (old) {
        // 如果之前已经设置了menubar，要把之前的删除
        old->deleteLater();
    }
    QMainWindow::setMenuWidget(bar);
    const int th = bar->titleBarHeight();
    // 设置window按钮
    if (nullptr == d_ptr->mWindowButtonGroup) {
        d_ptr->mWindowButtonGroup = RibbonSubElementFactory->createWindowButtonGroup(this);
    }
    SAWindowButtonGroup* wg = d_ptr->mWindowButtonGroup;
    wg->setWindowStates(windowState());
    wg->setWindowTitleHeight(th);
    wg->show();
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
    auto helper = d_ptr->mFramelessHelper;
    helper->setTitleBar(bar);
    // 以下这些窗口，需要允许点击
    helper->setHitTestVisible(wg);                          // IMPORTANT!
    helper->setHitTestVisible(bar->ribbonTabBar());         // IMPORTANT!
    helper->setHitTestVisible(bar->rightButtonGroup());     // IMPORTANT!
    helper->setHitTestVisible(bar->applicationButton());    // IMPORTANT!
    helper->setHitTestVisible(bar->quickAccessBar());       // IMPORTANT!
    helper->setHitTestVisible(bar->ribbonStackedWidget());  // IMPORTANT!
    if (wg->closeButton()) {
        helper->setSystemButton(QWK::WindowAgentBase::Close, wg->closeButton());
    }
    if (wg->minimizeButton()) {
        helper->setSystemButton(QWK::WindowAgentBase::Minimize, wg->minimizeButton());
    }
    if (wg->maximizeButton()) {
        helper->setSystemButton(QWK::WindowAgentBase::Maximize, wg->maximizeButton());
    }
#else
    bar->installEventFilter(this);
    // 设置窗体的标题栏高度
    d_ptr->mFramelessHelper->setTitleHeight(th);
    d_ptr->mFramelessHelper->setRubberBandOnResize(false);
#endif
}

#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER

/**
 * @brief 如果ribbon中有自定义的窗口在标题栏等非点击区域加入后，想能点击，需要调用此接口告知可点击
 * @param w
 * @param visible
 */
void SARibbonMainWindow::setFramelessHitTestVisible(const QWidget* w, bool visible)
{
    auto helper = d_ptr->mFramelessHelper;
    helper->setHitTestVisible(w, visible);
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
    // 因此必须ribbonBar()->installEventFilter(this);
    // 20240101发现installEventFilter后，SARibbonMainWindow没有执行这个回调
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
    if (SAWindowButtonGroup* g = d_ptr->mWindowButtonGroup) {
        g->updateWindowFlag(flags);
    }
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
        auto theme = ribbonTheme();
        // 尺寸修正
        switch (theme) {
        case RibbonThemeWindows7:
        case RibbonThemeOffice2013:
        case RibbonThemeOffice2016Blue:
        case RibbonThemeDark:
        case RibbonThemeDark2: {
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
        case RibbonThemeOffice2021Blue: {
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
        // 基线颜色设置
        if (RibbonThemeOffice2013 == theme) {
            bar->setTabBarBaseLineColor(QColor(186, 201, 219));
        } else {
            bar->setTabBarBaseLineColor(QColor());
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
    QMainWindow::resizeEvent(event);
    SARibbonBar* bar        = ribbonBar();
    SAWindowButtonGroup* wg = d_ptr->mWindowButtonGroup;

    if (wg) {
        if (bar) {
            const int th = bar->titleBarHeight();
            if (th != wg->height()) {
                wg->setWindowTitleHeight(th);
            }
        }
        QSize wgSizeHint = wg->sizeHint();
        // 需要先resize，有可能不能设置到对应的size，以设置后的size为准
        wg->resize(wgSizeHint);
        wgSizeHint = wg->size();
        wg->setGeometry(frameGeometry().width() - wgSizeHint.width(), 0, wgSizeHint.width(), wgSizeHint.height());
    }
    if (bar) {
        if (wg) {
            bar->setWindowButtonGroupSize(wg->size());
        }
        if (bar->size().width() != (this->size().width())) {
            bar->setFixedWidth(this->size().width());
        }
    }
}

void SARibbonMainWindow::changeEvent(QEvent* e)
{
    if (e) {
        switch (e->type()) {
        case QEvent::WindowStateChange: {
            if (SAWindowButtonGroup* wg = d_ptr->mWindowButtonGroup) {
                wg->setWindowStates(windowState());
            }
        } break;

        default:
            break;
        }
    }
    QMainWindow::changeEvent(e);
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
    case SARibbonMainWindow::RibbonThemeDark2:
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
