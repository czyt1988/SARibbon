#include "RibbonToolButton.h"
#include "../menu/RibbonMenuItem.h"
#include "../panel/RibbonPanel.h"
#include "../metrics/RibbonMetrics.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QRectF>

namespace SARibbonQml {

namespace {
// Icon side lengths (widgets parity: SARibbonBar small icon 20, large 32).
// Rendering parameters may live per front end (v2 §2.2 double-render rule);
// the layout *algorithm* stays in the core engines.
constexpr int kSmallIconSide = 20;
constexpr int kLargeIconSide = 32;
// Widget-side SARibbonToolButton layout factors (SARibbonToolButton.cpp)
constexpr qreal kLargeMinWidthRatio = 0.75;  ///< largeButtonMinimumWidthRatio
constexpr qreal kMaxAspectRatio     = 1.4;   ///< buttonMaximumAspectRatio
// Menu indicator lengths (widgets SARibbonToolButtonConstants)
constexpr int kSmallIndicatorLen = 12;
constexpr int kLargeIndicatorLen = 8;
}  // namespace

RibbonToolButton::RibbonToolButton(QQuickItem* parent) : RibbonLayoutItemHost(parent)
{
    // contract field default follows the 2.x createItem behavior (Large)
    rowProportion = SARibbon::Core::SARibbonRowProportion::Large;
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
    updateHitRects();
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
    updateHitRects();
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
    updateHitRects();
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
    updateHitRects();
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
    updateHitRects();
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
        updateHitRects();
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
    updateHitRects();
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
    const SARibbon::Core::SARibbonMetrics& m = RibbonMetrics::instance()->coreMetrics();
    const QFontMetrics fm = m.fontMetrics();
    const int textW = fm.horizontalAdvance(mText);
    // iconRightText forces the small rendering/hint regardless of the
    // proportion (widgets PrivateData::effectiveButtonType parity)
    const bool isLarge = (rowProportion == SARibbon::Core::SARibbonRowProportion::Large) && !mIconRightText;
    const bool hasInd = hasMenu();
    if (isLarge) {
        // Large button (icon above, text below). The effective large height is
        // panel-driven; before the first engine pass falls back to the metrics
        // derivation (three-row category minus title strip and margins).
        const int largeH = largeButtonHeightContext() > 0
                               ? largeButtonHeightContext()
                               : m.calcCategoryHeight(true, false) - m.panelTitleHeight - 4 - 2;
        // single line when the text fits the aspect-ratio box or word wrap is
        // disabled (style propagation), otherwise the two-line wrap estimate
        // (widgets uses a binary search here; the half-width approximation
        // stays within a few pixels)
        int w;
        if (!mWordWrap || textW <= int(largeH * kMaxAspectRatio)) {
            w = textW + 2 + (hasInd ? kLargeIndicatorLen : 0);
        } else {
            w = textW / 2 + fm.horizontalAdvance(QLatin1String("xx")) + 8 + (hasInd ? kLargeIndicatorLen : 0);
        }
        w = qMax(w, qMax(int(largeH * kLargeMinWidthRatio), kLargeIconSide + 4));
        return QSize(w, qMax(largeH, 22));
    }
    // Small/Medium button (icon left, text right): mirrors the widgets
    // sizeHint iconW + spacing + text width (+ two trailing spaces of slack)
    const int spaceW = 2 * fm.horizontalAdvance(QLatin1Char(' '));
    return QSize(kSmallIconSide + 3 + textW + spaceW + (hasInd ? kSmallIndicatorLen : 0),
                 qMax(qMax(fm.lineSpacing(), kSmallIconSide), 16));
}

void RibbonToolButton::updateHitRects()
{
    // Host-computed hit zones (geometry authority; the leaf binds MouseAreas
    // to them). Mirrors the widgets SARibbonToolButton sub-control split:
    // - small MenuButtonPopup: the indicator strip on the trailing edge opens
    //   the menu, the rest triggers the action
    // - large MenuButtonPopup: the bottom text strip (+ arrow) opens the
    //   menu, the icon zone above triggers the action
    // - InstantPopup: the whole button opens the menu, there is no action zone
    // - DelayedPopup / no menu: the whole button triggers the action (menu
    //   opens on press-hold for DelayedPopup)
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
            if ((rowProportion == SARibbon::Core::SARibbonRowProportion::Large) && !mIconRightText) {
                // bottom strip below the icon zone (icon 32 + top margin 2 + gap)
                const qreal stripTop = qMin(qreal(kLargeIconSide + 3), height());
                newMenu   = QRectF(0, stripTop, width(), qMax(height() - stripTop, 0.0));
                newAction = QRectF(0, 0, width(), stripTop);
            } else {
                const qreal stripW = qMin(qreal(kSmallIndicatorLen), width());
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
}

}
