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
#include "SARibbonElementManager.h"
#include "QCursor"

#define LITE_LARGE_BUTTON_ICON_HIGHT_RATE	0.52
#define ARROW_WIDTH				10

#ifdef SA_RIBBON_DEBUG_HELP_DRAW
#define HELP_DRAW_RECT(p, rect)	      \
    do{			      \
        p.save();	      \
        p.setPen(Qt::red);    \
        p.setBrush(QBrush()); \
        p.drawRect(rect);     \
        p.restore();	      \
    }while(0)

QDebug operator<<(QDebug debug, const QStyleOptionToolButton& opt)
{
    debug	<< "=============="
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

    return (debug);
}


#endif

SARibbonToolButton::SARibbonToolButton(QWidget *parent)
    : QToolButton(parent)
    , m_buttonType(LargeButton)
    , m_largeButtonType(Normal)
    , m_mouseOnSubControl(false)
    , m_menuButtonPressed(false)
    , m_isWordWrap(false)
{
    setAutoRaise(true);
    setButtonType(SmallButton);
}


SARibbonToolButton::SARibbonToolButton(QAction *defaultAction, QWidget *parent)
    : QToolButton(parent)
    , m_buttonType(LargeButton)
    , m_mouseOnSubControl(false)
    , m_menuButtonPressed(false)
    , m_isWordWrap(false)
{
    setAutoRaise(true);
    setButtonType(SmallButton);
    setDefaultAction(defaultAction);
}


void SARibbonToolButton::paintEvent(QPaintEvent *event)
{
    switch (m_buttonType)
    {
    case LargeButton:
        paintLargeButton(event);
        return;

    case SmallButton:
        paintSmallButton(event);
        return;

    default:
        return;
    }
}


void SARibbonToolButton::resizeEvent(QResizeEvent *e)
{
//    QStyleOptionToolButton opt;

//    initStyleOption(&opt);
//    m_iconRect = QRect(0, 0, height(), height());
//    QSize iconSize = SARibbonDrawHelper::iconActualSize(opt.icon, &opt, m_iconRect.size().boundedTo(opt.iconSize));

//    if (iconSize.width() > m_iconRect.width()) {
//        m_iconRect.setWidth(iconSize.width());
//    }
    QToolButton::resizeEvent(e);
}


void SARibbonToolButton::mouseMoveEvent(QMouseEvent *e)
{
    bool isMouseOnSubControl(false);

    if (LargeButton == m_buttonType) {
        isMouseOnSubControl = (e->pos().y() > (height()/2));
    }else {
        isMouseOnSubControl = !m_iconRect.contains(e->pos());
    }

    if (m_mouseOnSubControl != isMouseOnSubControl) {
        m_mouseOnSubControl = isMouseOnSubControl;
        update();
    }
    QToolButton::mouseMoveEvent(e);
}


void SARibbonToolButton::mousePressEvent(QMouseEvent *e)
{
    if ((e->button() == Qt::LeftButton) && (popupMode() == MenuButtonPopup)) {
        if (LargeButton == m_buttonType) {
            QRect popupr = rect().adjusted(0, height()/2, 0, 0);
            if (popupr.isValid() && popupr.contains(e->pos())) {
                m_menuButtonPressed = true;
                showMenu();
                return;
            }
        }else {
            if (m_iconRect.isValid() && !m_iconRect.contains(e->pos())) {
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
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup
        ||
        opt.features & QStyleOptionToolButton::HasMenu
        ) {
        if (!this->rect().contains(this->mapFromGlobal(QCursor::pos()))) {
            opt.state &= ~QStyle::State_MouseOver;
        }
    }
    bool autoRaise = opt.state & QStyle::State_AutoRaise;
    QStyle::State bflags = opt.state & ~QStyle::State_Sunken;
    QStyle::State mflags = bflags;

    if (autoRaise) {
        if (!(bflags & QStyle::State_MouseOver) || !(bflags & QStyle::State_Enabled)) {
            bflags &= ~QStyle::State_Raised;
        }
    }
    if (opt.state & QStyle::State_Sunken) {
        if (opt.activeSubControls & QStyle::SC_ToolButton) {
            bflags |= QStyle::State_Sunken;
            mflags |= QStyle::State_MouseOver | QStyle::State_Sunken;
        }else if (opt.activeSubControls & QStyle::SC_ToolButtonMenu) {
            mflags |= QStyle::State_Sunken;
            bflags |= QStyle::State_MouseOver;
        }
    }
    //绘制背景
    QStyleOption tool(0);

    tool.palette = opt.palette;

    if ((opt.subControls & QStyle::SC_ToolButton)
        &&
        (opt.features & QStyleOptionToolButton::MenuButtonPopup)) {
        tool.rect = opt.rect;
        tool.state = bflags;
        if (opt.activeSubControls &= QStyle::SC_ToolButtonMenu) {
            //菜单激活
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            tool.rect = m_iconRect.adjusted(1, 1, -1, -1);
            tool.state = (QStyle::State_Raised);//把图标区域显示为正常
            if (autoRaise) {
                style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            }else {
                style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
            }
        }else {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            if (tool.state & QStyle::State_MouseOver) {
                if (m_mouseOnSubControl) { //此时鼠标在indecater那
                    //鼠标在文字区，把图标显示为正常
                    tool.rect = m_iconRect.adjusted(1, 1, -1, -1);
                    tool.state = (QStyle::State_Raised);//把图标区域显示为正常
                    if (autoRaise) {
                        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                    }else {
                        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                    }
                }else {
                    //鼠标在图标区，把文字显示为正常
                    tool.state = (QStyle::State_Raised);//把图标区域显示为正常
                    tool.rect = opt.rect.adjusted(m_iconRect.width()+1, 1, -1, -1);
                    if (autoRaise) {
                        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                    }else {
                        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                    }
                }
            }
        }
    }else if ((opt.subControls & QStyle::SC_ToolButton) &&
        (opt.features & QStyleOptionToolButton::HasMenu)
        ) {
        tool.rect = opt.rect;
        tool.state = bflags;
        if (autoRaise) {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
        }else {
            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
        }
    }else if (opt.subControls & QStyle::SC_ToolButton) {
        tool.rect = opt.rect;
        tool.state = bflags;
        if (opt.state & QStyle::State_Sunken) {
            tool.state &= ~QStyle::State_MouseOver;
        }
        if (autoRaise) {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
        }else {
            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
        }
    }

    drawIconAndLabel(p, opt);
}


void SARibbonToolButton::paintLargeButton(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    QStyleOptionToolButton opt;

    initStyleOption(&opt);

    if (opt.features & QStyleOptionToolButton::MenuButtonPopup
        ||
        opt.features & QStyleOptionToolButton::HasMenu
        ) {
        if (!this->rect().contains(this->mapFromGlobal(QCursor::pos()))) {
            opt.state &= ~QStyle::State_MouseOver;
        }
    }


    bool autoRaise = opt.state & QStyle::State_AutoRaise;

#if 0
    QStyle::State bflags = opt.state;
#else
    QStyle::State bflags = opt.state & ~QStyle::State_Sunken;
#endif

    if (autoRaise) {
        //如果autoRaise，但鼠标不在按钮上或者按钮不是激活状态，去除raised状态
        if (!(bflags & QStyle::State_MouseOver) || !(bflags & QStyle::State_Enabled)) {
            bflags &= ~QStyle::State_Raised;
        }
    }

    if (opt.state & QStyle::State_Sunken) {
        if (opt.activeSubControls & QStyle::SC_ToolButton) {
            bflags |= QStyle::State_Sunken;
        }else if (opt.activeSubControls & QStyle::SC_ToolButtonMenu) {
            bflags |= QStyle::State_MouseOver;
        }
    }

//绘制背景
    QStyleOption tool(0);
    tool.palette = opt.palette;

    //MenuButtonPopup特殊处理
    if ((opt.subControls & QStyle::SC_ToolButton)
        &&
        (opt.features & QStyleOptionToolButton::MenuButtonPopup)) {
        //此时按钮的菜单弹出
        tool.rect = opt.rect;
        tool.state = bflags;
        if (opt.activeSubControls &= QStyle::SC_ToolButtonMenu) {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            //菜单激活
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            tool.rect = m_iconRect.adjusted(1, 1, -1, -1);
            tool.state = (QStyle::State_Raised);//把图标区域显示为正常
            if (autoRaise) {
                style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            }else {
                style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
            }
        }else {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            if (tool.state & QStyle::State_MouseOver) {
                if (m_mouseOnSubControl) { //此时鼠标在indecater那
                    //鼠标在文字区，把图标显示为正常
                    tool.rect = m_iconRect.adjusted(1, 1, -1, -1);
                    tool.state = (QStyle::State_Raised);//把图标区域显示为正常
                    if (autoRaise) {
                        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                    }else {
                        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                    }
                }else {
                    //鼠标在图标区，把文字显示为正常
                    tool.state = (QStyle::State_Raised);//把图标区域显示为正常
                    tool.rect = opt.rect.adjusted(1, m_iconRect.height()+1, -1, -1);
                    if (autoRaise) {
                        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                    }else {
                        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                    }
                }
            }
        }
    }else if ((opt.subControls & QStyle::SC_ToolButton) &&
        (opt.features & QStyleOptionToolButton::HasMenu)
        ) {
        //按钮含有菜单
        tool.rect = opt.rect;
        tool.state = bflags;
        if (autoRaise) {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
        }else {
            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
        }
    }else if (opt.subControls & QStyle::SC_ToolButton) {
        tool.rect = opt.rect;
        tool.state = bflags;
        if (opt.state & QStyle::State_Sunken) {
            tool.state &= ~QStyle::State_MouseOver;
        }
        if (autoRaise) {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
        }else {
            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
        }
    }


    //绘制Focus
    if (opt.state & QStyle::State_HasFocus) {
        QStyleOptionFocusRect fr;
        fr.QStyleOption::operator=(opt);
        fr.rect.adjust(3, 3, -3, -3);
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup) {
            fr.rect.adjust(0, 0, -style()->pixelMetric(QStyle::PM_MenuButtonIndicator,
                &opt, this), 0);
        }
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fr, &p, this);
    }

    drawIconAndLabel(p, opt);
}


bool SARibbonToolButton::hitButton(const QPoint& pos) const
{
    if (QAbstractButton::hitButton(pos)) {
        return (!m_menuButtonPressed);
    }
    return (false);
}


QSize SARibbonToolButton::sizeHint() const
{
    QSize s = QToolButton::sizeHint();
    QStyleOptionToolButton opt;

    initStyleOption(&opt);
    //QToolButton的sizeHint已经考虑了菜单箭头的位置
    if (LargeButton == buttonType()) {
        //计算最佳大小
        SARibbonToolButton *that = const_cast<SARibbonToolButton *>(this);

        if (s.width() > s.height()*1.4) {
            //文本对齐方式
            int alignment = Qt::TextShowMnemonic | Qt::TextWordWrap;
            //如果宽度大于高度，就看看换行是否能满足
            QFontMetrics fm = fontMetrics();
            //计算默认的文本区域
            QRect textRange = calcTextRect(QRect(0, 0, s.width()/2, s.height()));
            textRange.moveTo(0, 0);
            //计算换行后的最大文本区域
            textRange = fm.boundingRect(textRange, alignment, text());
            //把区域设置给size
            s.setWidth(textRange.width()+4);
            //确认是否换行
            that->m_isWordWrap = (textRange.height() > fm.lineSpacing());

            if ((opt.features & QStyleOptionToolButton::Menu) ||
                (opt.features & QStyleOptionToolButton::HasMenu)) {
                //如果有菜单
                if (largeButtonType() == Lite) {
                    //lite模式下都要偏移
                    s.rwidth() += ARROW_WIDTH;
                }else{
                    //Normal模式下，仅对换行的偏移
                    if (m_isWordWrap) {
                        s.rwidth() += ARROW_WIDTH;
                    }
                }
            }
        }else{
            //TODO : 还有一种情况，显示的使用\n导致误判
            that->m_isWordWrap = text().contains('\n');
            QStyleOptionToolButton opt;
            initStyleOption(&opt);
            if ((opt.features & QStyleOptionToolButton::Menu) ||
                (opt.features & QStyleOptionToolButton::HasMenu)) {
                //如果有菜单
                if ((largeButtonType() == Normal) && m_isWordWrap) {
                    s.rwidth() += ARROW_WIDTH;
                }
            }
        }
    }else{
        // InstantPopup在qtoolbutton不会添加控件来放下箭头，这里处理的和MenuButtonPopup一致
        // 在仅有图标的小模式显示时，预留一个下拉箭头位置
        if (Qt::ToolButtonIconOnly == toolButtonStyle()) {
            if (opt.features & QStyleOptionToolButton::Menu ||
                opt.features & QStyleOptionToolButton::HasMenu) {
                //如果有菜单
                //            s.rwidth() += style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);
                s.rwidth() += ARROW_WIDTH;
            }
        }else{
            if (opt.features & QStyleOptionToolButton::Menu ||
                opt.features & QStyleOptionToolButton::HasMenu) {
            }else{
                s.rwidth() -= 4;// QToolButton::sizeHint()预留了4像素，有点稀疏
            }
        }
    }
    return (s);
}


/**
 * @brief SARibbonToolButton::drawIconAndLabel
 *
 * 在LargeButtonType == Normal模式下，icon占大按钮的一半区域，在wps模式下，icon占大按钮的60%，文字占40%，且文字不换行
 * @param p
 * @param opt
 */
void SARibbonToolButton::drawIconAndLabel(QPainter& p, QStyleOptionToolButton& opt)
{
    calcIconRect(opt);
    QPixmap pm = createIconPixmap(opt);

    if (LargeButton == m_buttonType) {
//绘制图标和文字
        QRect textRect = calcTextRect(opt);
        bool hasArrow = opt.features & QStyleOptionToolButton::Arrow;
        if (((!hasArrow && opt.icon.isNull()) && !opt.text.isEmpty()) ||
            (opt.toolButtonStyle == Qt::ToolButtonTextOnly)) {
            //没有箭头 且 没图标 有文字 || 只有文字模式
            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextWordWrap; //纯文本下，居中对齐,换行
            if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
                alignment |= Qt::TextHideMnemonic;
            }
            p.setFont(opt.font);
            style()->drawItemText(&p, textRect, alignment, opt.palette,
                opt.state & QStyle::State_Enabled, opt.text,
                QPalette::ButtonText);
        }else {
            //非纯文本
            if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
                //文本加图标情况
                p.setFont(opt.font);
                int alignment = Qt::TextShowMnemonic | Qt::TextWordWrap; // 换行
                if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
                    alignment |= Qt::TextHideMnemonic;
                }
                //文字在icon下
                //先绘制图标
                if (!hasArrow) {
#if SA_RIBBON_DEBUG_HELP_DRAW
                    HELP_DRAW_RECT(p, m_iconRect);
#endif
                    style()->drawItemPixmap(&p, m_iconRect, Qt::AlignCenter, pm);
                }else {
                    drawArrow(style(), &opt, m_iconRect, &p, this);
                }
#if SA_RIBBON_DEBUG_HELP_DRAW
                HELP_DRAW_RECT(p, textRect);
#endif
                if (Normal == m_largeButtonType) {
                    alignment |= Qt::AlignHCenter | Qt::AlignTop;//文字是顶部对齐
                }else{
                    alignment |= Qt::AlignCenter;
                }

                //再绘制文本，对于Normal模式下的Largebutton，如果有菜单，且m_isWordWrap是true，箭头将在文本旁边
                style()->drawItemText(&p, QStyle::visualRect(opt.direction, opt.rect, textRect), alignment, opt.palette,
                    opt.state & QStyle::State_Enabled, opt.text,
                    QPalette::ButtonText);
            }else {
                //只有图标情况
                if (hasArrow) {
                    drawArrow(style(), &opt, opt.rect, &p, this);
                }else {
                    style()->drawItemPixmap(&p, m_iconRect, Qt::AlignCenter, pm);
                }
            }
            //绘制sub control 的下拉箭头
            if (opt.features & QStyleOptionToolButton::HasMenu) {
                opt.rect = calcIndicatorArrowDownRect(opt);
                style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &opt, &p, this);
            }
        }
    }else {
        //小图标
        QSize pmSize = opt.iconSize;
        if (!opt.icon.isNull()) {
            pmSize = pm.size() / pm.devicePixelRatio();

            if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
                p.save();
                p.setFont(opt.font);

                QRect pr = m_iconRect;                                  //图标区域
                QRect tr = opt.rect.adjusted(pr.width()+2, 0, -1, 0);   //文本区域
                int alignment = Qt::TextShowMnemonic;
                //快捷键的下划线
                if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
                    alignment |= Qt::TextHideMnemonic;
                }

                if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                    //ribbonbutton在小图标下，不支持ToolButtonTextUnderIcon
                }else {
                    style()->drawItemPixmap(&p, QStyle::visualRect(opt.direction, opt.rect, pr), Qt::AlignCenter, pm);
                    alignment |= Qt::AlignLeft | Qt::AlignVCenter;
                }
                style()->drawItemText(&p, QStyle::visualRect(opt.direction, opt.rect, tr), alignment, opt.palette,
                    opt.state & QStyle::State_Enabled, opt.text,
                    QPalette::ButtonText);
                p.restore();
            }else {
                style()->drawItemPixmap(&p, m_iconRect, Qt::AlignCenter, pm);
            }
        }else {// 只有文字
            int alignment = Qt::TextShowMnemonic;
            //快捷键的下划线
            if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
                alignment |= Qt::TextHideMnemonic;
            }
            style()->drawItemText(&p, QStyle::visualRect(opt.direction, opt.rect, opt.rect.adjusted(2, 1, -2, -1)), alignment, opt.palette,
                opt.state & QStyle::State_Enabled, opt.text,
                QPalette::ButtonText);
        }


        //绘制sub control 的下拉箭头
        if (opt.features & QStyleOptionToolButton::HasMenu) {
            QStyleOptionToolButton tool = opt;
            tool.rect = calcIndicatorArrowDownRect(tool);
            style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &tool, &p, this);
        }
    }
}


void SARibbonToolButton::drawArrow(const QStyle *style, const QStyleOptionToolButton *toolbutton, const QRect& rect, QPainter *painter, const QWidget *widget)
{
    QStyle::PrimitiveElement pe;

    switch (toolbutton->arrowType)
    {
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
    return (m_buttonType);
}


/**
 * @brief 设置按钮样式
 * @note 设置按钮样式过程会调用setToolButtonStyle，如果要改变toolButtonStyle,如设置为Qt::ToolButtonIconOnly,需要在此函数之后设置
 * @param buttonType
 */
void SARibbonToolButton::setButtonType(const RibbonButtonType& buttonType)
{
    m_buttonType = buttonType;
    if (LargeButton == buttonType) {
        setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        setIconSize(RibbonSubElementStyleOpt.toolButtonLargeIconSize);
        setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    }else {
        setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        setIconSize(RibbonSubElementStyleOpt.toolButtonSmallIconSize);
        setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Maximum);
    }
    setMouseTracking(true);
}


QSize SARibbonToolButton::minimumSizeHint() const
{
    return (sizeHint());
}


void SARibbonToolButton::setLargeButtonType(SARibbonToolButton::LargeButtonType type)
{
    m_largeButtonType = type;
}


SARibbonToolButton::LargeButtonType SARibbonToolButton::largeButtonType() const
{
    return (m_largeButtonType);
}


int SARibbonToolButton::liteLargeButtonIconHeight(int buttonHeight) const
{
    return (buttonHeight*LITE_LARGE_BUTTON_ICON_HIGHT_RATE);
}


/**
 * @brief 根据设定计算图标的绘制区域
 * @param opt
 * @return
 */
void SARibbonToolButton::calcIconRect(const QStyleOptionToolButton& opt)
{
    if (LargeButton == m_buttonType) {
        m_iconRect = opt.rect;
        if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
            if (Normal == m_largeButtonType) {
                m_iconRect.setHeight(opt.rect.height()/2);
            }else if (Lite == m_largeButtonType) {
                m_iconRect.setHeight(liteLargeButtonIconHeight(opt.rect.height()));
            }
        }
    }else {
        if (opt.toolButtonStyle == Qt::ToolButtonIconOnly) {
            // InstantPopup在qtoolbutton不会添加控件来放下箭头，这里处理的和MenuButtonPopup一致
            // 在仅有图标的小模式显示时，预留一个下拉箭头位置
            m_iconRect = rect();
            if (opt.features & QStyleOptionToolButton::Menu ||
                opt.features & QStyleOptionToolButton::HasMenu) {
                //如果有菜单
                //            s.rwidth() += style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);
                m_iconRect.adjust(0, 0, -ARROW_WIDTH, 0);
            }
        }else {
            m_iconRect = QRect(0, 0, qMax(opt.rect.height(), opt.iconSize.width()), opt.rect.height());
        }
    }
}


/**
 * @brief 根据设定计算文本显示区域
 * @param opt
 * @return
 */
QRect SARibbonToolButton::calcTextRect(const QStyleOptionToolButton& opt) const
{
    int shiftX = 0;
    int shiftY = 0;

    if (opt.state & (QStyle::State_Sunken | QStyle::State_On)) {
        shiftX = style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal, &opt, this);
        shiftY = style()->pixelMetric(QStyle::PM_ButtonShiftVertical, &opt, this);
    }

    QRect rect = calcTextRect(opt.rect
        , (opt.features & QStyleOptionToolButton::HasMenu || opt.features & QStyleOptionToolButton::Menu));

    rect.translate(shiftX, shiftY);
    return (rect);
}


/**
 * @brief 根据按钮区域计算文字范围
 *
 * 在大按钮模式下，有几种情况:
 *
 * 第一种，文字没有菜单，此时文本矩形占满下半矩形
 *
 * 第二种，文本带有菜单，但没换行，此时下拉箭头在底部预留的10像素区域
 *
 * 第三种，文本带有菜单，且换行了，此时下拉箭头在旁边
 *
 * @param buttonRect 按钮的几何区域
 * @param hasMenu 是否有菜单
 * @return 返回文字区域
 */
QRect SARibbonToolButton::calcTextRect(const QRect& buttonRect, bool hasMenu) const
{
    QRect rect(buttonRect);

    if ((Qt::ToolButtonTextOnly == toolButtonStyle()) || icon().isNull()) {
        return (rect);
    }
    if (LargeButton == m_buttonType) {
        if (Normal == m_largeButtonType) {
            if (hasMenu) {
                if (m_isWordWrap) {
                    rect.adjust(1, buttonRect.height()/2, -ARROW_WIDTH, -1);        //预留ARROW_WIDTH绘制箭头，1px的上下边界
                }else{
                    rect.adjust(1, buttonRect.height()/2, -1, -ARROW_WIDTH);        //预留ARROW_WIDTH绘制箭头，1px的上下边界
                }
            }else{
                rect.adjust(1, buttonRect.height()/2, -1, -1); //没有菜单不用预留箭头，1px的上下边界
            }
        }else if (Lite == m_largeButtonType) {
            if (hasMenu) {
                rect.adjust(1, liteLargeButtonIconHeight(buttonRect.height()), -ARROW_WIDTH, -1);
            }else{
                rect.adjust(1, liteLargeButtonIconHeight(buttonRect.height()), -1, -1);
            }
        }
    }else {
        if (!(Qt::ToolButtonIconOnly == toolButtonStyle())) {
            if (hasMenu) {
                rect = buttonRect.adjusted(m_iconRect.width(), 0, -ARROW_WIDTH, 0);
            }else{
                rect = buttonRect.adjusted(m_iconRect.width(), 0, -1, 0);
            }
        }
    }
    return (rect);
}


/**
 * @brief sub control 的下拉箭头的位置
 * @param opt
 * @return
 */
QRect SARibbonToolButton::calcIndicatorArrowDownRect(const QStyleOptionToolButton& opt)
{
    //预留8px绘制箭头，1px的上下边界
    QRect rect = opt.rect;

    if (LargeButton == m_buttonType) {
        if (Normal == m_largeButtonType) {
            if (m_isWordWrap) {
                rect.adjust(rect.width()-ARROW_WIDTH, rect.height()/2, 1, 1);
            }else{
                rect.adjust(1, rect.height()-ARROW_WIDTH, 1, 1);
            }
        }else {
            rect.adjust(rect.width()-ARROW_WIDTH, liteLargeButtonIconHeight(opt.rect.height()), 1, 1);
        }
    }else {
        rect.adjust(rect.width()-ARROW_WIDTH, 1, 1, 1);
    }
    return (rect);
}


QPixmap SARibbonToolButton::createIconPixmap(const QStyleOptionToolButton& opt)
{
    if (!opt.icon.isNull()) {//有图标
        QIcon::State state = opt.state & QStyle::State_On ? QIcon::On : QIcon::Off;
        QIcon::Mode mode;
        if (!(opt.state & QStyle::State_Enabled)) {
            mode = QIcon::Disabled;
        }else if ((opt.state & QStyle::State_MouseOver) && (opt.state & QStyle::State_AutoRaise)) {
            mode = QIcon::Active;
        }else{
            mode = QIcon::Normal;
        }
//        return opt.icon.pixmap(this->window()->windowHandle()
//            , opt.rect.size().boundedTo(opt.iconSize),
//            mode, state);
        return (opt.icon.pixmap(this->window()->windowHandle()
               , opt.rect.size().boundedTo(opt.iconSize),
               mode, state));
    }
    return (QPixmap());
}


bool SARibbonToolButton::event(QEvent *e)
{
    switch (e->type())
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

    return (QToolButton::event(e));
}
