#ifndef RIBBONCOLORTOOLBUTTON_H
#define RIBBONCOLORTOOLBUTTON_H
#include "SARibbonQmlGlobal.h"
#include "../button/RibbonToolButton.h"
#include <QColor>
#include <QRectF>

namespace SARibbonQml {

class RibbonColorMenu;

/**
 * \if ENGLISH
 * @brief Color tool button host: a ribbon tool button that paints a color swatch
 * @details The QML counterpart of the widgets SARibbonColorToolButton. It is a
 *          RibbonToolButton, so the whole button layout — the icon / caption /
 *          indicator rects, the size hint, the hit-zone split — still comes from
 *          the one core algorithm; this class only adds the color rectangles on
 *          top of the icon slot the base already published. Both rendering
 *          styles are the widgets ones: ColorUnderIcon keeps the icon and puts a
 *          band underneath (core SA::calcColorUnderIconMetrics), ColorFillToIcon
 *          replaces the icon with the swatch, inset by the same 1-in-32 border
 *          the widgets generated color icon carries. An invalid color draws the
 *          "no color" mark, whose slash inset comes from core
 *          SA::noneColorSlashLine so the button and the color menu cannot drift.
 *          WithColorMenu (the default) builds a RibbonColorMenu child host and
 *          switches the popup mode to MenuButtonPopup, exactly what
 *          SARibbonColorToolButton::setupStandardColorMenu does; the picked
 *          color flows back through setColor. The menu host is reachable as
 *          colorMenu for the caller to configure — including the custom color
 *          row, whose customColorRequested() stands in for the widgets
 *          QColorDialog this module deliberately does not embed.
 *          One documented divergence from widgets: with ColorUnderIcon and no
 *          iconSource the widgets button bails out of createIconPixmap and paints
 *          no color at all, while this host reserves the icon slot anyway and
 *          lets the band span the full slot width. That is the "no icon" meaning
 *          core SA::calcColorUnderIconMetrics already defines, and it is what
 *          makes a text-only color button usable.
 * \endif
 *
 * \if CHINESE
 * @brief 颜色工具按钮宿主：绘制色块的 ribbon 工具按钮
 * @details 对应 widgets 侧的 SARibbonColorToolButton。它就是一个 RibbonToolButton，
 *          因此整套按钮布局——图标/文字/指示箭头矩形、sizeHint、命中区划分——仍然
 *          出自同一个 core 算法；本类只在基类已经发布的图标槽之上追加颜色矩形。
 *          两种绘制样式与 widgets 一致：ColorUnderIcon 保留图标、在其下方画色带
 *          （core 的 SA::calcColorUnderIconMetrics），ColorFillToIcon 用色块取代图标，
 *          内缩量与 widgets 生成的颜色图标所带的 1/32 边框相同。无效色绘制"无颜色"
 *          标记，其斜线内缩量取自 core 的 SA::noneColorSlashLine，按钮与颜色菜单因此
 *          不会各自漂移。WithColorMenu（默认）会构建一个 RibbonColorMenu 子宿主并把
 *          弹出模式切到 MenuButtonPopup，与 SARibbonColorToolButton::
 *          setupStandardColorMenu 的做法一致；选中的颜色经 setColor 回流。菜单宿主可
 *          通过 colorMenu 访问以便调用方配置——包括"自定义颜色"行，其
 *          customColorRequested() 代替了本模块刻意不内嵌的 widgets QColorDialog。
 *          与 widgets 有一处成文的差异：ColorUnderIcon 且未设 iconSource 时，widgets
 *          按钮在 createIconPixmap 里提前返回、根本不画颜色，本宿主则照样保留图标槽，
 *          让色带占满整个槽宽。这正是 core 的 SA::calcColorUnderIconMetrics 已经定义
 *          的"没有图标"语义，也是纯文字颜色按钮可用的前提。
 * \endif
 */
class SA_RIBBON_QML_EXPORT RibbonColorToolButton : public RibbonToolButton
{
    Q_OBJECT
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)
    Q_PROPERTY(bool hasValidColor READ hasValidColor NOTIFY colorChanged)
    Q_PROPERTY(RibbonEnums::ColorStyle colorStyle READ colorStyle WRITE setColorStyle NOTIFY colorStyleChanged)
    Q_PROPERTY(RibbonEnums::ColorMenuStyle colorMenuStyle READ colorMenuStyle WRITE setColorMenuStyle NOTIFY colorMenuStyleChanged)
    Q_PROPERTY(bool noneColorEnabled READ isNoneColorEnabled WRITE setNoneColorEnabled NOTIFY noneColorEnabledChanged)
    Q_PROPERTY(SARibbonQml::RibbonColorMenu* colorMenu READ colorMenu NOTIFY colorMenuChanged)
    Q_PROPERTY(QRectF colorRect READ colorRect NOTIFY colorGeometryChanged)
    Q_PROPERTY(QRectF iconDrawRect READ iconDrawRect NOTIFY colorGeometryChanged)
    Q_PROPERTY(int colorSlashInset READ colorSlashInset NOTIFY colorGeometryChanged)
public:
    explicit RibbonColorToolButton(QQuickItem* parent = nullptr);
    ~RibbonColorToolButton() override;

    // The displayed color; an invalid color draws the "no color" mark
    QColor color() const;
    void setColor(const QColor& c);
    bool hasValidColor() const;

    RibbonEnums::ColorStyle colorStyle() const;
    void setColorStyle(RibbonEnums::ColorStyle s);

    RibbonEnums::ColorMenuStyle colorMenuStyle() const;
    void setColorMenuStyle(RibbonEnums::ColorMenuStyle s);

    // "No color" entry of the owned color menu (widgets enableNoneColorAction)
    bool isNoneColorEnabled() const;
    void setNoneColorEnabled(bool on);

    /// The owned color menu, nullptr while colorMenuStyle is NoColorMenu
    RibbonColorMenu* colorMenu() const;

    // ---- color geometry publication (leaf renders, host computes) ----
    /// Swatch rectangle in this item's coordinates; invalid when nothing is painted
    QRectF colorRect() const;
    /// Where the icon goes in ColorUnderIcon mode; empty in ColorFillToIcon or without an icon
    QRectF iconDrawRect() const;
    /// Horizontal inset of the "no color" slash, relative to colorRect
    int colorSlashInset() const;

    bool hasMenu() const override;

    Q_INVOKABLE void openMenu() override;
    Q_INVOKABLE void closeMenu() override;

Q_SIGNALS:
    void colorChanged(const QColor& color);
    void colorStyleChanged();
    void colorMenuStyleChanged();
    void noneColorEnabledChanged();
    void colorMenuChanged();
    void colorGeometryChanged();

    /**
     * \if ENGLISH
     * @brief The action zone was clicked
     * @param color The color carried at click time
     * @param checked Checked state after the click
     * \endif
     *
     * \if CHINESE
     * @brief 动作区被点击
     * @param color 点击时刻携带的颜色
     * @param checked 点击之后的选中状态
     * \endif
     */
    void colorClicked(const QColor& color, bool checked);

protected:
    QUrl leafUrl() const override;
    void componentComplete() override;
    void updateLayout() override;
    SARibbon::Core::SARibbonToolButtonLayout::Input layoutInput() const override;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#else
    void geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry) override;
#endif

private:
    // Build (or drop) the RibbonColorMenu child host behind colorMenuStyle
    void ensureColorMenu();
    void destroyColorMenu();
    // Republish colorRect / iconDrawRect / colorSlashInset from the base geometry
    void updateColorGeometry();

    QColor mColor;  ///< widgets SARibbonColorToolButton::PrivateData::mColor: invalid until set, i.e. the "no color" mark
    RibbonEnums::ColorStyle mColorStyle       = RibbonEnums::ColorUnderIcon;
    RibbonEnums::ColorMenuStyle mColorMenuStyle = RibbonEnums::WithColorMenu;
    bool mNoneColorEnabled     = true;  ///< widgets setupStandardColorMenu enables it
    RibbonColorMenu* mColorMenu = nullptr;
    QRectF mColorRect;
    QRectF mIconDrawRect;
    int mColorSlashInset = 0;
};

}

#endif  // RIBBONCOLORTOOLBUTTON_H
