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
namespace SARibbonColorToolButtonConstants
{
constexpr int COLOR_BLOCK_HEIGHT       = 5;   ///< 颜色块的高度
constexpr int COLOR_BLOCK_MARGIN       = 1;   ///< 颜色块边距
constexpr int COLOR_BLOCK_EXTRA_WIDTH  = 4;   ///< 颜色块额外宽度
constexpr int COLOR_BLOCK_EXTRA_HEIGHT = 4;   ///< 颜色块额外高度
constexpr int ICON_OFFSET_ADJUSTMENT   = 2;   ///< 图标偏移调整值
constexpr int DEFAULT_COLOR_ICON_SIZE  = 32;  ///< 默认颜色图标尺寸
constexpr int INVALID_COLOR_PEN_WIDTH  = 1;   ///< 无效颜色时边框线宽
constexpr int INVALID_COLOR_LINE_RATIO = 3;   ///< 无效颜色对角线比例分母
}

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
    QSize realIconSize = iconsize
                         - QSize(0,
                                 SARibbonColorToolButtonConstants::COLOR_BLOCK_HEIGHT
                                     + SARibbonColorToolButtonConstants::COLOR_BLOCK_MARGIN);
    QPixmap pixmap = SA::iconToPixmap(opt.icon, realIconSize, SA::widgetDevicePixelRatio(q_ptr), mode, state);
    // QPixmap pixmap     = opt.icon.pixmap(q_ptr->window()->windowHandle(), realIconSize, mode, state);
    QPixmap res(pixmap.size()
                + QSize(SARibbonColorToolButtonConstants::COLOR_BLOCK_EXTRA_WIDTH,
                        SARibbonColorToolButtonConstants::COLOR_BLOCK_EXTRA_HEIGHT
                            + SARibbonColorToolButtonConstants::COLOR_BLOCK_HEIGHT));
    res.fill(Qt::transparent);
    QPainter painter(&res);
    int xpixmap = (res.width() - pixmap.width()) / 2;
    int ypixmap = (res.height() - SARibbonColorToolButtonConstants::COLOR_BLOCK_HEIGHT
                   - SARibbonColorToolButtonConstants::ICON_OFFSET_ADJUSTMENT - pixmap.height())
                  / 2;
    int w         = pixmap.width();
    int h         = pixmap.height();
    QRect rpixmap = QRect(xpixmap, ypixmap, w, h);
    painter.drawPixmap(rpixmap, pixmap);
    QRect colorRect = rpixmap.adjusted(0,
                                       h + SARibbonColorToolButtonConstants::COLOR_BLOCK_MARGIN,
                                       0,
                                       SARibbonColorToolButtonConstants::COLOR_BLOCK_HEIGHT
                                           + SARibbonColorToolButtonConstants::COLOR_BLOCK_MARGIN);
    if (mColor.isValid()) {
        painter.fillRect(colorRect, mColor);
    } else {
        QPen pen(Qt::red, SARibbonColorToolButtonConstants::INVALID_COLOR_PEN_WIDTH, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::Antialiasing, true);
        int ss = colorRect.width() / SARibbonColorToolButtonConstants::INVALID_COLOR_LINE_RATIO;
        painter.drawLine(QPoint(colorRect.x() + ss, colorRect.bottom()), QPoint(colorRect.right() - ss, colorRect.top()));
        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.drawRect(colorRect);
    }
    return res;
}

QIcon SARibbonColorToolButton::PrivateData::createColorIcon(const QColor& c, const QSize& size) const
{
    // using Constants = SARibbonColorToolButtonConstants;
    QPixmap res(size);
    res.fill(Qt::transparent);
    QPainter painter(&res);
    QRect colorRect(SARibbonColorToolButtonConstants::COLOR_BLOCK_MARGIN,
                    SARibbonColorToolButtonConstants::COLOR_BLOCK_MARGIN,
                    res.height() - 2 * SARibbonColorToolButtonConstants::COLOR_BLOCK_MARGIN,
                    res.width() - 2 * SARibbonColorToolButtonConstants::COLOR_BLOCK_MARGIN);
    if (c.isValid()) {
        painter.fillRect(colorRect, c);
    } else {
        QPen pen(Qt::black, SARibbonColorToolButtonConstants::INVALID_COLOR_PEN_WIDTH, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(pen);
        painter.drawRect(colorRect);
        pen.setColor(Qt::red);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawLine(QPoint(SARibbonColorToolButtonConstants::COLOR_BLOCK_MARGIN,
                                size.height() - SARibbonColorToolButtonConstants::COLOR_BLOCK_MARGIN),
                         QPoint(size.width() - SARibbonColorToolButtonConstants::COLOR_BLOCK_MARGIN,
                                SARibbonColorToolButtonConstants::COLOR_BLOCK_MARGIN));
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
        setIcon(d_ptr->createColorIcon(d_ptr->mColor,
                                       QSize(SARibbonColorToolButtonConstants::DEFAULT_COLOR_ICON_SIZE,
                                             SARibbonColorToolButtonConstants::DEFAULT_COLOR_ICON_SIZE)));
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
    if (QAction* customColor = m->customColorAction()) {
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
            setIcon(d_ptr->createColorIcon(c,
                                           QSize(SARibbonColorToolButtonConstants::DEFAULT_COLOR_ICON_SIZE,
                                                 SARibbonColorToolButtonConstants::DEFAULT_COLOR_ICON_SIZE)));
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
 * @brief Override createIconPixmap function to add color under the icon
 * @param opt Style option for the tool button
 * @param iconSize Size of the icon
 * @return Pixmap with color under the icon
 * \endif
 *
 * \if CHINESE
 * @brief 重写createIconPixmap函数，把颜色加到icon下面
 * @param opt 工具按钮的样式选项
 * @param iconSize 图标尺寸
 * @return 带有颜色的图标pixmap
 * \endif
 */
QPixmap SARibbonColorToolButton::createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconSize) const
{
    if (ColorUnderIcon == colorStyle()) {
        // 在图标下方显示颜色
        return d_ptr->createIconPixmap(opt, iconSize);
    } else {
        // 使用父类的实现
        return SARibbonToolButton::createIconPixmap(opt, iconSize);
    }
}
