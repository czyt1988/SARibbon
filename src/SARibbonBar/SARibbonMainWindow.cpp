#include "SARibbonMainWindow.h"
#include "SAFramelessHelper.h"
#include "SARibbonBar.h"
#include "SARibbonDrawHelper.h"
#include "SARibbonElementManager.h"
#include "SARibbonTabBar.h"
#include "SAWindowButtonGroup.h"
#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QHash>
#include <QWindowStateChangeEvent>
/**
 * @brief The SARibbonMainWindowPrivate class
 */
class SARibbonMainWindow::PrivateData {
    SA_RIBBON_DECLARE_PUBLIC(SARibbonMainWindow)
public:
    PrivateData(SARibbonMainWindow* p);
    void init();

public:
    SARibbonMainWindow::RibbonTheme mCurrentRibbonTheme{ SARibbonMainWindow::Office2013 };
    SARibbonBar* mRibbonBar{ nullptr };
    SAWindowButtonGroup* mWindowButtonGroup{ nullptr };
    SAFramelessHelper* mFramelessHelper{ nullptr };
    bool mUseRibbon{ true };
};

SARibbonMainWindow::PrivateData::PrivateData(SARibbonMainWindow* p)
    : q_ptr(p)
{
}

void SARibbonMainWindow::PrivateData::init()
{
}

//===================================================
// SARibbonMainWindow
//===================================================

SARibbonMainWindow::SARibbonMainWindow(QWidget* parent, bool useRibbon)
    : QMainWindow(parent)
    , d_ptr(new SARibbonMainWindow::PrivateData(this))
{
    d_ptr->init();
    d_ptr->mUseRibbon = useRibbon;
    if (useRibbon) {
        setRibbonTheme(ribbonTheme());
        setMenuWidget(createRibbonBar());
        qDebug() << RibbonSubElementStyleOpt;
#ifdef Q_OS_UNIX
        //某些系统会对FramelessWindowHint异常
        // FramelessHelper用如下这种setWindowFlags(w->windowFlags()|Qt::FramelessWindowHint);方式设置，改为强制取消其他
        setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
#endif
    }
}

SARibbonMainWindow::~SARibbonMainWindow()
{
}

const SARibbonBar* SARibbonMainWindow::ribbonBar() const
{
    return (d_ptr->mRibbonBar);
}

/**
 * @brief 如果useRibbon为false，会返回nullptr
 * @return
 */
SARibbonBar* SARibbonMainWindow::ribbonBar()
{
    return (d_ptr->mRibbonBar);
}

SAFramelessHelper* SARibbonMainWindow::framelessHelper()
{
    return (d_ptr->mFramelessHelper);
}

void SARibbonMainWindow::setRibbonTheme(SARibbonMainWindow::RibbonTheme theme)
{
    switch (theme) {
    case NormalTheme:
        loadTheme(":/theme/resource/default.qss");
        break;
    case Office2013:
        loadTheme(":/theme/resource/office2013.qss");
        break;
    default:
        loadTheme(":/theme/resource/default.qss");
        break;
    }
}

SARibbonMainWindow::RibbonTheme SARibbonMainWindow::ribbonTheme() const
{
    return (d_ptr->mCurrentRibbonTheme);
}

bool SARibbonMainWindow::isUseRibbon() const
{
    return (d_ptr->mUseRibbon);
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

void SARibbonMainWindow::setMenuWidget(QWidget* menubar)
{
    QMainWindow::setMenuWidget(menubar);
    SARibbonBar* bar = qobject_cast<SARibbonBar*>(menubar);

    if (bar) {
        d_ptr->mRibbonBar = bar;
        d_ptr->mRibbonBar->installEventFilter(this);
        //设置窗体的标题栏高度
        if (nullptr == d_ptr->mFramelessHelper) {
            d_ptr->mFramelessHelper = new SAFramelessHelper(this);
        }
        d_ptr->mFramelessHelper->setTitleHeight(d_ptr->mRibbonBar->titleBarHeight());
        //设置window按钮
        if (nullptr == d_ptr->mWindowButtonGroup) {
            d_ptr->mWindowButtonGroup = new SAWindowButtonGroup(this);
        }
        QSize s = d_ptr->mWindowButtonGroup->sizeHint();
        s.setHeight(d_ptr->mRibbonBar->titleBarHeight());
        d_ptr->mWindowButtonGroup->setFixedSize(s);
        d_ptr->mWindowButtonGroup->setWindowStates(windowState());
        d_ptr->mUseRibbon = true;
        d_ptr->mWindowButtonGroup->show();
    } else {
        d_ptr->mRibbonBar = nullptr;
        d_ptr->mUseRibbon = false;
        if (d_ptr->mFramelessHelper) {
            delete d_ptr->mFramelessHelper;
            d_ptr->mFramelessHelper = nullptr;
        }
        if (d_ptr->mWindowButtonGroup) {
            d_ptr->mWindowButtonGroup->hide();
        }
    }
}

void SARibbonMainWindow::setMenuBar(QMenuBar* menuBar)
{
    QMenuBar* old = QMainWindow::menuBar();
    if (old) {
        //如果之前已经设置了menubar，要把之前的删除
        old->deleteLater();
    }

    QMainWindow::setMenuBar(menuBar);
    SARibbonBar* bar = qobject_cast<SARibbonBar*>(menuBar);

    if (bar) {
        d_ptr->mRibbonBar = bar;
        d_ptr->mRibbonBar->installEventFilter(this);
        //设置窗体的标题栏高度
        if (nullptr == d_ptr->mFramelessHelper) {
            d_ptr->mFramelessHelper = new SAFramelessHelper(this);
        }
        d_ptr->mFramelessHelper->setTitleHeight(d_ptr->mRibbonBar->titleBarHeight());
        //设置window按钮
        if (nullptr == d_ptr->mWindowButtonGroup) {
            d_ptr->mWindowButtonGroup = new SAWindowButtonGroup(this);
        }
        QSize s = d_ptr->mWindowButtonGroup->sizeHint();
        s.setHeight(d_ptr->mRibbonBar->titleBarHeight());
        d_ptr->mWindowButtonGroup->setFixedSize(s);
        d_ptr->mWindowButtonGroup->setWindowStates(windowState());
        d_ptr->mUseRibbon = true;
        d_ptr->mWindowButtonGroup->show();
    } else {
        d_ptr->mRibbonBar = nullptr;
        d_ptr->mUseRibbon = false;
        if (d_ptr->mFramelessHelper) {
            delete d_ptr->mFramelessHelper;
            d_ptr->mFramelessHelper = nullptr;
        }
        if (d_ptr->mWindowButtonGroup) {
            d_ptr->mWindowButtonGroup->hide();
        }
    }
}

/**
 * @brief 创建ribbonbar的工厂函数
 * @return
 */
SARibbonBar* SARibbonMainWindow::createRibbonBar()
{
    SARibbonBar* bar = new SARibbonBar(this);
    switch (ribbonTheme()) {
    case NormalTheme:
        break;
    case Office2013: {
        //在设置qss后需要针对margin信息重新设置进SARibbonTabBar中
        // office2013.qss的margin信息如下设置，em是字符M所对应的宽度的长度
        // margin-top: 0em;
        // margin-right: 0em;
        // margin-left: 0.2em;
        // margin-bottom: 0em;

        SARibbonTabBar* tab = bar->ribbonTabBar();
        if (!tab) {
            break;
        }
        QFontMetrics fm = tab->fontMetrics();
        int emWidth = SA_FONTMETRICS_WIDTH(fm, "M");
        tab->setTabMargin(QMargins(0.2 * emWidth, 0, 0, 0));
    } break;
    default:
        break;
    }
    bar->setContentsMargins(3, 0, 3, 0);
    return bar;
}

void SARibbonMainWindow::resizeEvent(QResizeEvent* event)
{
    if (d_ptr->mRibbonBar) {
        if (d_ptr->mRibbonBar->size().width() != (this->size().width())) {
            d_ptr->mRibbonBar->setFixedWidth(this->size().width());
        }
        if (d_ptr->mWindowButtonGroup) {
            d_ptr->mRibbonBar->setWindowButtonSize(d_ptr->mWindowButtonGroup->size());
        }
    }
    QMainWindow::resizeEvent(event);
}

bool SARibbonMainWindow::eventFilter(QObject* obj, QEvent* e)
{
    //这个过滤是为了把ribbonBar上的动作传递到mainwindow，再传递到frameless，
    //由于ribbonbar会遮挡刁frameless的区域，导致frameless无法捕获这些消息
    if (obj == d_ptr->mRibbonBar) {
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

void SARibbonMainWindow::loadTheme(const QString& themeFile)
{
    QFile file(themeFile);

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return;
    }
    setStyleSheet(file.readAll());
}
