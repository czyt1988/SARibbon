#include "RibbonLayoutItemHost.h"
#include "../panel/RibbonPanel.h"

namespace SARibbonQml {

RibbonLayoutItemHost::RibbonLayoutItemHost(QQuickItem* parent) : RibbonQuickHost(parent)
{
}

RibbonLayoutItemHost::~RibbonLayoutItemHost()
{
}

bool RibbonLayoutItemHost::isHidden() const
{
    return !isVisible();
}

void RibbonLayoutItemHost::applyGeometry(const QRect& rect)
{
    setPosition(QPointF(rect.topLeft()));
    setSize(QSizeF(rect.size()));
}

QString RibbonLayoutItemHost::debugName() const
{
    return objectName();
}

Qt::Orientations RibbonLayoutItemHost::expandingDirections() const
{
    return Qt::Orientations();  // plain items never expand; the gallery overrides
}

void RibbonLayoutItemHost::setLargeButtonHeightContext(int h)
{
    if (mLargeButtonHeightContext == h) {
        return;
    }
    mLargeButtonHeightContext = h;
    largeHeightContextChanged();
}

void RibbonLayoutItemHost::invalidatePanelLayout()
{
    if (RibbonPanel* panel = qobject_cast< RibbonPanel* >(parentItem())) {
        panel->invalidateChildCache(this);
    }
}

void RibbonLayoutItemHost::largeHeightContextChanged()
{
    invalidatePanelLayout();
}

}
