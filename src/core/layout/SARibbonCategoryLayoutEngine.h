#ifndef SARIBBONCATEGORYLAYOUTENGINE_H
#define SARIBBONCATEGORYLAYOUTENGINE_H
#include <SARibbonCore/SARibbonCoreGlobal.h>
#include <SARibbonCore/SARibbonEnums.h>
#include <SARibbonCore/SARibbonAbstractLayoutItem.h>
#include <QMargins>
#include <QRect>
#include <QSize>
#include <QVector>

namespace SARibbon
{
namespace Core
{

/**
 * \if ENGLISH
 * @brief Scroll button visibility flags (plan-02 S6-3)
 * @details The flag decision logic used to exist twice in SARibbonCategoryLayout.cpp
 * (inside updateGeometryArr and inside updateScrollButtonVisibility); it is now this
 * single pure function shared by both call paths (section 4-4 no-dual-implementation rule).
 * \endif
 *
 * \if CHINESE
 * @brief 滚动按钮可见性标志（计划 02 S6-3）
 * @details 原逻辑在 SARibbonCategoryLayout.cpp 中存在两份实现（updateGeometryArr
 * 内与 updateScrollButtonVisibility 内），现收敛为本单一纯函数，两处调用共用
 * （§4-4 禁止双实现条款，NOTES B12-1）。
 * \endif
 */
struct SARibbonScrollFlags
{
    bool showLeft = false;
    bool showRight = false;
};

/**
 * \if ENGLISH
 * @brief Decide scroll button visibility from the scroll state (pure)
 * @param totalWidth total content width
 * @param viewportWidth visible viewport width
 * @param xBase current scroll base (RTL: [0, total-viewport]; LTR: [viewport-total, 0])
 * @param isRTL right-to-left layout
 * \endif
 *
 * \if CHINESE
 * @brief 由滚动状态判定滚动按钮可见性（纯函数）
 * @param totalWidth 内容总宽
 * @param viewportWidth 可视区宽度
 * @param xBase 当前滚动基（RTL: [0, total-viewport]；LTR: [viewport-total, 0]）
 * @param isRTL 从右到左布局
 * \endif
 */
inline SARibbonScrollFlags scrollButtonFlags(int totalWidth, int viewportWidth, int xBase, bool isRTL)
{
    SARibbonScrollFlags f;
    const bool needsScrolling = (totalWidth > viewportWidth);
    if (!needsScrolling) {
        return f;  // both false
    }
    if (isRTL) {
        // RTL: mXBase ranges from 0 (start, rightmost) to total-viewportWidth (end, leftmost)
        const int maxBase = totalWidth - viewportWidth;
        if (0 == xBase) {
            // At start (rightmost), can scroll left only
            f.showRight = false;
            f.showLeft  = true;
        } else if (xBase >= maxBase) {
            // At end (leftmost), can scroll right only
            f.showRight = true;
            f.showLeft  = false;
        } else {
            f.showRight = true;
            f.showLeft  = true;
        }
    } else {
        // LTR: mXBase ranges from viewportWidth-total (negative, end, rightmost) to 0 (start, leftmost)
        if (0 == xBase) {
            // Already moved to leftmost, can scroll right
            f.showRight = true;
            f.showLeft  = false;
        } else if (xBase <= (viewportWidth - totalWidth)) {
            // Already moved to rightmost, can scroll left
            f.showRight = false;
            f.showLeft  = true;
        } else {
            f.showRight = true;
            f.showLeft  = true;
        }
    }
    return f;
}

/**
 * \if ENGLISH
 * @brief Clamp a requested scroll position into the legal range (pure)
 * @param requested requested scroll base
 * @param contentWidth total content width
 * @param viewportWidth visible viewport width
 * @param isRTL right-to-left layout
 * @return clamped position (RTL: [0, max(0,content-viewport)]; LTR: [min(viewport-content,0), 0])
 * \endif
 *
 * \if CHINESE
 * @brief 把请求的滚动位置钳制到合法区间（纯函数）
 * @param requested 请求的滚动基
 * @param contentWidth 内容总宽
 * @param viewportWidth 可视区宽度
 * @param isRTL 从右到左布局
 * @return 钳制后的位置（RTL: [0, max(0,content-viewport)]；LTR: [min(viewport-content,0), 0]）
 * \endif
 */
inline int clampScrollOffset(int requested, int contentWidth, int viewportWidth, bool isRTL)
{
    if (isRTL) {
        // RTL: base ranges from 0 (start, rightmost) to contentWidth-viewportWidth (end, leftmost)
        const int maxBase = qMax(0, contentWidth - viewportWidth);
        return qBound(0, requested, maxBase);
    } else {
        // LTR: base ranges from viewportWidth-contentWidth (negative, end) to 0 (start, leftmost)
        const int minBase = qMin(viewportWidth - contentWidth, 0);
        return qBound(minBase, requested, 0);
    }
}

/// Size hints collected per item by the widgets adapter (plan-02 S6-2: the struct
/// moves into core as the engine input carrier; the collector stays adapter-side
/// because separator size hints are widget queries the contract does not expose)
struct SARibbonCategorySizeHints
{
    int totalWidth { 0 };            ///< Total width including margins
    int canExpandingCount { 0 };     ///< Number of expanding panels
    QVector< QSize > panelSizes;     ///< Per-item panel size hints
    QVector< QSize > separatorSizes; ///< Per-item separator size hints
};

/**
 * \if ENGLISH
 * @brief Category layout engine (plan-02 S6, Step B pure move)
 * @details The updateGeometryArr() body was moved verbatim from
 * SARibbonCategoryLayout.cpp; category widget queries became Input values,
 * per-item reads go through SARibbonAbstractCategoryItem. Per-item outputs are
 * written to the contract fields (resultGeometry / resultSeparatorGeometry /
 * rowIndex / columnIndex); panel-level outputs come back through Result.
 * \endif
 *
 * \if CHINESE
 * @brief Category 布局引擎（计划 02 S6，Step B 纯搬移）
 * @details updateGeometryArr() 函数体自 SARibbonCategoryLayout.cpp 纯 move；
 * category 控件查询换为 Input 值，逐项读取经 SARibbonAbstractCategoryItem 契约。
 * 逐项输出写契约字段（resultGeometry / resultSeparatorGeometry）；面板级输出经
 * Result 带回，由适配器回写。
 * \endif
 */
class SA_RIBBON_CORE_EXPORT SARibbonCategoryLayoutEngine
{
public:
    struct Input
    {
        int categoryWidth { 0 };
        int height { 0 };
        int y { 0 };
        QMargins margins;
        SARibbonAlignment alignment { SARibbonAlignment::AlignLeft };
        bool isRTL { false };
        int xBase { 0 };
        SARibbonCategorySizeHints sizeHints;  ///< collected by the adapter
    };

    struct Result
    {
        int totalWidth { 0 };
        QSize sizeHint;
        QSize minSizeHint;
        int newXBase { 0 };  ///< 2.x non-scrolling branch resets mXBase to 0 (state side effect brought back)
        SARibbonScrollFlags scrollFlags;
    };

    SARibbonCategoryLayoutEngine();

    // Layout all panels. Items are SARibbonAbstractCategoryItem (widgets adapter
    // supplies SARibbonCategoryLayoutItem which implements the contract).
    Result layout(QVector< SARibbonAbstractCategoryItem* > items, const Input& input);
};

}
}

#endif  // SARIBBONCATEGORYLAYOUTENGINE_H
