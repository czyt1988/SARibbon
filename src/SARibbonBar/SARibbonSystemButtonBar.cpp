#include "SARibbonSystemButtonBar.h"
#include <QToolButton>
#include <QResizeEvent>
#include <QStyle>
#include <QDebug>
#include <QScopedPointer>
#include <QWindowStateChangeEvent>
#include "SARibbonMainWindow.h"
#include "SARibbonBar.h"
#include "SARibbonButtonGroupWidget.h"

// 为了避免使用此框架的app设置了全局的qpushbutton 的 qss样式影响此按钮，定义了一个类

/**
 * @brief The SARibbonSystemButtonBar class
 */
class SARibbonSystemButtonBar::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonSystemButtonBar)
public:
    SARibbonSystemToolButton* buttonClose { nullptr };
    SARibbonSystemToolButton* buttonMinimize { nullptr };
    SARibbonSystemToolButton* buttonMaximize { nullptr };
    int mCloseStretch { 4 };
    int mMaxStretch { 3 };
    int mMinStretch { 3 };
    int mWindowButtonWidth { 35 };
    int mTitleBarHeight { 28 };
    Qt::WindowFlags mFlags { Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint };
    SARibbonButtonGroupWidget* mButtonGroup;

public:
    PrivateData(SARibbonSystemButtonBar* p) : q_ptr(p)
    {
        mButtonGroup = new SARibbonButtonGroupWidget(p);
        mButtonGroup->setObjectName("SASystemButtonGroup");
    }

    void setupMinimizeButton(bool on)
    {
        SARibbonSystemButtonBar* par = q_ptr;

        if (on) {
            if (buttonMinimize) {
                buttonMinimize->deleteLater();
                buttonMinimize = nullptr;
            }
            buttonMinimize = new SARibbonSystemToolButton(par);
            buttonMinimize->setObjectName(QStringLiteral("SAMinimizeWindowButton"));
            buttonMinimize->setFocusPolicy(Qt::NoFocus);  // 避免铺抓到
            buttonMinimize->show();
            par->connect(buttonMinimize, &QAbstractButton::clicked, par, &SARibbonSystemButtonBar::minimizeWindow);
        } else {
            if (buttonMinimize) {
                buttonMinimize->deleteLater();
                buttonMinimize = nullptr;
            }
        }
        updateSize();
    }

    void setupMaximizeButton(bool on)
    {
        SARibbonSystemButtonBar* par = q_ptr;

        if (on) {
            if (buttonMaximize) {
                buttonMaximize->deleteLater();
                buttonMaximize = nullptr;
            }
            buttonMaximize = new SARibbonSystemToolButton(par);
            buttonMaximize->setObjectName(QStringLiteral("SAMaximizeWindowButton"));
            buttonMaximize->setCheckable(true);
            buttonMaximize->setFocusPolicy(Qt::NoFocus);  // 避免铺抓到
            //            buttonMaximize->setIconSize(buttonMaximize->size() * mIconscale);
            buttonMaximize->show();
            par->connect(buttonMaximize, &QAbstractButton::clicked, par, &SARibbonSystemButtonBar::maximizeWindow);
        } else {
            if (buttonMaximize) {
                buttonMaximize->deleteLater();
                ;
                buttonMaximize = nullptr;
            }
        }
        updateSize();
    }

    void setupCloseButton(bool on)
    {
        SARibbonSystemButtonBar* par = q_ptr;

        if (on) {
            if (buttonClose) {
                buttonClose->deleteLater();
                buttonClose = nullptr;
            }
            buttonClose = new SARibbonSystemToolButton(par);
            buttonClose->setObjectName(QStringLiteral("SACloseWindowButton"));
            buttonClose->setFocusPolicy(Qt::NoFocus);  // 避免铺抓到
            // buttonClose->setFlat(true);
            par->connect(buttonClose, &QAbstractButton::clicked, par, &SARibbonSystemButtonBar::closeWindow);
            //            buttonClose->setIconSize(buttonClose->size() * mIconscale);
            buttonClose->show();
        } else {
            if (buttonClose) {
                buttonClose->deleteLater();
                ;
                buttonClose = nullptr;
            }
        }
        updateSize();
    }

    void updateSize()
    {
        resizeElement(q_ptr->size());
    }

    void resizeElement(QSize size)
    {
        int x = size.width();
        if (buttonClose) {
            int w = closeButtonWidthHint();
            x -= w;
            buttonClose->setGeometry(x, 0, w, size.height());
        }
        if (buttonMaximize) {
            int w = maxButtonWidthHint();
            x -= w;
            buttonMaximize->setGeometry(x, 0, w, size.height());
        }
        if (buttonMinimize) {
            int w = minButtonWidthHint();
            x -= w;
            buttonMinimize->setGeometry(x, 0, w, size.height());
        }
        if (mButtonGroup) {
            mButtonGroup->setGeometry(0, 0, x, size.height());
        }
    }

    int closeButtonWidthHint() const
    {
        qreal t = mCloseStretch + mMaxStretch + mMinStretch;
        return (mCloseStretch * (3 * mWindowButtonWidth)) / t;
    }

    int maxButtonWidthHint() const
    {
        qreal t = mCloseStretch + mMaxStretch + mMinStretch;
        return (mMaxStretch * (3 * mWindowButtonWidth)) / t;
    }

    int minButtonWidthHint() const
    {
        qreal t = mCloseStretch + mMaxStretch + mMinStretch;
        return (mMinStretch * (3 * mWindowButtonWidth)) / t;
    }

    QSize sizeHint() const
    {
        int height = mTitleBarHeight;
        if (height < 20) {
            height = 20;
        }
        QSize res(0, 0);
        if (mButtonGroup) {
            res = mButtonGroup->sizeHint();
        }
        res.setHeight(height);
        if (buttonClose) {
            res.setWidth(res.width() + closeButtonWidthHint());
        }
        if (buttonMaximize) {
            res.setWidth(res.width() + maxButtonWidthHint());
        }
        if (buttonMinimize) {
            res.setWidth(res.width() + minButtonWidthHint());
        }
        return res;
    }
};

//===================================================
// SARibbonSystemToolButton
//===================================================
SARibbonSystemToolButton::SARibbonSystemToolButton(QWidget* p) : QToolButton(p)
{
    setAutoRaise(true);
}
//===================================================
// SARibbonSystemButtonBar
//===================================================
SARibbonSystemButtonBar::SARibbonSystemButtonBar(QWidget* parent)
    : QFrame(parent), d_ptr(new SARibbonSystemButtonBar::PrivateData(this))
{
    updateWindowFlag();
}

/**
 * @brief 构造函数，强制使用flags，而不是用parent的flags进行构造
 * @param parent
 * @param flags
 */
SARibbonSystemButtonBar::SARibbonSystemButtonBar(QWidget* parent, Qt::WindowFlags flags)
    : QFrame(parent), d_ptr(new SARibbonSystemButtonBar::PrivateData(this))
{
    d_ptr->mFlags = flags;
    updateWindowFlag();
}

SARibbonSystemButtonBar::~SARibbonSystemButtonBar()
{
}

void SARibbonSystemButtonBar::setupMinimizeButton(bool on)
{
    d_ptr->setupMinimizeButton(on);
}

void SARibbonSystemButtonBar::setupMaximizeButton(bool on)
{
    d_ptr->setupMaximizeButton(on);
}

void SARibbonSystemButtonBar::setupCloseButton(bool on)
{
    d_ptr->setupCloseButton(on);
}

void SARibbonSystemButtonBar::updateWindowFlag()
{
    QWidget* topedWidget = this;
    // 找到最顶层窗口
    while (topedWidget->parentWidget()) {
        topedWidget = topedWidget->parentWidget();
    }
    Qt::WindowFlags flags = topedWidget->windowFlags();
    updateWindowFlag(flags);
}

/**
 * @brief 此函数仅用于控制最小最大化和关闭按钮的显示
 * @param flags
 */
void SARibbonSystemButtonBar::updateWindowFlag(Qt::WindowFlags flags)
{
    d_ptr->mFlags = flags;
    setupMinimizeButton(flags & Qt::WindowMinimizeButtonHint);
    setupMaximizeButton(flags & Qt::WindowMaximizeButtonHint);
    setupCloseButton(flags & Qt::WindowCloseButtonHint);
}

/**
 * @brief 设置按钮的宽度比例,最终按钮宽度将按照此比例进行设置
 * @param close 关闭按钮比例
 * @param max 最大化按钮比例
 * @param min 最小化按钮比例
 */
void SARibbonSystemButtonBar::setButtonWidthStretch(int close, int max, int min)
{
    d_ptr->mMaxStretch   = max;
    d_ptr->mMinStretch   = min;
    d_ptr->mCloseStretch = close;
}

/**
 * @brief 标题栏高度
 *
 * 标题栏高度会影响sizeHint
 * @param h
 */
void SARibbonSystemButtonBar::setWindowTitleHeight(int h)
{
    d_ptr->mTitleBarHeight = h;
}

/**
 * @brief 标题栏高度
 * @return
 */
int SARibbonSystemButtonBar::windowTitleHeight() const
{
    return d_ptr->mTitleBarHeight;
}

/**
 * @brief 系统按钮的宽度
 * @param w
 */
void SARibbonSystemButtonBar::setWindowButtonWidth(int w)
{
    d_ptr->mWindowButtonWidth = w;
}

/**
 * @brief 系统按钮的宽度
 * @param w
 */
int SARibbonSystemButtonBar::windowButtonWidth() const
{
    return d_ptr->mWindowButtonWidth;
}

/**
 * @brief 设置窗口状态（最大最小化按钮状态）
 * @param s
 */
void SARibbonSystemButtonBar::setWindowStates(Qt::WindowStates s)
{
    if (d_ptr->buttonMaximize) {
        bool on = s.testFlag(Qt::WindowMaximized);
        d_ptr->buttonMaximize->setChecked(on);
        d_ptr->buttonMaximize->setToolTip(on ? tr("Restore") : tr("Maximize"));
    }
}

/**
 * @brief 此函数返回的flags仅包括 Qt::WindowCloseButtonHint，Qt::WindowMaximizeButtonHint，Qt::WindowMinimizeButtonHint
 * 三个
 *
 * @return
 */
Qt::WindowFlags SARibbonSystemButtonBar::windowButtonFlags() const
{
    Qt::WindowFlags f = Qt::Widget;  // widget是000

    if (d_ptr->mFlags & Qt::WindowCloseButtonHint) {
        f |= Qt::WindowCloseButtonHint;
    }
    if (d_ptr->mFlags & Qt::WindowMaximizeButtonHint) {
        f |= Qt::WindowMaximizeButtonHint;
    }
    if (d_ptr->mFlags & Qt::WindowMinimizeButtonHint) {
        f |= Qt::WindowMinimizeButtonHint;
    }

    return (f);
}

QSize SARibbonSystemButtonBar::sizeHint() const
{
    return (d_ptr->sizeHint());
}

bool SARibbonSystemButtonBar::eventFilter(QObject* obj, QEvent* event)
{
    if (obj && event) {
        SARibbonMainWindow* mainWindow = qobject_cast< SARibbonMainWindow* >(obj);
        if (!mainWindow) {
            // 所有事件都不消费
            return QFrame::eventFilter(obj, event);
        }
        // SARibbonMainWindow的事件
        switch (event->type()) {
        case QEvent::Resize: {
            int th = 25;

            SARibbonBar* ribbonBar = mainWindow->ribbonBar();
            if (ribbonBar) {
                th = ribbonBar->titleBarHeight();
            }
            if (th != height()) {
                setWindowTitleHeight(th);
            }
            QRect fr         = mainWindow->geometry();
            QSize wgSizeHint = sizeHint();
            setGeometry(fr.width() - wgSizeHint.width(), 0, wgSizeHint.width(), wgSizeHint.height());
            // 把设置好的尺寸给ribbonbar
            if (ribbonBar) {
                ribbonBar->setSystemButtonGroupSize(size());
            }
        } break;
        case QEvent::WindowStateChange: {
            setWindowStates(mainWindow->windowState());
        } break;
        default:
            break;
        }
    }
    return QFrame::eventFilter(obj, event);
}

QAbstractButton* SARibbonSystemButtonBar::minimizeButton() const
{
    return d_ptr->buttonMinimize;
}

QAbstractButton* SARibbonSystemButtonBar::maximizeButton() const
{
    return d_ptr->buttonMaximize;
}

QAbstractButton* SARibbonSystemButtonBar::closeButton() const
{
    return d_ptr->buttonClose;
}

void SARibbonSystemButtonBar::setIconSize(const QSize& ic)
{
    d_ptr->mButtonGroup->setIconSize(ic);
}

QSize SARibbonSystemButtonBar::iconSize() const
{
    return d_ptr->mButtonGroup->iconSize();
}

void SARibbonSystemButtonBar::addAction(QAction* a)
{
    d_ptr->mButtonGroup->addAction(a);
}

void SARibbonSystemButtonBar::addMenuAction(QAction* menuAction, QToolButton::ToolButtonPopupMode popupMode)
{
    d_ptr->mButtonGroup->addMenuAction(menuAction, popupMode);
}

QAction* SARibbonSystemButtonBar::addMenuAction(QMenu* menu, QToolButton::ToolButtonPopupMode popupMode)
{
    return d_ptr->mButtonGroup->addMenuAction(menu, popupMode);
}

QAction* SARibbonSystemButtonBar::addSeparator()
{
    return d_ptr->mButtonGroup->addSeparator();
}

QAction* SARibbonSystemButtonBar::addWidget(QWidget* w)
{
    return d_ptr->mButtonGroup->addWidget(w);
}

void SARibbonSystemButtonBar::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);
    d_ptr->resizeElement(size());
}

void SARibbonSystemButtonBar::closeWindow()
{
    if (parentWidget()) {
        parentWidget()->close();
    }
}

void SARibbonSystemButtonBar::minimizeWindow()
{
    if (parentWidget()) {
        parentWidget()->showMinimized();
    }
}

void SARibbonSystemButtonBar::maximizeWindow()
{
    QWidget* par = parentWidget();

    if (par) {
        if (par->isMaximized()) {
            par->showNormal();
        } else {
            par->showMaximized();
        }
    }
}
