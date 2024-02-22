#include "SAColorToolButton.h"
#include <QApplication>
#include <QPaintEvent>
#include <QStylePainter>
#include <QStyleOption>
#include <QStyleOptionToolButton>
#include <QResizeEvent>
#include <QDebug>
#include "SAColorMenu.h"

#define SAColorToolButton_DEBUG_PRINT 0
class SAColorToolButton::PrivateData
{
    SA_COLOR_WIDGETS_DECLARE_PUBLIC(SAColorToolButton)
public:
    PrivateData(SAColorToolButton* p);
    void calcSizeOfToolButtonIconOnly(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
    void calcSizeOfToolButtonTextOnly(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
    void calcSizeOfToolButtonTextBesideIcon(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
    void calcSizeOfToolButtonTextUnderIcon(const QStyleOptionToolButton& opt, QRect& iconRect, QRect& textRect, QRect& colorRect);
    QPixmap createIconPixmap(const QStyleOptionToolButton& opt, const QRect& iconRect);
    QRect getButtonRect(const QStyleOptionToolButton& opt);
    QRect getIndicatorRect(const QStyleOptionToolButton& opt);
    QStyle::State getButtonStyleState(const QStyleOptionToolButton& opt);
    QStyle::State getButtonMenuStyleState(const QStyleOptionToolButton& opt);
    void removeMenu();

public:
    QColor mColor { Qt::white };
    int mSpacing { 2 };                                        ///< 间隔
    QMargins mMargins { 3, 3, 3, 3 };                          ///< box
    ColorToolButtonStyle mColorButtonStyle { WithColorMenu };  ///< 样式
    static int s_indicatorArrorWidth;                          ///< 菜单宽度
};

int SAColorToolButton::PrivateData::s_indicatorArrorWidth = 8;

SAColorToolButton::PrivateData::PrivateData(SAColorToolButton* p) : q_ptr(p)
{
}

void SAColorToolButton::PrivateData::calcSizeOfToolButtonIconOnly(const QStyleOptionToolButton& opt,
                                                                  QRect& iconRect,
                                                                  QRect& textRect,
                                                                  QRect& colorRect)
{
    // 确定文本区域
    textRect         = QRect();
    QRect buttonRect = getButtonRect(opt);
    if (opt.icon.isNull()) {
        colorRect = buttonRect;
        iconRect  = QRect();
    } else {
        QSize tmpSize = opt.iconSize;
        if (tmpSize.isNull()) {
            tmpSize = QSize(16, 16);
        }
        tmpSize         = buttonRect.size().boundedTo(tmpSize);
        int colorHeight = tmpSize.height() / 4;
        int totalHeight = colorHeight + tmpSize.height() + mSpacing;
        if (totalHeight > buttonRect.height()) {
            // 过高，这时要通过高度反推tmpSize和colorHeight
            colorHeight = (buttonRect.height() - mSpacing) / 5;
            tmpSize.setHeight(colorHeight * 4);
            if (opt.iconSize.height() > 0) {
                tmpSize.setWidth(tmpSize.height() * opt.iconSize.width() / opt.iconSize.height());  // 等比例
            }
            tmpSize = buttonRect.size().boundedTo(tmpSize);
        }
        // 有icon，颜色位于图标下面
        int y = (buttonRect.height() - colorHeight - mSpacing - tmpSize.height()) / 2;  //(ButtonHeight-TotalHeight)/2
        int x = (buttonRect.width() - tmpSize.width()) / 2;
        iconRect  = QRect(buttonRect.left() + x, buttonRect.top() + y, tmpSize.width(), tmpSize.height());
        colorRect = QRect(iconRect.x(), iconRect.bottom() + mSpacing, iconRect.width(), colorHeight);
    }
}

/**
 * @brief 纯文本情况，左边有5像素显示颜色
 * @param opt
 * @param iconRect
 * @param textRect
 * @param colorRect 左边有5像素显示颜色
 */
void SAColorToolButton::PrivateData::calcSizeOfToolButtonTextOnly(const QStyleOptionToolButton& opt,
                                                                  QRect& iconRect,
                                                                  QRect& textRect,
                                                                  QRect& colorRect)
{
    QRect buttonRect = getButtonRect(opt);
    iconRect         = QRect();
    QSize colorSize  = opt.iconSize;
    if (colorSize.isNull()) {
        colorSize = QSize(16, 16);
    }
    colorSize = buttonRect.size().boundedTo(colorSize);
    colorRect = QRect(buttonRect.left(),
                      buttonRect.top() + (buttonRect.height() - colorSize.height()) / 2,
                      colorSize.width(),
                      colorSize.height());
    textRect  = buttonRect.adjusted(colorRect.right() + mSpacing, 0, 0, 0);
}

void SAColorToolButton::PrivateData::calcSizeOfToolButtonTextBesideIcon(const QStyleOptionToolButton& opt,
                                                                        QRect& iconRect,
                                                                        QRect& textRect,
                                                                        QRect& colorRect)
{
    QRect buttonRect = getButtonRect(opt);
    if (opt.icon.isNull()) {
        QSize colorSize = opt.iconSize;
        if (colorSize.isNull()) {
            colorSize = QSize(16, 16);
        }
        // 说明没有icon
        // 这时所有都是color
        // iconRect=Null
        colorSize = buttonRect.size().boundedTo(colorSize);
        iconRect  = QRect();
        colorRect = QRect(buttonRect.left(),
                          buttonRect.top() + (buttonRect.height() - colorSize.height()) / 2,
                          colorSize.width(),
                          colorSize.height());
        textRect  = buttonRect.adjusted(colorRect.width() + mSpacing, 0, 0, 0);
    } else {  // 有图标
        QSize tmpSize = opt.iconSize;
        if (tmpSize.isNull()) {
            tmpSize = QSize(16, 16);
        }
        tmpSize         = buttonRect.size().boundedTo(tmpSize);
        int colorHeight = tmpSize.height() / 4;
        int totalHeight = colorHeight + tmpSize.height() + mSpacing;
        if (totalHeight > buttonRect.height()) {
            // 过高，这时要通过高度反推tmpSize和colorHeight
            colorHeight = (buttonRect.height() - mSpacing) / 5;
            tmpSize.setHeight(colorHeight * 4);
            tmpSize.setWidth(tmpSize.height());  // 对于异形也设置为正方行
        }
        // 有icon，颜色位于图标下面
        int y = (buttonRect.height() - colorHeight - mSpacing - tmpSize.height()) / 2;  //(ButtonHeight-TotalHeight)/2
        iconRect  = QRect(buttonRect.left(), buttonRect.top() + y, tmpSize.width(), tmpSize.height());
        colorRect = QRect(iconRect.x(), iconRect.bottom() + mSpacing, iconRect.width(), colorHeight);
        textRect  = buttonRect.adjusted(iconRect.right() + mSpacing, 0, 0, 0);
    }
}

void SAColorToolButton::PrivateData::calcSizeOfToolButtonTextUnderIcon(const QStyleOptionToolButton& opt,
                                                                       QRect& iconRect,
                                                                       QRect& textRect,
                                                                       QRect& colorRect)
{
    QRect buttonRect = getButtonRect(opt);
    QSize tmpSize    = opt.iconSize;
    if (tmpSize.isNull()) {
        tmpSize = QSize(16, 16);
    }
    // 获取字体高度
    int textHeight = opt.fontMetrics.height();
    tmpSize        = buttonRect.size().boundedTo(tmpSize);
    if (opt.icon.isNull()) {
        int totalHeight = textHeight + opt.iconSize.height() + mSpacing;
        if (totalHeight < buttonRect.height()) {
            // 足够高
            colorRect = QRect(buttonRect.left() + (buttonRect.width() - tmpSize.width()) / 2,
                              buttonRect.top() + (buttonRect.height() - totalHeight) / 2,
                              tmpSize.width(),
                              opt.iconSize.height());
        } else {
            // 空间不足
            colorRect = QRect(buttonRect.left() + (buttonRect.width() - tmpSize.width()) / 2,
                              buttonRect.top() + mSpacing,
                              tmpSize.width(),
                              opt.iconSize.height());
        }
        iconRect = QRect();
        textRect = QRect(buttonRect.left(), colorRect.bottom() + mSpacing, buttonRect.width(), textHeight);

    } else {
        // 有图标
        int colorHeight = tmpSize.height() / 4;
        int totalHeight = textHeight + opt.iconSize.height() + colorHeight + 2 * mSpacing;
        if (totalHeight < buttonRect.height()) {
            // 高度空间足够
            // 先布置icon
            iconRect = QRect(buttonRect.left() + (buttonRect.width() - tmpSize.width()) / 2,
                             buttonRect.top() + (buttonRect.height() - totalHeight) / 2,
                             tmpSize.width(),
                             opt.iconSize.height());

        } else {
            // 空间不足
            iconRect = QRect(buttonRect.left() + (buttonRect.width() - tmpSize.width()) / 2,
                             buttonRect.top() + mSpacing,
                             tmpSize.width(),
                             opt.iconSize.height());
        }
        colorRect = QRect(iconRect.x(), iconRect.bottom() + mSpacing, iconRect.width(), colorHeight);
        textRect  = QRect(buttonRect.left(), colorRect.bottom() + mSpacing, buttonRect.width(), textHeight);
    }
}

QPixmap SAColorToolButton::PrivateData::createIconPixmap(const QStyleOptionToolButton& opt, const QRect& iconRect)
{
    if (opt.icon.isNull()) {
        return (QPixmap());
    }
    // 有图标
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
    return (opt.icon.pixmap(iconRect.size(), mode, state));
}

/**
 * @brief 获取按钮的矩形区域
 * @param opt
 * @return
 */
QRect SAColorToolButton::PrivateData::getButtonRect(const QStyleOptionToolButton& opt)
{
    QRect btnRect = q_ptr->style()->subControlRect(QStyle::CC_ToolButton, &opt, QStyle::SC_ToolButton, q_ptr);
    return btnRect.marginsRemoved(mMargins);
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

void SAColorToolButton::PrivateData::removeMenu()
{
    QMenu* m = q_ptr->menu();
    if (m) {
        q_ptr->setMenu(nullptr);
        m->deleteLater();
    }
}

//==============================================================
// SAColorToolButton
//==============================================================

SAColorToolButton::SAColorToolButton(QWidget* parent) : QToolButton(parent), d_ptr(new PrivateData(this))
{
    setColorToolButtonStyle(WithColorMenu);
    connect(this, &QToolButton::clicked, this, &SAColorToolButton::onButtonClicked);
}

SAColorToolButton::SAColorToolButton(ColorToolButtonStyle style, QWidget* parent)
    : QToolButton(parent), d_ptr(new PrivateData(this))
{
    setColorToolButtonStyle(style);
    connect(this, &QToolButton::clicked, this, &SAColorToolButton::onButtonClicked);
}

SAColorToolButton::~SAColorToolButton()
{
}

/**
 * @brief 获取颜色
 * @return
 */
QColor SAColorToolButton::color() const
{
    return d_ptr->mColor;
}

/**
 * @brief 设置Margins
 * @param mg
 */
void SAColorToolButton::setMargins(const QMargins& mg)
{
    d_ptr->mMargins = mg;
    repaint();
}

QMargins SAColorToolButton::margins() const
{
    return d_ptr->mMargins;
}

/**
 * @brief 绘制无颜色表示
 * @param p
 * @param colorRect 绘制的区域
 */
void SAColorToolButton::paintNoneColor(QPainter* p, const QRect& colorRect)
{
    p->save();
    QPen pen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap);
    p->setPen(pen);
    p->setRenderHint(QPainter::SmoothPixmapTransform, true);
    p->setRenderHint(QPainter::Antialiasing, true);
    int ss = colorRect.width() / 3;
    p->drawLine(QPoint(colorRect.x() + ss, colorRect.bottom()), QPoint(colorRect.right() - ss, colorRect.top()));
    pen.setColor(Qt::black);
    p->setPen(pen);
    p->drawRect(colorRect);
    p->restore();
}

/**
 * @brief 设置颜色按钮的样式
 * @param s
 */
void SAColorToolButton::setColorToolButtonStyle(ColorToolButtonStyle s)
{
    if (NoColorMenu == s) {
        d_ptr->removeMenu();
    } else {
        createColorMenu();
    }
    d_ptr->mColorButtonStyle = s;
}

/**
 * @brief 颜色按钮的样式
 * @return
 */
SAColorToolButton::ColorToolButtonStyle SAColorToolButton::colorToolButtonStyle() const
{
    return d_ptr->mColorButtonStyle;
}

/**
 * @brief 获取colorMenu
 *
 * @note 注意，这个函数很有可能会返回nullptr，如果ColorToolButtonStyle设置为NoColorMenu或者自己设置了菜单，此函数返回nullptr
 * @return SAColorMenu指针，如果没有，返回nullptr
 */
SAColorMenu* SAColorToolButton::colorMenu() const
{
    return qobject_cast< SAColorMenu* >(menu());
}

/**
 * @brief 建立标准的颜色菜单
 * @return
 */
SAColorMenu* SAColorToolButton::createColorMenu()
{
    setPopupMode(QToolButton::InstantPopup);
    SAColorMenu* m = new SAColorMenu(this);
    m->enableNoneColorAction(true);
    QAction* customColor = m->customColorAction();
    if (customColor) {
        customColor->setIcon(QIcon(":/image/resource/define-color.svg"));
    }
    connect(m, &SAColorMenu::selectedColor, this, &SAColorToolButton::setColor);
    setMenu(m);
    return m;
}

/**
 * @brief 设置颜色
 * @note 会发射@sa colorChanged 信号
 * @param c
 */
void SAColorToolButton::setColor(const QColor& c)
{
    if (d_ptr->mColor != c) {
        d_ptr->mColor = c;
        repaint();
        emit colorChanged(c);
    }
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
        d_ptr->calcSizeOfToolButtonIconOnly(opt, iconRect, textRect, colorRect);
        break;
    }
}

void SAColorToolButton::paintEvent(QPaintEvent* e)
{
    Q_UNUSED(e);
    QStylePainter p(this);

    QStyleOptionToolButton opt;

    initStyleOption(&opt);
    QRect iconRect, colorRect, textRect;
    calcRect(opt, iconRect, textRect, colorRect);
    paintButton(&p, opt);
    // 绘制图标
    paintIcon(&p, iconRect, opt);
    // 绘制文字
    paintText(&p, textRect, opt);
    // 绘制颜色
    paintColor(&p, colorRect, d_ptr->mColor, opt);
}

void SAColorToolButton::resizeEvent(QResizeEvent* e)
{
    // 在resizeevent计算绘图所需的尺寸，避免在绘图过程中实时绘制提高效率
    QToolButton::resizeEvent(e);
}

/**
 * @brief sizeHint重载
 * @return
 */
QSize SAColorToolButton::sizeHint() const
{
    ensurePolished();
    QStyleOptionToolButton opt;
    initStyleOption(&opt);
    int w = 0, h = 0;
    if (Qt::ToolButtonIconOnly == opt.toolButtonStyle || Qt::ToolButtonFollowStyle == opt.toolButtonStyle) {
        // 和文本无关
        w = opt.iconSize.width() + d_ptr->mMargins.left() + d_ptr->mMargins.right();
        h = opt.iconSize.height() + d_ptr->mMargins.top() + d_ptr->mMargins.bottom();
    } else if (Qt::ToolButtonTextOnly == opt.toolButtonStyle || Qt::ToolButtonTextBesideIcon == opt.toolButtonStyle) {
        QSize textSize = opt.fontMetrics.size(Qt::TextSingleLine | Qt::TextShowMnemonic, opt.text);
        textSize.setHeight(textSize.height() + 4);
        QSize iconSize = opt.iconSize;
        if (!opt.icon.isNull()) {
            // 有图标，要有iconsize高度的1/4给颜色
            iconSize.setHeight(iconSize.height() + iconSize.height() / 4 + d_ptr->mSpacing);
        }
        w = textSize.width() + d_ptr->mSpacing + iconSize.width() + d_ptr->mMargins.left() + d_ptr->mMargins.right();
        h = qMax(textSize.height(), iconSize.height()) + d_ptr->mMargins.top() + d_ptr->mMargins.bottom();
    } else if (Qt::ToolButtonTextUnderIcon == opt.toolButtonStyle) {
        QSize textSize = opt.fontMetrics.size(Qt::TextSingleLine | Qt::TextShowMnemonic, opt.text);
        textSize.setHeight(textSize.height() + 4);
        QSize iconSize = opt.iconSize;
        if (!opt.icon.isNull()) {
            // 有图标，要有iconsize高度的1/4给颜色
            iconSize.setHeight(iconSize.height() + iconSize.height() / 4 + d_ptr->mSpacing);
        }
        w = qMax(textSize.width(), iconSize.width()) + d_ptr->mMargins.left() + d_ptr->mMargins.right();
        h = textSize.height() + iconSize.height() + d_ptr->mSpacing + d_ptr->mMargins.top() + d_ptr->mMargins.bottom();
    }
    opt.rect.setSize(QSize(w, h));  // PM_MenuButtonIndicator depends on the height
    if (opt.features & QStyleOptionToolButton::MenuButtonPopup || opt.features & QStyleOptionToolButton::HasMenu) {
        w += style()->pixelMetric(QStyle::PM_MenuButtonIndicator, &opt, this);
    }
    //! Qt6.4 取消了QApplication::globalStrut
    return style()->sizeFromContents(QStyle::CT_ToolButton, &opt, QSize(w, h), this).expandedTo(QSize(2, 2));
}

void SAColorToolButton::onButtonClicked(bool checked)
{
    emit colorClicked(color(), checked);
}

/**
 * @brief 绘制按钮
 * @param p
 * @param opt
 */
void SAColorToolButton::paintButton(QStylePainter* p, const QStyleOptionToolButton& opt)
{
    bool autoRaise = opt.state & QStyle::State_AutoRaise;
    // 绘制按钮
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

    // 绘制focus
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
        QPixmap pm = d_ptr->createIconPixmap(opt, iconRect);
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
    // 快捷键的下划线
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
void SAColorToolButton::paintColor(QStylePainter* p,
                                   const QRect& colorRect,
                                   const QColor& color,
                                   const QStyleOptionToolButton& opt)
{
    Q_UNUSED(opt);
    if (colorRect.isNull()) {
        return;
    }
    // 绘制颜色
    if (color.isValid()) {
        p->fillRect(colorRect, color);
    } else {
        paintNoneColor(p, colorRect);
    }
}
