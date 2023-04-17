#include "SAColorToolButton.h"
#include <QPaintEvent>
#include <QStylePainter>
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
    void calcSizeOfToolButtonIconOnly(const QStyleOptionToolButton& opt);
    void calcSizeOfToolButtonTextOnly(const QStyleOptionToolButton& opt);
    void calcSizeOfToolButtonTextBesideIcon(const QStyleOptionToolButton& opt);
    void calcSizeOfToolButtonTextUnderIcon(const QStyleOptionToolButton& opt);
    void adjustIndicatorArror(const QStyleOptionToolButton& opt);
    QPixmap createIconPixmap(const QStyleOptionToolButton& opt);

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
    switch (opt.toolButtonStyle) {
    case Qt::ToolButtonTextOnly:
        calcSizeOfToolButtonTextOnly(opt);
        break;
    case Qt::ToolButtonTextBesideIcon:
        calcSizeOfToolButtonTextBesideIcon(opt);
        break;
    case Qt::ToolButtonTextUnderIcon:
        calcSizeOfToolButtonTextUnderIcon(opt);
        break;
    case Qt::ToolButtonFollowStyle:
    case Qt::ToolButtonIconOnly:
        calcSizeOfToolButtonIconOnly(opt);
        break;
    default:
        break;
    }
#if SAColorToolButton_DEBUG_PRINT
    qDebug() << q_ptr->objectName() << "SAColorToolButton::PrivateData::calcSize,toolButtonStyle(opt.rect=" << opt.rect
             << ",opt.iconSize=" << opt.iconSize << ",mIconRect=" << mIconRect << ",mColorRect=" << mColorRect
             << ",mTextRect=" << mTextRect;
#endif
}

void SAColorToolButton::PrivateData::calcSizeOfToolButtonIconOnly(const QStyleOptionToolButton& opt)
{
    if (opt.icon.isNull()) {
        //没有icon，则整个区域都是颜色
        mIconRect  = QRect();
        mColorRect = opt.rect.adjusted(mSpacing, mSpacing, -mSpacing, -mSpacing);
    } else {
        // 有icon
        // 有icon的情况下，icon在上2/3，color在下1/3
        int h = (opt.rect.height() - 2 * mSpacing) / 3.0;  //计算按钮1/3高度
        // 如果定义了iconsize,需要判断iconsize，iconsize设置的比较小，图标会跟随比较小，如果不设置iconsize，或者设置了一个null
        // size 则iconrect，会和toolbutton相关
        if (opt.iconSize.isNull()) {
            mIconRect  = opt.rect.adjusted(mSpacing, mSpacing, -mSpacing, -mSpacing - h);
            mColorRect = opt.rect.adjusted(mSpacing,
                                           mSpacing + 2 * h + mSpacing,  //这里加mSpacing是为了把颜色和图标分开
                                           -mSpacing,
                                           -mSpacing);
        } else {
            //说明有iconsize
            if (opt.iconSize.height() < 2 * h) {
                //说明iconsize比较小
                //这时以iconsize为主
                //先计算颜色条的高度
                int colorHeight = opt.iconSize.height() / 4;
                if (colorHeight < 5) {
                    colorHeight = 5;
                }
                int y      = opt.rect.height() - opt.iconSize.height() - 3 * mSpacing - colorHeight;  // 3个spacing，其中一个是icon和colorbar之间
                int x      = (opt.rect.width() - opt.iconSize.width()) / 2;  //居中
                mIconRect  = QRect(x, y, opt.iconSize.width(), opt.iconSize.height());
                mColorRect = QRect(x, mIconRect.bottom() + mSpacing, opt.iconSize.width(), colorHeight);
            } else {
                //说明iconsize比较大，则要进行缩放了
                //保证iconsize的高度是h，宽度等比例
                int sizeW = 0;
                if (opt.iconSize.height() > 0) {
                    sizeW = h * opt.iconSize.width() / opt.iconSize.height();
                }
                int x      = (opt.rect.width() - sizeW) / 2;  //居中
                mIconRect  = QRect(x, mSpacing, sizeW, 2 * h);
                mColorRect = QRect(x, mIconRect.bottom() + mSpacing, sizeW, h);
            }
        }
    }
    //确定文本区域
    mTextRect = QRect();
    //最后对IndicatorArror区域进行调整
    adjustIndicatorArror(opt);
}

void SAColorToolButton::PrivateData::calcSizeOfToolButtonTextOnly(const QStyleOptionToolButton& opt)
{
    mIconRect  = QRect();
    mColorRect = QRect();
    mTextRect  = opt.rect.adjusted(mSpacing, mSpacing, -mSpacing, -mSpacing);
    //最后对IndicatorArror区域进行调整
    adjustIndicatorArror(opt);
}

void SAColorToolButton::PrivateData::calcSizeOfToolButtonTextBesideIcon(const QStyleOptionToolButton& opt)
{
    if (opt.icon.isNull()) {
        //没有icon，颜色占用iconsize
        mIconRect = QRect();
        if (opt.iconSize.isNull()) { }
    } else {
    }
}

void SAColorToolButton::PrivateData::calcSizeOfToolButtonTextUnderIcon(const QStyleOptionToolButton& opt)
{
}

void SAColorToolButton::PrivateData::adjustIndicatorArror(const QStyleOptionToolButton& opt)
{
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
        //如果有菜单，要预留s_indicatorArrorWidth的宽度在右手边，显示一个菜单箭头
        if (!mIconRect.isNull()) {
            mIconRect.adjust(0, 0, -s_indicatorArrorWidth, 0);
        }
        if (!mColorRect.isNull()) {
            mColorRect.adjust(0, 0, -s_indicatorArrorWidth, 0);
        }
        if (!mTextRect.isNull()) {
            mTextRect.adjust(0, 0, -s_indicatorArrorWidth, 0);
        }
    }
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

void SAColorToolButton::paintEvent(QPaintEvent* e)
{
    QStylePainter p(this);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    bool autoRaise = opt.state & QStyle::State_AutoRaise;
    //先绘制按钮
    if (autoRaise) {
        style()->drawPrimitive(QStyle::PE_PanelButtonTool, &opt, &p, this);
    } else {
        style()->drawPrimitive(QStyle::PE_PanelButtonBevel, &opt, &p, this);
    }
    if (!d_ptr->mIconRect.isNull()) {
        //绘制图标
        QPixmap pm = d_ptr->createIconPixmap(opt);
        style()->drawItemPixmap(&p, d_ptr->mIconRect, Qt::AlignCenter, pm);
    }
    if (!d_ptr->mTextRect.isNull()) {
        int alignment = Qt::TextShowMnemonic;
        style()->drawItemText(&p,
                              QStyle::visualRect(opt.direction, opt.rect, d_ptr->mTextRect),
                              alignment,
                              opt.palette,
                              opt.state & QStyle::State_Enabled,
                              opt.text,
                              QPalette::ButtonText);
    }
    if (!d_ptr->mColorRect.isNull()) {
        //绘制颜色
        p.fillRect(d_ptr->mColorRect, d_ptr->mColor);
    }
}

void SAColorToolButton::resizeEvent(QResizeEvent* e)
{
    //在resizeevent计算绘图所需的尺寸，避免在绘图过程中实时绘制提高效率
    QToolButton::resizeEvent(e);
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    d_ptr->calcSize(opt);
}
