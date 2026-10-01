#include "RibbonSeparator.h"
#include "../metrics/RibbonMetrics.h"
#include "../SARibbonQmlTypes.h"
#include <SARibbonCore/SARibbonEnums.h>

namespace SARibbonQml {

namespace {
// widgets parity: the separator widget keeps small margins around the line
constexpr int kSeparatorMargins = 3;
constexpr int kSeparatorWidth   = 1;
}  // namespace

RibbonSeparator::RibbonSeparator(QQuickItem* parent) : RibbonLayoutItemHost(parent)
{
    // widgets addSeparator wraps the separator with Large proportion (own
    // column, full body height)
    rowProportion = SARibbon::Core::SARibbonRowProportion::Large;
    const SARibbon::Core::SARibbonMetrics& m = RibbonMetrics::instance()->coreMetrics();
    const int largeH = m.calcCategoryHeight(true, false) - m.panelTitleHeight - 4 - 2;
    mCachedSizeHint = QSize(2 * kSeparatorMargins + kSeparatorWidth, qMax(largeH, 22));
    setImplicitWidth(mCachedSizeHint.width());
    setImplicitHeight(mCachedSizeHint.height());
}

RibbonSeparator::~RibbonSeparator()
{
}

QUrl RibbonSeparator::leafUrl() const
{
    return SARibbonQmlLeafUrls::separatorLeaf();
}

QSize RibbonSeparator::sizeHint() const
{
    return mCachedSizeHint;
}

void RibbonSeparator::largeHeightContextChanged()
{
    // the separator height follows the panel body height hint
    const int largeH = largeButtonHeightContext() > 0 ? largeButtonHeightContext() : mCachedSizeHint.height();
    mCachedSizeHint  = QSize(2 * kSeparatorMargins + kSeparatorWidth, qMax(largeH, 22));
    setImplicitHeight(mCachedSizeHint.height());
    invalidatePanelLayout();
}

}
