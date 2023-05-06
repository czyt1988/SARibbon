#include "SAWindowButtonGroup.h"
#include <QToolButton>
#include <QResizeEvent>
#include <QStyle>
#include <QDebug>
#include <QScopedPointer>
#include "SARibbonElementManager.h"
//为了避免使用此框架的app设置了全局的qpushbutton 的 qss样式影响此按钮，定义了一个类

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
    qreal mIconscale { 0.5 };
    Qt::WindowFlags mFlags;
    PrivateData(SAWindowButtonGroup* p) : q_ptr(p), mFlags(Qt::WindowFlags())
    {
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
            buttonMinimize->setFixedSize(30, RibbonSubElementStyleOpt.titleBarHeight() - 2);
            buttonMinimize->setFocusPolicy(Qt::NoFocus);  //避免铺抓到
            buttonMinimize->setIconSize(buttonMinimize->size() * mIconscale);
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
            buttonMaximize->setFixedSize(30, RibbonSubElementStyleOpt.titleBarHeight() - 2);
            buttonMaximize->setCheckable(true);
            buttonMaximize->setFocusPolicy(Qt::NoFocus);  //避免铺抓到
            buttonMaximize->setIconSize(buttonMaximize->size() * mIconscale);
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
            buttonClose->setFixedSize(40, RibbonSubElementStyleOpt.titleBarHeight() - 2);
            buttonClose->setFocusPolicy(Qt::NoFocus);  //避免铺抓到
            // buttonClose->setFlat(true);
            par->connect(buttonClose, &QAbstractButton::clicked, par, &SAWindowButtonGroup::closeWindow);
            buttonClose->setIconSize(buttonClose->size() * mIconscale);
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
        q_ptr->setFixedSize(sizeHint());
        resize(q_ptr->size());
        //        int span = 0;

        //        if (buttonClose) {
        //            buttonClose->move(q_d->width()-buttonClose->width()
        //                , q_d->y());
        //            span = buttonClose->width();
        //        }
        //        if (buttonMaximize) {
        //            buttonMaximize->move(q_d->width()-buttonMaximize->width()-span
        //                , q_d->y());
        //            span += buttonMaximize->width();
        //        }
        //        if (buttonMinimize) {
        //            buttonMinimize->move(q_d->width()-buttonMinimize->width()-span
        //                , q_d->y());
        //        }
    }

    void resize(QSize size)
    {
        qreal tw = 0;

        if (buttonClose) {
            tw += mCloseStretch;
        }
        if (buttonMaximize) {
            tw += mMaxStretch;
        }
        if (buttonMinimize) {
            tw += mMinStretch;
        }
        //调整按钮

        int x = 0;

        if (buttonMinimize) {
            int w = (mMinStretch / tw) * size.width();
            // buttonMinimize->setGeometry(x, 0, w, size.height());
            //受到qss样式影响会导致最小宽度、高度设置不了，因此要设置fixsize再move
            buttonMinimize->setFixedSize(w, size.height());
            buttonMinimize->move(x, 0);
            x += w;
        }
        if (buttonMaximize) {
            int w = (mMaxStretch / tw) * size.width();
            // buttonMaximize->setGeometry(x, 0, w, size.height());
            //受到qss样式影响会导致最小宽度、高度设置不了，因此要设置fixsize再move
            buttonMaximize->setFixedSize(w, size.height());
            buttonMaximize->move(x, 0);
            x += w;
        }
        if (buttonClose) {
            int w = (mCloseStretch / tw) * size.width();
            // buttonClose->setGeometry(x, 0, w, size.height());
            //受到qss样式影响会导致最小宽度、高度设置不了，因此要设置fixsize再move
            buttonClose->setFixedSize(w, size.height());
            buttonClose->move(x, 0);
        }
    }

    QSize sizeHint() const
    {
        int width  = 0;
        int height = RibbonSubElementStyleOpt.titleBarHeight();

        if (buttonClose) {
            width += 40;
        }
        if (buttonMaximize) {
            width += 30;
        }
        if (buttonMinimize) {
            width += 30;
        }
        return (QSize(width, height));
    }
};

//===================================================
// SAWindowToolButton
//===================================================
SAWindowToolButton::SAWindowToolButton(QWidget* p) : QPushButton(p)
{
    setFlat(true);
}

SAWindowButtonGroup::SAWindowButtonGroup(QWidget* parent)
    : QWidget(parent), d_ptr(new SAWindowButtonGroup::PrivateData(this))
{
    updateWindowFlag();
    if (parent) {
        parent->installEventFilter(this);
    }
}

/**
 * @brief 构造函数，强制使用flags，而不是用parent的flags进行构造
 * @param parent
 * @param flags
 */
SAWindowButtonGroup::SAWindowButtonGroup(QWidget* parent, Qt::WindowFlags flags)
    : QWidget(parent), d_ptr(new SAWindowButtonGroup::PrivateData(this))
{
    d_ptr->mFlags = flags;
    updateWindowFlag();
    if (parent) {
        parent->installEventFilter(this);
    }
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
    Qt::WindowFlags flags = parentWidget()->windowFlags();

    d_ptr->mFlags = flags;

    setupMinimizeButton(flags & Qt::WindowMinimizeButtonHint);

    setupMaximizeButton(flags & Qt::WindowMaximizeButtonHint);

    setupCloseButton(flags & Qt::WindowCloseButtonHint);
}

/**
 * @brief 此函数仅用于控制最小最大化和关闭按钮的显示
 * @param flags
 */
void SAWindowButtonGroup::updateWindowFlag(Qt::WindowFlags flags)
{
    if (flags & Qt::WindowCloseButtonHint) {
        d_ptr->mFlags |= Qt::WindowCloseButtonHint;
    } else {
        d_ptr->mFlags &= (~Qt::WindowCloseButtonHint);
    }

    if (flags & Qt::WindowMaximizeButtonHint) {
        d_ptr->mFlags |= Qt::WindowMaximizeButtonHint;
    } else {
        d_ptr->mFlags &= (~Qt::WindowMaximizeButtonHint);
    }

    if (flags & Qt::WindowMinimizeButtonHint) {
        d_ptr->mFlags |= Qt::WindowMinimizeButtonHint;
    } else {
        d_ptr->mFlags &= (~Qt::WindowMinimizeButtonHint);
    }
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
 * @brief 设置按钮的缩放比例
 * @param iconscale 数值越小按钮越小
 */
void SAWindowButtonGroup::setIconScale(qreal iconscale)
{
    d_ptr->mIconscale = iconscale;
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

bool SAWindowButtonGroup::eventFilter(QObject* watched, QEvent* e)
{
    //用于监听父窗口改变尺寸
    if (watched == parentWidget()) {
        switch (e->type()) {
        case QEvent::Resize:
            parentResize();
            break;

        default:
            break;
        }
    }
    return (false);  //不截断任何事件
}

void SAWindowButtonGroup::parentResize()
{
    QWidget* par = parentWidget();

    if (par) {
        QSize parSize = par->size();
        move(parSize.width() - width() - 1, 0);
    }
}

void SAWindowButtonGroup::resizeEvent(QResizeEvent* e)
{
    d_ptr->resize(e->size());
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
