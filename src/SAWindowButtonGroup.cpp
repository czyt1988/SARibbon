#include "SAWindowButtonGroup.h"
#include <QPushButton>
#include <QResizeEvent>
#include <QStyle>
#include <QDebug>
class SAWindowButtonGroupPrivate
{
public:

    QPushButton* buttonClose;
    QPushButton* buttonMinimize;
    QPushButton* buttonMaximize;
    SAWindowButtonGroupPrivate()
        :buttonClose(nullptr)
        ,buttonMinimize(nullptr)
        ,buttonMaximize(nullptr)
    {

    }

    void setupMinimizeButton(SAWindowButtonGroup* par,bool on)
    {
        if(on)
        {
            if(!buttonMinimize)
            {
                buttonMinimize = new QPushButton(par);
                buttonMinimize->setObjectName(QStringLiteral("SAMinimizeWindowButton"));
                buttonMinimize->setFixedSize(30,30);
                buttonMinimize->setStyleSheet(QString("QPushButton "
                                                   "{ "
                                                   "    background-color: #E3E3E5; "
                                                   "    border:none;"
                                                   "}"
                                                   "QPushButton:hover {background-color:#C1D1B8}"
                                                   "QPushButton:pressed {background-color:#A5AF9B}"
                                                   "QPushButton:focus{outline: none;}"));
                QIcon icon = par->style()->standardIcon(QStyle::SP_TitleBarMinButton);
                buttonMinimize->setIcon(icon);
                par->connect(buttonMinimize,&QAbstractButton::clicked
                             ,par,&SAWindowButtonGroup::minimizeWindow);
            }
        }
        else
        {
            if(buttonMinimize)
            {
                delete buttonMinimize;
            }
        }
        updateSize(par);
    }
    void setupMaximizeButton(SAWindowButtonGroup* par,bool on)
    {
        if(on)
        {
            if(!buttonMaximize)
            {
                buttonMaximize = new QPushButton(par);
                buttonMaximize->setObjectName(QStringLiteral("SAMaximizeWindowButton"));
                buttonMaximize->setFixedSize(30,30);
                buttonMaximize->setStyleSheet(QString("QPushButton "
                                                   "{ "
                                                   "    background-color: #E3E3E5; "
                                                   "    border:none;"
                                                   "}"
                                                   "QPushButton:hover {background-color:#C1D1B8}"
                                                   "QPushButton:pressed {background-color:#A5AF9B}"
                                                   "QPushButton:focus{outline: none;}"));
                QIcon icon = par->style()->standardIcon(QStyle::SP_TitleBarMaxButton);
                buttonMaximize->setIcon(icon);
                par->connect(buttonMaximize,&QAbstractButton::clicked
                             ,par,&SAWindowButtonGroup::maximizeWindow);
            }
        }
        else
        {
            if(buttonMaximize)
            {
                delete buttonMaximize;
            }
        }
        updateSize(par);
    }
    void setupCloseButton(SAWindowButtonGroup* par,bool on)
    {
        if(on)
        {
            if(!buttonClose)
            {
                buttonClose = new QPushButton(par);
                buttonClose->setObjectName(QStringLiteral("SACloseWindowButton"));
                buttonClose->setFixedSize(40,30);
                //buttonClose->setFlat(true);
                par->connect(buttonClose,&QAbstractButton::clicked
                             ,par,&SAWindowButtonGroup::closeWindow);
                buttonClose->setStyleSheet(QString("QPushButton "
                                                   "{ "
                                                   "    background-color: #E3E3E5; "
                                                   "    border:none;"
                                                   "}"
                                                   "QPushButton:hover {background-color:#F0604D}"
                                                   "QPushButton:pressed {background-color:#F0604D}"
                                                   "QPushButton:focus{outline: none;}"));
                QIcon icon = par->style()->standardIcon(QStyle::SP_TitleBarCloseButton);
                buttonClose->setIcon(icon);
            }
        }
        else
        {
            if(buttonClose)
            {
                delete buttonClose;
            }
        }
        updateSize(par);
    }

    void updateSize(SAWindowButtonGroup* par)
    {
        int span = 0;
        if(buttonClose)
        {
            buttonClose->move(par->width()-buttonClose->width()
                              ,par->y());
            span = buttonClose->width();
        }
        if(buttonMaximize)
        {
            buttonMaximize->move(par->width()-buttonMaximize->width()-span
                              ,par->y());
            span += buttonMaximize->width();
        }
        if(buttonMinimize)
        {
            buttonMinimize->move(par->width()-buttonMinimize->width()-span
                              ,par->y());
        }
    }

    QSize sizeHint()
    {
        int width = 0;
        int height = 0;
        if(buttonClose)
        {
            width += buttonClose->width();
            height = qMax(height,buttonClose->height());
        }
        if(buttonMaximize)
        {
            width += buttonMaximize->width();
            height = qMax(height,buttonMaximize->height());
        }
        if(buttonMinimize)
        {
            width += buttonMinimize->width();
            height = qMax(height,buttonMinimize->height());
        }
        return QSize(width,height);
    }

};

SAWindowButtonGroup::SAWindowButtonGroup(QWidget *parent):QWidget(parent)
  ,m_d(new SAWindowButtonGroupPrivate)
{
    updateWindowFlag();
    if(parent)
        parent->installEventFilter(this);
}

SAWindowButtonGroup::~SAWindowButtonGroup()
{
    delete m_d;
}

void SAWindowButtonGroup::setupMinimizeButton(bool on)
{
    m_d->setupMinimizeButton(this,on);
}

void SAWindowButtonGroup::setupMaximizeButton(bool on)
{
    m_d->setupMaximizeButton(this,on);
    updateMaximizeButtonIcon();
}

void SAWindowButtonGroup::setupCloseButton(bool on)
{
    m_d->setupCloseButton(this,on);
}

void SAWindowButtonGroup::updateWindowFlag()
{
    Qt::WindowFlags flags = parentWidget()->windowFlags();

    setupMinimizeButton(flags & Qt::WindowMinimizeButtonHint);

    setupMaximizeButton(flags & Qt::WindowMaximizeButtonHint);

    setupCloseButton(flags & Qt::WindowCloseButtonHint);

}


QSize SAWindowButtonGroup::sizeHint()
{
    return m_d->sizeHint();
}

bool SAWindowButtonGroup::eventFilter(QObject *watched, QEvent *e)
{
    if(watched == parentWidget())
    {
        switch(e->type())
        {
        case QEvent::Resize:
            parentResize();
            break;
        default:
            break;
        }
    }
    return false;//不截断任何事件
}

void SAWindowButtonGroup::parentResize()
{
    QWidget* par = parentWidget();
    if(par)
    {
        QSize parSize = par->size();
        move(parSize.width() - width()-1,1);
    }
}

void SAWindowButtonGroup::updateMaximizeButtonIcon()
{
    QWidget* par = parentWidget();
    if(par)
    {
        if(par->isMaximized())
        {
            if(m_d->buttonMaximize)
            {
                m_d->buttonMaximize->setIcon(style()->standardIcon(QStyle::SP_TitleBarNormalButton));
            }
        }
        else
        {
            if(m_d->buttonMaximize)
            {
                m_d->buttonMaximize->setIcon(style()->standardIcon(QStyle::SP_TitleBarMaxButton));
            }
        }
    }
}

void SAWindowButtonGroup::closeWindow()
{
    if(parentWidget())
        parentWidget()->close();
}

void SAWindowButtonGroup::minimizeWindow()
{
    if(parentWidget())
        parentWidget()->showMinimized();
}

void SAWindowButtonGroup::maximizeWindow()
{
    QWidget* par = parentWidget();
    if(par)
    {
        if(par->isMaximized())
        {
            par->showNormal();
        }
        else
        {
            par->showMaximized();
        }
        updateMaximizeButtonIcon();
    }
}
