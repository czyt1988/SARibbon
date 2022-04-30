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

#define SA_INDICATOR_ARROW_WIDTH 8
#define SA_WIDTH_HEIGHT_RATIO 1.2
/**
 * @def 开启此宏会打印一些常见信息
 */
#define DebugSARibbonToolButton_TextDrawPrint 0

//#define SA_RIBBON_DEBUG_HELP_DRAW

#ifdef SA_RIBBON_DEBUG_HELP_DRAW
#define HELP_DRAW_RECT(p, rect)                                                                                        \
    do {                                                                                                               \
        p.save();                                                                                                      \
        p.setPen(Qt::red);                                                                                             \
        p.setBrush(QBrush());                                                                                          \
        p.drawRect(rect);                                                                                              \
        p.restore();                                                                                                   \
    } while (0)

QDebug operator<<(QDebug debug, const QStyleOptionToolButton& opt)
{
    debug << "=============="
          << "\nQStyleOption(" << (QStyleOption)opt << ")"
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
          << opt.toolButtonStyle << ")";

    return (debug);
}

#endif

//静态参数初始化
bool SARibbonToolButton::s_isToolButtonTextShift   = false;
bool SARibbonToolButton::s_liteStyleEnableWordWrap = false;

SARibbonToolButton::SARibbonToolButton(QWidget* parent)
    : QToolButton(parent)
    , m_buttonType(LargeButton)
    , m_largeButtonType(Normal)
    , m_mouseOnSubControl(false)
    , m_menuButtonPressed(false)
    , m_isWordWrap(false)
    , m_iconAndTextSpace(2)
{
    setAutoRaise(true);
    setButtonType(SmallButton);
}

SARibbonToolButton::SARibbonToolButton(QAction* defaultAction, QWidget* parent)
    : QToolButton(parent), m_buttonType(LargeButton), m_mouseOnSubControl(false), m_menuButtonPressed(false), m_isWordWrap(false)
{
    setAutoRaise(true);
    setButtonType(SmallButton);
    setDefaultAction(defaultAction);
}

void SARibbonToolButton::paintEvent(QPaintEvent* event)
{
    switch (m_buttonType) {
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

void SARibbonToolButton::mouseMoveEvent(QMouseEvent* e)
{
    bool isMouseOnSubControl(false);

    if (m_iconRect.isValid()) {
        isMouseOnSubControl = !m_iconRect.contains(e->pos());
    }

    if (m_mouseOnSubControl != isMouseOnSubControl) {
        m_mouseOnSubControl = isMouseOnSubControl;

        //从icon变到text过程中刷新一次
        update();
    }
    QToolButton::mouseMoveEvent(e);
}

void SARibbonToolButton::mousePressEvent(QMouseEvent* e)
{
    if ((e->button() == Qt::LeftButton) && (popupMode() == MenuButtonPopup)) {
        if (LargeButton == m_buttonType) {
            QRect popupr = rect().adjusted(0, height() / 2, 0, 0);
            if (popupr.isValid() && popupr.contains(e->pos())) {
                m_menuButtonPressed = true;
                showMenu();
                return;
            }
        } else {
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

void SARibbonToolButton::mouseReleaseEvent(QMouseEvent* e)
{
    QToolButton::mouseReleaseEvent(e);
    m_menuButtonPressed = false;
}

void SARibbonToolButton::focusOutEvent(QFocusEvent* e)
{
    QToolButton::focusOutEvent(e);
    m_mouseOnSubControl = false;
}

void SARibbonToolButton::leaveEvent(QEvent* e)
{
    m_mouseOnSubControl = false;
    QToolButton::leaveEvent(e);
}

void SARibbonToolButton::paintSmallButton(QPaintEvent* e)
{
    Q_UNUSED(e);
    QStylePainter p(this);
    QStyleOptionToolButton opt;

    initStyleOption(&opt);
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
        if (!this->rect().contains(this->mapFromGlobal(QCursor::pos()))) {
            opt.state &= ~QStyle::State_MouseOver;
        }
    }
    bool autoRaise       = opt.state & QStyle::State_AutoRaise;
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
        } else if (opt.activeSubControls & QStyle::SC_ToolButtonMenu) {
            mflags |= QStyle::State_Sunken;
            bflags |= QStyle::State_MouseOver;
        }
    }
    //绘制背景
    QStyleOption tool(0);

    tool.palette = opt.palette;

    if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::MenuButtonPopup)) {
        tool.rect  = opt.rect;
        tool.state = bflags;
        if (opt.activeSubControls &= QStyle::SC_ToolButtonMenu) {
            //菜单激活,整个按钮都绘制为选中
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
        } else {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            if (tool.state & QStyle::State_MouseOver) {
                if (m_mouseOnSubControl) {  //此时鼠标在indecater那
                    //鼠标在文字区，把图标显示为正常
                    tool.rect  = m_iconRect;
                    tool.state = (QStyle::State_Raised);  //把图标区域显示为正常
                    if (autoRaise) {
                        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                    } else {
                        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                    }
                } else {
                    //鼠标在图标区，把文字显示为正常
                    tool.state = (QStyle::State_Raised);  //把图标区域显示为正常
                    tool.rect  = opt.rect.adjusted(m_iconRect.width() + m_iconAndTextSpace, m_iconAndTextSpace, -m_iconAndTextSpace, -m_iconAndTextSpace);
                    if (autoRaise) {
                        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                    } else {
                        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                    }
                }
            }
        }
    } else if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::HasMenu)) {
        tool.rect  = opt.rect;
        tool.state = bflags;
        if (autoRaise) {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
        } else {
            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
        }
    } else if (opt.subControls & QStyle::SC_ToolButton) {
        tool.rect  = opt.rect;
        tool.state = bflags;
        if (opt.state & QStyle::State_Sunken) {
            tool.state &= ~QStyle::State_MouseOver;
        }
        if (autoRaise) {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
        } else {
            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
        }
    }

    drawIconAndLabel(p, opt);
}

void SARibbonToolButton::paintLargeButton(QPaintEvent* e)
{
    Q_UNUSED(e);
    QPainter p(this);
    QStyleOptionToolButton opt;

    initStyleOption(&opt);

    if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
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
        } else if (opt.activeSubControls & QStyle::SC_ToolButtonMenu) {
            bflags |= QStyle::State_MouseOver;
        }
    }

    //绘制背景
    QStyleOption tool(0);
    tool.palette = opt.palette;

    // MenuButtonPopup特殊处理
    if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::MenuButtonPopup)) {
        //此时按钮的菜单弹出
        tool.rect  = opt.rect;
        tool.state = bflags;
        //先把整个按钮绘制为选中
        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
        if (opt.activeSubControls &= QStyle::SC_ToolButtonMenu) {
            //菜单激活 不做处理
        } else {
            //菜单没有激活,这时候要把图标域或者文字域绘制为正常模式
            // style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            if (tool.state & QStyle::State_MouseOver) {
                if (m_mouseOnSubControl) {  //此时鼠标在indecater那
                    //鼠标在文字区，把图标显示为正常
                    tool.rect  = m_iconRect;
                    tool.state = (QStyle::State_Raised);  //把图标区域显示为正常
                    if (autoRaise) {
                        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                    } else {
                        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                    }
                } else {
                    //鼠标在图标区，把文字显示为正常
                    tool.state = (QStyle::State_Raised);  //把图标区域显示为正常
                    tool.rect.setRect(m_textRect.x(),
                                      m_textRect.y(),
                                      tool.rect.width() - 2 * m_iconAndTextSpace,
                                      m_textRect.height());
                    if (autoRaise) {
                        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
                    } else {
                        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
                    }
                }
            }
        }
    } else if ((opt.subControls & QStyle::SC_ToolButton) && (opt.features & QStyleOptionToolButton::HasMenu)) {
        //按钮含有菜单
        tool.rect  = opt.rect;
        tool.state = bflags;
        if (autoRaise) {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
        } else {
            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
        }
    } else if (opt.subControls & QStyle::SC_ToolButton) {
        tool.rect  = opt.rect;
        tool.state = bflags;
        if (opt.state & QStyle::State_Sunken) {
            tool.state &= ~QStyle::State_MouseOver;
        }
        if (autoRaise) {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
        } else {
            style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &tool, &p, this);
        }
    }

    //绘制Focus
    if (opt.state & QStyle::State_HasFocus) {
        QStyleOptionFocusRect fr;
        fr.QStyleOption::operator=(opt);
        fr.rect.adjust(m_iconAndTextSpace, m_iconAndTextSpace, -m_iconAndTextSpace, -m_iconAndTextSpace);
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup) {
            fr.rect.adjust(0, 0, -style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this), 0);
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

/**
 * @brief 在resizeevent计算绘图所需的尺寸，避免在绘图过程中实时绘制提高效率
 * @param e
 */
void SARibbonToolButton::resizeEvent(QResizeEvent* e)
{
#if DebugSARibbonToolButton_TextDrawPrint
    qDebug() << "SARibbonToolButton::resizeEvent, text=" << text() << " obj=" << objectName() << " size=" << e->size();
#endif
    //在resizeevent计算绘图所需的尺寸，避免在绘图过程中实时绘制提高效率
    QToolButton::resizeEvent(e);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    calcIconAndTextRect(opt);
}

/**
 * @brief toolbutton的尺寸确定是先定下字体的尺寸，再定下icon的尺寸，自底向上，保证字体能显示两行
 * @note m_sizeHint的刷新需要注意
 * @return
 */
QSize SARibbonToolButton::sizeHint() const
{
    //    if (m_sizeHint.isValid()) {
    //        return m_sizeHint;
    //    }

    SARibbonToolButton* that = const_cast< SARibbonToolButton* >(this);
    that->reCalcSizeHint();
    return m_sizeHint;
}

/**
 * @brief 重新计算sizehint，sizehint函数在第一次计算完成后会记录结果，如果文字字体或者内容不变是不需要进行重复计算
 *
 * 正常来讲，高度是不需要管，因为高度是有SARibbonPannelLayout来控制，这个函数主要是为了提供宽度信息，宽度信息给到SARibbonPannelLayout，
 * 然后对button进行布局，最终会触发resize函数，resize函数会再进行一次计算确定具体的尺寸
 *
 * 因此，resize函数确定的尺寸方式必须和reCalcSizeHint保持一致，尤其是涉及字体设置
 *
 * 一般布局前调用sizehint，布局后会调用resizeevent，尤其字体信息会计算两遍，这里后续可以有更好的优化空间
 */
void SARibbonToolButton::reCalcSizeHint(QSize s)
{

    QStyleOptionToolButton opt;

    initStyleOption(&opt);
    // QToolButton的sizeHint已经考虑了菜单箭头的位置
    //从源码看，QToolButton的sizeHint是不会考虑换行的
    if (LargeButton == buttonType()) {
        //计算最佳大小
        if (SARibbonPannel* pannel = qobject_cast< SARibbonPannel* >(parent())) {
            //对于建立在SARibbonPannel的基础上的大按钮，把高度设置为SARibbonPannel计算的大按钮高度
            s.setHeight(pannel->largeHeight());
        }
        if (Normal == m_largeButtonType || (Lite == m_largeButtonType && s_liteStyleEnableWordWrap)) {
            //普通模式下才涉及到文字换行，或者lite模式下指定了文字换行
            if (s.width() > s.height() * SA_WIDTH_HEIGHT_RATIO) {
                //说明是一个长方形按钮
                //文本对齐方式
                //如果宽度大于高度，就看看换行是否能满足
                QFontMetrics fm = fontMetrics();
                QRect textRange(0, 0, s.width(), s.height());

                //! 1 先获取最优的文字textRange
                {
                    const int maxTrycount = 3;
                    int trycount          = 0;
                    int alignment         = Qt::TextShowMnemonic | Qt::TextWordWrap;
                    do {
                        //先计算两行文本的紧凑矩形
                        //从一半开始逐渐递增
                        textRange.setWidth(s.width() / 2 + (s.width() / 2) * (float(trycount) / maxTrycount));
                        textRange = fm.boundingRect(textRange, alignment, text());
                        if (textRange.height() <= (fm.lineSpacing() * 2)) {
                            //保证在两行
                            m_isWordWrap = (textRange.height() > fm.lineSpacing());
                            break;
                        }
                        ++trycount;
                    } while (trycount < 3);
                }
                //左右留2像素
                s.setWidth(textRange.width() + m_iconAndTextSpace * 2);
                //! 2再判断箭头来调整textRange
                //把区域设置给size
                //确认是否换行
                if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) || (opt.features & QStyleOptionToolButton::HasMenu)) {
                    //如果有菜单
                    if (largeButtonType() == Lite) {
                        // lite模式下都要偏移
                        s.rwidth() += SA_INDICATOR_ARROW_WIDTH;

                    } else {
                        // Normal模式下，仅对换行的偏移
                        if (m_isWordWrap) {
                            s.rwidth() += SA_INDICATOR_ARROW_WIDTH;
                        }
                    }
                }
            } else {
                m_isWordWrap = text().contains('\n');
                if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) || (opt.features & QStyleOptionToolButton::HasMenu)) {
                    //如果有菜单
                    if (m_isWordWrap) {
                        s.rwidth() += SA_INDICATOR_ARROW_WIDTH;
                    }
                }
            }
        } else {
            //否则就是lite模式，只允许1行，有菜单就偏移
            if (s.width() > s.height() * 1.5) {
                //过于宽的按钮，把文字用...来替代
                s.rwidth() = s.height() * 1.5;
            }
            if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) || (opt.features & QStyleOptionToolButton::HasMenu)) {
                //如果有菜单
                s.rwidth() += SA_INDICATOR_ARROW_WIDTH;
            }
        }
        //无需在这里进行计算，在resizeevent里进行计算
        //! 3在这时候能确定m_textRect,m_iconRect
        //        that->m_textRect = textRange.moveBottomLeft(QPoint(s.width() - m_iconAndTextSpace, s.height() -
        //        m_iconAndTextSpace)); that->calcIconRect(QRect(0, 0, s.width(), s.height()), m_textRect, opt);
    } else {
        // InstantPopup在qtoolbutton不会添加控件来放下箭头，这里处理的和MenuButtonPopup一致
        // 在仅有图标的小模式显示时，预留一个下拉箭头位置
        if (Qt::ToolButtonIconOnly == toolButtonStyle()) {
            if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
                //如果有菜单
                //            s.rwidth() += style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);
                s.rwidth() += SA_INDICATOR_ARROW_WIDTH;
            }
        }
    }
    m_sizeHint = s;
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
    if (LargeButton == m_buttonType) {
        //绘制图标和文字
        QRect textRect = adjustedTextRect(opt);
        bool hasArrow  = opt.features & QStyleOptionToolButton::Arrow;
        if (((!hasArrow && opt.icon.isNull()) && !opt.text.isEmpty()) || (opt.toolButtonStyle == Qt::ToolButtonTextOnly)) {
            //没有箭头 且 没图标 有文字 || 只有文字模式
            int alignment = Qt::AlignCenter | Qt::TextShowMnemonic | Qt::TextWordWrap;  //纯文本下，居中对齐,换行
            if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
                alignment |= Qt::TextHideMnemonic;
            }
            p.setFont(opt.font);
            style()->drawItemText(&p, textRect, alignment, opt.palette, opt.state & QStyle::State_Enabled, opt.text, QPalette::ButtonText);
        } else {
            //非纯文本
            if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
                //文本加图标情况
                p.setFont(opt.font);
                int alignment = Qt::TextShowMnemonic | Qt::TextWordWrap;  // 换行
                if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
                    alignment |= Qt::TextHideMnemonic;
                }
                //文字在icon下
                //先绘制图标
                if (!hasArrow) {
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
                    HELP_DRAW_RECT(p, m_iconRect);
#endif
                    QPixmap pm = createIconPixmap(opt, m_iconRect.size());
                    style()->drawItemPixmap(&p, m_iconRect, Qt::AlignCenter, pm);
                } else {
                    drawArrow(style(), &opt, m_iconRect, &p, this);
                }
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
                HELP_DRAW_RECT(p, textRect);
#endif
                if (Normal == m_largeButtonType) {
                    alignment |= Qt::AlignHCenter | Qt::AlignTop;  //文字是顶部对齐
                } else {
                    alignment |= Qt::AlignCenter;
                }

                //再绘制文本，对于Normal模式下的Largebutton，如果有菜单，且m_isWordWrap是true，箭头将在文本旁边
                if (Lite == m_largeButtonType && !s_liteStyleEnableWordWrap) {
                    // lite 模式，文字不换行
                    //显示的内容需要进行省略处理
                    opt.text = fontMetrics().elidedText(opt.text, Qt::ElideRight, textRect.width(), Qt::TextShowMnemonic);
                }
                style()->drawItemText(&p,
                                      QStyle::visualRect(opt.direction, opt.rect, textRect),
                                      alignment,
                                      opt.palette,
                                      opt.state & QStyle::State_Enabled,
                                      opt.text,
                                      QPalette::ButtonText);
            } else {
                //只有图标情况
                if (hasArrow) {
                    drawArrow(style(), &opt, opt.rect, &p, this);
                } else {
                    QPixmap pm = createIconPixmap(opt, m_iconRect.size());
                    style()->drawItemPixmap(&p, m_iconRect, Qt::AlignCenter, pm);
                }
            }
            //绘制sub control 的下拉箭头
            if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
                opt.rect = calcIndicatorArrowDownRect(opt);
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
                HELP_DRAW_RECT(p, opt.rect);
#endif
                style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &opt, &p, this);
            }
        }
    } else {
        //小图标
        QSize pmSize = opt.iconSize;
        if (!opt.icon.isNull()) {
            QPixmap pm = createIconPixmap(opt, m_iconRect.size());
            pmSize     = pm.size() / pm.devicePixelRatio();

            if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
                p.save();
                p.setFont(opt.font);

                QRect pr      = m_iconRect;                                   //图标区域
                QRect tr      = opt.rect.adjusted(pr.width() + 2, 0, -1, 0);  //文本区域
                int alignment = Qt::TextShowMnemonic;
                //快捷键的下划线
                if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
                    alignment |= Qt::TextHideMnemonic;
                }

                if (opt.toolButtonStyle == Qt::ToolButtonTextUnderIcon) {
                    // ribbonbutton在小图标下，不支持ToolButtonTextUnderIcon
                } else {
                    style()->drawItemPixmap(&p, QStyle::visualRect(opt.direction, opt.rect, pr), Qt::AlignCenter, pm);
                    alignment |= Qt::AlignLeft | Qt::AlignVCenter;
                }
                style()->drawItemText(&p,
                                      QStyle::visualRect(opt.direction, opt.rect, tr),
                                      alignment,
                                      opt.palette,
                                      opt.state & QStyle::State_Enabled,
                                      opt.text,
                                      QPalette::ButtonText);
                p.restore();
            } else {
                style()->drawItemPixmap(&p, m_iconRect, Qt::AlignCenter, pm);
            }
        } else {  // 只有文字
            int alignment = Qt::TextShowMnemonic;
            //快捷键的下划线
            if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
                alignment |= Qt::TextHideMnemonic;
            }
            style()->drawItemText(&p,
                                  QStyle::visualRect(opt.direction, opt.rect, opt.rect.adjusted(2, 1, -2, -1)),
                                  alignment,
                                  opt.palette,
                                  opt.state & QStyle::State_Enabled,
                                  opt.text,
                                  QPalette::ButtonText);
        }

        //绘制sub control 的下拉箭头
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
            QStyleOptionToolButton tool = opt;
            tool.rect                   = calcIndicatorArrowDownRect(tool);
#ifdef SA_RIBBON_DEBUG_HELP_DRAW
            HELP_DRAW_RECT(p, tool.rect);
#endif
            style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &tool, &p, this);
        }
    }
}

void SARibbonToolButton::drawArrow(const QStyle* style,
                                   const QStyleOptionToolButton* toolbutton,
                                   const QRect& rect,
                                   QPainter* painter,
                                   const QWidget* widget)
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

void SARibbonToolButton::changeEvent(QEvent* e)
{
    if (e) {
        if (e->type() == QEvent::FontChange) {
            // todo 说明字体改变，需要重新计算和字体相关的信息
            QStyleOptionToolButton opt;
            initStyleOption(&opt);
            calcIconAndTextRect(opt);
        }
    }
    QToolButton::changeEvent(e);
}

void SARibbonToolButton::actionEvent(QActionEvent* e)
{
    QToolButton::actionEvent(e);
    // action的文字发生变更时要重新计算尺寸--sizehint实时计算情况下，此处不需要这样设置
    //    if (e) {
    //        // action发生变化
    //        if (e->action()) {
    //            if (!e->action()->text().isEmpty()) {
    //                qDebug() << text() << ":actionEvent,sizehint=" << m_sizeHint;
    //                reCalcSizeHint();
    //            }
    //        }
    //    }
}

/**
 * @brief 按钮样式
 * @sa setButtonType
 * @return
 */
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
    //计算iconrect
    //根据字体计算文字的高度

    if (LargeButton == buttonType) {
        setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
        setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    } else {
        setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
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

void SARibbonToolButton::reCalcSizeHint()
{
    QSize s = QToolButton::sizeHint();
    reCalcSizeHint(s);
}

/**
 * @brief 设置按钮点击时文字会闪烁一下
 * @param on
 */
void SARibbonToolButton::setToolButtonTextShift(bool on)
{
    s_isToolButtonTextShift = on;
}

/**
 * @brief 判断按钮点击时文字是否会闪烁一下
 * @return
 */
bool SARibbonToolButton::isToolButtonTextShift()
{
    return s_isToolButtonTextShift;
}

/**
 * @brief 设置在lite模式下是否允许文字换行，如果允许，则图标相对比较小，默认不允许
 * @param on
 * @note 此函数的调用最好在ribbonbar布局之前设置，且调用之后需要调用@sa SARibbonStyleOption::recalc 刷新
 * @code
 * RibbonSubElementStyleOpt.recalc();
 * @endcode
 */
void SARibbonToolButton::setLiteStyleEnableWordWrap(bool on)
{
    s_liteStyleEnableWordWrap = on;
}

/**
 * @brief 在lite模式下是否允许文字换行
 * @return
 */
bool SARibbonToolButton::isLiteStyleEnableWordWrap()
{
    return s_liteStyleEnableWordWrap;
}

/**
 * @brief 根据设定计算图标的绘制区域
 * @param opt
 * @return
 */
void SARibbonToolButton::calcIconAndTextRect(const QStyleOptionToolButton& opt)
{
    if (LargeButton == m_buttonType) {
        m_iconRect = opt.rect.adjusted(m_iconAndTextSpace, m_iconAndTextSpace, -m_iconAndTextSpace, -m_iconAndTextSpace);
        if (opt.toolButtonStyle != Qt::ToolButtonIconOnly) {
            // sizehint已经保证宽度能显示两行
            // icon 的区域就是文字以外的区域，如果m_isEnableWordWrap=true，就保证有两行文本的空间
            QFontMetrics fm = fontMetrics();
            int maxheight   = 30;
            if (Normal == m_largeButtonType || (Lite == m_largeButtonType && s_liteStyleEnableWordWrap)) {
                maxheight = opt.rect.height() - (fm.lineSpacing() * 2)  //减去2行文本
                            - 2 * m_iconAndTextSpace                    //减去赏析边距
                            - m_iconAndTextSpace  //这里减去m_iconAndTextSpace，m_iconAndTextSpace是icon和text的分隔距离
                        ;
            } else {
                maxheight = opt.rect.height() - (fm.lineSpacing() * 1.2)  //减去1行文本
                            - 2 * m_iconAndTextSpace                      //减去赏析边距
                            - m_iconAndTextSpace  //这里减去m_iconAndTextSpace，m_iconAndTextSpace是icon和text的分隔距离
                        ;
            }
            m_iconRect.setHeight(maxheight);

            //没有菜单
            m_textRect.setRect(m_iconAndTextSpace,
                               m_iconRect.bottom() + m_iconAndTextSpace,
                               m_iconRect.width(),
                               opt.rect.bottom() - m_iconRect.bottom() - m_iconAndTextSpace  // 高度是矩形底部减去icon底部再减去2*Space
            );
            //保证能显示两行文本
            //下面确定文字区域
            if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) || (opt.features & QStyleOptionToolButton::HasMenu)) {
                //有菜单且换行,宽度偏移ARROW_WIDTH
                if (m_isWordWrap) {
                    m_textRect.adjust(0, 0, -SA_INDICATOR_ARROW_WIDTH, 0);
                }else if(Lite == m_largeButtonType && !s_liteStyleEnableWordWrap){
                    //在lite模式下，不允许换行的时候，也需要偏移下三角
                    m_textRect.adjust(0, 0, -SA_INDICATOR_ARROW_WIDTH, 0);
                }
            }

        } else {
            m_textRect = QRect();
        }
    } else {
        //小按钮
        if (opt.toolButtonStyle == Qt::ToolButtonIconOnly) {
            // InstantPopup在qtoolbutton不会添加控件来放下箭头，这里处理的和MenuButtonPopup一致
            // 在仅有图标的小模式显示时，预留一个下拉箭头位置
            m_iconRect = opt.rect.adjusted(m_iconAndTextSpace, m_iconAndTextSpace, -m_iconAndTextSpace, -m_iconAndTextSpace);
            if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
                m_iconRect.adjust(0, 0, -SA_INDICATOR_ARROW_WIDTH, 0);
            }
            //确定文本区域
            m_textRect = QRect();
        } else {
            m_iconRect = QRect(m_iconAndTextSpace,
                               m_iconAndTextSpace,
                               qMax(opt.rect.height(), opt.iconSize.width()) - 2 * m_iconAndTextSpace,
                               opt.rect.height() - 2 * m_iconAndTextSpace);
            m_textRect.setRect(m_iconRect.right() + m_iconAndTextSpace,
                               m_iconAndTextSpace,
                               opt.rect.right() - m_iconRect.right() - 2 * m_iconAndTextSpace,
                               opt.rect.height() - 2 * m_iconAndTextSpace);
            //确定文本区域
            if ((opt.features & QStyleOptionToolButton::MenuButtonPopup) || (opt.features & QStyleOptionToolButton::HasMenu)) {
                //有菜单,宽度偏移ARROW_WIDTH
                m_textRect.adjust(0, 0, -SA_INDICATOR_ARROW_WIDTH, 0);
            }
        }
    }

    //纯文本的文字位置
    if ((Qt::ToolButtonTextOnly == toolButtonStyle()) || icon().isNull()) {
        m_textRect = opt.rect.adjusted(m_iconAndTextSpace, m_iconAndTextSpace, -m_iconAndTextSpace, -m_iconAndTextSpace);
    }
}

/**
 * @brief 根据设定计算文本显示区域
 * @param opt
 * @return
 */
QRect SARibbonToolButton::adjustedTextRect(const QStyleOptionToolButton& opt) const
{

    QRect rect = m_textRect;

    //这两个偏移量是有些按钮在点击时文字会出现一个类似抖动的效果，但据观察office2013的ribbon是不会动的，因此把这个效果取消
    if (s_isToolButtonTextShift) {
        int shiftX = 0;
        int shiftY = 0;

        if (opt.state & (QStyle::State_Sunken | QStyle::State_On)) {
            shiftX = style()->pixelMetric(QStyle::PM_ButtonShiftHorizontal, &opt, this);
            shiftY = style()->pixelMetric(QStyle::PM_ButtonShiftVertical, &opt, this);
        }

        rect.translate(shiftX, shiftY);
    }
    return (rect);
}

/**
 * @brief sub control 的下拉箭头的位置
 *
 * 对于大模式，如果文字换行，箭头会在最右侧，如果文字不换行，箭头就在原来文字第二行的位置
 * @param opt
 * @return
 * @note IndicatorArrow尺寸的计算依赖text，前置需要先调用calcIconAndTextRect
 */
QRect SARibbonToolButton::calcIndicatorArrowDownRect(const QStyleOptionToolButton& opt)
{
    //预留ARROW_WIDTHpx绘制箭头，1px的上下边界
    QRect rect = opt.rect;
    if (LargeButton == m_buttonType) {
        if ((Lite == m_largeButtonType && !s_liteStyleEnableWordWrap)) {
            //首先判断是否为lite且不允许换行
            rect.setRect(m_textRect.right(),
                         m_textRect.top(),
                         opt.rect.right() - m_iconAndTextSpace - m_textRect.right(),
                         m_textRect.height());
        } else if (m_isWordWrap) {
            //如果不是lite，且允许换行，那么就在文本第二行最后端
            //菜单的下拉箭头位于第二行文本的矩形区域
            rect.setRect(m_textRect.right(),
                         m_textRect.top() + m_textRect.height() / 2,
                         opt.rect.right() - m_iconAndTextSpace - m_textRect.right(),
                         m_textRect.height() / 2);

        } else {
            //都不是的情况下就第二行文本位置
            rect.setRect(m_textRect.left(),
                         m_textRect.bottom() - SA_INDICATOR_ARROW_WIDTH,  // 这里ARROW_WIDTH也作为高度
                         m_textRect.width(),
                         SA_INDICATOR_ARROW_WIDTH);
        }
    } else {
        rect.setRect(rect.width() - SA_INDICATOR_ARROW_WIDTH - m_iconAndTextSpace,
                     m_iconAndTextSpace,
                     SA_INDICATOR_ARROW_WIDTH,  // 箭头宽度
                     rect.height() - 2 * m_iconAndTextSpace);
    }
    return (rect);
}

QPixmap SARibbonToolButton::createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconsize)
{
    if (!opt.icon.isNull()) {  //有图标
        QIcon::State state = opt.state & QStyle::State_On ? QIcon::On : QIcon::Off;
        QIcon::Mode mode;
        if (!(opt.state & QStyle::State_Enabled)) {
            mode = QIcon::Disabled;
        } else if ((opt.state & QStyle::State_MouseOver) && (opt.state & QStyle::State_AutoRaise)) {
            mode = QIcon::Active;
        } else {
            mode = QIcon::Normal;
        }
        // return (opt.icon.pixmap(this->window()->windowHandle(), opt.rect.size().boundedTo(realConSize), mode, state));
        return (opt.icon.pixmap(this->window()->windowHandle(), iconsize, mode, state));
    }
    return (QPixmap());
}

bool SARibbonToolButton::event(QEvent* e)
{
    switch (e->type()) {
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
