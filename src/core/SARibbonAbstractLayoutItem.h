#ifndef SARIBBONABSTRACTLAYOUTITEM_H
#define SARIBBONABSTRACTLAYOUTITEM_H
#include <SARibbonCore/SARibbonCoreGlobal.h>
#include <SARibbonCore/SARibbonEnums.h>
#include <QRect>
#include <QSize>
#include <QString>
#include <Qt>

namespace SARibbon
{
namespace Core
{

/**
 * @brief Narrow contract interface between layout engines and front ends (plan-02 S4.1)
 * @details Engines read geometry constraints only through this interface and write
 *          results back through public fields; they never see widgets. Panel-side
 *          isHidden() means "action not visible", Category-side means the
 *          QWidgetItem::isEmpty() widget semantics - each front end implements its
 *          own 2.x semantics, they must not be unified.
 */
class SA_RIBBON_CORE_EXPORT SARibbonAbstractLayoutItem
{
public:
    virtual ~SARibbonAbstractLayoutItem();
    // -- provided by the front end (engine inputs, pure virtual) --
    virtual QSize sizeHint() const = 0;
    virtual bool isHidden() const = 0;
    virtual Qt::Orientations expandingDirections() const = 0;
    // -- provided by the front end (defaults, override as needed) --
    virtual int maximumWidth() const { return 16777215; }  // QWIDGETSIZE_MAX value (QtWidgets macro, core forbids the include)
    virtual int stretchFactor() const { return 0; }        // only the Gallery adapter overrides
    virtual QString debugName() const { return {}; }       // diagnostics name for golden-test dumps
    // -- implemented by the front end (engine output application) --
    virtual void applyGeometry(const QRect& rect) = 0;
    // -- engine-written result fields (both ends read) --
    int rowIndex = -1;                 // 2.x was short; the contract uses int (plan-02 S4.1-3)
    int columnIndex = -1;
    QRect resultGeometry;              // corresponds to 2.x itemWillSetGeometry / mWillSetGeometry
    bool isExpandItem = false;
    // -- shared data --
    SARibbonRowProportion rowProportion = SARibbonRowProportion::Large;  // 2.x ctor default (PanelItem.cpp:15)
};

/**
 * @brief Category-side extension: panel body + separator dual geometry (plan-02 S4.1-1)
 */
class SA_RIBBON_CORE_EXPORT SARibbonAbstractCategoryItem : public SARibbonAbstractLayoutItem
{
public:
    ~SARibbonAbstractCategoryItem() override;
    QRect resultSeparatorGeometry;      // corresponds to 2.x mWillSetSeparatorGeometry
    bool isSeparatorHidden = false;    // engine output, adapter hides/shows separatorWidget accordingly
};

}
}

#endif  // SARIBBONABSTRACTLAYOUTITEM_H
