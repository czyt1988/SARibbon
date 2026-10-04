#include "SARibbonQmlColorToolButton.h"
#include "SARibbonQmlColorMenu.h"
#include "SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QRect>

namespace SARibbonQml {

namespace CTBC = SA::ColorToolButtonConstants;
using SARibbonToolButtonLayout = SARibbon::Core::SARibbonToolButtonLayout;

/**
 * \if ENGLISH
 * @brief Constructor
 * @details The popup mode follows SARibbonColorToolButton::setupStandardColorMenu,
 *          which turns the button into a MenuButtonPopup as soon as the standard
 *          color menu exists. setPopupMode also re-runs the size hint through the
 *          derived layoutInput(), which is what installs the "the icon slot is
 *          always reserved" rule — the base constructor's own pass still resolved
 *          to RibbonToolButton::layoutInput() because the derived vtable was not
 *          in place yet.
 * \endif
 *
 * \if CHINESE
 * @brief 构造函数
 * @details 弹出模式跟随 SARibbonColorToolButton::setupStandardColorMenu：标准颜色
 *          菜单一旦存在，按钮即成为 MenuButtonPopup。setPopupMode 同时会用派生版的
 *          layoutInput() 重算 sizeHint，"图标槽永远保留"这条规则由此生效——基类构造
 *          函数自己那一趟仍解析到 RibbonToolButton::layoutInput()，因为当时派生类
 *          虚表尚未就位。
 * \endif
 */
RibbonColorToolButton::RibbonColorToolButton(QQuickItem* parent) : RibbonToolButton(parent)
{
    setPopupMode(RibbonEnums::MenuButtonPopup);
    // widgets leaves the menu to an explicit setupStandardColorMenu() call; the
    // declarative front end owns it instead, so WithColorMenu (the default)
    // always has a menu object. Its visual leaf stays lazy: a host constructed
    // in C++ has no engine yet, and RibbonColorMenu::openMenu re-runs
    // ensureQmlLeaf once the button is really in a scene
    ensureColorMenu();
    connect(this, &RibbonToolButton::clicked, this, [this]() {
        Q_EMIT colorClicked(mColor, isChecked());
    });
}

/**
 * \if ENGLISH
 * @brief Destructor
 * \endif
 *
 * \if CHINESE
 * @brief 析构函数
 * \endif
 */
RibbonColorToolButton::~RibbonColorToolButton() = default;

QUrl RibbonColorToolButton::leafUrl() const
{
    return SARibbonQmlLeafUrls::colorToolButtonLeaf();
}

QColor RibbonColorToolButton::color() const
{
    return mColor;
}

void RibbonColorToolButton::setColor(const QColor& c)
{
    if (mColor == c) {
        return;
    }
    mColor = c;
    // no geometry depends on the color value itself, only on its validity, and
    // the validity flag is republished through the same notification
    Q_EMIT colorChanged(mColor);
}

bool RibbonColorToolButton::hasValidColor() const
{
    return mColor.isValid();
}

RibbonEnums::ColorStyle RibbonColorToolButton::colorStyle() const
{
    return mColorStyle;
}

void RibbonColorToolButton::setColorStyle(RibbonEnums::ColorStyle s)
{
    if (mColorStyle == s) {
        return;
    }
    mColorStyle = s;
    Q_EMIT colorStyleChanged();
    updateColorGeometry();
}

RibbonEnums::ColorMenuStyle RibbonColorToolButton::colorMenuStyle() const
{
    return mColorMenuStyle;
}

void RibbonColorToolButton::setColorMenuStyle(RibbonEnums::ColorMenuStyle s)
{
    if (mColorMenuStyle == s) {
        return;
    }
    mColorMenuStyle = s;
    Q_EMIT colorMenuStyleChanged();
    if (RibbonEnums::NoColorMenu == s) {
        destroyColorMenu();
        setMenuVisible(false);
    } else {
        ensureColorMenu();
    }
    // the indicator arrow appears/disappears with the menu, so both the hint and
    // the published rects move
    updateSizeHint();
    updateLayout();
}

bool RibbonColorToolButton::isNoneColorEnabled() const
{
    return mNoneColorEnabled;
}

void RibbonColorToolButton::setNoneColorEnabled(bool on)
{
    if (mNoneColorEnabled == on) {
        return;
    }
    mNoneColorEnabled = on;
    Q_EMIT noneColorEnabledChanged();
    if (mColorMenu) {
        mColorMenu->setNoneColorEnabled(on);
    }
}

RibbonColorMenu* RibbonColorToolButton::colorMenu() const
{
    return mColorMenu;
}

QRectF RibbonColorToolButton::colorRect() const
{
    return mColorRect;
}

QRectF RibbonColorToolButton::iconDrawRect() const
{
    return mIconDrawRect;
}

int RibbonColorToolButton::colorSlashInset() const
{
    return mColorSlashInset;
}

bool RibbonColorToolButton::hasMenu() const
{
    // the style flag, not the built menu: this predicate feeds layoutInput(),
    // which can run long before a QML engine exists to build the leaf
    return RibbonEnums::WithColorMenu == mColorMenuStyle;
}

void RibbonColorToolButton::openMenu()
{
    if (!isEnabled() || !hasMenu() || isMenuVisible()) {
        return;
    }
    ensureColorMenu();
    if (mColorMenu) {
        // the menu host drives its own Popup; its onOpened flips menuVisible back
        // onto this button through the connection ensureColorMenu() installed
        mColorMenu->openMenu();
        return;
    }
    setMenuVisible(true);  // headless fallback (no engine, e.g. tests)
}

void RibbonColorToolButton::closeMenu()
{
    if (!isMenuVisible()) {
        return;
    }
    if (mColorMenu) {
        mColorMenu->closeMenu();
        return;
    }
    setMenuVisible(false);
}

void RibbonColorToolButton::componentComplete()
{
    RibbonToolButton::componentComplete();
    ensureColorMenu();
    if (mColorMenu) {
        // the menu was built in the constructor, before any engine context
        // existed; its leaf can only be created from here on
        mColorMenu->ensureQmlLeaf();
    }
    // the base constructor computed its hint with the base layoutInput(); redo it
    // now that the derived override is in effect
    updateSizeHint();
    updateLayout();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void RibbonColorToolButton::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonToolButton::geometryChange(newGeometry, oldGeometry);
#else
void RibbonColorToolButton::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonToolButton::geometryChanged(newGeometry, oldGeometry);
#endif
    if (mColorMenu) {
        // the popup drops out at (0, root.height) of the menu leaf, so the menu
        // host has to cover this button for the popup to land below it
        mColorMenu->setSize(newGeometry.size());
    }
}

/**
 * \if ENGLISH
 * @brief Reserve the icon slot unconditionally
 * @details A color button always paints something into that slot: the icon in
 *          ColorUnderIcon mode, the generated swatch in ColorFillToIcon mode. The
 *          widgets button reaches the same result by swapping a real QIcon in and
 *          out (mOldIcon); here the swap is unnecessary because the leaf renders
 *          the swatch directly, so the layout input just reports an icon present.
 *          Without it a text-only color button would collapse to Qt::ToolButtonTextOnly
 *          and lose the slot the band is drawn into.
 * \endif
 *
 * \if CHINESE
 * @brief 无条件保留图标槽
 * @details 颜色按钮总会在该槽里画东西：ColorUnderIcon 模式画图标，ColorFillToIcon
 *          模式画生成的色块。widgets 按钮靠换进换出一个真正的 QIcon（mOldIcon）达到
 *          同样效果；这里不需要换，因为叶子直接绘制色块，所以布局输入只需声明"有图标"。
 *          否则纯文字的颜色按钮会退化成 Qt::ToolButtonTextOnly，连带丢掉色带要画进去的槽。
 * \endif
 */
SARibbonToolButtonLayout::Input RibbonColorToolButton::layoutInput() const
{
    SARibbonToolButtonLayout::Input in = RibbonToolButton::layoutInput();
    in.hasIcon                         = true;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    in.toolButtonStyle = Qt::ToolButtonTextBesideIcon;
#endif
    return in;
}

void RibbonColorToolButton::updateLayout()
{
    RibbonToolButton::updateLayout();
    updateColorGeometry();
}

/**
 * \if ENGLISH
 * @brief Build the color menu child host
 * @details Created as an item child of this button so createVisualLeaf() finds the
 *          engine through the parent chain and the Popup has a window, and sized to
 *          this button so the popup opens right below it. The two connections are
 *          the whole integration: a picked color becomes the button color, and the
 *          menu's own visibility flag is mirrored onto the inherited menuVisible so
 *          the base hit-zone logic and the tests keep reading one property.
 * \endif
 *
 * \if CHINESE
 * @brief 构建颜色菜单子宿主
 * @details 作为本按钮的 item 子项创建，createVisualLeaf() 由此沿父子链找到引擎，
 *          Popup 也有窗口可挂；尺寸撑到与本按钮一致，弹窗因此正好落在按钮下方。
 *          两个连接就是全部的集成：选中的颜色变成按钮颜色；菜单自己的可见标志镜像到
 *          继承而来的 menuVisible，基类的命中区逻辑与测试因此始终只读一个属性。
 * \endif
 */
void RibbonColorToolButton::ensureColorMenu()
{
    if (mColorMenu || RibbonEnums::WithColorMenu != mColorMenuStyle) {
        return;
    }
    mColorMenu = new RibbonColorMenu(this);
    mColorMenu->setObjectName(QStringLiteral("colorMenu"));
    mColorMenu->setSize(size());
    mColorMenu->setNoneColorEnabled(mNoneColorEnabled);
    connect(mColorMenu, &RibbonColorMenu::selectedColor, this, [this](const QColor& c) {
        setColor(c);
    });
    connect(mColorMenu, &RibbonColorMenu::menuVisibleChanged, this, [this]() {
        if (mColorMenu) {
            setMenuVisible(mColorMenu->isMenuVisible());
        }
    });
    // Forwarded so the QML side never has to dig into colorMenu for the one
    // interaction the host cannot serve itself (the color dialog)
    connect(mColorMenu, &RibbonColorMenu::customColorRequested, this, &RibbonColorToolButton::customColorRequested);
    Q_EMIT colorMenuChanged();
}

void RibbonColorToolButton::destroyColorMenu()
{
    if (!mColorMenu) {
        return;
    }
    RibbonColorMenu* old = mColorMenu;
    mColorMenu           = nullptr;
    // the documented host teardown: unparent, then defer the delete
    old->setParentItem(nullptr);
    old->deleteLater();
    Q_EMIT colorMenuChanged();
}

/**
 * \if ENGLISH
 * @brief Derive the color rectangles from the icon slot the base published
 * @details ColorUnderIcon hands the slot to core SA::calcColorUnderIconMetrics —
 *          the very function the widgets icon painter's placement step uses — with
 *          the fitted icon size the widgets side ends up with: SA::iconToPixmap is
 *          QIcon::pixmap, which returns exactly the requested box, so the down-only
 *          fit factor is 1.0 and the icon box is the whole reduced area. Passing an
 *          empty fitted size (no iconSource) makes core give the band the full slot
 *          width. ColorFillToIcon instead uses the natural icon box the base
 *          already computed and insets it by the 1-in-32 border the widgets
 *          generated color icon carries, scaled to the rendered side. The slash
 *          inset is core SA::noneColorSlashLine evaluated on the band, so this
 *          button and RibbonColorMenu publish the same number for the same box.
 * \endif
 *
 * \if CHINESE
 * @brief 由基类发布的图标槽推导颜色矩形
 * @details ColorUnderIcon 把槽交给 core 的 SA::calcColorUnderIconMetrics——正是
 *          widgets 图标绘制摆放那一步用的函数——并喂给它 widgets 侧最终得到的图标尺寸：
 *          SA::iconToPixmap 就是 QIcon::pixmap，返回的正是请求的盒子，所以只缩不放的
 *          适配系数恒为 1.0，图标盒即整个缩小后的区域。传入空的图标尺寸（没有
 *          iconSource）时，core 会让色带占满整个槽宽。ColorFillToIcon 则改用基类已经
 *          算好的自然图标盒，并按 widgets 生成的颜色图标所带的 1/32 边框、依渲染边长等比
 *          内缩。斜线内缩量由 core 的 SA::noneColorSlashLine 在该色带上求得，因此本按钮
 *          与 RibbonColorMenu 对同样的盒子发布同样的数字。
 * \endif
 */
void RibbonColorToolButton::updateColorGeometry()
{
    const QRectF slot = iconGeometry();
    const int slotW   = int(slot.width());
    const int slotH   = int(slot.height());
    QRectF newColor;
    QRectF newIconDraw;
    if (slotW > 0 && slotH > 0) {
        if (RibbonEnums::ColorFillToIcon == mColorStyle) {
            const int side = iconSide();
            if (side > 0) {
                // the natural icon box: centred in the slot, side x side
                const QRectF natural(slot.x() + (slot.width() - side) / 2.0, slot.y() + (slot.height() - side) / 2.0, side, side);
                // widgets paints createColorIcon into QRectF(1, 1, 30, 30) of a
                // 32x32 icon and the button then scales that icon to `side`, so
                // the border scales with it
                const qreal inset = CTBC::COLOR_BLOCK_MARGIN * qreal(side) / qreal(CTBC::DEFAULT_COLOR_ICON_SIZE);
                newColor          = natural.adjusted(inset, inset, -inset, -inset);
            }
        } else {
            const int colorHeight    = SA::colorBandHeight(slotH);
            const int iconAreaHeight = slotH - colorHeight - CTBC::COLOR_BLOCK_MARGIN;
            if (colorHeight > 0 && iconAreaHeight > 0) {
                // no icon: an empty fitted size, which core reads as "band spans
                // the whole slot width" (the documented divergence)
                const QSize fitted               = iconSource().isEmpty() ? QSize() : QSize(slotW, iconAreaHeight);
                const SA::ColorUnderIconMetrics m = SA::calcColorUnderIconMetrics(QSize(slotW, slotH), fitted);
                if (m.colorRect.isValid()) {
                    newColor = QRectF(m.colorRect).translated(slot.x(), slot.y());
                }
                if (m.iconRect.isValid()) {
                    newIconDraw = QRectF(m.iconRect).translated(slot.x(), slot.y());
                }
            }
        }
    }
    int newInset = 0;
    if (newColor.width() > 0 && newColor.height() > 0) {
        newInset = SA::noneColorSlashLine(QRect(0, 0, int(newColor.width()), int(newColor.height()))).x1();
    }
    if (newColor != mColorRect || newIconDraw != mIconDrawRect || newInset != mColorSlashInset) {
        mColorRect       = newColor;
        mIconDrawRect    = newIconDraw;
        mColorSlashInset = newInset;
        Q_EMIT colorGeometryChanged();
    }
}

}
