#include "RibbonPanel.h"
#include "../host/RibbonLayoutItemHost.h"
#include "../button/RibbonToolButton.h"
#include "../category/RibbonCategory.h"
#include "../metrics/RibbonMetrics.h"
#include "../theme/RibbonTheme.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QQuickItem>

namespace SARibbonQml {

RibbonPanel::RibbonPanel(QQuickItem* parent) : RibbonQuickHost(parent)
{
    // RTL flip re-runs the engine pass (SA::saIsRTL() re-read on polish);
    // the theme singleton owns the rtl property and broadcasts the change
    connect(RibbonTheme::instance(), &RibbonTheme::rtlChanged, this, [this]() { polish(); });
}

RibbonPanel::~RibbonPanel()
{
}

QUrl RibbonPanel::leafUrl() const
{
    return SARibbonQmlLeafUrls::panelLeaf();
}

QString RibbonPanel::panelTitle() const
{
    return mPanelTitle;
}

void RibbonPanel::setPanelTitle(const QString& t)
{
    if (mPanelTitle == t) {
        return;
    }
    mPanelTitle = t;
    Q_EMIT panelTitleChanged();
    polish();
}

RibbonEnums::LayoutMode RibbonPanel::layoutMode() const
{
    return mLayoutMode;
}

void RibbonPanel::setLayoutMode(RibbonEnums::LayoutMode mode)
{
    if (mLayoutMode == mode) {
        return;
    }
    mLayoutMode = mode;
    Q_EMIT layoutModeChanged();
    polish();
}

bool RibbonPanel::enableShowPanelTitle() const
{
    return mEnableShowPanelTitle;
}

void RibbonPanel::setEnableShowPanelTitle(bool on)
{
    if (mEnableShowPanelTitle == on) {
        return;
    }
    mEnableShowPanelTitle = on;
    Q_EMIT enableShowPanelTitleChanged();
    polish();
}

void RibbonPanel::applyRibbonStyle(RibbonEnums::LayoutMode mode, bool showPanelTitle, bool wordWrap, bool iconRightText)
{
    // remember the flags so panels/buttons registered later inherit them
    mWordWrap      = wordWrap;
    mIconRightText = iconRightText;
    setEnableShowPanelTitle(showPanelTitle);
    setLayoutMode(mode);
    // forward the item-level flags to the registered tool buttons
    for (RibbonLayoutItemHost* item : mChildItems) {
        if (auto* btn = qobject_cast< RibbonToolButton* >(item)) {
            btn->setWordWrap(wordWrap);
            btn->setIconRightText(iconRightText);
        }
    }
}

void RibbonPanel::registerChildItem(RibbonLayoutItemHost* item)
{
    if (!mChildItems.contains(item)) {
        mChildItems.append(item);
        // buttons registered later inherit the current style flags and the
        // layout knobs already pushed down from the category / bar
        if (auto* btn = qobject_cast< RibbonToolButton* >(item)) {
            btn->setWordWrap(mWordWrap);
            btn->setIconRightText(mIconRightText);
            btn->setSmallIconSize(mSmallIconSize);
            btn->setLargeIconSize(mLargeIconSize);
            btn->setButtonMaximumAspectRatio(mButtonMaximumAspectRatio);
            btn->setLargeButtonMinimumWidthRatio(mLargeButtonMinimumWidthRatio);
        }
        polish();
    }
}

/**
 * \if ENGLISH
 * @brief Push the bar-level layout factors down to every registered button
 * @details Counterpart of SARibbonPanel::setButtonMaximumAspectRatio /
 *          setLargeButtonMinimumWidthRatio: the widgets panel forwards the pair
 *          to its layout and to each SARibbonToolButton child, and the QML host
 *          does the same through the button setters. The values are stored so a
 *          button declared after the push inherits them (registerChildItem).
 *          Each button setter drops the engine sizeHint cache entry and
 *          re-polishes the panel, so no extra invalidate call is needed here.
 * \endif
 *
 * \if CHINESE
 * @brief 把 bar 级布局系数下发到每个已注册按钮
 * @details 对应 SARibbonPanel::setButtonMaximumAspectRatio /
 *          setLargeButtonMinimumWidthRatio：widgets 面板把这对系数转给自身布局
 *          与每个 SARibbonToolButton 子项，QML 宿主经按钮设置函数做同样的事。
 *          数值会被记住，以便下发之后才声明的按钮继承（registerChildItem）。
 *          按钮的设置函数会丢弃引擎 sizeHint 缓存并重新 polish 面板，故此处
 *          无需再调一次失效接口。
 * \endif
 */
void RibbonPanel::applyLayoutFactors(qreal buttonMaximumAspectRatio, qreal largeButtonMinimumWidthRatio)
{
    mButtonMaximumAspectRatio     = buttonMaximumAspectRatio;
    mLargeButtonMinimumWidthRatio = largeButtonMinimumWidthRatio;
    for (RibbonLayoutItemHost* item : mChildItems) {
        if (auto* btn = qobject_cast< RibbonToolButton* >(item)) {
            btn->setButtonMaximumAspectRatio(buttonMaximumAspectRatio);
            btn->setLargeButtonMinimumWidthRatio(largeButtonMinimumWidthRatio);
        }
    }
}

QSize RibbonPanel::smallIconSize() const
{
    return mSmallIconSize;
}

void RibbonPanel::setSmallIconSize(const QSize& size)
{
    if (mSmallIconSize == size) {
        return;
    }
    mSmallIconSize = size;
    Q_EMIT smallIconSizeChanged();
    for (RibbonLayoutItemHost* item : mChildItems) {
        if (auto* btn = qobject_cast< RibbonToolButton* >(item)) {
            btn->setSmallIconSize(size);
        }
    }
}

QSize RibbonPanel::largeIconSize() const
{
    return mLargeIconSize;
}

void RibbonPanel::setLargeIconSize(const QSize& size)
{
    if (mLargeIconSize == size) {
        return;
    }
    mLargeIconSize = size;
    Q_EMIT largeIconSizeChanged();
    for (RibbonLayoutItemHost* item : mChildItems) {
        if (auto* btn = qobject_cast< RibbonToolButton* >(item)) {
            btn->setLargeIconSize(size);
        }
    }
}

void RibbonPanel::unregisterChildItem(RibbonLayoutItemHost* item)
{
    if (mChildItems.removeOne(item)) {
        mEngine.removeFromCache(item);
        polish();
    }
}

void RibbonPanel::invalidateChildCache(SARibbon::Core::SARibbonAbstractLayoutItem* item)
{
    // engine caches button sizeHints keyed by largeHeight only: a text/proportion
    // change must drop the entry explicitly, then a fresh pass repacks the columns
    mEngine.removeFromCache(item);
    polish();
}

bool RibbonPanel::hasOptionAction() const
{
    return mHasOptionAction;
}

void RibbonPanel::setHasOptionAction(bool on)
{
    if (mHasOptionAction == on) {
        return;
    }
    mHasOptionAction = on;
    Q_EMIT hasOptionActionChanged();
    polish();
}

void RibbonPanel::triggerOptionAction()
{
    Q_EMIT optionActionTriggered();
}

QRectF RibbonPanel::optionButtonRect() const
{
    return QRectF(mLastOptionButtonGeometry);
}

int RibbonPanel::rowCountForMode() const
{
    switch (mLayoutMode) {
    case RibbonEnums::ThreeRowMode:
        return 3;
    case RibbonEnums::TwoRowMode:
        return 2;
    case RibbonEnums::SingleRowMode:
        return 1;
    }
    return 3;
}

void RibbonPanel::componentComplete()
{
    RibbonQuickHost::componentComplete();
    ensureQmlLeaf();
    polish();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void RibbonPanel::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonQuickHost::geometryChange(newGeometry, oldGeometry);
#else
void RibbonPanel::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonQuickHost::geometryChanged(newGeometry, oldGeometry);
#endif
    if (newGeometry.size() != oldGeometry.size()) {
        polish();  // the category host sizes this item: re-run the engine on resize
    }
}

QRectF RibbonPanel::titleGeometry() const
{
    return QRectF(mLastTitleGeometry);
}

void RibbonPanel::itemChange(ItemChange change, const ItemChangeData& data)
{
    if (change == QQuickItem::ItemChildAddedChange) {
        // declaration order (sibling componentComplete runs reversed)
        if (RibbonLayoutItemHost* item = qobject_cast< RibbonLayoutItemHost* >(data.item)) {
            registerChildItem(item);
        }
    } else if (change == QQuickItem::ItemChildRemovedChange) {
        if (RibbonLayoutItemHost* item = qobject_cast< RibbonLayoutItemHost* >(data.item)) {
            unregisterChildItem(item);
        }
    } else if (change == QQuickItem::ItemVisibleHasChanged) {
        polish();
    }
    RibbonQuickHost::itemChange(change, data);
}

void RibbonPanel::updatePolish()
{
    runLayout();
}

void RibbonPanel::runLayout()
{
    if (mChildItems.isEmpty()) {
        return;
    }
    SARibbon::Core::SARibbonPanelLayoutEngine::Input input;
    input.rowCount        = rowCountForMode();
    input.showPanelTitle  = mEnableShowPanelTitle && !mPanelTitle.isEmpty();
    input.hasTitleLabel   = true;
    input.hasOptionAction = mHasOptionAction;
    input.isRTL           = SA::saIsRTL();
    input.contentsMargins = QMargins(2, 2, 2, 2);  // widgets parity (SARibbonPanelLayout)
    input.spacing         = 2;
    // widgets adapter contract (engine comment: titleTextWidth = fm advance + 4;
    // -1 means "no title"); optionBtnSize mirrors the widgets option button
    const QFontMetrics fm = RibbonMetrics::instance()->coreMetrics().fontMetrics();
    input.titleTextWidth  = input.showPanelTitle ? fm.horizontalAdvance(mPanelTitle) + 4 : -1;
    input.optionBtnSize   = mHasOptionAction ? QSize(16, 16) : QSize();
    input.titleHeight     = RibbonMetrics::instance()->panelTitleHeight();
    input.titleSpace      = 2;
    input.fontMetrics     = fm;
    input.previousSizeHintWidth = mLastSizeHint.width();

    QVector< SARibbon::Core::SARibbonAbstractLayoutItem* > items;
    items.reserve(mChildItems.size());
    for (RibbonLayoutItemHost* item : mChildItems) {
        items.append(item);
    }

    SARibbon::Core::SARibbonPanelLayoutEngine::Result r = mEngine.layout(items, QRect(0, 0, int(width()), int(height())), input);

    mLastSizeHint = r.sizeHint;
    mLastColumnCount = r.columnCount;
    mLastLargeHeight = r.largeHeight;
    if (r.titleGeometry != mLastTitleGeometry) {
        mLastTitleGeometry = r.titleGeometry;
        Q_EMIT titleGeometryChanged();
    }
    if (r.optionBtnGeometry != mLastOptionButtonGeometry) {
        mLastOptionButtonGeometry = r.optionBtnGeometry;
        Q_EMIT optionButtonRectChanged();
    }
    // publish implicit sizes even at zero geometry: the sizeHint derives from the
    // C++ item hints (metrics-driven, rect-independent), and the category reads
    // implicitWidth as its layout hint — gating this on our own size would
    // deadlock the hint chain (category waits for the panel, panel for category)
    setImplicitWidth(qMax(qreal(r.sizeHint.width()), width()));
    setImplicitHeight(qreal(r.sizeHint.height()));

    if (width() <= 0 || height() <= 0) {
        return;  // degenerate rect: hints published, item placement skipped
    }
    // apply engine outputs to the REAL quick items
    for (RibbonLayoutItemHost* item : mChildItems) {
        if (!item->isHidden()) {
            item->applyGeometry(item->resultGeometry);
        }
    }
    // publish the fresh large row height to every item (widgets parity: the
    // large-proportion sizeHint width depends on the panel's largeButtonHeight,
    // so the width hint must follow the height whenever it changes). A change
    // invalidates the item hints and re-requests polish; the second pass
    // repacks with the final widths (the engine cache is keyed on largeHeight
    // and drops together with this notification)
    for (RibbonLayoutItemHost* item : mChildItems) {
        item->setLargeButtonHeightContext(r.largeHeight);
    }
}

}
