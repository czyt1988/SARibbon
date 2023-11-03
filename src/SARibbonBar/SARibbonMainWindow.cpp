#include "SARibbonMainWindow.h"
#include "SARibbonBar.h"
#include "SARibbonDrawHelper.h"
#include "SARibbonElementManager.h"
#include "SARibbonTabBar.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QHash>
#include <QWindowStateChangeEvent>
#include <QWindow>
#include "SAWindowButtonGroup.h"
// frameless
#include "FramelessWindowConverter.h"

/**
 * @brief The SARibbonMainWindowPrivate class
 */
class SARibbonMainWindow::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonMainWindow)
public:
    PrivateData(SARibbonMainWindow* p);
    void init(SARibbonMainWindow* mainwindow);

public:
    SARibbonMainWindow::RibbonTheme mCurrentRibbonTheme { SARibbonMainWindow::RibbonThemeOffice2013 };
    SARibbonBar* mRibbonBar { nullptr };
    SAWindowButtonGroup* mWindowButtonGroup { nullptr };
    FWC::FramelessWindowConverter framelessWindowConverter;
};

SARibbonMainWindow::PrivateData::PrivateData(SARibbonMainWindow* p) : q_ptr(p)
{
}

void SARibbonMainWindow::PrivateData::init(SARibbonMainWindow* mainwindow)
{
    // On Windows Qt::FramelessWindowHint needs to be set for translucency
    // Disable all transparency effects on all platforms when not frameless
    mainwindow->setWindowFlag(Qt::FramelessWindowHint, true);
    mainwindow->setWindowFlag(Qt::NoDropShadowWindowHint, true);
    //    mainwindow->setAttribute(Qt::WA_TranslucentBackground, true);
    //    mainwindow->setAttribute(Qt::WA_NoSystemBackground, true);
    // Convert window
    framelessWindowConverter.convertWindowToFrameless(mainwindow->winId(), [ mainwindow ]() {
        mainwindow->windowHandle()->setMouseGrabEnabled(false);
    });
    //    framelessWindowConverter.setBorderWidth(1);
    //
    framelessWindowConverter.setShouldPerformWindowDrag([ this, mainwindow ](int x, int y) -> bool {
        // 这里要判断x，y是否允许拖动窗口
        // 如果点到WindowButtonGroup不拖动
        if (this->mWindowButtonGroup) {
            QRect buttonGroupGeo = this->mWindowButtonGroup->geometry();
            if (buttonGroupGeo.contains(x, y)) {
                return false;
            }
        }
        QWidget* w = mainwindow->childAt(x, y);
        if (nullptr == w) {
            // 点击的就是自己
            return false;
        }
        SARibbonBar* ribbonbar = qobject_cast< SARibbonBar* >(w);
        if (!ribbonbar) {
            // 没有点击到ribbonbar,childAt会把最底层的窗口获取，
            // title是直接绘制到ribbonbar上的，因此获取到的child是ribbonbar就说明点击就是title，
            // 点击到ribbonbar的其他子窗口获取到的是那个子窗口的指针
            return false;
        }
        QPoint clickPointOfRibbon = ribbonbar->mapFrom(mainwindow, QPoint(x, y));
        if (clickPointOfRibbon.y() < ribbonbar->titleBarHeight()) {
            return true;
        }
        return false;
    });
    framelessWindowConverter.setEnableResizing(true);
}

//===================================================
// SARibbonMainWindow
//===================================================
SARibbonMainWindow::SARibbonMainWindow(QWidget* parent, bool useRibbon, const Qt::WindowFlags flags)
    : QMainWindow(parent, flags), d_ptr(new SARibbonMainWindow::PrivateData(this))
{
    d_ptr->init(this);
    if (useRibbon) {
        installRibbonBar(createRibbonBar());
        setRibbonTheme(ribbonTheme());
        qDebug() << RibbonSubElementStyleOpt;
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

bool SARibbonMainWindow::eventFilter(QObject* obj, QEvent* e)
{
    // 这个过滤是为了把ribbonBar上的动作传递到mainwindow，再传递到frameless，
    // 由于ribbonbar会遮挡刁frameless的区域，导致frameless无法捕获这些消息
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

void SARibbonMainWindow::setRibbonTheme(SARibbonMainWindow::RibbonTheme theme)
{
    sa_set_ribbon_theme(this, theme);
    d_ptr->mCurrentRibbonTheme = theme;
    if (SARibbonBar* bar = ribbonBar()) {
        switch (ribbonTheme()) {
        case RibbonThemeWindows7:
            break;
        case RibbonThemeOffice2013:
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
                Qt::WindowStates st = windowState();
                qDebug() << "WindowStateChange" << st;
                d_ptr->mWindowButtonGroup->setWindowStates(st);
                if (st.testFlag(Qt::WindowMinimized)) {
                    d_ptr->framelessWindowConverter.minimizeWindow();
                } else if (st.testFlag(Qt::WindowMaximized)) {
                    d_ptr->framelessWindowConverter.maximizeWindow();
                } else if (st.testFlag(Qt::WindowFullScreen)) {
                    d_ptr->framelessWindowConverter.toggleFullscreen();
                }
            }
        } break;

        default:
            break;
        }
    }
    return (QMainWindow::event(e));
}

bool SARibbonMainWindow::nativeEvent(const QByteArray& eventType, void* message, long* result)
{
    Q_UNUSED(eventType);
    return d_ptr->framelessWindowConverter.filterNativeEvents(message, result);
}

void SARibbonMainWindow::installRibbonBar(SARibbonBar* bar)
{
    QWidget* old = QMainWindow::menuWidget();
    if (old) {
        // 如果之前已经设置了menubar，要把之前的删除
        old->deleteLater();
    }
    // 安装ribbonbar
    setMenuWidget(bar);
    // 设置window按钮
    if (nullptr == d_ptr->mWindowButtonGroup) {
        d_ptr->mWindowButtonGroup = new SAWindowButtonGroup(this);
    }
    QSize s = d_ptr->mWindowButtonGroup->sizeHint();
    s.setHeight(bar->titleBarHeight());
    d_ptr->mWindowButtonGroup->setFixedSize(s);
    d_ptr->mWindowButtonGroup->setWindowStates(windowState());
    d_ptr->mWindowButtonGroup->show();
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
    w->setStyleSheet(file.readAll());
}
