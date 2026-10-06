#include "SARibbonQmlToolButton.h"
#include "SARibbonQmlMenuItem.h"
#include "SARibbonQmlPanel.h"
#include "SARibbonQmlMetrics.h"
#include "SARibbonQmlTheme.h"
#include "SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QRectF>

namespace SARibbonQml {

namespace TBLC = SARibbon::Core::ToolButtonLayoutConstants;
using SARibbonToolButtonLayout = SARibbon::Core::SARibbonToolButtonLayout;

RibbonToolButton::RibbonToolButton(QQuickItem* parent) : RibbonLayoutItemHost(parent)
{
    // contract field default follows the 2.x createItem behavior (Large)
    rowProportion = SARibbon::Core::SARibbonRowProportion::Large;
    // RTL flip re-mirrors the menu hit strip (SA::saIsRTL() re-read). The
    // rect math runs directly — polish delivery to items deep in panel-
    // driven geometry proved unreliable in offscreen tests (round-6 trace:
    // the panel's updatePolish delivered, the button's never did)
    connect(RibbonTheme::instance(), &RibbonTheme::rtlChanged, this, [this]() {
        updateLayout();
        polish();
    });
    updateSizeHint();
}

RibbonToolButton::~RibbonToolButton()
{
}

QUrl RibbonToolButton::leafUrl() const
{
    return SARibbonQmlLeafUrls::toolButtonLeaf();
}

QString RibbonToolButton::text() const
{
    return mText;
}

void RibbonToolButton::setText(const QString& t)
{
    if (mText == t) {
        return;
    }
    mText = t;
    Q_EMIT textChanged();
    updateSizeHint();
    updateLayout();
}

QString RibbonToolButton::iconSource() const
{
    return mIconSource;
}

void RibbonToolButton::setIconSource(const QString& s)
{
    if (mIconSource == s) {
        return;
    }
    mIconSource = s;
    Q_EMIT iconSourceChanged();
    updateSizeHint();
    updateLayout();
}

RibbonEnums::RowProportion RibbonToolButton::proportion() const
{
    switch (rowProportion) {  // the contract base field
    case SARibbon::Core::SARibbonRowProportion::None:
        return RibbonEnums::None;
    case SARibbon::Core::SARibbonRowProportion::Large:
        return RibbonEnums::Large;
    case SARibbon::Core::SARibbonRowProportion::Medium:
        return RibbonEnums::Medium;
    case SARibbon::Core::SARibbonRowProportion::Small:
        return RibbonEnums::Small;
    }
    return RibbonEnums::Large;
}

void RibbonToolButton::setProportion(RibbonEnums::RowProportion rp)
{
    SARibbon::Core::SARibbonRowProportion coreRp;
    switch (rp) {
    case RibbonEnums::None:
        coreRp = SARibbon::Core::SARibbonRowProportion::None;
        break;
    case RibbonEnums::Large:
        coreRp = SARibbon::Core::SARibbonRowProportion::Large;
        break;
    case RibbonEnums::Medium:
        coreRp = SARibbon::Core::SARibbonRowProportion::Medium;
        break;
    case RibbonEnums::Small:
        coreRp = SARibbon::Core::SARibbonRowProportion::Small;
        break;
    default:
        return;
    }
    if (rowProportion == coreRp) {
        return;
    }
    rowProportion = coreRp;
    Q_EMIT proportionChanged();
    updateSizeHint();
    updateLayout();
}

bool RibbonToolButton::isCheckable() const
{
    return mCheckable;
}

void RibbonToolButton::setCheckable(bool on)
{
    if (mCheckable == on) {
        return;
    }
    mCheckable = on;
    Q_EMIT checkableChanged();
}

bool RibbonToolButton::isChecked() const
{
    return mChecked;
}

void RibbonToolButton::setChecked(bool on)
{
    if (mChecked == on) {
        return;
    }
    mChecked = on;
    Q_EMIT checkedChanged();
    Q_EMIT toggled(mChecked);
}

bool RibbonToolButton::isWordWrap() const
{
    return mWordWrap;
}

void RibbonToolButton::setWordWrap(bool on)
{
    if (mWordWrap == on) {
        return;
    }
    mWordWrap = on;
    Q_EMIT wordWrapChanged();
    updateSizeHint();
    updateLayout();
}

bool RibbonToolButton::isIconRightText() const
{
    return mIconRightText;
}

void RibbonToolButton::setIconRightText(bool on)
{
    if (mIconRightText == on) {
        return;
    }
    mIconRightText = on;
    Q_EMIT iconRightTextChanged();
    updateSizeHint();
    updateLayout();
}

bool RibbonToolButton::isFlat() const
{
    return mFlat;
}

void RibbonToolButton::setFlat(bool on)
{
    if (mFlat == on) {
        return;
    }
    mFlat = on;
    Q_EMIT flatChanged();
}

RibbonEnums::ToolButtonStyle RibbonToolButton::toolButtonStyle() const
{
    return mToolButtonStyle;
}

void RibbonToolButton::setToolButtonStyle(RibbonEnums::ToolButtonStyle style)
{
    if (mToolButtonStyleSet && mToolButtonStyle == style) {
        return;
    }
    mToolButtonStyle    = style;
    mToolButtonStyleSet = true;
    Q_EMIT toolButtonStyleChanged();
    updateSizeHint();
    updateLayout();
}

bool RibbonToolButton::isTitleRow() const
{
    return mTitleRow;
}

void RibbonToolButton::setTitleRow(bool on)
{
    if (mTitleRow == on) {
        return;
    }
    mTitleRow = on;
    // the context changes both the effective type (proportion becomes
    // meaningless: toolbar rendering) and the unset style default, so the
    // sizeHint and the published draw geometry both need a recompute
    updateSizeHint();
    updateLayout();
}

/**
 * \if ENGLISH
 * @brief Resolve the core toolButtonStyle from the host state
 * @details Priority: an explicitly written property always wins; an unset
 *          button follows the rendering context (panel = TextBesideIcon, the
 *          widgets SARibbonToolButton look; title-row container = IconOnly,
 *          the widgets QToolBar look). Two fallbacks keep a button from ever
 *          rendering blank: an IconOnly button without an icon renders its
 *          text instead, and (Qt6 parity with QToolButton::initStyleOption)
 *          a TextBesideIcon button without an icon downgrades to TextOnly so
 *          no empty icon slot is reserved. Qt5 leaves TextBesideIcon alone,
 *          mirroring the widgets side.
 * \endif
 *
 * \if CHINESE
 * @brief 由宿主状态解析出 core 侧的 toolButtonStyle
 * @details 优先级：显式写入的属性永远生效；未设置的按钮跟随渲染上下文
 *          （面板 = TextBesideIcon，即 widgets SARibbonToolButton 的观感；
 *          标题行容器 = IconOnly，即 widgets QToolBar 的观感）。两个回退保证
 *          按钮绝不渲染成空白：无图标的 IconOnly 按钮改渲染文字；无图标的
 *          TextBesideIcon 按钮降级为 TextOnly（Qt6 与
 *          QToolButton::initStyleOption 对齐，不预留空图标位）。Qt5 保留
 *          TextBesideIcon 不降级，与 widgets 侧行为一致。
 * \endif
 */
Qt::ToolButtonStyle RibbonToolButton::effectiveToolButtonStyle() const
{
    Qt::ToolButtonStyle style = Qt::ToolButtonTextBesideIcon;
    if (mToolButtonStyleSet) {
        switch (mToolButtonStyle) {
        case RibbonEnums::IconOnly:
            style = Qt::ToolButtonIconOnly;
            break;
        case RibbonEnums::TextOnly:
            style = Qt::ToolButtonTextOnly;
            break;
        case RibbonEnums::TextBesideIcon:
            style = Qt::ToolButtonTextBesideIcon;
            break;
        case RibbonEnums::TextUnderIcon:
            style = Qt::ToolButtonTextUnderIcon;
            break;
        }
    } else if (mTitleRow) {
        // title-row containers render their buttons toolbar-style: icon only
        // (text takes over below when the button carries no icon)
        style = Qt::ToolButtonIconOnly;
    }
    const bool hasIcon = !mIconSource.isEmpty();
    if (Qt::ToolButtonIconOnly == style && !hasIcon && !mText.isEmpty()) {
        // icon-less fallback: an IconOnly button without an icon would render
        // blank — the caption takes the slot instead (toolbar parity: a
        // text-only entry still shows in a QToolBar)
        style = Qt::ToolButtonTextOnly;
    }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // Qt6 QToolButton::initStyleOption downgrades TextBesideIcon to TextOnly
    // when the button carries no icon, and the widgets side hands core exactly
    // that value; mirroring it keeps an icon-less small button from reserving
    // an empty icon slot (Qt5 leaves TextBesideIcon alone, hence the split)
    if (Qt::ToolButtonTextBesideIcon == style && !hasIcon) {
        style = Qt::ToolButtonTextOnly;
    }
#endif
    return style;
}

int RibbonToolButton::spacing() const
{
    return mSpacing;
}

void RibbonToolButton::setSpacing(int v)
{
    if (mSpacing == v) {
        return;
    }
    mSpacing = v;
    Q_EMIT spacingChanged();
    updateSizeHint();
    updateLayout();
}

qreal RibbonToolButton::twoLineHeightFactor() const
{
    return mFactors.twoLineHeightFactor;
}

void RibbonToolButton::setTwoLineHeightFactor(qreal v)
{
    if (qFuzzyCompare(mFactors.twoLineHeightFactor, v)) {
        return;
    }
    mFactors.twoLineHeightFactor = v;
    Q_EMIT layoutFactorsChanged();
    updateSizeHint();
    updateLayout();
}

qreal RibbonToolButton::oneLineHeightFactor() const
{
    return mFactors.oneLineHeightFactor;
}

void RibbonToolButton::setOneLineHeightFactor(qreal v)
{
    if (qFuzzyCompare(mFactors.oneLineHeightFactor, v)) {
        return;
    }
    mFactors.oneLineHeightFactor = v;
    Q_EMIT layoutFactorsChanged();
    updateSizeHint();
    updateLayout();
}

qreal RibbonToolButton::buttonMaximumAspectRatio() const
{
    return mFactors.buttonMaximumAspectRatio;
}

void RibbonToolButton::setButtonMaximumAspectRatio(qreal v)
{
    if (qFuzzyCompare(mFactors.buttonMaximumAspectRatio, v)) {
        return;
    }
    mFactors.buttonMaximumAspectRatio = v;
    Q_EMIT layoutFactorsChanged();
    updateSizeHint();
    updateLayout();
}

qreal RibbonToolButton::largeButtonMinimumWidthRatio() const
{
    return mFactors.largeButtonMinimumWidthRatio;
}

void RibbonToolButton::setLargeButtonMinimumWidthRatio(qreal v)
{
    // zero and negatives are meaningful here (they drop the height-based
    // minimum width), so the comparison must tolerate them: qFuzzyCompare
    // misbehaves around zero, hence the explicit zero test
    const bool same = (qFuzzyIsNull(mFactors.largeButtonMinimumWidthRatio) && qFuzzyIsNull(v))
                      || qFuzzyCompare(mFactors.largeButtonMinimumWidthRatio, v);
    if (same) {
        return;
    }
    mFactors.largeButtonMinimumWidthRatio = v;
    Q_EMIT layoutFactorsChanged();
    updateSizeHint();
    updateLayout();
}

QSize RibbonToolButton::smallIconSize() const
{
    return mSmallIconSize;
}

void RibbonToolButton::setSmallIconSize(const QSize& size)
{
    if (mSmallIconSize == size) {
        return;
    }
    mSmallIconSize = size;
    Q_EMIT iconSizesChanged();
    updateSizeHint();
    updateLayout();
}

QSize RibbonToolButton::largeIconSize() const
{
    return mLargeIconSize;
}

void RibbonToolButton::setLargeIconSize(const QSize& size)
{
    if (mLargeIconSize == size) {
        return;
    }
    mLargeIconSize = size;
    Q_EMIT iconSizesChanged();
    updateSizeHint();
    updateLayout();
}

QString RibbonToolButton::toolTip() const
{
    return mToolTip;
}

void RibbonToolButton::setToolTip(const QString& t)
{
    if (mToolTip == t) {
        return;
    }
    mToolTip = t;
    Q_EMIT toolTipChanged();
}

RibbonEnums::PopupMode RibbonToolButton::popupMode() const
{
    return mPopupMode;
}

void RibbonToolButton::setPopupMode(RibbonEnums::PopupMode mode)
{
    if (mPopupMode == mode) {
        return;
    }
    mPopupMode = mode;
    Q_EMIT popupModeChanged();
    updateSizeHint();
    updateLayout();
}

QQmlListProperty< RibbonMenuItem > RibbonToolButton::menuItems()
{
    return QQmlListProperty< RibbonMenuItem >(this, this, &RibbonToolButton::appendMenuItem, &RibbonToolButton::menuItemCountCb,
                                              &RibbonToolButton::menuItemAtCb, &RibbonToolButton::clearMenuItems);
}

int RibbonToolButton::menuItemCount() const
{
    return mMenuItems.size();
}

RibbonMenuItem* RibbonToolButton::menuItemAt(int index) const
{
    return (index >= 0 && index < mMenuItems.size()) ? mMenuItems[ index ] : nullptr;
}

bool RibbonToolButton::isMenuVisible() const
{
    return mMenuVisible;
}

bool RibbonToolButton::hasMenu() const
{
    return !mMenuItems.isEmpty();
}

QRectF RibbonToolButton::actionRect() const
{
    return mActionRect;
}

QRectF RibbonToolButton::menuRect() const
{
    return mMenuRect;
}

bool RibbonToolButton::isLargeType() const
{
    // iconRightText forces the small rendering regardless of the proportion
    // (widgets PrivateData::effectiveButtonType parity); so does the title-row
    // context — quick access bar / right group buttons are toolbar buttons
    // there, the proportion carries no meaning (widgets parity: their quick
    // access buttons are plain QToolButtons, never SARibbonToolButtons)
    return (SARibbon::Core::SARibbonRowProportion::Large == rowProportion) && !mIconRightText && !mTitleRow;
}

QString RibbonToolButton::displayText() const
{
    return mDisplayText;
}

QRectF RibbonToolButton::iconGeometry() const
{
    return mIconGeometry;
}

QRectF RibbonToolButton::textGeometry() const
{
    return mTextGeometry;
}

QRectF RibbonToolButton::indicatorGeometry() const
{
    return mIndicatorGeometry;
}

bool RibbonToolButton::isTextWordWrap() const
{
    // the caption box is a top-aligned two-line budget whenever word wrap is
    // on for a large button (widgets getTextAlignment keys off enableWordWrap,
    // not off the binary-search wrap verdict)
    return isLargeType() && mWordWrap;
}

int RibbonToolButton::iconSide() const
{
    const QSize origin = isLargeType() ? mLargeIconSize : mSmallIconSize;
    const QSize fit    = SARibbonToolButtonLayout::adjustIconSize(mIconGeometry.toRect(), origin);
    return qMax(qMin(fit.width(), fit.height()), 0);
}

void RibbonToolButton::click()
{
    // a disabled host swallows the click (widgets: disabled QAction/toolbutton)
    if (!isEnabled()) {
        return;
    }
    if (mCheckable) {
        setChecked(!mChecked);
    }
    Q_EMIT clicked();
}

void RibbonToolButton::openMenu()
{
    if (!isEnabled() || mMenuItems.isEmpty() || mMenuVisible) {
        return;
    }
    QQuickItem* leaf = qmlLeaf();
    // bare member name: the Qt6 overload appends "()" and finds the QML
    // function (verified by qml_Conformance::toolButtonPopupStates). On Qt5
    // builds where the bare-name lookup fails, invokeMethod returns false and
    // the headless fallback below keeps menuVisible functional.
    if (leaf && QMetaObject::invokeMethod(leaf, "openMenu")) {
        // the leaf opened the popup; its onOpened callback flips menuVisible
        return;
    }
    setMenuVisible(true);  // headless fallback (no rendered leaf, e.g. tests)
}

void RibbonToolButton::closeMenu()
{
    if (!mMenuVisible) {
        return;
    }
    QQuickItem* leaf = qmlLeaf();
    if (leaf && QMetaObject::invokeMethod(leaf, "closeMenu")) {
        return;
    }
    setMenuVisible(false);
}

void RibbonToolButton::activateMenuItem(int index)
{
    QVariantList path;
    path.append(index);
    activateMenuItemPath(path);
}

/**
 * \if ENGLISH
 * @brief Activate the entry an index path addresses
 * @details The leaf hands over a path instead of an item pointer: the host
 *          stays the only place that knows the menu tree (widgets side knows it
 *          through QAction parenting). A checkable entry flips its own state
 *          before menuTriggered fires, so a handler observes the new checked
 *          value — QAction::trigger ordering. Separators, disabled entries and
 *          bad paths are refused silently, and a successful activation closes
 *          the popup exactly like the flat index overload always did.
 * \endif
 *
 * \if CHINESE
 * @brief 激活索引路径指向的菜单项
 * @details 叶子交上来的是路径而不是菜单项指针：认识整棵菜单树的地方仍然只有宿主
 *          一个（widgets 侧靠 QAction 父子关系认识它）。可勾选的菜单项在
 *          menuTriggered 之前翻转自身状态，因此槽函数看到的是新的 checked 值——
 *          与 QAction::trigger 的顺序一致。分隔符、禁用项与非法路径一律静默拒绝；
 *          激活成功后关闭弹窗，与一直以来的单下标重载行为相同。
 * \endif
 */
void RibbonToolButton::activateMenuItemPath(const QVariantList& indexPath)
{
    RibbonMenuItem* item = RibbonMenuItem::resolvePath(mMenuItems, indexPath);
    if (!item || !item->isEnabled() || item->isSeparator()) {
        return;
    }
    item->activate();
    Q_EMIT menuTriggered(item);
    closeMenu();
}

QSize RibbonToolButton::sizeHint() const
{
    return mCachedSizeHint;
}

void RibbonToolButton::componentComplete()
{
    RibbonLayoutItemHost::componentComplete();
    ensureQmlLeaf();
    updateLayout();
}

void RibbonToolButton::updatePolish()
{
    updateLayout();  // RTL flip re-mirrors the menu strip
}

void RibbonToolButton::largeHeightContextChanged()
{
    updateSizeHint();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void RibbonToolButton::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonLayoutItemHost::geometryChange(newGeometry, oldGeometry);
#else
void RibbonToolButton::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonLayoutItemHost::geometryChanged(newGeometry, oldGeometry);
#endif
    if (newGeometry.size() != oldGeometry.size()) {
        updateLayout();
    }
}

// ---- menu list property callbacks ----
void RibbonToolButton::appendMenuItem(QQmlListProperty< RibbonMenuItem >* prop, RibbonMenuItem* item)
{
    auto* self = static_cast< RibbonToolButton* >(prop->data);
    if (self && item && !self->mMenuItems.contains(item)) {
        self->mMenuItems.append(item);
        item->setParent(self);
        self->emitMenuItemsChanged();
    }
}

RibbonToolButton::ListIndex RibbonToolButton::menuItemCountCb(QQmlListProperty< RibbonMenuItem >* prop)
{
    auto* self = static_cast< RibbonToolButton* >(prop->data);
    return self ? self->mMenuItems.size() : RibbonToolButton::ListIndex(0);
}

RibbonMenuItem* RibbonToolButton::menuItemAtCb(QQmlListProperty< RibbonMenuItem >* prop, ListIndex index)
{
    auto* self = static_cast< RibbonToolButton* >(prop->data);
    return self ? self->menuItemAt(int(index)) : nullptr;
}

void RibbonToolButton::clearMenuItems(QQmlListProperty< RibbonMenuItem >* prop)
{
    auto* self = static_cast< RibbonToolButton* >(prop->data);
    if (self && !self->mMenuItems.isEmpty()) {
        self->mMenuItems.clear();
        self->emitMenuItemsChanged();
    }
}

void RibbonToolButton::emitMenuItemsChanged()
{
    Q_EMIT menuItemsChanged();
    updateSizeHint();
    updateLayout();
}

void RibbonToolButton::setMenuVisible(bool on)
{
    if (mMenuVisible == on) {
        return;
    }
    mMenuVisible = on;
    Q_EMIT menuVisibleChanged();
}

void RibbonToolButton::updateSizeHint()
{
    // iron rule (plan-04 S5-1): sizeHint derives in C++ from core metrics,
    // NEVER from the QML leaf's implicit sizes
    mCachedSizeHint = computeSizeHintFromMetrics();
    setImplicitWidth(mCachedSizeHint.width());
    setImplicitHeight(mCachedSizeHint.height());
    // hint changed: drop the stale engine cache entry and re-run the panel layout
    invalidatePanelLayout();
}

QSize RibbonToolButton::computeSizeHintFromMetrics()
{
    // the very same core algorithm the widgets button runs (NOTES B49): the
    // large two-line text budget, the aspect-ratio cap and the binary-searched
    // wrap width all come from SARibbon::Core::SARibbonToolButtonLayout, so a
    // QML button and a widget button with equal text/font/panel height hint
    // the same size
    const SARibbonToolButtonLayout::SizeHintResult r = SARibbonToolButtonLayout::calcSizeHint(layoutInput());
    mIsTextNeedWrap = r.isTextNeedWrap;
    return r.sizeHint;
}

/**
 * \if ENGLISH
 * @brief Fill the core layout Input from the host state
 * @details Field-by-field counterpart of SARibbonToolButton::PrivateData::
 *          layoutInput(): the style option becomes plain values, so both front
 *          ends feed the algorithm identical numbers. Three QML-only details:
 *          toolButtonStyle comes from effectiveToolButtonStyle() (explicit
 *          property over rendering context, then the icon-less fallbacks that
 *          reproduce what QToolButton::initStyleOption hands the widgets side);
 *          the rect falls back to a font-derived height before the first
 *          engine pass (the small-button text height derives from
 *          rect.height()); and panelLargeButtonHeight falls back to the metrics
 *          derivation of the three-row large height while
 *          largeButtonHeightContext() is still unset — the widgets button reads
 *          that number from its panel parent.
 * \endif
 *
 * \if CHINESE
 * @brief 用宿主状态填充 core 布局输入
 * @details 与 SARibbonToolButton::PrivateData::layoutInput() 一一对应：样式选项
 *          换成纯值，两个前端因此喂给算法完全相同的数值。三处 QML 特有处理：
 *          toolButtonStyle 取自 effectiveToolButtonStyle()（显式属性优先于
 *          渲染上下文，再叠加复现 QToolButton::initStyleOption 行为的无图标
 *          回退）；引擎首次布局前 rect 退化为按字体推导的高度（小按钮的文字
 *          高度取自 rect.height()）；largeButtonHeightContext() 尚未赋值时，
 *          panelLargeButtonHeight 退化为度量推导的三行制大按钮高度——widgets
 *          按钮的这个数值直接来自其父 panel。
 * \endif
 */
SARibbonToolButtonLayout::Input RibbonToolButton::layoutInput() const
{
    const SARibbon::Core::SARibbonMetrics& m = RibbonMetrics::instance()->coreMetrics();
    const QFontMetrics fm                    = m.fontMetrics();
    SARibbonToolButtonLayout::Input in;
    int w = int(width());
    int h = int(height());
    if (h <= 0) {
        h = fm.lineSpacing() + 4;
    }
    if (w <= 0) {
        w = qMax(mCachedSizeHint.width(), 1);
    }
    in.rect          = QRect(0, 0, w, h);
    in.hasIcon       = !mIconSource.isEmpty();
    in.isLargeButton = isLargeType();
    in.enableWordWrap = mWordWrap;
    // effective style: explicit property > rendering context (panel =
    // TextBesideIcon, title row = IconOnly), then the icon-less fallbacks
    // (see effectiveToolButtonStyle)
    in.toolButtonStyle = effectiveToolButtonStyle();
    in.hasIndicator    = hasMenu();
    in.isRTL           = SA::saIsRTL();
    in.iconSize        = mSmallIconSize;
    in.largeIconSize   = mLargeIconSize;
    in.text            = mText;
    in.fontMetrics     = fm;
    in.spacing         = mSpacing;
    in.indicatorLen    = in.isLargeButton ? TBLC::DEFAULT_INDICATOR_LEN_LARGE : TBLC::DEFAULT_INDICATOR_LEN_SMALL;
    if (largeButtonHeightContext() > 0) {
        in.panelLargeButtonHeight = largeButtonHeightContext();
    } else if (in.isLargeButton) {
        // pre-engine fallback: the metrics derivation of the three-row large
        // height (category height minus title strip and margins)
        in.panelLargeButtonHeight = m.calcCategoryHeight(true, false) - m.panelTitleHeight - 4 - 2;
    } else {
        in.panelLargeButtonHeight = -1;
    }
    in.maximumWidth = TBLC::UNLIMITED_WIDTH;
    in.factors      = mFactors;
    return in;
}

void RibbonToolButton::updateLayout()
{
    // Geometry authority: the core algorithm computes the icon / text /
    // indicator draw rects exactly as the widgets button paints them; the host
    // publishes them and the QML leaf only renders. The hit zones follow the
    // widgets sub-control split:
    // - small MenuButtonPopup: the indicator strip on the trailing edge opens
    //   the menu, the rest triggers the action
    // - large MenuButtonPopup: the bottom text strip united with the arrow
    //   (widgets mDrawTextRect.united(mDrawIndicatorArrowRect)) opens the menu,
    //   the icon zone above triggers the action
    // - InstantPopup: the whole button opens the menu, there is no action zone
    // - DelayedPopup / no menu: the whole button triggers the action (menu
    //   opens on press-hold for DelayedPopup)
    const SARibbonToolButtonLayout::Input in = layoutInput();
    const SARibbonToolButtonLayout::DrawRectResult r = SARibbonToolButtonLayout::calcDrawRects(in, mIsTextNeedWrap);

    const QRectF newIcon = r.iconRect.isValid() ? QRectF(r.iconRect) : QRectF();
    const QRectF newText = r.textRect.isValid() ? QRectF(r.textRect) : QRectF();
    const QRectF newInd  = r.indicatorArrowRect.isValid() ? QRectF(r.indicatorArrowRect) : QRectF();

    // caption: a wrapping large button renders the raw text through its
    // word-wrap box, everything else is elided to the box width (paintText)
    QString newDisplay;
    if (in.isLargeButton && in.enableWordWrap) {
        newDisplay = in.text;
    } else {
        newDisplay = in.fontMetrics.elidedText(
            SARibbonToolButtonLayout::simplifiedText(in.text), Qt::ElideRight, int(newText.width()), Qt::TextShowMnemonic);
    }

    QRectF newAction;
    QRectF newMenu;
    const QRectF full(0, 0, width(), height());
    // hasMenu(), not mMenuItems.isEmpty(): a subclass may own a popup that is
    // not a RibbonMenuItem list (RibbonColorToolButton and its color menu)
    if (hasMenu()) {
        switch (mPopupMode) {
        case RibbonEnums::InstantPopup: {
            newMenu = full;
            break;
        }
        case RibbonEnums::MenuButtonPopup: {
            if (in.isLargeButton) {
                const QRectF strip = newText.united(newInd);
                const qreal stripTop = qBound(qreal(0), strip.isEmpty() ? height() : strip.top(), height());
                newMenu   = QRectF(0, stripTop, width(), qMax(height() - stripTop, 0.0));
                newAction = QRectF(0, 0, width(), stripTop);
            } else {
                const qreal stripW = qMin(qreal(TBLC::DEFAULT_INDICATOR_LEN_SMALL), width());
                qreal x            = width() - stripW;  // trailing edge (LTR)
                if (SA::saIsRTL()) {
                    x = SA::saMirrorX(int(x), int(width()), int(stripW));
                }
                newMenu   = QRectF(x, 0, stripW, height());
                newAction = QRectF(0, 0, width() - stripW, height());
            }
            break;
        }
        case RibbonEnums::DelayedPopup:
            newAction = full;
            break;
        }
    } else {
        newAction = full;
    }
    if (newAction != mActionRect || newMenu != mMenuRect) {
        mActionRect = newAction;
        mMenuRect   = newMenu;
        Q_EMIT hitRectsChanged();
    }
    if (newIcon != mIconGeometry || newText != mTextGeometry || newInd != mIndicatorGeometry
        || newDisplay != mDisplayText) {
        mIconGeometry      = newIcon;
        mTextGeometry      = newText;
        mIndicatorGeometry = newInd;
        mDisplayText       = newDisplay;
        Q_EMIT layoutChanged();
    }
}

}
