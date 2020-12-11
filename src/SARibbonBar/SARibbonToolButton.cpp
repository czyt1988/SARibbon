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
#include "QCursor"
int SARibbonToolButton::LargeButtonSize = 78;
int SARibbonToolButton::SmallButtonSize = 24;
#define DEBUG_PRINT_SARibbonToolButton 1
#if DEBUG_PRINT_SARibbonToolButton
QDebug operator<<(QDebug debug, const QStyleOptionToolButton &opt)
{
    debug << "=============="
          << "\nQStyleOption("
            << (QStyleOption)opt
            << ")"
            << "\n  QStyleOptionComplex:"
               "\n     subControls("
            << opt.subControls
            << " ) "
               "\n     activeSubControls("
            << opt.activeSubControls
            << "\n  QStyleOptionToolButton"
               "\n     features("
            << opt.features
            << ")"
               "\n     toolButtonStyle("
            << opt.toolButtonStyle
            << ")"
                       ;

    return debug;
}
#endif

SARibbonToolButton::SARibbonToolButton(QWidget *parent)
    :QToolButton(parent)
    ,m_buttonType(LargeButton)
    ,m_mouseOnSubControl(false)
    ,m_borderColor(242,202,88)
    ,m_menuButtonPressed(false)
{
    setAutoRaise(true);
    setButtonType(SmallButton);
}

SARibbonToolButton::SARibbonToolButton(QAction *defaultAction, QWidget *parent)
    :QToolButton(parent)
    ,m_buttonType(LargeButton)
    ,m_mouseOnSubControl(false)
    ,m_borderColor(242,202,88)
    ,m_menuButtonPressed(false)
{
    setAutoRaise(true);
    setButtonType(SmallButton);
    setDefaultAction(defaultAction);
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
    QToolButton::mousePressEvent(e);
}

void SARibbonToolButton::mouseReleaseEvent(QMouseEvent *e)
{
    QToolButton::mouseReleaseEvent(e);
    m_menuButtonPressed = false;
}

void SARibbonToolButton::focusOutEvent(QFocusEvent *e)
{
    QToolButton::focusOutEvent(e);
    m_mouseOnSubControl = false;
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
    if(opt.features & QStyleOptionToolButton::MenuButtonPopup
            ||
            opt.features & QStyleOptionToolButton::HasMenu
            )
    {
        if(!this->rect().contains(this->mapFromGlobal(QCursor::pos())))
        {
            opt.state &= ~QStyle::State_MouseOver;
        }
    }
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

    if(opt.features & QStyleOptionToolButton::MenuButtonPopup
            ||
            opt.features & QStyleOptionToolButton::HasMenu
            )
    {
        if(!this->rect().contains(this->mapFromGlobal(QCursor::pos())))
        {
            opt.state &= ~QStyle::State_MouseOver;
        }
    }


    bool autoRaise = opt.state & QStyle::State_AutoRaise;
#if 0
    QStyle::State bflags = opt.state;
#else
    QStyle::State bflags = opt.state & ~QStyle::State_Sunken;
#endif
    if (autoRaise)
    {
        //如果autoRaise，但鼠标不在按钮上或者按钮不是激活状态，去除raised状态
        if (!(bflags & QStyle::State_MouseOver) || !(bflags & QStyle::State_Enabled)) {
            bflags &= ~QStyle::State_Raised;
        }
    }

    if (opt.state & QStyle::State_Sunken)
    {
        if (opt.activeSubControls & QStyle::SC_ToolButton)
        {
            bflags |= QStyle::State_Sunken;
        }
        else if (opt.activeSubControls & QStyle::SC_ToolButtonMenu)
        {
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

#if DEBUG_PRINT_SARibbonToolButton
            if(this->objectName() == "MenuButtonPopup")
            {

                static int s_e = 0;
                qDebug() << s_e << " under mouse:" << this->underMouse()
                         << " \ncontinue:" <<this->rect().contains(this->mapFromGlobal(QCursor::pos()))
                         //<< " \nmapFromGlobal continue:" <<this->geometry().contains(this->mapFromGlobal(QCursor::pos()))
                         << " \n rect:" << this->geometry()
                         << " \n QCursor:" << QCursor::pos()
                         << " \n nmapFromGlobal QCursor:" << this->mapFromGlobal(QCursor::pos())
                         << "   " <<autoRaise << opt;
                ++s_e;
            }
#endif

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
    if (opt.state & QStyle::State_HasFocus)
    {
        QStyleOptionFocusRect fr;
        fr.QStyleOption::operator=(opt);
        fr.rect.adjust(3, 3, -3, -3);
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup)
            fr.rect.adjust(0, 0, -style()->pixelMetric(QStyle::PM_MenuButtonIndicator,
                                              &opt, this), 0);
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fr, &p, this);
    }

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
//        QStyleOptionToolButton label = opt;
//        QStyle::State bflags = opt.state & ~QStyle::State_Sunken;//去除图标和文字的抖动
//        label.state = bflags;
//        style()->drawControl(QStyle::CE_ToolButtonLabel, &label, &p, this);
        QRect rect = opt.rect;
        int shiftX = 0;
        int shiftY = 0;
        if (opt.state & (QStyle::State_Sunken | QStyle::State_On))
        {
            shiftX = style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal, &opt, this);
            shiftY = style()->pixelMetric(QStyle::PM_ButtonShiftVertical, &opt, this);
        }
        bool hasArrow = opt.features & QStyleOptionToolButton::Arrow;
        if (((!hasArrow && opt.icon.isNull()) && !opt.text.isEmpty())
            || opt.toolButtonStyle == Qt::ToolButtonTextOnly)
        {//没有箭头 且 没图标 有文字 || 只有文字模式
            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic;//纯文本下，居中对齐
            if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this))
                alignment |= Qt::TextHideMnemonic;
            rect.translate(shiftX, shiftY);
            p.setFont(opt.font);
            style()->drawItemText(&p, rect, alignment, opt.palette,
                         opt.state & QStyle::State_Enabled, opt.text,
                         QPalette::ButtonText);
        }
        else
        {//非纯文本
            QPixmap pm;
            QSize pmSize = opt.iconSize;
            if (!opt.icon.isNull())
            {//有图标
                QIcon::State state = opt.state & QStyle::State_On ? QIcon::On : QIcon::Off;
                QIcon::Mode mode;
                if (!(opt.state & QStyle::State_Enabled))
                    mode = QIcon::Disabled;
                else if ((opt.state & QStyle::State_MouseOver) && (opt.state & QStyle::State_AutoRaise))
                    mode = QIcon::Active;
                else
                    mode = QIcon::Normal;
                pm = opt.icon.pixmap(this->window()->windowHandle()
                                     , opt.rect.size().boundedTo(opt.iconSize),
                                             mode, state);
                pmSize = pm.size() / pm.devicePixelRatio();
            }

            if (opt.toolButtonStyle != Qt::ToolButtonIconOnly)
            {
                p.setFont(opt.font);
                QRect pr = rect,
                tr = rect;
                int alignment = Qt::TextShowMnemonic;
                if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this))
                    alignment |= Qt::TextHideMnemonic;

                if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon)
                {
                    //文字在icon下
                    pr.setHeight(pmSize.height() + 6);
                    tr.adjust(0, pr.height() - 1, 0, -1);
                    pr.translate(shiftX, shiftY);
                    if (!hasArrow) {
                        style()->drawItemPixmap(&p, pr, Qt::AlignCenter, pm);
                    }
                    else
                    {
                        drawArrow(style(), &opt, pr, &p, this);
                    }
                    tr.adjust(0,3,0,0);
                    alignment |= Qt::AlignHCenter | Qt::AlignTop;//文字是顶部对齐
                }
                else
                {
                    //文字在icon其他位置
                    pr.setWidth(pmSize.width() + 8);
                    tr.adjust(pr.width(), 0, 0, 0);
                    pr.translate(shiftX, shiftY);
                    if (!hasArrow) {
                        style()->drawItemPixmap(&p, QStyle::visualRect(opt.direction, rect, pr), Qt::AlignCenter, pm);
                    } else {
                        drawArrow(style(), &opt, pr, &p, this);
                    }
                    alignment |= Qt::AlignLeft | Qt::AlignVCenter;
                }
                tr.translate(shiftX, shiftY);
                style()->drawItemText(&p, QStyle::visualRect(opt.direction, rect, tr), alignment, opt.palette,
                             opt.state & QStyle::State_Enabled, opt.text,
                             QPalette::ButtonText);
            }
            else
            {
                rect.translate(shiftX, shiftY);
                if (hasArrow) {
                    drawArrow(style(), &opt, rect, &p, this);
                }
                else
                {
                    style()->drawItemPixmap(&p, rect, Qt::AlignCenter, pm);
                }
            }
        }
    }
    else
    {
        QPixmap pm;
        QSize pmSize = opt.iconSize;
        if (!opt.icon.isNull())
        {
            pm = SARibbonDrawHelper::iconToPixmap(opt.icon,this,&opt,opt.rect.size().boundedTo(opt.iconSize));
            pmSize = pm.size() / pm.devicePixelRatio();

            if (opt.toolButtonStyle != Qt::ToolButtonIconOnly)
            {
                p.save();
                p.setFont(opt.font);

                QRect pr = m_iconRect;
                QRect tr = opt.rect.adjusted(pr.width(),0,-1,0);
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
        }
        else // 只有文字
        {
            int alignment = Qt::TextShowMnemonic;
            //快捷键的下划线
            if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this))
            {
                alignment |= Qt::TextHideMnemonic;
            }
            style()->drawItemText(&p, QStyle::visualRect(opt.direction, opt.rect, opt.rect.adjusted(2,1,-2,-1)), alignment, opt.palette,
                                             opt.state & QStyle::State_Enabled, opt.text,
                                             QPalette::ButtonText);
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

void SARibbonToolButton::drawArrow(const QStyle *style, const QStyleOptionToolButton *toolbutton, const QRect &rect, QPainter *painter, const QWidget *widget)
{
    QStyle::PrimitiveElement pe;
    switch (toolbutton->arrowType) {
    case Qt::LeftArrow:
        pe = QStyle::PE_IndicatorArrowLeft;
        break;
    case Qt::RightArrow:
        pe = QStyle::PE_IndicatorArrowRight;
        break;
    case Qt::UpArrow:
        pe = QStyle::PE_IndicatorArrowUp;
        break;
    case Qt::DownArrow:
        pe = QStyle::PE_IndicatorArrowDown;
        break;
    default:
        return;
    }
    QStyleOption arrowOpt = *toolbutton;
    arrowOpt.rect = rect;
    style->drawPrimitive(pe, &arrowOpt, painter, widget);

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
		setFixedHeight(LargeButtonSize);
        setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		setIconSize(QSize(SARibbonToolButton::LargeButtonSize / 2, SARibbonToolButton::LargeButtonSize / 2));
    }
    else
    {
		setFixedHeight(SmallButtonSize);
        setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    }
    setMouseTracking(true);
}

QSize SARibbonToolButton::minimumSizeHint() const
{
    return sizeHint();
}

bool SARibbonToolButton::event(QEvent *e)
{
    switch(e->type())
    {
    case QEvent::WindowDeactivate:
    case QEvent::ActionChanged:
    case QEvent::ActionRemoved:
    case QEvent::ActionAdded:
        m_mouseOnSubControl = false;
        break;
    default:
        break;
    }

    return QToolButton::event(e);
}


