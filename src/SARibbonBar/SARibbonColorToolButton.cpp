#include "SARibbonColorToolButton.h"
#include <QStylePainter>
#include <QStyleOptionToolButton>
#include <QDebug>
#include "colorWidgets/SAColorMenu.h"
//===================================================
// SARibbonColorToolButton::PrivateData
//===================================================
const int c_ribbonbutton_color_height = 5;  ///< 颜色块的高度

class SARibbonColorToolButton::PrivateData
{
    SA_RIBBON_DECLARE_PUBLIC(SARibbonColorToolButton)
public:
    PrivateData(SARibbonColorToolButton* p);
    QPixmap createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconsize) const;
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
    QPixmap pixmap     = opt.icon.pixmap(q_ptr->window()->windowHandle(), realIconSize, mode, state);
    QPixmap res(pixmap.size() + QSize(4, c_ribbonbutton_color_height + 4));  // 宽度上，颜色块多出2px
    res.fill(Qt::transparent);
    QPainter painter(&res);
    int xpixmap = (res.width() - pixmap.width()) / 2;
    int ypixmap = (res.height() - c_ribbonbutton_color_height - 2 - pixmap.height()) / 2;  // 这里要减去2而不是1，这样奇数偶数都不会影响
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
 * @brief 获取按钮维护的颜色
 * @return
 */
QColor SARibbonColorToolButton::color() const
{
    return d_ptr->mColor;
}

/**
 * @brief 设置颜色显示的样式
 * @param s
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
 * @brief 颜色显示的样式
 * @return
 */
SARibbonColorToolButton::ColorStyle SARibbonColorToolButton::colorStyle() const
{
    return d_ptr->mColorStyle;
}

/**
 * @brief 建立标准的颜色菜单
 * @return
 */
SAColorMenu* SARibbonColorToolButton::setupStandardColorMenu()
{
    setPopupMode(QToolButton::MenuButtonPopup);
    SAColorMenu* m = new SAColorMenu(this);
    m->enableNoneColorAction(true);
    QAction* customColor = m->customColorAction();
    if (customColor) {
        customColor->setIcon(QIcon(":/image/resource/define-color.svg"));
    }
    connect(m, &SAColorMenu::selectedColor, this, &SARibbonColorToolButton::setColor);
    setMenu(m);

    updateRect();
    return m;
}

/**
 * @brief 设置按钮的颜色
 *
 * 此时会生成一个新的icon
 * @note 会发射@sa colorChanged 信号
 * @param c
 */
void SARibbonColorToolButton::setColor(const QColor& c)
{
    if (d_ptr->mColor != c) {
        d_ptr->mColor = c;
        if (ColorFillToIcon == colorStyle()) {
            setIcon(d_ptr->createColorIcon(c, QSize(32, 32)));
        }
        repaint();
        emit colorChanged(c);
    }
}

void SARibbonColorToolButton::onButtonClicked(bool checked)
{
    emit colorClicked(d_ptr->mColor, checked);
}

/**
 * @brief 重写paintIcon函数，把颜色加到icon下面
 * @param p
 * @param opt
 * @param iconDrawRect
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
