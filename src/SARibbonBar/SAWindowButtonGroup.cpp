#include "SAWindowButtonGroup.h"
#include <QToolButton>
#include <QResizeEvent>
#include <QStyle>
#include <QDebug>
#include <QScopedPointer>
#include "SARibbonMainWindow.h"
#include "SARibbonBar.h"
#include "SARibbonElementManager.h"
#include "SARibbonButtonGroupWidget.h"

// 为了避免使用此框架的app设置了全局的qpushbutton 的 qss样式影响此按钮，定义了一个类

/**
 * @brief The SAWindowButtonGroupPrivate class
 */
class SAWindowButtonGroup::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SAWindowButtonGroup)
public:
    SAWindowToolButton* buttonClose { nullptr };
    SAWindowToolButton* buttonMinimize { nullptr };
    SAWindowToolButton* buttonMaximize { nullptr };
    int mCloseStretch { 4 };
    int mMaxStretch { 3 };
    int mMinStretch { 3 };
    int mWindowButtonWidth { 35 };
    int mTitleBarHeight { 28 };
    Qt::WindowFlags mFlags { Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint };
    SARibbonButtonGroupWidget* mButtonGroup;

public:
    PrivateData(SAWindowButtonGroup* p) : q_ptr(p)
    {
        mButtonGroup = new SARibbonButtonGroupWidget(p);
        mButtonGroup->setObjectName("SAWindowButtonGroupToolBar");
    }

    void setupMinimizeButton(bool on)
    {
        SAWindowButtonGroup* par = q_ptr;

        if (on) {
            if (buttonMinimize) {
                buttonMinimize->deleteLater();
                buttonMinimize = nullptr;
            }
            buttonMinimize = new SAWindowToolButton(par);
            buttonMinimize->setObjectName(QStringLiteral("SAMinimizeWindowButton"));
            buttonMinimize->setFocusPolicy(Qt::NoFocus);  // 避免铺抓到
            buttonMinimize->show();
            par->connect(buttonMinimize, &QAbstractButton::clicked, par, &SAWindowButtonGroup::minimizeWindow);
        } else {
            if (buttonMinimize) {
                delete buttonMinimize;
                buttonMinimize = nullptr;
            }
        }
        updateSize();
    }

    void setupMaximizeButton(bool on)
    {
        SAWindowButtonGroup* par = q_ptr;

        if (on) {
            if (buttonMaximize) {
                buttonMaximize->deleteLater();
                buttonMaximize = nullptr;
            }
            buttonMaximize = new SAWindowToolButton(par);
            buttonMaximize->setObjectName(QStringLiteral("SAMaximizeWindowButton"));
            buttonMaximize->setCheckable(true);
            buttonMaximize->setFocusPolicy(Qt::NoFocus);  // 避免铺抓到
            //            buttonMaximize->setIconSize(buttonMaximize->size() * mIconscale);
            buttonMaximize->show();
            par->connect(buttonMaximize, &QAbstractButton::clicked, par, &SAWindowButtonGroup::maximizeWindow);
        } else {
            if (buttonMaximize) {
                delete buttonMaximize;
                buttonMaximize = nullptr;
            }
        }
        updateSize();
    }

    void setupCloseButton(bool on)
    {
        SAWindowButtonGroup* par = q_ptr;

        if (on) {
            if (buttonClose) {
                buttonClose->deleteLater();
                buttonClose = nullptr;
            }
            buttonClose = new SAWindowToolButton(par);
            buttonClose->setObjectName(QStringLiteral("SACloseWindowButton"));
            buttonClose->setFocusPolicy(Qt::NoFocus);  // 避免铺抓到
            // buttonClose->setFlat(true);
            par->connect(buttonClose, &QAbstractButton::clicked, par, &SAWindowButtonGroup::closeWindow);
            //            buttonClose->setIconSize(buttonClose->size() * mIconscale);
            buttonClose->show();
        } else {
            if (buttonClose) {
                delete buttonClose;
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
// SAWindowToolButton
//===================================================
SAWindowToolButton::SAWindowToolButton(QWidget* p) : QToolButton(p)
{
    setAutoRaise(true);
}
//===================================================
// SAWindowButtonGroup
//===================================================
SAWindowButtonGroup::SAWindowButtonGroup(QWidget* parent)
    : QFrame(parent), d_ptr(new SAWindowButtonGroup::PrivateData(this))
{
    updateWindowFlag();
}

/**
 * @brief 构造函数，强制使用flags，而不是用parent的flags进行构造
 * @param parent
 * @param flags
 */
SAWindowButtonGroup::SAWindowButtonGroup(QWidget* parent, Qt::WindowFlags flags)
    : QFrame(parent), d_ptr(new SAWindowButtonGroup::PrivateData(this))
{
    d_ptr->mFlags = flags;
    updateWindowFlag();
}

SAWindowButtonGroup::~SAWindowButtonGroup()
{
}

void SAWindowButtonGroup::setupMinimizeButton(bool on)
{
    d_ptr->setupMinimizeButton(on);
}

void SAWindowButtonGroup::setupMaximizeButton(bool on)
{
    d_ptr->setupMaximizeButton(on);
}

void SAWindowButtonGroup::setupCloseButton(bool on)
{
    d_ptr->setupCloseButton(on);
}

void SAWindowButtonGroup::updateWindowFlag()
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
void SAWindowButtonGroup::updateWindowFlag(Qt::WindowFlags flags)
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
void SAWindowButtonGroup::setButtonWidthStretch(int close, int max, int min)
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
void SAWindowButtonGroup::setWindowTitleHeight(int h)
{
    d_ptr->mTitleBarHeight = h;
}

/**
 * @brief 标题栏高度
 * @return
 */
int SAWindowButtonGroup::windowTitleHeight() const
{
    return d_ptr->mTitleBarHeight;
}

/**
 * @brief 系统按钮的宽度
 * @param w
 */
void SAWindowButtonGroup::setWindowButtonWidth(int w)
{
    d_ptr->mWindowButtonWidth = w;
}

/**
 * @brief 系统按钮的宽度
 * @param w
 */
int SAWindowButtonGroup::windowButtonWidth() const
{
    return d_ptr->mWindowButtonWidth;
}

/**
 * @brief 设置窗口状态（最大最小化按钮状态）
 * @param s
 */
void SAWindowButtonGroup::setWindowStates(Qt::WindowStates s)
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
Qt::WindowFlags SAWindowButtonGroup::windowButtonFlags() const
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

QSize SAWindowButtonGroup::sizeHint() const
{
    return (d_ptr->sizeHint());
}

QAbstractButton* SAWindowButtonGroup::minimizeButton() const
{
    return d_ptr->buttonMinimize;
}

QAbstractButton* SAWindowButtonGroup::maximizeButton() const
{
    return d_ptr->buttonMaximize;
}

QAbstractButton* SAWindowButtonGroup::closeButton() const
{
    return d_ptr->buttonClose;
}

void SAWindowButtonGroup::setIconSize(const QSize& ic)
{
    d_ptr->mButtonGroup->setIconSize(ic);
}

QSize SAWindowButtonGroup::iconSize() const
{
    return d_ptr->mButtonGroup->iconSize();
}

QAction* SAWindowButtonGroup::addAction(QAction* a, Qt::ToolButtonStyle buttonStyle, QToolButton::ToolButtonPopupMode popMode)
{
    return d_ptr->mButtonGroup->addAction(a, buttonStyle, popMode);
}

QAction* SAWindowButtonGroup::addAction(const QString& text, const QIcon& icon, Qt::ToolButtonStyle buttonStyle, QToolButton::ToolButtonPopupMode popMode)
{
    return d_ptr->mButtonGroup->addAction(text, icon, buttonStyle, popMode);
}

QAction* SAWindowButtonGroup::addMenu(QMenu* menu, Qt::ToolButtonStyle buttonStyle, QToolButton::ToolButtonPopupMode popMode)
{
    return d_ptr->mButtonGroup->addMenu(menu, buttonStyle, popMode);
}

QAction* SAWindowButtonGroup::addSeparator()
{
    return d_ptr->mButtonGroup->addSeparator();
}

QAction* SAWindowButtonGroup::addWidget(QWidget* w)
{
    return d_ptr->mButtonGroup->addWidget(w);
}

void SAWindowButtonGroup::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);
    d_ptr->resizeElement(size());
}

void SAWindowButtonGroup::closeWindow()
{
    if (parentWidget()) {
        parentWidget()->close();
    }
}

void SAWindowButtonGroup::minimizeWindow()
{
    if (parentWidget()) {
        parentWidget()->showMinimized();
    }
}

void SAWindowButtonGroup::maximizeWindow()
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
