#include "SARibbonToolButton.h"
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QAction>
#include <QDebug>
#include <QStyleOptionFocusRect>
#include <QStyleOption>
#include <QPaintEvent>
#include <QMouseEvent>
#include "SARibbonDrawHelper.h"
SARibbonToolButton::SARibbonToolButton(QWidget *parent)
    :QToolButton(parent)
    ,m_buttonType(LargeButton)
    ,m_mouseOnSubControl(false)
    ,m_borderColor(242,202,88)
    ,m_menuButtonPressed(false)
{
    setButtonType(SmallButton);
}

void SARibbonToolButton::paintEvent(QPaintEvent *event)
{
    switch(m_buttonType)
    {
    case LargeButton:paintLargeButton(event);return;
    case SmallButton:paintSmallButton(event);return;
    default:
        return;
    }
}

void SARibbonToolButton::resizeEvent(QResizeEvent *e)
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    m_iconRect = QRect(0,0,height(),height());
    QSize iconSize = SARibbonDrawHelper::iconActualSize(opt.icon,&opt,m_iconRect.size().boundedTo(opt.iconSize));
    if(iconSize.width() > m_iconRect.width())
    {
        m_iconRect.setWidth(iconSize.width());
    }
    QToolButton::resizeEvent(e);
}



void SARibbonToolButton::mouseMoveEvent(QMouseEvent *e)
{
    bool isMouseOnSubControl(false);
    if(LargeButton == m_buttonType)
    {
        isMouseOnSubControl = (e->pos().y() > (height()/2));
    }
    else
    {
        isMouseOnSubControl = !m_iconRect.contains(e->pos());
    }

    if(m_mouseOnSubControl != isMouseOnSubControl)
    {
        m_mouseOnSubControl = isMouseOnSubControl;
        update();
    }
    QToolButton::mouseMoveEvent(e);
}

void SARibbonToolButton::mousePressEvent(QMouseEvent *e)
{
    if (e->button() == Qt::LeftButton && (popupMode() == MenuButtonPopup))
    {
        if(LargeButton == m_buttonType)
        {
            QRect popupr = rect().adjusted(0,height()/2,0,0);
            if (popupr.isValid() && popupr.contains(e->pos()))
            {
                m_menuButtonPressed = true;
                showMenu();
                return;
            }
        }
        else
        {
            if(m_iconRect.isValid() && !m_iconRect.contains(e->pos()))
            {
                m_menuButtonPressed = true;
                showMenu();
                return;
            }
        }
    }
    m_menuButtonPressed = false;
    QAbstractButton::mousePressEvent(e);
}

void SARibbonToolButton::mouseReleaseEvent(QMouseEvent *e)
{
    QAbstractButton::mouseReleaseEvent(e);
    m_menuButtonPressed = false;
}

void SARibbonToolButton::leaveEvent(QEvent *e)
{
    m_mouseOnSubControl = false;
    QToolButton::leaveEvent(e);
}


void SARibbonToolButton::paintSmallButton(QPaintEvent *e)
{
    Q_UNUSED(e);
    QStylePainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
#if 0
    p.drawComplexControl(QStyle::CC_ToolButton, opt);
#else
    bool autoRaise = opt.state & QStyle::State_AutoRaise;
    QStyle::State bflags = opt.state & ~QStyle::State_Sunken;
    QStyle::State mflags = bflags;
    if (autoRaise)
    {
        if (!(bflags & QStyle::State_MouseOver) || !(bflags & QStyle::State_Enabled)) {
            bflags &= ~QStyle::State_Raised;
        }
    }
    if (opt.state & QStyle::State_Sunken)
    {
        if (opt.activeSubControls & QStyle::SC_ToolButton)
        {
            bflags |= QStyle::State_Sunken;
            mflags |= QStyle::State_MouseOver | QStyle::State_Sunken;
        }
        else if (opt.activeSubControls & QStyle::SC_ToolButtonMenu)
        {
            mflags |= QStyle::State_Sunken;
            bflags |= QStyle::State_MouseOver;
        }
    }
    //绘制背景
    QStyleOption tool(0);
    tool.palette = opt.palette;

    if ((opt.subControls & QStyle::SC_ToolButton)
            &&
            (opt.features & QStyleOptionToolButton::MenuButtonPopup))
    {

        tool.rect = opt.rect;
        tool.state = bflags;
        if(opt.activeSubControls &= QStyle::SC_ToolButtonMenu)
        {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            p.save();
            p.setPen(m_borderColor);
            p.setBrush(Qt::NoBrush);
            p.drawRect(opt.rect.adjusted(0,0,0,-1));
            p.drawRect(m_iconRect);//分界线
            p.restore();
        }
        else
        {
            if(m_mouseOnSubControl)//此时鼠标在indecater那
            {
                tool.rect.adjust(m_iconRect.width(),0,0,0);
            }
            else
            {
                tool.rect = m_iconRect;
            }


            if (autoRaise)
            {
                style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            }
            else
            {
                style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
            }
        }

    }
    else if((opt.subControls & QStyle::SC_ToolButton)
            && (opt.features & QStyleOptionToolButton::HasMenu)
            )
    {
        tool.rect = opt.rect;
        tool.state = bflags;
        if (autoRaise)
        {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
        }
        else
        {
            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
        }
    }
    else if(opt.subControls & QStyle::SC_ToolButton)
    {
        tool.rect = opt.rect;
        tool.state = bflags;
        if(opt.state & QStyle::State_Sunken)
        {
            tool.state &= ~QStyle::State_MouseOver;
        }
        if (autoRaise)
        {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
        }
        else
        {
            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
        }
    }

    // 绘制边框
    if(((opt.state & QStyle::State_MouseOver) && (opt.features & QStyleOptionToolButton::MenuButtonPopup))
            ||
           ((isChecked()) && (opt.features & QStyleOptionToolButton::MenuButtonPopup)) )//checked
    {
        p.save();
        p.setPen(m_borderColor);
        p.setBrush(Qt::NoBrush);
        p.drawRect(opt.rect.adjusted(0,0,-1,-1));
        p.drawRect(m_iconRect);//分界线
        p.restore();
    }
    drawIconAndLabel(p,opt);
#endif
}

void SARibbonToolButton::paintLargeButton(QPaintEvent *e)
{
    Q_UNUSED(e);

    QPainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);

    bool autoRaise = opt.state & QStyle::State_AutoRaise;
    QStyle::State bflags = opt.state;
    QStyle::State mflags = bflags;

//    bool autoRaise = opt.state & QStyle::State_AutoRaise;
//    QStyle::State bflags = opt.state & ~QStyle::State_Sunken;
//    QStyle::State mflags = bflags;
    if (autoRaise)
    {
        if (!(bflags & QStyle::State_MouseOver) || !(bflags & QStyle::State_Enabled)) {
            bflags &= ~QStyle::State_Raised;
        }
    }

    if (opt.state & QStyle::State_Sunken)
    {
        if (opt.activeSubControls & QStyle::SC_ToolButton)
        {
            bflags |= QStyle::State_Sunken;
            mflags |= QStyle::State_MouseOver | QStyle::State_Sunken;
        }
        else if (opt.activeSubControls & QStyle::SC_ToolButtonMenu)
        {
            mflags |= QStyle::State_Sunken;
            bflags |= QStyle::State_MouseOver;
        }
    }
//绘制背景
    QStyleOption tool(0);
    tool.palette = opt.palette;

    if ((opt.subControls & QStyle::SC_ToolButton)
            &&
            (opt.features & QStyleOptionToolButton::MenuButtonPopup))
    {
        tool.rect = opt.rect;
        tool.state = bflags;
        if(opt.activeSubControls &= QStyle::SC_ToolButtonMenu)
        {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            p.save();
            p.setPen(m_borderColor);
            p.setBrush(Qt::NoBrush);
            p.drawRect(opt.rect.adjusted(1,1,-1,-1));
            p.drawLine(0,opt.rect.height()/2,opt.rect.width(),opt.rect.height()/2);//分界线
            p.restore();
        }
        else
        {
            if(m_mouseOnSubControl)//此时鼠标在下半部分
            {
                tool.rect.adjust(0,tool.rect.height()/2,0,0);
            }
            else
            {
                tool.rect.adjust(0,0,0,-tool.rect.height()/2);
            }

            if (autoRaise)
            {
                style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            }
            else
            {
                style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
            }
        }

    }
    else if((opt.subControls & QStyle::SC_ToolButton)
            && (opt.features & QStyleOptionToolButton::HasMenu)
            )
    {
        tool.rect = opt.rect;
        tool.state = bflags;
        if (autoRaise)
        {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
        }
        else
        {
            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
        }
    }
    else if(opt.subControls & QStyle::SC_ToolButton)
    {
        tool.rect = opt.rect;
        tool.state = bflags;
        if(opt.state & QStyle::State_Sunken)
        {
            tool.state &= ~QStyle::State_MouseOver;
        }
        if (autoRaise)
        {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
        }
        else
        {
            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
        }
    }



    // 绘制边框
    if(((opt.state & QStyle::State_MouseOver) && (opt.features & QStyleOptionToolButton::MenuButtonPopup))
            ||
           ((isChecked()) && (opt.features & QStyleOptionToolButton::MenuButtonPopup)) )//checked
    {
        p.save();
        p.setPen(m_borderColor);
        p.setBrush(Qt::NoBrush);
        p.drawRect(opt.rect.adjusted(1,1,-1,-1));
        p.drawLine(0,opt.rect.height()/2,opt.rect.width(),opt.rect.height()/2);//分界线
        p.restore();
    }


    //绘制sub control 的下拉箭头
    if (opt.features & QStyleOptionToolButton::HasMenu)
    {
        tool.rect = opt.rect.adjusted(0,68,0,0);
        style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &tool, &p, this);
    }

    //绘制Focus
//    if (opt.state & QStyle::State_HasFocus)
//    {
//        qDebug() << "HasFocus";
//        QStyleOptionFocusRect fr;
//        fr.QStyleOption::operator=(opt);
//        fr.rect.adjust(3, 3, -3, -3);
//        if (opt.features & QStyleOptionToolButton::MenuButtonPopup)
//            fr.rect.adjust(0, 0, -style()->pixelMetric(QStyle::PM_MenuButtonIndicator,
//                                              &opt, this), 0);
//        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fr, &p, this);
//    }

    drawIconAndLabel(p,opt);
}

bool SARibbonToolButton::hitButton(const QPoint &pos) const
{
    if(QAbstractButton::hitButton(pos))
        return !m_menuButtonPressed;
    return false;
}

void SARibbonToolButton::drawIconAndLabel(QPainter &p, const QStyleOptionToolButton &opt)
{
    if(LargeButton == m_buttonType)
    {
        //绘制图标和文字
        QStyleOptionToolButton label = opt;
        QStyle::State bflags = opt.state & ~QStyle::State_Sunken;//去除图标和文字的抖动
        label.state = bflags;
        style()->drawControl(QStyle::CE_ToolButtonLabel, &label, &p, this);
    }
    else
    {
        QPixmap pm;
        QSize pmSize = opt.iconSize;
        if (!opt.icon.isNull())
        {
            pm = SARibbonDrawHelper::iconToPixmap(opt.icon,this,&opt,opt.rect.size().boundedTo(opt.iconSize));
            pmSize = pm.size() / pm.devicePixelRatio();
        }

        if (opt.toolButtonStyle != Qt::ToolButtonIconOnly)
        {
            p.save();
            p.setFont(opt.font);

            QRect pr = m_iconRect;
            QRect tr = opt.rect.adjusted(pr.width(),0,-8,0);
            int alignment = Qt::TextShowMnemonic;
            //快捷键的下划线
            if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this))
            {
                alignment |= Qt::TextHideMnemonic;
            }

            if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon)
            {

            }
            else
            {
                style()->drawItemPixmap(&p, QStyle::visualRect(opt.direction, opt.rect, pr), Qt::AlignCenter, pm);
                alignment |= Qt::AlignLeft | Qt::AlignVCenter;
            }
            style()->drawItemText(&p, QStyle::visualRect(opt.direction, opt.rect, tr), alignment, opt.palette,
                                             opt.state & QStyle::State_Enabled, opt.text,
                                             QPalette::ButtonText);
            p.restore();
        }
        else
        {
            style()->drawItemPixmap(&p, opt.rect, Qt::AlignCenter, pm);
        }

        //绘制sub control 的下拉箭头
        if (opt.features & QStyleOptionToolButton::HasMenu)
        {
            QStyleOptionToolButton tool = opt;
            tool.rect = opt.rect.adjusted(opt.rect.width()-8,0,0,0);
            style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &tool, &p, this);
        }
    }
}

SARibbonToolButton::RibbonButtonType SARibbonToolButton::buttonType() const
{
    return m_buttonType;
}

void SARibbonToolButton::setButtonType(const RibbonButtonType &buttonType)
{
    m_buttonType = buttonType;
    if(LargeButton == buttonType)
    {
        setFixedHeight(78);
        setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    }
    else
    {
        setFixedHeight(26);
        setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
    setMouseTracking(true);
}

QSize SARibbonToolButton::minimumSizeHint() const
{
    return sizeHint();
}


