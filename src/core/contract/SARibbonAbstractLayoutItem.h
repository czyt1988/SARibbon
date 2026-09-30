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
 * \if ENGLISH
 * @brief Narrow contract interface between layout engines and front ends (plan 02 S4.1)
 * @details The engines only read geometry constraints through this interface and write
 * results back through public fields; they never see widgets. Panel-side isHidden()
 * means "action not visible", Category-side means the QWidgetItem::isEmpty() widget
 * semantics — each front end implements its own 2.x semantics, they must not be unified.
 * \endif
 *
 * \if CHINESE
 * @brief 布局引擎与前端之间的窄契约接口（计划 02 S4.1）
 * @details 引擎只经本接口读几何约束、经公有字段回写结果，不见任何控件。
 * Panel 侧 isHidden() 语义为 "action 不可见"；Category 侧为 QWidgetItem::isEmpty()
 * 的控件语义——各前端按各自 2.x 语义实现，不得统一。
 * @note 输出字段命名 resultGeometry（非 geometry）：与 QLayoutItem::geometry()
 * 成员函数在双继承下冲突，v2 草案字段名已废弃（计划 02 S4.1-2）。
 * \endif
 */
class SA_RIBBON_CORE_EXPORT SARibbonAbstractLayoutItem
{
public:
    virtual ~SARibbonAbstractLayoutItem();
    // —— front end provides (engine inputs, pure virtual) ——
    virtual QSize sizeHint() const = 0;
    virtual QSize minimumSizeHint() const = 0;
    virtual bool isHidden() const = 0;   // Panel: action not visible; Category: QWidgetItem::isEmpty semantics
    virtual Qt::Orientations expandingDirections() const = 0;
    // —— front end provides (with defaults, override as needed) ——
    virtual int maximumWidth() const { return 16777215; }  // QWIDGETSIZE_MAX value (macro is QtWidgets, core forbids)
    virtual int stretchFactor() const { return 0; }        // only the Gallery adapter overrides
    virtual QString debugName() const { return {}; }       // diagnostics name for golden-test dumps
    // —— front end implements (engine output application) ——
    virtual void applyGeometry(const QRect& rect) = 0;
    // —— engine-written result fields (both ends read) ——
    int rowIndex = -1;                 // 2.x was short; contract uses int (plan 02 S4.1-3)
    int columnIndex = -1;
    QRect resultGeometry;              // corresponds to 2.x itemWillSetGeometry / mWillSetGeometry
    bool isExpandItem = false;
    // —— shared data ——
    SARibbonRowProportion rowProportion = SARibbonRowProportion::Large;  // 2.x ctor default (PanelItem.cpp:15)
};

/**
 * \if ENGLISH
 * @brief Category-side extension: panel body + separator dual geometry (plan 02 S4.1-1)
 * \endif
 *
 * \if CHINESE
 * @brief Category 侧扩展：panel 本体 + 分割线双几何（计划 02 S4.1-1）
 * \endif
 */
class SA_RIBBON_CORE_EXPORT SARibbonAbstractCategoryItem : public SARibbonAbstractLayoutItem
{
public:
    ~SARibbonAbstractCategoryItem() override;
    QRect resultSeparatorGeometry;      // corresponds to 2.x mWillSetSeparatorGeometry
    bool isSeparatorHidden = false;     // engine output, adapter hides/shows separatorWidget accordingly
};

}
}

#endif  // SARIBBONABSTRACTLAYOUTITEM_H
