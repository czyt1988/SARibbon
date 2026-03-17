#include "SARibbonColorToolButton.h"
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QDebug>
#include <QApplication>
#include <QScreen>
#include "colorWidgets/SAColorMenu.h"
#include "SARibbonUtil.h"
//===================================================
// SARibbonColorToolButton::PrivateData
//===================================================
const int c_ribbonbutton_color_height = 5;  ///< 颜色块的高度

/**
 * \if ENGLISH
 * @brief Private data class for SARibbonColorToolButton
 * @details This class holds private data for SARibbonColorToolButton to implement the PIMPL idiom.
 * \endif
 *
 * \if CHINESE
 * @brief SARibbonColorToolButton 的私有数据类
 * @details 此类持有 SARibbonColorToolButton 的私有数据，实现 PIMPL 设计模式。
 * \endif
 */
class SARibbonColorToolButton::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonColorToolButton)
public:
    PrivateData(SARibbonColorToolButton* p);

    /**
     * \if ENGLISH
     * @brief Create an icon pixmap with color under the icon
     * @param opt Style option for the tool button
     * @param iconsize Size of the icon
     * @return Pixmap with color under the icon
     * \endif
     *
     * \if CHINESE
     * @brief 创建一个带有颜色的图标 pixmap
     * @param opt 工具按钮的样式选项
     * @param iconsize 图标的大小
     * @return 带有颜色的图标 pixmap
     * \endif
     */
    QPixmap createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconsize) const;

    /**
     * \if ENGLISH
     * @brief Create a color icon
     * @param c Color for the icon
     * @param size Size of the icon
     * @return Icon with the specified color
     * \endif
     *
     * \if CHINESE
     * @brief 创建一个颜色图标
     * @param c 图标的颜色
     * @param size 图标的大小
     * @return 带有指定颜色的图标
     * \endif
     */
    QIcon createColorIcon(const QColor& c, const QSize& size) const;

public:
    QColor mColor;                                                                                ///< 记录颜色
    SARibbonColorToolButton::ColorStyle mColorStyle { SARibbonColorToolButton::ColorUnderIcon };  ///< 颜色显示样式
    QIcon mOldIcon;                                                                               ///< 记录旧的icon
};

SARibbonColorToolButton::PrivateData::PrivateData(SARibbonColorToolButton* p) : q_ptr(p)
{
}

QPixmap SARibbonColorToolButton::PrivateData::createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconsize) const
{
    if (opt.icon.isNull()) {  // 没有有图标
        return QPixmap();
    }
    // 有icon，在icon下方加入颜色
    QIcon::State state = (opt.state & QStyle::State_On) ? QIcon::On : QIcon::Off;
    QIcon::Mode mode;
    if (!(opt.state & QStyle::State_Enabled)) {
        mode = QIcon::Disabled;
    } else if ((opt.state & QStyle::State_MouseOver) && (opt.state & QStyle::State_AutoRaise)) {
        mode = QIcon::Active;
    } else {
        mode = QIcon::Normal;
    }
    QSize realIconSize = iconsize - QSize(0, c_ribbonbutton_color_height + 1);
    QPixmap pixmap     = SA::iconToPixmap(opt.icon, realIconSize, SA::widgetDevicePixelRatio(q_ptr), mode, state);
    // QPixmap pixmap     = opt.icon.pixmap(q_ptr->window()->windowHandle(), realIconSize, mode, state);
    QPixmap res(pixmap.size() + QSize(4, c_ribbonbutton_color_height + 4));  // 宽度上，颜色块多出2px
    res.fill(Qt::transparent);
    QPainter painter(&res);
    int xpixmap = (res.width() - pixmap.width()) / 2;
    int ypixmap = (res.height() - c_ribbonbutton_color_height - 2 - pixmap.height())
                  / 2;  // 这里要减去2而不是1，这样奇数偶数都不会影响
    int w         = pixmap.width();
    int h         = pixmap.height();
    QRect rpixmap = QRect(xpixmap, ypixmap, w, h);
    painter.drawPixmap(rpixmap, pixmap);
    QRect colorRect = rpixmap.adjusted(0, h + 1, 0, c_ribbonbutton_color_height + 1);
    if (mColor.isValid()) {
        painter.fillRect(colorRect, mColor);
    } else {
        QPen pen(Qt::red, 1, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::Antialiasing, true);
        int ss = colorRect.width() / 3;
        painter.drawLine(QPoint(colorRect.x() + ss, colorRect.bottom()), QPoint(colorRect.right() - ss, colorRect.top()));
        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.drawRect(colorRect);
    }
    return res;
}

QIcon SARibbonColorToolButton::PrivateData::createColorIcon(const QColor& c, const QSize& size) const
{
    QPixmap res(size);
    res.fill(Qt::transparent);
    QPainter painter(&res);
    if (c.isValid()) {
        painter.fillRect(QRect(1, 1, res.height() - 2, res.width() - 2), c);
    } else {
        QPen pen(Qt::black, 1, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(pen);
        painter.drawRect(QRect(1, 1, res.height() - 2, res.width() - 2));
        pen.setColor(Qt::red);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawLine(QPoint(1, size.height()), QPoint(size.width() - 1, 1));
    }
    return QIcon(res);
}

//===================================================
// SARibbonColorToolButton
//===================================================

SARibbonColorToolButton::SARibbonColorToolButton(QWidget* parent)
    : SARibbonToolButton(parent), d_ptr(new SARibbonColorToolButton::PrivateData(this))
{
    connect(this, &QAbstractButton::clicked, this, &SARibbonColorToolButton::onButtonClicked);
}

SARibbonColorToolButton::SARibbonColorToolButton(QAction* defaultAction, QWidget* parent)
    : SARibbonToolButton(defaultAction, parent), d_ptr(new SARibbonColorToolButton::PrivateData(this))
{
    connect(this, &QAbstractButton::clicked, this, &SARibbonColorToolButton::onButtonClicked);
}

SARibbonColorToolButton::~SARibbonColorToolButton()
{
}

/**
 * \if ENGLISH
 * @brief Get the color maintained by the button
 * @return Current color
 * \endif
 *
 * \if CHINESE
 * @brief 获取按钮维护的颜色
 * @return 当前颜色
 * \endif
 */
QColor SARibbonColorToolButton::color() const
{
    return d_ptr->mColor;
}

/**
 * \if ENGLISH
 * @brief Set the color display style
 * @param s Color style to set
 * \endif
 *
 * \if CHINESE
 * @brief 设置颜色显示的样式
 * @param s 要设置的颜色样式
 * \endif
 */
void SARibbonColorToolButton::setColorStyle(SARibbonColorToolButton::ColorStyle s)
{
    if (d_ptr->mColorStyle == s) {
        return;
    }
    d_ptr->mColorStyle = s;
    if (ColorUnderIcon == s) {
        setIcon(d_ptr->mOldIcon);
    } else {
        d_ptr->mOldIcon = icon();
        setIcon(d_ptr->createColorIcon(d_ptr->mColor, QSize(32, 32)));
    }
    repaint();
}

/**
 * \if ENGLISH
 * @brief Get the color display style
 * @return Current color style
 * \endif
 *
 * \if CHINESE
 * @brief 颜色显示的样式
 * @return 当前颜色样式
 * \endif
 */
SARibbonColorToolButton::ColorStyle SARibbonColorToolButton::colorStyle() const
{
    return d_ptr->mColorStyle;
}

/**
 * \if ENGLISH
 * @brief Set up a standard color menu
 * @return Created SAColorMenu object
 * \endif
 *
 * \if CHINESE
 * @brief 建立标准的颜色菜单
 * @return 创建的 SAColorMenu 对象
 * \endif
 */
SAColorMenu* SARibbonColorToolButton::setupStandardColorMenu()
{
    setPopupMode(QToolButton::MenuButtonPopup);
    SAColorMenu* m = new SAColorMenu(this);
    m->enableNoneColorAction(true);
    QAction* customColor = m->customColorAction();
    if (customColor) {
        customColor->setIcon(QIcon(":/SARibbon/image/resource/define-color.svg"));
    }
    connect(m, &SAColorMenu::selectedColor, this, &SARibbonColorToolButton::setColor);
    setMenu(m);

    updateRect();
    return m;
}

/**
 * \if ENGLISH
 * @brief Set the color of the button
 * @param c Color to set
 * @note This will generate a new icon and emit the colorChanged signal
 * \endif
 *
 * \if CHINESE
 * @brief 设置按钮的颜色
 * @param c 要设置的颜色
 * @note 此时会生成一个新的icon，并发射 colorChanged 信号
 * \endif
 */
void SARibbonColorToolButton::setColor(const QColor& c)
{
    if (d_ptr->mColor != c) {
        d_ptr->mColor = c;
        if (ColorFillToIcon == colorStyle()) {
            setIcon(d_ptr->createColorIcon(c, QSize(32, 32)));
        }
        repaint();
        Q_EMIT colorChanged(c);
    }
}

void SARibbonColorToolButton::onButtonClicked(bool checked)
{
    Q_EMIT colorClicked(d_ptr->mColor, checked);
}

/**
 * \if ENGLISH
 * @brief Override paintIcon function to add color under the icon
 * @param p Painter to use for drawing
 * @param opt Style option for the tool button
 * @param iconDrawRect Rectangle where the icon should be drawn
 * \endif
 *
 * \if CHINESE
 * @brief 重写paintIcon函数，把颜色加到icon下面
 * @param p 用于绘制的painter
 * @param opt 工具按钮的样式选项
 * @param iconDrawRect 图标应该绘制的矩形区域
 * \endif
 */
void SARibbonColorToolButton::paintIcon(QPainter& p, const QStyleOptionToolButton& opt, const QRect& iconDrawRect)
{
    if (ColorUnderIcon == colorStyle()) {
        // 有icon
        QPixmap pm = d_ptr->createIconPixmap(opt, iconDrawRect.size());
        style()->drawItemPixmap(&p, iconDrawRect, Qt::AlignCenter, pm);
    } else {
        SARibbonToolButton::paintIcon(p, opt, iconDrawRect);
    }
}
