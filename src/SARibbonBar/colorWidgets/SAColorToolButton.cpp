#include "SAColorToolButton.h"
#include <QPaintEvent>
#include <QStylePainter>
#include <QStyleOptionToolButton>
class SAColorToolButton::PrivateData
{
    SA_DECLARE_PUBLIC(SAColorToolButton)
public:
    PrivateData(SAColorToolButton* p);
    void calcIconAndTextRect(const QStyleOptionToolButton& opt);

public:
    QColor mColor { Qt::white };
    QRect mIconRect;     ///< 记录icon的范围
    QRect mColorRect;    ///< 记录color的范围
    int mSpacing { 2 };  ///< 间隔
};

SAColorToolButton::PrivateData::PrivateData(SAColorToolButton* p) : q_ptr(p)
{
}

void SAColorToolButton::PrivateData::calcIconAndTextRect(const QStyleOptionToolButton& opt)
{
    if (opt.toolButtonStyle == Qt::ToolButtonIconOnly) {
        // InstantPopup在qtoolbutton不会添加控件来放下箭头，这里处理的和MenuButtonPopup一致
        // 在仅有图标的小模式显示时，预留一个下拉箭头位置
        if (opt.icon.isNull()) {
            //没有icon，则整个区域都是颜色
            mIconRect  = QRect();
            mColorRect = opt.rect.adjusted(mSpacing, mSpacing, -mSpacing, -mSpacing);
        } else {
            // 有icon
            // 有icon的情况下，icon在上2/3，color在下1/3
            int h      = (opt.rect.height() - 2 * mSpacing) / 3.0;
            mIconRect  = opt.rect.adjusted(mSpacing, mSpacing, -mSpacing, -mSpacing - h);
            mColorRect = opt.rect.adjusted(mSpacing,
                                           mSpacing + 2 * h + 2,  //这里加2是为了把颜色和图标分开
                                           -mSpacing,
                                           -mSpacing);
        }
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

//==============================================================
// SAColorToolButton
//==============================================================
SAColorToolButton::SAColorToolButton(QWidget* parent) : QToolButton(parent)
{
}

QColor SAColorToolButton::getColor() const
{
    return d_ptr->mColor;
}

void SAColorToolButton::setColor(const QColor& c)
{
    d_ptr->mColor = c;
}

void SAColorToolButton::paintEvent(QPaintEvent* e)
{
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
        //有菜单的按钮，且当前状态是菜单弹出状态
        if (opt.activeSubControls &= QStyle::SC_ToolButtonMenu) {
            //菜单激活,整个按钮都绘制为选中
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
        } else {
            style()->drawPrimitive(QStyle::PE_PanelButtonTool, &tool, &p, this);
            if (tool.state & QStyle::State_MouseOver) {
                //鼠标在按钮上
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
        //有菜单的按钮，当前菜单没有弹出
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
}

void SAColorToolButton::resizeEvent(QResizeEvent* e)
{
    //在resizeevent计算绘图所需的尺寸，避免在绘图过程中实时绘制提高效率
    QToolButton::resizeEvent(e);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    calcIconAndTextRect(opt);
}
