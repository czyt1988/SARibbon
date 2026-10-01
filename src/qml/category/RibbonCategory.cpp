#include "RibbonCategory.h"
#include "../panel/RibbonPanel.h"
#include "../button/RibbonToolButton.h"
#include "../bar/RibbonBar.h"
#include "../theme/RibbonTheme.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QVariantList>

namespace SARibbonQml {

namespace {
// adapter item bridging a RibbonPanel into the category contract (the panel is a
// QQuickItem, not a QWidgetItem; its layout item face is this thin wrapper)
class CategoryItemAdapter : public SARibbon::Core::SARibbonAbstractCategoryItem
{
public:
    explicit CategoryItemAdapter(RibbonPanel* panel) : mPanel(panel) {}
    QSize sizeHint() const override;
    bool isHidden() const override { return !mPanel || !mPanel->isVisible(); }
    Qt::Orientations expandingDirections() const override { return Qt::Orientations(); }
    void applyGeometry(const QRect& rect) override;

    RibbonPanel* mPanel;
};
}  // namespace

RibbonCategory::RibbonCategory(QQuickItem* parent) : RibbonQuickHost(parent)
{
    // RTL flip re-runs the engine pass (SA::saIsRTL() re-read on polish)
    connect(RibbonTheme::instance(), &RibbonTheme::rtlChanged, this, [this]() { polish(); });
}

RibbonCategory::~RibbonCategory()
{
}

QUrl RibbonCategory::leafUrl() const
{
    return SARibbonQmlLeafUrls::categoryLeaf();
}

void RibbonCategory::componentComplete()
{
    RibbonQuickHost::componentComplete();
    ensureQmlLeaf();
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void RibbonCategory::geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonQuickHost::geometryChange(newGeometry, oldGeometry);
#else
void RibbonCategory::geometryChanged(const QRectF& newGeometry, const QRectF& oldGeometry)
{
    RibbonQuickHost::geometryChanged(newGeometry, oldGeometry);
#endif
    if (newGeometry.size() != oldGeometry.size()) {
        polish();  // the bar host sizes this item: relayout panels on every resize
    }
}

QString RibbonCategory::title() const
{
    return mTitle;
}

void RibbonCategory::setTitle(const QString& t)
{
    if (mTitle == t) {
        return;
    }
    mTitle = t;
    Q_EMIT titleChanged();
    polish();
}

int RibbonCategory::scrollPosition() const
{
    return mScrollXBase;
}

void RibbonCategory::setScrollPosition(int pos)
{
    // engine-clamped target (plan-04 S4: QML Behavior animates the visual x;
    // the logical position goes through clampScrollOffset)
    const int clamped = SARibbon::Core::clampScrollOffset(pos, mTotalWidth, int(width()), SA::saIsRTL());
    if (mScrollXBase == clamped) {
        return;
    }
    mScrollXBase = clamped;
    Q_EMIT scrollPositionChanged();
    polish();
}

void RibbonCategory::registerPanel(RibbonPanel* panel)
{
    if (!mPanels.contains(panel)) {
        mPanels.append(panel);
        // apply the last pushed bar style so dynamically added panels match
        applyRibbonStyle(mStyleRowCount, mStyleShowPanelTitle, mStyleWordWrap, mStyleIconRightText);
        // panel implicit sizes are the layout hints: any change re-runs relayout
        connect(panel, &QQuickItem::implicitWidthChanged, this, [this]() { polish(); });
        connect(panel, &QQuickItem::implicitHeightChanged, this, [this]() { polish(); });
        polish();
    }
}

void RibbonCategory::applyRibbonStyle(int rowCount, bool showPanelTitle, bool wordWrap, bool iconRightText)
{
    mStyleRowCount       = rowCount;
    mStyleShowPanelTitle = showPanelTitle;
    mStyleWordWrap       = wordWrap;
    mStyleIconRightText  = iconRightText;
    const RibbonEnums::LayoutMode mode = (rowCount <= 1)  ? RibbonEnums::SingleRowMode
                                         : (rowCount == 2) ? RibbonEnums::TwoRowMode
                                                           : RibbonEnums::ThreeRowMode;
    for (RibbonPanel* panel : mPanels) {
        panel->applyRibbonStyle(mode, showPanelTitle, wordWrap, iconRightText);
    }
    polish();
}

void RibbonCategory::unregisterPanel(RibbonPanel* panel)
{
    if (mPanels.removeOne(panel)) {
        disconnect(panel, nullptr, this, nullptr);
        polish();
    }
}

int RibbonCategory::contentWidth() const
{
    return mTotalWidth;
}

QVariantList RibbonCategory::separatorXs() const
{
    return mSeparatorXs;
}

void RibbonCategory::itemChange(ItemChange change, const ItemChangeData& data)
{
    if (change == QQuickItem::ItemChildAddedChange) {
        // declaration order (sibling componentComplete runs reversed)
        if (RibbonPanel* p = qobject_cast< RibbonPanel* >(data.item)) {
            registerPanel(p);
        }
    } else if (change == QQuickItem::ItemChildRemovedChange) {
        if (RibbonPanel* p = qobject_cast< RibbonPanel* >(data.item)) {
            unregisterPanel(p);
        }
    } else if (change == QQuickItem::ItemVisibleHasChanged) {
        polish();
    }
    RibbonQuickHost::itemChange(change, data);
}

void RibbonCategory::updatePolish()
{
    relayout();
}

void RibbonCategory::relayout()
{
    if (mPanels.isEmpty() || width() <= 0) {
        return;
    }
    // collect per-panel size hints (panel implicit sizes as inputs)
    SARibbon::Core::SARibbonCategorySizeHints hints;
    hints.panelSizes.resize(mPanels.size());
    hints.separatorSizes.resize(mPanels.size());
    QVector< SARibbon::Core::SARibbonAbstractCategoryItem* > items;
    items.reserve(mPanels.size());
    for (int i = 0; i < mPanels.size(); ++i) {
        RibbonPanel* p = mPanels[ i ];
        CategoryItemAdapter* adapter = new CategoryItemAdapter(p);
        const QSize hint = QSize(int(p->implicitWidth()), int(p->implicitHeight()));
        hints.panelSizes[ i ]   = QSize(hint.width(), int(height()));
        hints.separatorSizes[ i ] = QSize(1, int(height()));
        hints.totalWidth += hint.width() + 1;
        items.append(adapter);
    }

    SARibbon::Core::SARibbonCategoryLayoutEngine::Input input;
    input.categoryWidth = int(width());
    input.height        = int(height());
    input.margins       = QMargins(0, 0, 0, 0);
    input.isRTL         = SA::saIsRTL();
    input.xBase         = mScrollXBase;
    input.sizeHints     = hints;

    auto r = mEngine.layout(items, input);
    mTotalWidth = r.totalWidth;

    // publish the engine-written separator geometry to the leaf (the QSS
    // margin-top/bottom 3px of `SARibbonCategory > SARibbonSeparatorWidget`
    // stays leaf-side; only the x positions are geometry authority)
    QVariantList separators;
    for (int i = 0; i < items.size(); ++i) {
        if (!items[ i ]->isHidden() && !items[ i ]->isSeparatorHidden) {
            separators.append(QVariant(qreal(items[ i ]->resultSeparatorGeometry.x())));
        }
    }
    if (separators != mSeparatorXs) {
        mSeparatorXs = separators;
        Q_EMIT separatorXsChanged();
    }

    for (int i = 0; i < items.size(); ++i) {
        items[ i ]->applyGeometry(items[ i ]->resultGeometry);
    }
    qDeleteAll(items);
    setImplicitWidth(qreal(mTotalWidth));
}

// ---- CategoryItemAdapter ----
QSize CategoryItemAdapter::sizeHint() const
{
    return QSize(mPanel->implicitWidth(), mPanel->implicitHeight());
}

void CategoryItemAdapter::applyGeometry(const QRect& rect)
{
    mPanel->setPosition(rect.topLeft());
    mPanel->setSize(rect.size());
}

}
