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
constexpr qreal COLOR_BLOCK_RATIO        = 0.25;  ///< 颜色块高度占图标高度的比例
constexpr int COLOR_BLOCK_MIN_HEIGHT     = 3;     ///< 颜色块最小高度
constexpr int COLOR_BLOCK_MARGIN         = 1;     ///< 颜色块边距
constexpr int DEFAULT_COLOR_ICON_SIZE    = 32;    ///< 默认颜色图标尺寸
constexpr int INVALID_COLOR_PEN_WIDTH    = 1;     ///< 无效颜色时边框线宽
constexpr int INVALID_COLOR_LINE_RATIO   = 3;     ///< 无效颜色对角线比例分母
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

    // Color pixmap cache (ColorUnderIcon mode only)
    // Cache key dimensions: icon cache key, size, mode, state, color
    mutable QPixmap mCachedColorPixmap;                            ///< 缓存的合成后 pixmap
    mutable QSize mCachedColorPixmapSize;                           ///< 缓存对应的图标尺寸
    mutable QIcon::Mode mCachedColorPixmapMode { QIcon::Normal };   ///< 缓存对应的模式
    mutable QIcon::State mCachedColorPixmapState { QIcon::Off };    ///< 缓存对应的状态
    mutable QColor mCachedColorPixmapColor;                         ///< 缓存对应的颜色
    mutable qint64 mCachedColorPixmapIconKey { 0 };                 ///< 缓存对应图标的 cacheKey
    mutable qreal mCachedColorPixmapDpr { 0 };                      ///< 缓存对应的devicePixelRatio
    mutable bool mColorPixmapCacheValid { false };                  ///< 缓存是否有效

    /// Invalidate the color pixmap cache so the next paint regenerates it
    void invalidateColorPixmapCache()
    {
        mColorPixmapCacheValid = false;
    }
};

SARibbonColorToolButton::PrivateData::PrivateData(SARibbonColorToolButton* p) : q_ptr(p)
{
}

QPixmap SARibbonColorToolButton::PrivateData::createIconPixmap(const QStyleOptionToolButton& opt, const QSize& iconsize) const
{
    namespace Constants = SARibbonColorToolButtonConstants;
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
    // Check color pixmap cache — key includes icon cache key, size, mode, state, color and dpr
    const qreal dpr = SA::widgetDevicePixelRatio(q_ptr);
    qint64 iconKey  = opt.icon.cacheKey();
    if (mColorPixmapCacheValid
        && mCachedColorPixmapSize == iconsize
        && mCachedColorPixmapMode == mode
        && mCachedColorPixmapState == state
        && mCachedColorPixmapColor == mColor
        && mCachedColorPixmapIconKey == iconKey
        && qFuzzyCompare(mCachedColorPixmapDpr + 1.0, dpr + 1.0)) {
        return mCachedColorPixmap;
    }
    // 颜色块高度随图标尺寸等比缩放，保证大按钮和小按钮下都清晰可见；同时不超过图标高度的一半
    // 注意：以下所有几何量均为逻辑像素。QPainter绘制在设置了devicePixelRatio的QPixmap上时
    // 会自动按dpr缩放，因此不能用设备像素坐标计算，否则高DPI下会被二次放大导致色块溢出被裁剪
    const int slotW = iconsize.width();
    const int slotH = iconsize.height();
    int colorHeight = qMax(Constants::COLOR_BLOCK_MIN_HEIGHT, qRound(slotH * Constants::COLOR_BLOCK_RATIO));
    colorHeight     = qMin(colorHeight, slotH / 2);
    const int margin         = Constants::COLOR_BLOCK_MARGIN;
    const int iconAreaHeight = slotH - colorHeight - margin;
    if (slotW <= 0 || iconAreaHeight <= 0) {
        return QPixmap();
    }
    QPixmap iconPm = SA::iconToPixmap(opt.icon, QSize(slotW, iconAreaHeight), dpr, mode, state);
    // 合成结果严格等于图标槽尺寸（含正确的devicePixelRatio），避免超出绘制区域被裁剪
    QPixmap res(qRound(slotW * dpr), qRound(slotH * dpr));
    res.setDevicePixelRatio(dpr);
    res.fill(Qt::transparent);
    QPainter painter(&res);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
    // 图标等比缩放到不超过上方区域（只缩小不放大），水平居中、底部与图标区域底边对齐
    int iconW = 0;
    int iconH = 0;
    if (!iconPm.isNull()) {
        const qreal pmDpr = iconPm.devicePixelRatio();
        QSizeF iconLogical(iconPm.width() / pmDpr, iconPm.height() / pmDpr);
        qreal fit          = qMin(slotW / iconLogical.width(), iconAreaHeight / iconLogical.height());
        fit                = qMin(fit, qreal(1.0));
        iconW              = qRound(iconLogical.width() * fit);
        iconH              = qRound(iconLogical.height() * fit);
        painter.drawPixmap(QRect((slotW - iconW) / 2, iconAreaHeight - iconH, iconW, iconH), iconPm);
    }
    // 色块位于图标正下方，上边缘与图标下边缘间隔margin像素
    int bandWidth = (iconW > 0) ? iconW : slotW;
    int bandX     = (slotW - bandWidth) / 2;
    QRectF colorRect(bandX, iconAreaHeight + margin, bandWidth, colorHeight);
    if (mColor.isValid()) {
        painter.fillRect(colorRect, mColor);
    } else {
        QPen pen(Qt::red, Constants::INVALID_COLOR_PEN_WIDTH, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::Antialiasing, true);
        qreal ss = colorRect.width() / Constants::INVALID_COLOR_LINE_RATIO;
        painter.drawLine(QPointF(colorRect.left() + ss, colorRect.bottom()),
                         QPointF(colorRect.right() - ss, colorRect.top()));
        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.drawRect(colorRect);
    }
    // Fill color pixmap cache
    mCachedColorPixmap        = res;
    mCachedColorPixmapSize    = iconsize;
    mCachedColorPixmapMode    = mode;
    mCachedColorPixmapState   = state;
    mCachedColorPixmapColor   = mColor;
    mCachedColorPixmapIconKey = iconKey;
    mCachedColorPixmapDpr     = dpr;
    mColorPixmapCacheValid    = true;
    return res;
}

QIcon SARibbonColorToolButton::PrivateData::createColorIcon(const QColor& c, const QSize& size) const
{
    namespace Constants = SARibbonColorToolButtonConstants;
    const qreal dpr = SA::widgetDevicePixelRatio(q_ptr);
    // 以设备像素创建并设置devicePixelRatio，保证高DPI下颜色图标清晰且尺寸正确
    QPixmap res(qRound(size.width() * dpr), qRound(size.height() * dpr));
    res.setDevicePixelRatio(dpr);
    res.fill(Qt::transparent);
    QPainter painter(&res);
    // QPainter在设置了devicePixelRatio的pixmap上已自动按dpr缩放，此处直接使用逻辑坐标
    QRectF colorRect(Constants::COLOR_BLOCK_MARGIN,
                     Constants::COLOR_BLOCK_MARGIN,
                     size.width() - 2 * Constants::COLOR_BLOCK_MARGIN,
                     size.height() - 2 * Constants::COLOR_BLOCK_MARGIN);
    if (c.isValid()) {
        painter.fillRect(colorRect, c);
    } else {
        QPen pen(Qt::black, Constants::INVALID_COLOR_PEN_WIDTH, Qt::SolidLine, Qt::RoundCap);
        painter.setPen(pen);
        painter.drawRect(colorRect);
        pen.setColor(Qt::red);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.drawLine(QPointF(Constants::COLOR_BLOCK_MARGIN, size.height() - Constants::COLOR_BLOCK_MARGIN),
                         QPointF(size.width() - Constants::COLOR_BLOCK_MARGIN, Constants::COLOR_BLOCK_MARGIN));
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
    d_ptr->invalidateColorPixmapCache();
    if (ColorUnderIcon == s) {
        SARibbonToolButton::invalidateIconCache();
        setIcon(d_ptr->mOldIcon);
    } else {
        d_ptr->mOldIcon = icon();
        SARibbonToolButton::invalidateIconCache();
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
    if (QMenu* oldMenu = menu()) {
        setMenu(nullptr);
        oldMenu->deleteLater();
    }
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
        d_ptr->invalidateColorPixmapCache();
        if (ColorFillToIcon == colorStyle()) {
            SARibbonToolButton::invalidateIconCache();
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
