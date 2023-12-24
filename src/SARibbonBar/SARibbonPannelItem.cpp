#include "SARibbonPannelItem.h"

SARibbonPannelItem::SARibbonPannelItem(QWidget* widget)
    : QWidgetItem(widget), rowIndex(-1), columnIndex(-1), action(nullptr), customWidget(false), rowProportion(Large)
{
}

bool SARibbonPannelItem::isEmpty() const
{
    return (action == nullptr || !action->isVisible());
}
