#include "SARibbonPannel.h"
#include "SARibbonToolButton.h"
#include <QAction>
#include <QIcon>
#include <QDebug>
#include <QGridLayout>
#include <QFontMetrics>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>
#include "SARibbonPannelOptionButton.h"
#include "SARibbonSeparatorWidget.h"
#include "SARibbonGallery.h"
#include "SARibbonElementManager.h"
SARibbonPannel::SARibbonPannel(QWidget *parent):QWidget(parent)
  ,m_nextElementPosition(3,3)
  ,m_row(0)
  ,m_optionActionButton(nullptr)
  ,m_titleOptionButtonSpace(6)
  ,m_titleHeight(21)
  ,m_titleY(77)
  ,m_defaultReduceButton(nullptr)
{
    setFixedHeight(98);
    setMinimumWidth(50);
    m_gridLayout = new QGridLayout(this);
    m_gridLayout->setSpacing(0);
    m_gridLayout->setContentsMargins(3,2,3,21);
    setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Fixed);
    //setMouseTracking(true);
}

SARibbonToolButton *SARibbonPannel::addLargeAction(QAction *action)
{
    SARibbonToolButton* btn = RibbonSubElementDelegate->createRibbonToolButton(this);
    btn->setButtonType(SARibbonToolButton::LargeButton);
    btn->setAutoRaise(true);
    btn->setDefaultAction(action);
    QSize iconSize = maxHightIconSize(action->icon().actualSize(QSize(32,32)),32);
    btn->setIconSize(iconSize);
    if(action->menu())
        btn->setPopupMode(QToolButton::MenuButtonPopup);
    m_gridLayout->addWidget(btn,0,m_gridLayout->columnCount(),6,1);
    m_row = 0;
    addAction(action);
    return btn;
}


SARibbonToolButton *SARibbonPannel::addLargeToolButton(const QString& text,const QIcon& icon,QToolButton::ToolButtonPopupMode popMode)
{
    SARibbonToolButton* btn = RibbonSubElementDelegate->createRibbonToolButton(this);
    btn->setButtonType(SARibbonToolButton::LargeButton);
    btn->setAutoRaise(true);
    QSize iconSize = maxHightIconSize(icon.actualSize(QSize(32,32)),32);
    btn->setIconSize(iconSize);
    btn->setIcon(icon);
    btn->setPopupMode(popMode);
    btn->setText(text);
    m_gridLayout->addWidget(btn,0,m_gridLayout->columnCount(),6,1);
    m_row = 0;
    return btn;
}


SARibbonToolButton *SARibbonPannel::addSmallToolButton(const QString &text, const QIcon &icon, QToolButton::ToolButtonPopupMode popMode)
{
    SARibbonToolButton* btn = RibbonSubElementDelegate->createRibbonToolButton(this);
    btn->setButtonType(SARibbonToolButton::SmallButton);
    btn->setAutoRaise(true);
    QSize iconSize = maxHightIconSize(icon.actualSize(QSize(16,16)),16);
    btn->setIconSize(iconSize);
    btn->setIcon(icon);
    btn->setPopupMode(popMode);
    btn->setText(text);
    if(0 == m_row)
        m_gridLayout->addWidget(btn,m_row,m_gridLayout->columnCount(),2,1);
    else
        m_gridLayout->addWidget(btn,m_row,m_gridLayout->columnCount()-1,2,1);
    m_row += 2;
    if(m_row >= 5)//
        m_row = 0;
    return btn;
}

SARibbonToolButton* SARibbonPannel::addSmallAction(QAction *action)
{
    SARibbonToolButton* btn = RibbonSubElementDelegate->createRibbonToolButton(this);
    btn->setButtonType(SARibbonToolButton::SmallButton);
    btn->setAutoRaise(true);
    btn->setDefaultAction(action);
    QSize iconSize = maxHightIconSize(action->icon().actualSize(QSize(16,16)),16);
    btn->setIconSize(iconSize);
    if(action->menu())
        btn->setPopupMode(QToolButton::MenuButtonPopup);
    if(0 == m_row)
        m_gridLayout->addWidget(btn,m_row,m_gridLayout->columnCount(),2,1);
    else
        m_gridLayout->addWidget(btn,m_row,m_gridLayout->columnCount()-1,2,1);
    m_row += 2;
    if(m_row >= 5)
        m_row = 0;
    addAction(action);
    return btn;
}

SARibbonGallery *SARibbonPannel::addGallery()
{
    SARibbonGallery* gallery = RibbonSubElementDelegate->createRibbonGallery(this);
    m_gridLayout->addWidget(gallery,0,m_gridLayout->columnCount(),6,1);
    m_row = 0;
    setExpanding();
    return gallery;
}

void SARibbonPannel::addSeparator()
{
#if 0
    QAction*action = new QAction(this);
    action->setSeparator(true);
    addAction(action);
#else
    SARibbonSeparatorWidget* sep = RibbonSubElementDelegate->createRibbonSeparatorWidget(height() - 10,this);
    m_gridLayout->addWidget(sep,0,m_gridLayout->columnCount(),6,1);
    m_row = 0;
#endif
}

void SARibbonPannel::addWidget(QWidget *w)
{
    int col = m_gridLayout->columnCount();
    if(0 != m_row)
    {
        col -= 1;
    }
    m_gridLayout->addWidget(w,m_row,col,2,1);
    m_row += 2;
    if(m_row >= 5)
        m_row = 0;
}

void SARibbonPannel::addWidget(QWidget *w, int row, int rowSpan)
{
    int col = m_gridLayout->columnCount();
    if(0 != row)
    {
        col -= 1;
    }
    m_gridLayout->addWidget(w,row,col,rowSpan,1);
    m_row = row + rowSpan;
    if(m_row >= 5)
        m_row = 0;
}

void SARibbonPannel::addWidget(QWidget *w, int row, int rowSpan, int column, int columnSpan)
{
    m_gridLayout->addWidget(w,row,column,rowSpan,columnSpan);
    m_row = row + rowSpan;
    if(row >= 5)
        m_row = 0;
}

int SARibbonPannel::gridLayoutColumnCount() const
{
    return m_gridLayout->columnCount();
}

void SARibbonPannel::addOptionAction(QAction *action)
{
    if(nullptr == action)
    {
        if(m_optionActionButton)
        {
            delete m_optionActionButton;
            m_optionActionButton = nullptr;
        }
        return;
    }
    if(nullptr == m_optionActionButton)
    {
        m_optionActionButton = new SARibbonPannelOptionButton(this);
    }
    m_optionActionButton->connectAction(action);
    repaint();
}


QSize SARibbonPannel::maxHightIconSize(const QSize &size, int height)
{
    if(size.height() < height)
    {
        return  (size * ((float)height/size.height()));
    }
    return size;
}

void SARibbonPannel::paintEvent(QPaintEvent *event)
{
    QPainter p(this);
    QFont f = font();
    f.setPixelSize(11);
    p.setFont(f);
    if(m_optionActionButton)
    {
        p.drawText(0,m_titleY
                   ,width()-m_optionActionButton->width() - m_titleOptionButtonSpace
                   ,m_titleHeight,Qt::AlignCenter,windowTitle());
    }
    else
    {
        p.drawText(0,m_titleY,width(),m_titleHeight,Qt::AlignCenter,windowTitle());
    }

    QWidget::paintEvent(event);
}

QSize SARibbonPannel::sizeHint() const
{
    QSize laySize = layout()->sizeHint();
    QFontMetrics fm = fontMetrics();
    QSize titleSize = fm.size(Qt::TextShowMnemonic,windowTitle());
    int maxWidth = laySize.width();
    if(m_defaultReduceButton)
    {
        maxWidth = qMax(laySize.width(),titleSize.width()) + m_titleOptionButtonSpace + m_defaultReduceButton->width();
    }
    else
    {
        maxWidth = qMax(laySize.width(),titleSize.width());
    }
    return QSize(maxWidth,laySize.height());
}

QSize SARibbonPannel::minimumSizeHint() const
{
    return layout()->minimumSize();
}

void SARibbonPannel::setReduce(bool isReduce)
{
    if(isReduce)
    {
        setWindowFlags(Qt::Popup);
    }
    else
    {
        setWindowFlags( Qt::Widget);
    }
}

void SARibbonPannel::setExpanding(bool isExpanding)
{
    setSizePolicy(isExpanding ? QSizePolicy::Expanding : QSizePolicy::Preferred
                                ,QSizePolicy::Fixed);
}

bool SARibbonPannel::isExpanding() const
{
    QSizePolicy sp = sizePolicy();
    return sp.horizontalPolicy() == QSizePolicy::Expanding;
}

bool SARibbonPannel::event(QEvent *event)
{
   // qDebug() << "SARibbonPannel event: "<<event->type();
    return QWidget::event(event);
}


void SARibbonPannel::resizeEvent(QResizeEvent *event)
{
    if(m_optionActionButton)
    {
        m_optionActionButton->move(width()-m_titleOptionButtonSpace/2 - m_optionActionButton->width()
                                   ,m_titleY+(m_titleHeight-m_optionActionButton->height())/2);
    }
    return QWidget::resizeEvent(event);
}


