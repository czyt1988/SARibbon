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
#include "SARibbonUtil.h"

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

    /**
     * \if ENGLISH
     * @brief Resizes and positions all elements in the button bar
     * @param size New size of the button bar
     * @details Handles both LTR and RTL layouts: in LTR, system buttons are positioned at the right edge;
     *          in RTL, system buttons are positioned at the left edge while maintaining the same order (min → max → close)
     * \endif
     *
     * \if CHINESE
     * @brief 调整按钮栏中所有元素的大小和位置
     * @param size 按钮栏的新尺寸
     * @details 同时处理LTR和RTL布局：在LTR模式下，系统按钮位于右边缘；
     *          在RTL模式下，系统按钮位于左边缘，同时保持相同的顺序（最小化→最大化→关闭）
     * \endif
     */
    void resizeElement(QSize size)
    {
        if (SA::saIsRTL()) {
            int x = 0;
            if (buttonMinimize) {
                int w = minButtonWidthHint();
                buttonMinimize->setGeometry(x, 0, w, size.height());
                x += w;
            }
            if (buttonMaximize) {
                int w = maxButtonWidthHint();
                buttonMaximize->setGeometry(x, 0, w, size.height());
                x += w;
            }
            if (buttonClose) {
                int w = closeButtonWidthHint();
                buttonClose->setGeometry(x, 0, w, size.height());
                x += w;
            }
            if (mButtonGroup) {
                mButtonGroup->setGeometry(x, 0, size.width() - x, size.height());
            }
        } else {
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
/**
 * \if ENGLISH
 * @brief Constructs a SARibbonSystemToolButton instance
 * @param p Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief 构造一个 SARibbonSystemToolButton 实例
 * @param p 父窗口部件
 * \endif
 */
SARibbonSystemToolButton::SARibbonSystemToolButton(QWidget* p) : QToolButton(p)
{
    setAutoRaise(true);
}
//===================================================
// SARibbonSystemButtonBar
//===================================================
/**
 * \if ENGLISH
 * @brief Constructs a SARibbonSystemButtonBar instance
 * @param parent Parent widget
 * \endif
 *
 * \if CHINESE
 * @brief 构造一个 SARibbonSystemButtonBar 实例
 * @param parent 父窗口部件
 * \endif
 */
SARibbonSystemButtonBar::SARibbonSystemButtonBar(QWidget* parent)
    : QFrame(parent), d_ptr(new SARibbonSystemButtonBar::PrivateData(this))
{
    updateWindowFlag();
}

/**
 * \if ENGLISH
 * @brief Constructor that forces the use of flags instead of the parent's flags
 * @param parent Parent widget
 * @param flags Window flags
 * \endif
 *
 * \if CHINESE
 * @brief 构造函数，强制使用flags，而不是用parent的flags进行构造
 * @param parent 父窗口部件
 * @param flags 窗口标志
 * \endif
 */
SARibbonSystemButtonBar::SARibbonSystemButtonBar(QWidget* parent, Qt::WindowFlags flags)
    : QFrame(parent), d_ptr(new SARibbonSystemButtonBar::PrivateData(this))
{
    d_ptr->mFlags = flags;
    updateWindowFlag();
}

/**
 * \if ENGLISH
 * @brief Destructor
 * \endif
 *
 * \if CHINESE
 * @brief 析构函数
 * \endif
 */
SARibbonSystemButtonBar::~SARibbonSystemButtonBar()
{
}

/**
 * \if ENGLISH
 * @brief Sets up the minimize button
 * @param on Whether to show the minimize button
 * \endif
 *
 * \if CHINESE
 * @brief 设置最小化按钮
 * @param on 是否显示最小化按钮
 * \endif
 */
void SARibbonSystemButtonBar::setupMinimizeButton(bool on)
{
    d_ptr->setupMinimizeButton(on);
}

/**
 * \if ENGLISH
 * @brief Sets up the maximize button
 * @param on Whether to show the maximize button
 * \endif
 *
 * \if CHINESE
 * @brief 设置最大化按钮
 * @param on 是否显示最大化按钮
 * \endif
 */
void SARibbonSystemButtonBar::setupMaximizeButton(bool on)
{
    d_ptr->setupMaximizeButton(on);
}

/**
 * \if ENGLISH
 * @brief Sets up the close button
 * @param on Whether to show the close button
 * \endif
 *
 * \if CHINESE
 * @brief 设置关闭按钮
 * @param on 是否显示关闭按钮
 * \endif
 */
void SARibbonSystemButtonBar::setupCloseButton(bool on)
{
    d_ptr->setupCloseButton(on);
}

/**
 * \if ENGLISH
 * @brief Updates the window flag based on the top-level window's flags
 * \endif
 *
 * \if CHINESE
 * @brief 根据顶层窗口的标志更新窗口标志
 * \endif
 */
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
 * \if ENGLISH
 * @brief This function is only used to control the display of minimize, maximize, and close buttons
 * @param flags Window flags
 * \endif
 *
 * \if CHINESE
 * @brief 此函数仅用于控制最小最大化和关闭按钮的显示
 * @param flags 窗口标志
 * \endif
 */
void SARibbonSystemButtonBar::updateWindowFlag(Qt::WindowFlags flags)
{
    d_ptr->mFlags = flags;
    setupMinimizeButton(flags & Qt::WindowMinimizeButtonHint);
    setupMaximizeButton(flags & Qt::WindowMaximizeButtonHint);
    setupCloseButton(flags & Qt::WindowCloseButtonHint);
}

/**
 * \if ENGLISH
 * @brief Sets the width ratio of the buttons, the final button width will be set according to this ratio
 * @param close Close button ratio
 * @param max Maximize button ratio
 * @param min Minimize button ratio
 * \endif
 *
 * \if CHINESE
 * @brief 设置按钮的宽度比例,最终按钮宽度将按照此比例进行设置
 * @param close 关闭按钮比例
 * @param max 最大化按钮比例
 * @param min 最小化按钮比例
 * \endif
 */
void SARibbonSystemButtonBar::setButtonWidthStretch(int close, int max, int min)
{
    d_ptr->mMaxStretch   = max;
    d_ptr->mMinStretch   = min;
    d_ptr->mCloseStretch = close;
}

/**
 * \if ENGLISH
 * @brief Sets the title bar height
 *
 * The title bar height affects the sizeHint
 * @param h Height
 * \endif
 *
 * \if CHINESE
 * @brief 标题栏高度
 *
 * 标题栏高度会影响sizeHint
 * @param h 高度
 * \endif
 */
void SARibbonSystemButtonBar::setWindowTitleHeight(int h)
{
    d_ptr->mTitleBarHeight = h;
}

/**
 * \if ENGLISH
 * @brief Gets the title bar height
 * @return Title bar height
 * \endif
 *
 * \if CHINESE
 * @brief 标题栏高度
 * @return 标题栏高度
 * \endif
 */
int SARibbonSystemButtonBar::windowTitleHeight() const
{
    return d_ptr->mTitleBarHeight;
}

/**
 * \if ENGLISH
 * @brief Sets the width of the system buttons
 * @param w Width
 * \endif
 *
 * \if CHINESE
 * @brief 系统按钮的宽度
 * @param w 宽度
 * \endif
 */
void SARibbonSystemButtonBar::setWindowButtonWidth(int w)
{
    d_ptr->mWindowButtonWidth = w;
}

/**
 * \if ENGLISH
 * @brief Gets the width of the system buttons
 * @return System button width
 * \endif
 *
 * \if CHINESE
 * @brief 系统按钮的宽度
 * @return 系统按钮宽度
 * \endif
 */
int SARibbonSystemButtonBar::windowButtonWidth() const
{
    return d_ptr->mWindowButtonWidth;
}

/**
 * \if ENGLISH
 * @brief Sets the window state (maximize/minimize button state)
 * @param s Window states
 * \endif
 *
 * \if CHINESE
 * @brief 设置窗口状态（最大最小化按钮状态）
 * @param s 窗口状态
 * \endif
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
 * \if ENGLISH
 * @brief This function returns flags that only include Qt::WindowCloseButtonHint, Qt::WindowMaximizeButtonHint, Qt::WindowMinimizeButtonHint
 * @return Window flags
 * \endif
 *
 * \if CHINESE
 * @brief 此函数返回的flags仅包括 Qt::WindowCloseButtonHint，Qt::WindowMaximizeButtonHint，Qt::WindowMinimizeButtonHint
 * 三个
 * @return 窗口标志
 * \endif
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

/**
 * \if ENGLISH
 * @brief Gets the recommended size for the widget
 * @return Recommended size
 * \endif
 *
 * \if CHINESE
 * @brief 获取部件的推荐尺寸
 * @return 推荐尺寸
 * \endif
 */
QSize SARibbonSystemButtonBar::sizeHint() const
{
    return (d_ptr->sizeHint());
}

/**
 * \if ENGLISH
 * @brief Event filter for handling events from SARibbonMainWindow
 * @param obj The object that is sending the event
 * @param event The event being sent
 * @return true if the event was handled, false otherwise
 * \endif
 *
 * \if CHINESE
 * @brief 用于处理来自SARibbonMainWindow的事件的事件过滤器
 * @param obj 发送事件的对象
 * @param event 正在发送的事件
 * @return 如果事件已处理则返回true，否则返回false
 * \endif
 */
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
            if (SA::saIsRTL()) {
                setGeometry(0, 0, wgSizeHint.width(), wgSizeHint.height());
            } else {
                setGeometry(fr.width() - wgSizeHint.width(), 0, wgSizeHint.width(), wgSizeHint.height());
            }
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

/**
 * \if ENGLISH
 * @brief Gets the minimize button
 * @return Pointer to the minimize button
 * \endif
 *
 * \if CHINESE
 * @brief 获取最小化按钮
 * @return 指向最小化按钮的指针
 * \endif
 */
QAbstractButton* SARibbonSystemButtonBar::minimizeButton() const
{
    return d_ptr->buttonMinimize;
}

/**
 * \if ENGLISH
 * @brief Gets the maximize button
 * @return Pointer to the maximize button
 * \endif
 *
 * \if CHINESE
 * @brief 获取最大化按钮
 * @return 指向最大化按钮的指针
 * \endif
 */
QAbstractButton* SARibbonSystemButtonBar::maximizeButton() const
{
    return d_ptr->buttonMaximize;
}

/**
 * \if ENGLISH
 * @brief Gets the close button
 * @return Pointer to the close button
 * \endif
 *
 * \if CHINESE
 * @brief 获取关闭按钮
 * @return 指向关闭按钮的指针
 * \endif
 */
QAbstractButton* SARibbonSystemButtonBar::closeButton() const
{
    return d_ptr->buttonClose;
}

/**
 * \if ENGLISH
 * @brief Sets the icon size
 * @param ic Icon size
 * \endif
 *
 * \if CHINESE
 * @brief 设置图标大小
 * @param ic 图标大小
 * \endif
 */
void SARibbonSystemButtonBar::setIconSize(const QSize& ic)
{
    d_ptr->mButtonGroup->setIconSize(ic);
}

/**
 * \if ENGLISH
 * @brief Gets the icon size
 * @return Icon size
 * \endif
 *
 * \if CHINESE
 * @brief 获取图标大小
 * @return 图标大小
 * \endif
 */
QSize SARibbonSystemButtonBar::iconSize() const
{
    return d_ptr->mButtonGroup->iconSize();
}

/**
 * \if ENGLISH
 * @brief Adds an action to the button group
 * @param a Action to add
 * \endif
 *
 * \if CHINESE
 * @brief 向按钮组添加一个动作
 * @param a 要添加的动作
 * \endif
 */
void SARibbonSystemButtonBar::addAction(QAction* a)
{
    d_ptr->mButtonGroup->addAction(a);
}

/**
 * \if ENGLISH
 * @brief Adds a menu action to the button group
 * @param menuAction Menu action to add
 * @param popupMode Popup mode
 * \endif
 *
 * \if CHINESE
 * @brief 向按钮组添加一个菜单动作
 * @param menuAction 要添加的菜单动作
 * @param popupMode 弹出模式
 * \endif
 */
void SARibbonSystemButtonBar::addMenuAction(QAction* menuAction, QToolButton::ToolButtonPopupMode popupMode)
{
    d_ptr->mButtonGroup->addMenuAction(menuAction, popupMode);
}

/**
 * \if ENGLISH
 * @brief Adds a menu to the button group
 * @param menu Menu to add
 * @param popupMode Popup mode
 * @return The added action
 * \endif
 *
 * \if CHINESE
 * @brief 向按钮组添加一个菜单
 * @param menu 要添加的菜单
 * @param popupMode 弹出模式
 * @return 添加的动作
 * \endif
 */
QAction* SARibbonSystemButtonBar::addMenuAction(QMenu* menu, QToolButton::ToolButtonPopupMode popupMode)
{
    return d_ptr->mButtonGroup->addMenuAction(menu, popupMode);
}

/**
 * \if ENGLISH
 * @brief Adds a separator to the button group
 * @return The added separator action
 * \endif
 *
 * \if CHINESE
 * @brief 向按钮组添加一个分隔符
 * @return 添加的分隔符动作
 * \endif
 */
QAction* SARibbonSystemButtonBar::addSeparator()
{
    return d_ptr->mButtonGroup->addSeparator();
}

/**
 * \if ENGLISH
 * @brief Adds a widget to the button group
 * @param w Widget to add
 * @return The added action
 * \endif
 *
 * \if CHINESE
 * @brief 向按钮组添加一个窗口部件
 * @param w 要添加的窗口部件
 * @return 添加的动作
 * \endif
 */
QAction* SARibbonSystemButtonBar::addWidget(QWidget* w)
{
    return d_ptr->mButtonGroup->addWidget(w);
}

/**
 * \if ENGLISH
 * @brief Handles resize events
 * @param e Resize event
 * \endif
 *
 * \if CHINESE
 * @brief 处理 resize 事件
 * @param e  resize 事件
 * \endif
 */
void SARibbonSystemButtonBar::resizeEvent(QResizeEvent* e)
{
    Q_UNUSED(e);
    d_ptr->resizeElement(size());
}

/**
 * \if ENGLISH
 * @brief Closes the window
 * \endif
 *
 * \if CHINESE
 * @brief 关闭窗口
 * \endif
 */
void SARibbonSystemButtonBar::closeWindow()
{
    if (parentWidget()) {
        parentWidget()->close();
    }
}

/**
 * \if ENGLISH
 * @brief Minimizes the window
 * \endif
 *
 * \if CHINESE
 * @brief 最小化窗口
 * \endif
 */
void SARibbonSystemButtonBar::minimizeWindow()
{
    if (parentWidget()) {
        parentWidget()->showMinimized();
    }
}

/**
 * \if ENGLISH
 * @brief Maximizes or restores the window
 * \endif
 *
 * \if CHINESE
 * @brief 最大化或还原窗口
 * \endif
 */
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
