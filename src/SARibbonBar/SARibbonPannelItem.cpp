#include "SARibbonPannelItem.h"

SARibbonPannelItem::SARibbonPannelItem(QWidget *widget) : QWidgetItem(widget)
    , action(nullptr)
    , customWidget(false)
{
}


bool SARibbonPannelItem::isEmpty() const
{
    return (action == 0 || !action->isVisible());
}
