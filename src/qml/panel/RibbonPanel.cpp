#include "RibbonPanel.h"
#include "../button/RibbonToolButton.h"
#include "../metrics/RibbonMetrics.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QQuickItem>
#include <QQmlEngine>
#include <QQmlComponent>
#include <QFile>

namespace SARibbonQml {

// leaf URL table (plan-04 S3 leaf organization rule: centralized, no factory class in P0)
namespace LeafUrls {
QUrl panelLeaf()
{
    return QUrl("qrc:/SARibbon/RibbonPanel.qml");
}
}  // namespace LeafUrls

RibbonPanel::RibbonPanel(QQuickItem* parent) : QQuickItem(parent)
{
}

RibbonPanel::~RibbonPanel()
{
    // leaf destruction: unparent + deleteLater, NEVER direct delete (a QML item may
    // sit inside its own mouse-handling call stack, KDDW Group.cpp same rule)
    if (mPanelQmlItem) {
        mPanelQmlItem->setParentItem(nullptr);
        mPanelQmlItem->setParent(nullptr);
        mPanelQmlItem->deleteLater();
        mPanelQmlItem = nullptr;
    }
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

QQuickItem* RibbonPanel::panelQmlItem() const
{
    return mPanelQmlItem;
}

void RibbonPanel::setPanelQmlItem(QQuickItem* item)
{
    if (mPanelQmlItem == item) {
        return;
    }
    mPanelQmlItem = item;
    Q_EMIT panelQmlItemChanged();
}

void RibbonPanel::registerChildItem(RibbonToolButton* item)
{
    if (!mChildButtons.contains(item)) {
        mChildButtons.append(item);
        polish();
    }
}

void RibbonPanel::unregisterChildItem(RibbonToolButton* item)
{
    if (mChildButtons.removeOne(item)) {
        polish();
    }
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
    QQuickItem::componentComplete();
    polish();
}

void RibbonPanel::itemChange(ItemChange change, const ItemChangeData& data)
{
    if (change == QQuickItem::ItemChildRemovedChange) {
        if (RibbonToolButton* btn = qobject_cast< RibbonToolButton* >(data.item)) {
            unregisterChildItem(btn);
        }
    } else if (change == QQuickItem::ItemVisibleHasChanged) {
        polish();
    }
    QQuickItem::itemChange(change, data);
}

void RibbonPanel::updatePolish()
{
    runLayout();
}

void RibbonPanel::runLayout()
{
    if (mChildButtons.isEmpty() || width() <= 0 || height() <= 0) {
        return;
    }
    SARibbon::Core::SARibbonPanelLayoutEngine::Input input;
    input.rowCount        = rowCountForMode();
    input.showPanelTitle  = !mPanelTitle.isEmpty();
    input.hasTitleLabel   = true;
    input.hasOptionAction = false;
    input.isRTL           = SA::saIsRTL();
    input.contentsMargins = QMargins(1, 1, 1, 1);
    input.spacing         = 2;
    input.titleTextWidth  = -1;  // no option button in P0; title width only feeds min width
    input.optionBtnSize   = QSize();
    input.titleHeight     = 15;
    input.titleSpace      = 2;
    input.fontMetrics     = RibbonMetrics::instance()->coreMetrics().fontMetrics();
    input.previousSizeHintWidth = mLastSizeHint.width();

    QVector< SARibbon::Core::SARibbonAbstractLayoutItem* > items;
    items.reserve(mChildButtons.size());
    for (RibbonToolButton* b : mChildButtons) {
        items.append(b);
    }

    SARibbon::Core::SARibbonPanelLayoutEngine::Result r = mEngine.layout(items, QRect(0, 0, int(width()), int(height())), input);

    // apply engine outputs to the REAL quick items
    for (RibbonToolButton* b : mChildButtons) {
        if (!b->isHidden()) {
            b->applyGeometry(b->resultGeometry);
        }
    }
    mLastSizeHint = r.sizeHint;
    mLastColumnCount = r.columnCount;
    mLastLargeHeight = r.largeHeight;
    // panel implicit height from the engine's height hint
    setImplicitWidth(qMax(qreal(r.sizeHint.width()), width()));
    setImplicitHeight(qreal(r.sizeHint.height()));
}

}
