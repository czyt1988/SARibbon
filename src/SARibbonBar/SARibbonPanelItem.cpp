#include "SARibbonPanelItem.h"

SARibbonPanelItem::SARibbonPanelItem(QWidget* widget)
    : QWidgetItem(widget), rowIndex(-1), columnIndex(-1), action(nullptr), customWidget(false), rowProportion(Large)
{
}

SARibbonPanelItem::~SARibbonPanelItem()
{
}

bool SARibbonPanelItem::isEmpty() const
{
    return (action == nullptr || !action->isVisible());
}
