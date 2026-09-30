#ifndef SARIBBONPANELLAYOUTENGINE_H
#define SARIBBONPANELLAYOUTENGINE_H
#include <SARibbonCore/SARibbonCoreGlobal.h>
#include <SARibbonCore/SARibbonEnums.h>
#include <SARibbonCore/SARibbonAbstractLayoutItem.h>
#include <QMargins>
#include <QRect>
#include <QSize>
#include <QVector>
#include <QHash>
#include <QFontMetrics>

namespace SARibbon
{
namespace Core
{

/**
 * \if ENGLISH
 * @brief Layout engine for the panel box algorithm (plan 02 S5, Step B pure move)
 * @details The updateGeomArray(QRect) / recalcExpandGeomArray bodies were moved
 * verbatim from SARibbonPanelLayout.cpp; widget touchpoints became Input values
 * and contract calls. The engine is stateful only through the button sizeHint
 * cache (key = contract item pointer, invalidated when largeHeight changes);
 * the adapter keeps one engine instance alive for the panel's lifetime.
 * \endif
 *
 * \if CHINESE
 * @brief Panel 装箱布局引擎（计划 02 S5，Step B 纯搬移）
 * @details updateGeomArray(QRect) / recalcExpandGeomArray 函数体自
 * SARibbonPanelLayout.cpp 纯 move；widget 触点换为 Input 值与契约调用。
 * 引擎的唯一状态是按钮 sizeHint 缓存（key=契约 item 指针，largeHeight
 * 变化时整体失效）；适配器为 panel 生命周期持有一个引擎实例。
 * \endif
 */
class SA_RIBBON_CORE_EXPORT SARibbonPanelLayoutEngine
{
public:
    struct Input
    {
        int rowCount { 3 };  ///< 3/2/1 rows (mapped from PanelLayoutMode by the adapter)
        bool showPanelTitle { true };
        bool hasTitleLabel { true };
        bool hasOptionAction { false };
        bool isRTL { false };
        QMargins contentsMargins;
        int spacing { 2 };
        int titleTextWidth { -1 };  ///< -1 = no title (adapter computes label fm advance + 4)
        QSize optionBtnSize;
        int titleHeight { 15 };  ///< effective (0 when title disabled)
        int titleSpace { 2 };    ///< effective (0 when title disabled)
        QFontMetrics fontMetrics { QFont() };  ///< for the sizeHint height derivation
        int previousSizeHintWidth { 0 };  ///< sizeHint width from the PREVIOUS layout pass (2.x reads stale mSizeHint in recalcExpandGeomArray)
    };

    struct Result
    {
        QSize sizeHint;
        int columnCount { 0 };
        int largeHeight { 0 };
        int totalWidth { 0 };
        QRect titleGeometry;
        QRect optionBtnGeometry;
    };

    SARibbonPanelLayoutEngine();

    // Core entry: layout all items inside setrect. Writes each item's
    // resultGeometry / rowIndex / columnIndex / isExpandItem.
    Result layout(QVector< SARibbonAbstractLayoutItem* > items, const QRect& setrect, const Input& input);

    // Cache lifetime invariants (plan-02 S5.1-2): the adapter calls these from
    // takeAt()/invalidate(); a stale entry would be reused when a new item
    // happens to get the same address.
    void removeFromCache(SARibbonAbstractLayoutItem* item);
    void clearCache();

    // Height hint derived from the font metrics (moved verbatim from
    // SARibbonPanel::panelHeightHint; rowCount replaces the PanelLayoutMode switch)
    static int panelHeightHint(const QFontMetrics& fm, int rowCount, int panelTitleHeight);

private:
    void recalcExpandGeomArray(QVector< SARibbonAbstractLayoutItem* >& items,
                               const QRect& setrect,
                               const Input& input);

    QHash< const SARibbonAbstractLayoutItem*, QSize > mButtonSizeHintCache;
    int mButtonSizeHintCacheLargeHeight { -1 };
};

}
}

#endif  // SARIBBONPANELLAYOUTENGINE_H
