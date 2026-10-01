#include "RibbonToolButton.h"
#include "../menu/RibbonMenuItem.h"
#include "../panel/RibbonPanel.h"
#include "../metrics/RibbonMetrics.h"
#include "../theme/RibbonTheme.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QRectF>

namespace SARibbonQml {

namespace TBLC = SARibbon::Core::ToolButtonLayoutConstants;
using SARibbonToolButtonLayout = SARibbon::Core::SARibbonToolButtonLayout;

namespace {
// Icon side lengths (widgets parity: SARibbonPanelLayout::mSmallToolButtonIconSize
// defaults to 22, SARibbonToolButton::PrivateData::mLargeButtonSizeHint to 32).
// Rendering parameters may live per front end (v2 §2.2 double-render rule);
// the layout *algorithm* itself lives in SARibbon::Core::SARibbonToolButtonLayout.
constexpr int kSmallIconSide = 22;
constexpr int kLargeIconSide = 32;
}  // namespace

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
    // (widgets PrivateData::effectiveButtonType parity)
    return (SARibbon::Core::SARibbonRowProportion::Large == rowProportion) && !mIconRightText;
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
    const QSize origin = isLargeType() ? QSize(kLargeIconSide, kLargeIconSide) : QSize(kSmallIconSide, kSmallIconSide);
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
    RibbonMenuItem* item = menuItemAt(index);
    if (!item || !item->isEnabled() || item->isSeparator()) {
        return;
    }
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
 *          toolButtonStyle reproduces what QToolButton::initStyleOption hands
 *          the widgets side (Qt6 downgrades TextBesideIcon to TextOnly when no
 *          icon is set); the rect falls back to a font-derived height before
 *          the first engine pass (the small-button text height derives from
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
 *          toolButtonStyle 复现 QToolButton::initStyleOption 交给 widgets 侧的
 *          结果（Qt6 在无图标时把 TextBesideIcon 降级为 TextOnly）；引擎首次
 *          布局前 rect 退化为按字体推导的高度（小按钮的文字高度取自
 *          rect.height()）；largeButtonHeightContext() 尚未赋值时，
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
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // Qt6 QToolButton::initStyleOption downgrades TextBesideIcon to TextOnly
    // when the button carries no icon, and the widgets side hands core exactly
    // that value; mirroring it keeps an icon-less small button from reserving
    // an empty icon slot (Qt5 leaves TextBesideIcon alone, hence the split)
    in.toolButtonStyle = in.hasIcon ? Qt::ToolButtonTextBesideIcon : Qt::ToolButtonTextOnly;
#else
    in.toolButtonStyle = Qt::ToolButtonTextBesideIcon;
#endif
    in.hasIndicator    = hasMenu();
    in.isRTL           = SA::saIsRTL();
    in.iconSize        = QSize(kSmallIconSide, kSmallIconSide);
    in.largeIconSize   = QSize(kLargeIconSide, kLargeIconSide);
    in.text            = mText;
    in.fontMetrics     = fm;
    in.spacing         = TBLC::DEFAULT_SPACING;
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
    if (!mMenuItems.isEmpty()) {
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
