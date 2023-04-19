#include "SAColorToolButton.h"
#include <QPaintEvent>
#include <QStylePainter>
#include <QStyleOption>
#include <QStyleOptionToolButton>
#include <QResizeEvent>
#include <QDebug>
#define SAColorToolButton_DEBUG_PRINT 1
class SAColorToolButton::PrivateData
{
    SA_DECLARE_PUBLIC(SAColorToolButton)
public:
    PrivateData(SAColorToolButton* p);
    void calcSize(const QStyleOptionToolButton& opt);
    void calcSizeOfToolButtonIconOnly(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
    void calcSizeOfToolButtonTextOnly(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
    void calcSizeOfToolButtonTextBesideIcon(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
    void calcSizeOfToolButtonTextUnderIcon(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
    QPixmap createIconPixmap(const QStyleOptionToolButton& opt);
    QRect getButtonRect(const QStyleOptionToolButton& opt);
    QRect getIndicatorRect(const QStyleOptionToolButton& opt);
    QStyle::State getButtonStyleState(const QStyleOptionToolButton& opt);
    QStyle::State getButtonMenuStyleState(const QStyleOptionToolButton& opt);
    void calcIconAndColorRect(const QIcon& icon, const QSize& iconSize, const QRect& buttonRect, Qt::ToolButtonStyle s, QRect& iconRect, QRect& colorRect);

public:
    QColor mColor { Qt::white };
    QRect mIconRect;                   ///< 记录icon的范围
    QRect mColorRect;                  ///< 记录color的范围
    QRect mTextRect;                   ///< 文本
    int mSpacing { 2 };                ///< 间隔
    static int s_indicatorArrorWidth;  ///< 菜单宽度
};

int SAColorToolButton::PrivateData::s_indicatorArrorWidth = 8;

SAColorToolButton::PrivateData::PrivateData(SAColorToolButton* p) : q_ptr(p)
{
}

void SAColorToolButton::PrivateData::calcSize(const QStyleOptionToolButton& opt)
{
    q_ptr->calcRect(opt, mIconRect, mTextRect, mColorRect);
#if SAColorToolButton_DEBUG_PRINT
    qDebug() << q_ptr->objectName() << "SAColorToolButton::PrivateData::calcSize,toolButtonStyle(opt.rect=" << opt.rect
             << ",opt.iconSize=" << opt.iconSize << ",mIconRect=" << mIconRect << ",mColorRect=" << mColorRect
             << ",mTextRect=" << mTextRect;
#endif
}

void SAColorToolButton::PrivateData::calcSizeOfToolButtonIconOnly(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect)
{
    //确定文本区域
    textRect         = QRect();
    QRect buttonRect = getButtonRect(opt);
    calcIconAndColorRect(opt.icon, opt.iconSize, buttonRect, opt.toolButtonStyle, iconRect, colorRect);
}

/**
 * @brief 纯文本情况，左边有5像素显示颜色
 * @param opt
 * @param iconRect
 * @param textRect
 * @param colorRect 左边有5像素显示颜色
 */
void SAColorToolButton::PrivateData::calcSizeOfToolButtonTextOnly(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect)
{
    QRect buttonRect = getButtonRect(opt);
    calcIconAndColorRect(opt.icon, opt.iconSize, buttonRect, opt.toolButtonStyle, iconRect, colorRect);
    mTextRect = buttonRect.adjusted(mColorRect.right(), mSpacing, -mSpacing, -mSpacing);
}

void SAColorToolButton::PrivateData::calcSizeOfToolButtonTextBesideIcon(const QStyleOptionToolButton& opt,
                                                                        QRect& iconRect,
                                                                        QRect& textRect,
                                                                        QRect& colorRect)
{
    QRect buttonRect = getButtonRect(opt);
}

void SAColorToolButton::PrivateData::calcSizeOfToolButtonTextUnderIcon(const QStyleOptionToolButton& opt,
                                                                       QRect& iconRect,
                                                                       QRect& textRect,
                                                                       QRect& colorRect)
{
}

QPixmap SAColorToolButton::PrivateData::createIconPixmap(const QStyleOptionToolButton& opt)
{
    if (opt.icon.isNull()) {
        return (QPixmap());
    }
    //有图标
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
    return (opt.icon.pixmap(q_ptr->window()->windowHandle(), mIconRect.size(), mode, state));
}

/**
 * @brief 获取按钮的矩形区域
 * @param opt
 * @return
 */
QRect SAColorToolButton::PrivateData::getButtonRect(const QStyleOptionToolButton& opt)
{
    return q_ptr->style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButton, q_ptr);
}

/**
 * @brief 获取ToolButtonMenu的区域
 * @note 注意，如果不是QStyleOptionToolButton::MenuButtonPopup|QStyleOptionToolButton::HasMenu下调用此函数，返回的是QRect()
 * @param opt
 * @return
 */
QRect SAColorToolButton::PrivateData::getIndicatorRect(const QStyleOptionToolButton& opt)
{
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
        return q_ptr->style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButtonMenu, q_ptr);
    }
    return QRect();
}

/**
 * @brief 基于Qt qcommonStyle源码，对QStyleOptionToolButton的style修正
 * @param opt
 * @return
 */
QStyle::State SAColorToolButton::PrivateData::getButtonStyleState(const QStyleOptionToolButton& opt)
{
    QStyle::State bflags = opt.state & ~QStyle::State_Sunken;
    if (bflags & QStyle::State_AutoRaise) {
        if (!(bflags & QStyle::State_MouseOver) || !(bflags & QStyle::State_Enabled)) {
            bflags &= ~QStyle::State_Raised;
        }
    }
    if (opt.state & QStyle::State_Sunken) {
        if (opt.activeSubControls & QStyle::SC_ToolButton) {
            bflags |= QStyle::State_Sunken;
        }
    }
    return bflags;
}

/**
 * @brief 基于Qt qcommonStyle源码，对QStyleOptionToolButton的style修正
 * @param opt
 * @return
 */
QStyle::State SAColorToolButton::PrivateData::getButtonMenuStyleState(const QStyleOptionToolButton& opt)
{
    QStyle::State mflags = opt.state & ~QStyle::State_Sunken;
    if (mflags & QStyle::State_AutoRaise) {
        if (!(mflags & QStyle::State_MouseOver) || !(mflags & QStyle::State_Enabled)) {
            mflags &= ~QStyle::State_Raised;
        }
    }
    if (opt.state & QStyle::State_Sunken) {
        mflags |= QStyle::State_Sunken;
    }
    return mflags;
}

void SAColorToolButton::PrivateData::calcIconAndColorRect(const QIcon& icon,
                                                          const QSize& iconSize,
                                                          const QRect& buttonRect,
                                                          Qt::ToolButtonStyle s,
                                                          QRect& iconRect,
                                                          QRect& colorRect)
{
    switch (s) {
    case Qt::ToolButtonTextOnly: {
        iconRect  = QRect();
        colorRect = QRect(2 * mSpacing, 2 * mSpacing, 5, buttonRect.height() - 4 * mSpacing);
    } break;
    case Qt::ToolButtonTextBesideIcon: {
        if (colorSize.isNull()) {
            colorSize = QSize(16, 16);
        }
        if (icon.isNull()) {
            // 说明没有icon
            // 这时所有都是color
            // iconRect=Null
            QSize colorSize = iconSize;
            if (colorSize.height() > buttonRect.height()) {
                colorSize.setHeight(buttonRect.height() - 2 * mSpacing);
            }
            if (colorSize.width() > buttonRect.width()) {
                colorSize.setWidth(buttonRect.width() - 2 * mSpacing);
            }
            iconRect  = QRect();
            colorRect = QRect(mSpacing * 2,
                              buttonRect.height() - colorSize.height() - 2 * mSpacing / 2,
                              colorSize.width(),
                              colorSize.height());
        } else {
            //有icon，颜色位于图标下面
            int h = (buttonRect.height() - 3 * mSpacing) / 3.0;  //计算按钮1/3高度,有一个spacing是图标和按钮之间

            //说明有iconsize
            if (iconSize.height() < 2 * h) {
                //说明iconsize比较小
                //这时以iconsize为主
                //先计算颜色条的高度
                int colorHeight = iconSize.height() / 4;
                if (colorHeight < 5) {
                    colorHeight = 5;
                }
                int y     = (buttonRect.height() - 2 * mSpacing - iconSize.height() - colorHeight - mSpacing) / 2;  // 3个spacing，其中一个是icon和colorbar之间
                int x     = (buttonRect.width() - iconSize.width()) / 2;  //居中
                iconRect  = QRect(x, y, iconSize.width(), iconSize.height());
                colorRect = QRect(iconRect.x(), iconRect.bottom() + mSpacing, iconRect.width(), colorHeight);
            } else {
                //说明iconsize比较大，则要进行缩放了
                //保证iconsize的高度是h，宽度等比例
                int sizeW = 0;
                if (iconSize.height() > 0) {
                    sizeW = h * iconSize.width() / iconSize.height();
                }
                int x     = (buttonRect.width() - sizeW) / 2;  //居中
                iconRect  = QRect(x, mSpacing, sizeW, 2 * h);
                colorRect = QRect(iconRect.x(), iconRect.bottom() + mSpacing, iconRect.width(), h);
            }
        }
    } break;
    case Qt::ToolButtonTextUnderIcon: {

    } break;
    case Qt::ToolButtonFollowStyle:
    case Qt::ToolButtonIconOnly: {
        if (icon.isNull()) {
            colorRect = buttonRect.adjusted(mSpacing, mSpacing, -mSpacing, -mSpacing);
            iconRect  = QRect();
        }
        int h = (buttonRect.height() - 3 * mSpacing) / 3.0;  //计算按钮1/3高度,有一个spacing是图标和按钮之间
        if (iconSize.isNull()) {
            // 如果定义了iconsize,需要判断iconsize，iconsize设置的比较小，图标会跟随比较小，如果不设置iconsize，或者设置了一个null
            // size 则icon rect，会和tool button相关
            iconRect  = buttonRect.adjusted(mSpacing, mSpacing, -mSpacing, -mSpacing - h);
            colorRect = buttonRect.adjusted(mSpacing,
                                            mSpacing + 2 * h + mSpacing,  //这里加mSpacing是为了把颜色和图标分开
                                            -mSpacing,
                                            -mSpacing);
        } else {
            //说明有iconsize
            if (iconSize.height() < 2 * h) {
                //说明iconsize比较小
                //这时以iconsize为主
                //先计算颜色条的高度
                int colorHeight = iconSize.height() / 4;
                if (colorHeight < 5) {
                    colorHeight = 5;
                }
                int y     = (buttonRect.height() - 2 * mSpacing - iconSize.height() - colorHeight - mSpacing) / 2;  // 3个spacing，其中一个是icon和colorbar之间
                int x     = (buttonRect.width() - iconSize.width()) / 2;  //居中
                iconRect  = QRect(x, y, iconSize.width(), iconSize.height());
                colorRect = QRect(iconRect.x(), iconRect.bottom() + mSpacing, iconRect.width(), colorHeight);
            } else {
                //说明iconsize比较大，则要进行缩放了
                //保证iconsize的高度是h，宽度等比例
                int sizeW = 0;
                if (iconSize.height() > 0) {
                    sizeW = h * iconSize.width() / iconSize.height();
                }
                int x     = (buttonRect.width() - sizeW) / 2;  //居中
                iconRect  = QRect(x, mSpacing, sizeW, 2 * h);
                colorRect = QRect(iconRect.x(), iconRect.bottom() + mSpacing, iconRect.width(), h);
            }
        }
    } break;
    default:
        break;
    }
}

//==============================================================
// SAColorToolButton
//==============================================================
SAColorToolButton::SAColorToolButton(QWidget* parent) : QToolButton(parent), d_ptr(new PrivateData(this))
{
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d_ptr->calcSize(opt);
}

QColor SAColorToolButton::getColor() const
{
    return d_ptr->mColor;
}

void SAColorToolButton::setColor(const QColor& c)
{
    d_ptr->mColor = c;
}

/**
 * @brief 计算各个基本组件的位置
 * @param opt
 * @param iconRect 图标区域
 * @param textRect 文本区域
 * @param colorRect 颜色区域
 */
void SAColorToolButton::calcRect(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect)
{
    switch (opt.toolButtonStyle) {
    case Qt::ToolButtonTextOnly:
        d_ptr->calcSizeOfToolButtonTextOnly(opt, iconRect, textRect, colorRect);
        break;
    case Qt::ToolButtonTextBesideIcon:
        d_ptr->calcSizeOfToolButtonTextBesideIcon(opt, iconRect, textRect, colorRect);
        break;
    case Qt::ToolButtonTextUnderIcon:
        d_ptr->calcSizeOfToolButtonTextUnderIcon(opt, iconRect, textRect, colorRect);
        break;
    case Qt::ToolButtonFollowStyle:
    case Qt::ToolButtonIconOnly:
        d_ptr->calcSizeOfToolButtonIconOnly(opt, iconRect, textRect, colorRect);
        break;
    default:
        break;
    }
}

void SAColorToolButton::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QStylePainter p(this);

    QStyleOptionToolButton opt;

    initStyleOption(&opt);
    paintButton(&p, opt);
    //绘制图标
    paintIcon(&p, d_ptr->mIconRect, opt);
    //绘制文字
    paintText(&p, d_ptr->mTextRect, opt);
    //绘制颜色
    paintColor(&p, d_ptr->mColorRect, d_ptr->mColor, opt);
}

void SAColorToolButton::resizeEvent(QResizeEvent* e)
{
    //在resizeevent计算绘图所需的尺寸，避免在绘图过程中实时绘制提高效率
    QToolButton::resizeEvent(e);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d_ptr->calcSize(opt);
}

/**
 * @brief 绘制按钮
 * @param p
 * @param opt
 */
void SAColorToolButton::paintButton(QStylePainter* p, const QStyleOptionToolButton& opt)
{
    bool autoRaise = opt.state & QStyle::State_AutoRaise;
    //绘制按钮
    if (autoRaise) {
        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, p, this);
    } else {
        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &opt, p, this);
    }
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup) {
        QStyleOption tool = opt;
        tool.state        = d_ptr->getButtonMenuStyleState(opt);
        tool.rect         = d_ptr->getIndicatorRect(opt);
        //        if (tool.state & (QStyle::State_Sunken | QStyle::State_On | QStyle::State_Raised)) {
        //            style()->drawPrimitive(QStyle::PE_IndicatorButtonDropDown, &tool, &p, this);
        //        }
        style()->drawPrimitive(QStyle::PE_IndicatorArrowDown, &tool, p, this);
    }

    //绘制focus
    if (opt.state & QStyle::State_HasFocus) {
        QStyleOptionFocusRect fr;
        fr.QStyleOption::operator=(opt);
        fr.rect.adjust(3, 3, -3, -3);
        if (opt.features & QStyleOptionToolButton::MenuButtonPopup) {
            fr.rect.adjust(0, 0, style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this), 0);
        }
        style()->drawPrimitive(QStyle::PE_FrameFocusRect, &fr, p, this);
    }
}

/**
 * @brief 绘制icon
 * @param p
 * @param iconRect
 * @param opt icon信息从QStyleOptionToolButton获取
 */
void SAColorToolButton::paintIcon(QStylePainter* p, const QRect& iconRect, const QStyleOptionToolButton& opt)
{
    if (!iconRect.isNull()) {
        QPixmap pm = d_ptr->createIconPixmap(opt);
        style()->drawItemPixmap(p, iconRect, Qt::AlignCenter, pm);
    }
}

/**
 * @brief 绘制文本
 * @param p
 * @param textRect
 * @param opt
 */
void SAColorToolButton::paintText(QStylePainter* p, const QRect& textRect, const QStyleOptionToolButton& opt)
{
    if (opt.text.isEmpty()) {
        return;
    }
    if (Qt::ToolButtonIconOnly == opt.toolButtonStyle) {
        return;
    }
    p->save();
    p->setFont(opt.font);
    int alignment = Qt::TextShowMnemonic;
    //快捷键的下划线
    if (!style()->styleHint(QStyle::SH_UnderlineShortcut, &opt, this)) {
        alignment |= Qt::TextHideMnemonic;
    }
    alignment |= Qt::AlignHCenter | Qt::AlignVCenter;
    style()->drawItemText(p,
                          QStyle::visualRect(opt.direction, opt.rect, textRect),
                          alignment,
                          opt.palette,
                          opt.state & QStyle::State_Enabled,
                          opt.text,
                          QPalette::ButtonText);
    p->restore();
}

/**
 * @brief 绘制color
 * @param p
 * @param iconRect
 * @param opt
 */
void SAColorToolButton::paintColor(QStylePainter* p, const QRect& colorRect, const QColor& color, const QStyleOptionToolButton& opt)
{
    Q_UNUSED(opt);
    if (!colorRect.isNull()) {
        //绘制颜色
        p->fillRect(colorRect, color);
    }
}
