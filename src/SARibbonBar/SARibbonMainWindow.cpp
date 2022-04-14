#include "SARibbonMainWindow.h"
#include <QWindowStateChangeEvent>
#include "SAFramelessHelper.h"
#include "SAWindowButtonGroup.h"
#include "SARibbonBar.h"
#include <QApplication>
#include <QDebug>
#include <QHash>
#include <QFile>
#include "SARibbonElementManager.h"
/**
 * @brief The SARibbonMainWindowPrivate class
 */
class SARibbonMainWindowPrivate
{
public:
    SARibbonMainWindowPrivate(SARibbonMainWindow* p);
    void init();

    SARibbonMainWindow* Parent;
    SARibbonBar* ribbonBar;
    SARibbonMainWindow::RibbonTheme currentRibbonTheme;
    SAWindowButtonGroup* windowButtonGroup;
    SAFramelessHelper* pFramelessHelper;
    bool useRibbon;
};

SARibbonMainWindowPrivate::SARibbonMainWindowPrivate(SARibbonMainWindow* p)
    : Parent(p)
    , ribbonBar(nullptr)
    , currentRibbonTheme(SARibbonMainWindow::Office2013)
    , windowButtonGroup(nullptr)
    , pFramelessHelper(nullptr)
    , useRibbon(true)
{
}

void SARibbonMainWindowPrivate::init()
{
}

SARibbonMainWindow::SARibbonMainWindow(QWidget* parent, bool useRibbon)
    : QMainWindow(parent), m_d(new SARibbonMainWindowPrivate(this))
{
    m_d->init();
    m_d->useRibbon = useRibbon;
    if (useRibbon) {
        setRibbonTheme(ribbonTheme());
        setMenuWidget(new SARibbonBar(this));
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
    delete m_d;
}

const SARibbonBar* SARibbonMainWindow::ribbonBar() const
{
    return (m_d->ribbonBar);
}

/**
 * @brief 如果useRibbon为false，会返回nullptr
 * @return
 */
SARibbonBar* SARibbonMainWindow::ribbonBar()
{
    return (m_d->ribbonBar);
}

SAFramelessHelper* SARibbonMainWindow::framelessHelper()
{
    return (m_d->pFramelessHelper);
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
    return (m_d->currentRibbonTheme);
}

bool SARibbonMainWindow::isUseRibbon() const
{
    return (m_d->useRibbon);
}

/**
 * @brief 此函数仅用于控制最小最大化和关闭按钮的显示
 */
void SARibbonMainWindow::updateWindowFlag(Qt::WindowFlags flags)
{
    if (isUseRibbon()) {
        m_d->windowButtonGroup->updateWindowFlag(flags);
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
        return (m_d->windowButtonGroup->windowButtonFlags());
    }
    return (windowFlags());
}

void SARibbonMainWindow::setMenuWidget(QWidget* menubar)
{
    QMainWindow::setMenuWidget(menubar);
    SARibbonBar* bar = qobject_cast< SARibbonBar* >(menubar);

    if (bar) {
        m_d->ribbonBar = bar;
        m_d->ribbonBar->installEventFilter(this);
        //设置窗体的标题栏高度
        if (nullptr == m_d->pFramelessHelper) {
            m_d->pFramelessHelper = new SAFramelessHelper(this);
        }
        m_d->pFramelessHelper->setTitleHeight(m_d->ribbonBar->titleBarHeight());
        //设置window按钮
        if (nullptr == m_d->windowButtonGroup) {
            m_d->windowButtonGroup = new SAWindowButtonGroup(this);
        }
        QSize s = m_d->windowButtonGroup->sizeHint();
        s.setHeight(m_d->ribbonBar->titleBarHeight());
        m_d->windowButtonGroup->setFixedSize(s);
        m_d->windowButtonGroup->setWindowStates(windowState());
        m_d->useRibbon = true;
        m_d->windowButtonGroup->show();
    } else {
        m_d->ribbonBar = nullptr;
        m_d->useRibbon = false;
        if (m_d->pFramelessHelper) {
            delete m_d->pFramelessHelper;
            m_d->pFramelessHelper = nullptr;
        }
        if (m_d->windowButtonGroup) {
            m_d->windowButtonGroup->hide();
        }
    }
}

void SARibbonMainWindow::setMenuBar(QMenuBar* menuBar)
{
    QMainWindow::setMenuBar(menuBar);
    SARibbonBar* bar = qobject_cast< SARibbonBar* >(menuBar);

    if (bar) {
        m_d->ribbonBar = bar;
        m_d->ribbonBar->installEventFilter(this);
        //设置窗体的标题栏高度
        if (nullptr == m_d->pFramelessHelper) {
            m_d->pFramelessHelper = new SAFramelessHelper(this);
        }
        m_d->pFramelessHelper->setTitleHeight(m_d->ribbonBar->titleBarHeight());
        //设置window按钮
        if (nullptr == m_d->windowButtonGroup) {
            m_d->windowButtonGroup = new SAWindowButtonGroup(this);
        }
        QSize s = m_d->windowButtonGroup->sizeHint();
        s.setHeight(m_d->ribbonBar->titleBarHeight());
        m_d->windowButtonGroup->setFixedSize(s);
        m_d->windowButtonGroup->setWindowStates(windowState());
        m_d->useRibbon = true;
        m_d->windowButtonGroup->show();
    } else {
        m_d->ribbonBar = nullptr;
        m_d->useRibbon = false;
        if (m_d->pFramelessHelper) {
            delete m_d->pFramelessHelper;
            m_d->pFramelessHelper = nullptr;
        }
        if (m_d->windowButtonGroup) {
            m_d->windowButtonGroup->hide();
        }
    }
}

void SARibbonMainWindow::resizeEvent(QResizeEvent* event)
{
    if (m_d->ribbonBar) {
        if (m_d->ribbonBar->size().width() != this->size().width()) {
            m_d->ribbonBar->setFixedWidth(this->size().width());
        }
        if (m_d->windowButtonGroup) {
            m_d->ribbonBar->setWindowButtonSize(m_d->windowButtonGroup->size());
        }
    }
    QMainWindow::resizeEvent(event);
}

bool SARibbonMainWindow::eventFilter(QObject* obj, QEvent* e)
{
    //这个过滤是为了把ribbonBar上的动作传递到mainwindow，再传递到frameless，
    //由于ribbonbar会遮挡刁frameless的区域，导致frameless无法捕获这些消息
    if (obj == m_d->ribbonBar) {
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
                m_d->windowButtonGroup->setWindowStates(windowState());
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
