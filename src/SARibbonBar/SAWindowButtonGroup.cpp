#include "SAWindowButtonGroup.h"
#include <QToolButton>
#include <QResizeEvent>
#include <QStyle>
#include <QDebug>

//为了避免使用此框架的app设置了全局的qpushbutton 的 qss样式影响此按钮，定义了一个类


class SAWindowButtonGroupPrivate
{
public:

    SAWindowToolButton *buttonClose;
    SAWindowToolButton *buttonMinimize;
    SAWindowToolButton *buttonMaximize;
    SAWindowButtonGroupPrivate()
        : buttonClose(nullptr)
        , buttonMinimize(nullptr)
        , buttonMaximize(nullptr)
    {
    }


    void setupMinimizeButton(SAWindowButtonGroup *par, bool on, qreal iconscale = 0.5)
    {
        if (on) {
            if (!buttonMinimize) {
                buttonMinimize = new SAWindowToolButton(par);
                buttonMinimize->setObjectName(QStringLiteral("SAMinimizeWindowButton"));
                buttonMinimize->setFixedSize(30, 28);
                buttonMinimize->setFocusPolicy(Qt::NoFocus);//避免铺抓到
                buttonMinimize->setStyleSheet(QString(""
                    "SAWindowToolButton"
                    "{ "
                    "    background-color: transparent; "
                    "    border:none;"
                    "}"
                    "SAWindowToolButton:hover {background-color:#C1D1B8;}"
                    "SAWindowToolButton:pressed {background-color:#A5AF9B;}"
                    "SAWindowToolButton:focus{outline: none}"));
                QIcon icon = par->style()->standardIcon(QStyle::SP_TitleBarMinButton);
                buttonMinimize->setIconSize(buttonMinimize->size()*iconscale);
                buttonMinimize->setIcon(icon);
                par->connect(buttonMinimize, &QAbstractButton::clicked
                    , par, &SAWindowButtonGroup::minimizeWindow);
            }
        }else {
            if (buttonMinimize) {
                delete buttonMinimize;
            }
        }
        updateSize(par);
    }


    void setupMaximizeButton(SAWindowButtonGroup *par, bool on, qreal iconscale = 0.5)
    {
        if (on) {
            if (!buttonMaximize) {
                buttonMaximize = new SAWindowToolButton(par);
                buttonMaximize->setObjectName(QStringLiteral("SAMaximizeWindowButton"));
                buttonMaximize->setFixedSize(30, 28);
                buttonMaximize->setFocusPolicy(Qt::NoFocus);//避免铺抓到
                buttonMaximize->setStyleSheet(QString(""
                    "SAWindowToolButton"
                    "{ "
                    "    background-color: transparent; "
                    "    border:none;"
                    "}\n"
                    "SAWindowToolButton:hover {background-color:#C1D1B8;}"
                    "SAWindowToolButton:pressed {background-color:#A5AF9B;}"
                    "SAWindowToolButton:focus{outline: none;}"));
                QIcon icon = par->style()->standardIcon(QStyle::SP_TitleBarMaxButton);
                buttonMaximize->setIconSize(buttonMaximize->size()*iconscale);
                buttonMaximize->setIcon(icon);
                par->connect(buttonMaximize, &QAbstractButton::clicked
                    , par, &SAWindowButtonGroup::maximizeWindow);
            }
        }else {
            if (buttonMaximize) {
                delete buttonMaximize;
            }
        }
        updateSize(par);
    }


    void setupCloseButton(SAWindowButtonGroup *par, bool on, qreal iconscale = 0.5)
    {
        if (on) {
            if (!buttonClose) {
                buttonClose = new SAWindowToolButton(par);
                buttonClose->setObjectName(QStringLiteral("SACloseWindowButton"));
                buttonClose->setFixedSize(40, 28);
                buttonClose->setFocusPolicy(Qt::NoFocus);//避免铺抓到
                //buttonClose->setFlat(true);
                par->connect(buttonClose, &QAbstractButton::clicked
                    , par, &SAWindowButtonGroup::closeWindow);
                buttonClose->setStyleSheet(QString(""
                    "SAWindowToolButton"
                    "{ "
                    "    background-color: transparent; "
                    "    border:none;"
                    "}"
                    "SAWindowToolButton:hover {background-color:#F0604D;}"
                    "SAWindowToolButton:pressed {background-color:#F0604D;}"
                    "SAWindowToolButton:focus{outline: none;}"));
                QIcon icon = par->style()->standardIcon(QStyle::SP_TitleBarCloseButton);
                buttonClose->setIconSize(buttonClose->size()*iconscale);
                buttonClose->setIcon(icon);
            }
        }else {
            if (buttonClose) {
                delete buttonClose;
            }
        }
        updateSize(par);
    }


    void updateSize(SAWindowButtonGroup *par)
    {
        par->setFixedSize(sizeHint());
        int span = 0;

        if (buttonClose) {
            buttonClose->move(par->width()-buttonClose->width()
                , par->y());
            span = buttonClose->width();
        }
        if (buttonMaximize) {
            buttonMaximize->move(par->width()-buttonMaximize->width()-span
                , par->y());
            span += buttonMaximize->width();
        }
        if (buttonMinimize) {
            buttonMinimize->move(par->width()-buttonMinimize->width()-span
                , par->y());
        }
    }


    QSize sizeHint() const
    {
        int width = 0;
        int height = 0;

        if (buttonClose) {
            width += buttonClose->width();
            height = qMax(height, buttonClose->height());
        }
        if (buttonMaximize) {
            width += buttonMaximize->width();
            height = qMax(height, buttonMaximize->height());
        }
        if (buttonMinimize) {
            width += buttonMinimize->width();
            height = qMax(height, buttonMinimize->height());
        }
        return (QSize(width, height));
    }
};

SAWindowToolButton::SAWindowToolButton(QWidget *p):QToolButton(p)
{
    setAutoRaise(true);
}

SAWindowButtonGroup::SAWindowButtonGroup(QWidget *parent, qreal iconscale) : QWidget(parent)
    , m_d(new SAWindowButtonGroupPrivate())
{
    updateWindowFlag(iconscale);
    if (parent) {
        parent->installEventFilter(this);
    }
}


SAWindowButtonGroup::~SAWindowButtonGroup()
{
    delete m_d;
}


void SAWindowButtonGroup::setupMinimizeButton(bool on, qreal iconscale)
{
    m_d->setupMinimizeButton(this, on, iconscale);
}


void SAWindowButtonGroup::setupMaximizeButton(bool on, qreal iconscale)
{
    m_d->setupMaximizeButton(this, on, iconscale);
    updateMaximizeButtonIcon();
}


void SAWindowButtonGroup::setupCloseButton(bool on, qreal iconscale)
{
    m_d->setupCloseButton(this, on, iconscale);
}


void SAWindowButtonGroup::updateWindowFlag(qreal iconscale)
{
    Qt::WindowFlags flags = parentWidget()->windowFlags();

    setupMinimizeButton(flags & Qt::WindowMinimizeButtonHint, iconscale);

    setupMaximizeButton(flags & Qt::WindowMaximizeButtonHint, iconscale);

    setupCloseButton(flags & Qt::WindowCloseButtonHint, iconscale);
}


QSize SAWindowButtonGroup::sizeHint() const
{
    return (m_d->sizeHint());
}


bool SAWindowButtonGroup::eventFilter(QObject *watched, QEvent *e)
{
    if (watched == parentWidget()) {
        switch (e->type())
        {
        case QEvent::Resize:
            parentResize();
            break;

        default:
            break;
        }
    }
    return (false);//不截断任何事件
}


void SAWindowButtonGroup::parentResize()
{
    QWidget *par = parentWidget();

    if (par) {
        QSize parSize = par->size();
        move(parSize.width() - width()-1, 1);
    }
}


void SAWindowButtonGroup::updateMaximizeButtonIcon()
{
    QWidget *par = parentWidget();

    if (par) {
        if (par->isMaximized()) {
            if (m_d->buttonMaximize) {
                m_d->buttonMaximize->setIcon(style()->standardIcon(QStyle::SP_TitleBarNormalButton));
            }
        }else {
            if (m_d->buttonMaximize) {
                m_d->buttonMaximize->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
            }
        }
    }
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
    QWidget *par = parentWidget();

    if (par) {
        if (par->isMaximized()) {
            par->showNormal();
        }else {
            par->showMaximized();
        }
        updateMaximizeButtonIcon();
    }
}




