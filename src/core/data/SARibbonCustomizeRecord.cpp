#include "SARibbonCustomizeRecord.h"

namespace SARibbon
{
namespace Core
{

SARibbonCustomizeRecord::SARibbonCustomizeRecord()
    : indexValue(-1)
    , actionRowProportionValue(SARibbonRowProportion::Large)
    , mType(UnknowActionType)
{
}

SARibbonCustomizeRecord::SARibbonCustomizeRecord(ActionType type)
    : indexValue(-1)
    , actionRowProportionValue(SARibbonRowProportion::Large)
    , mType(type)
{
}

SARibbonCustomizeRecord::~SARibbonCustomizeRecord()
{
}

SARibbonCustomizeRecord::ActionType SARibbonCustomizeRecord::actionType() const
{
    return (mType);
}

void SARibbonCustomizeRecord::setActionType(SARibbonCustomizeRecord::ActionType a)
{
    mType = a;
}

bool SARibbonCustomizeRecord::isValid() const
{
    return (actionType() != UnknowActionType);
}

}
}
