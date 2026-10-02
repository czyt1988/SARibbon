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

/// Arrow button width in pixels (2.x widgets hardcoded 12 in SARibbonCategoryLayout::doLayout)
constexpr int SCROLL_BUTTON_WIDTH = 12;

/// Animated scroll duration in milliseconds (2.x widgets setupAnimateScroll), easing OutQuad
constexpr int SCROLL_ANIMATION_DURATION = 300;

/**
 * \if ENGLISH
 * @brief Left/right scroll arrow button rectangles (plan-04 WS-A3)
 * \endif
 *
 * \if CHINESE
 * @brief 左右滚动箭头按钮矩形（计划 04 WS-A3）
 * \endif
 */
struct SARibbonScrollButtonRects
{
    QRect left;  ///< left arrow button rectangle
    QRect right; ///< right arrow button rectangle
};

/**
 * \if ENGLISH
 * @brief Compute the two scroll arrow button rectangles (pure)
 * @details Moved out of SARibbonCategoryLayout::doLayout so the QML front end can
 * place the same arrows without copying the geometry (section 4-4 no-dual-implementation
 * rule). Both rectangles span the full category height and sit on the outer edges;
 * under RTL the two sides are swapped, exactly as widgets does.
 * @param categoryWidth category viewport width
 * @param categoryHeight category height
 * @param isRTL right-to-left layout
 * @param buttonWidth arrow button width (defaults to SCROLL_BUTTON_WIDTH)
 * \endif
 *
 * \if CHINESE
 * @brief 计算两个滚动箭头按钮的矩形（纯函数）
 * @details 自 SARibbonCategoryLayout::doLayout 下沉，使 QML 前端无需复制几何即可
 * 摆放同样的箭头（§4-4 禁止双实现条款）。两个矩形都占满 category 高度并贴外侧边；
 * RTL 下左右互换，与 widgets 一致。
 * @param categoryWidth category 可视区宽度
 * @param categoryHeight category 高度
 * @param isRTL 从右到左布局
 * @param buttonWidth 箭头按钮宽度（默认 SCROLL_BUTTON_WIDTH）
 * \endif
 */
inline SARibbonScrollButtonRects scrollButtonRects(int categoryWidth, int categoryHeight, bool isRTL, int buttonWidth = SCROLL_BUTTON_WIDTH)
{
    SARibbonScrollButtonRects r;
    const int w = qBound(0, buttonWidth, qMax(0, categoryWidth));
    const int h = qMax(0, categoryHeight);
    if (isRTL) {
        r.left  = QRect(categoryWidth - w, 0, w, h);
        r.right = QRect(0, 0, w, h);
    } else {
        r.left  = QRect(0, 0, w, h);
        r.right = QRect(categoryWidth - w, 0, w, h);
    }
    return r;
}

/**
 * \if ENGLISH
 * @brief Scroll delta produced by one arrow button click (pure)
 * @details widgets steps by half the viewport width and flips the sign under RTL;
 * both front ends must produce the same number, hence the shared function.
 * @param viewportWidth category viewport width
 * @param isLeftButton true for the left arrow, false for the right one
 * @param isRTL right-to-left layout
 * @return signed delta to add to the scroll base
 * \endif
 *
 * \if CHINESE
 * @brief 一次箭头按钮点击产生的滚动增量（纯函数）
 * @details widgets 以可视区宽度的一半为步长，并在 RTL 下翻转符号；两个前端必须给出
 * 同一个数值，故收敛为本函数。
 * @param viewportWidth category 可视区宽度
 * @param isLeftButton 左箭头为 true，右箭头为 false
 * @param isRTL 从右到左布局
 * @return 加到滚动基上的带符号增量
 * \endif
 */
inline int scrollButtonStep(int viewportWidth, bool isLeftButton, bool isRTL)
{
    const int half = viewportWidth / 2;
    // LTR: left arrow moves the content forward (+), right arrow backward (-);
    // RTL mirrors both
    return (isLeftButton != isRTL) ? half : -half;
}

/**
 * \if ENGLISH
 * @brief Pick the wheel delta a category scroll should react to (pure)
 * @details Horizontal components win over vertical ones (trackpad horizontal
 * gestures), and the angle delta is normalized by the 8-degrees-per-notch
 * convention exactly as widgets SARibbonCategory does.
 * @param pixelDelta QWheelEvent::pixelDelta()
 * @param angleDelta QWheelEvent::angleDelta()
 * @return chosen delta, 0 when the event carries none
 * \endif
 *
 * \if CHINESE
 * @brief 选出 category 滚动应当响应的滚轮增量（纯函数）
 * @details 水平分量优先于垂直分量（触控板水平手势），角度增量按 8 度一格的惯例归一，
 * 与 widgets SARibbonCategory 完全一致。
 * @param pixelDelta QWheelEvent::pixelDelta()
 * @param angleDelta QWheelEvent::angleDelta()
 * @return 选中的增量，事件不含增量时为 0
 * \endif
 */
inline int wheelScrollDelta(const QPoint& pixelDelta, const QPoint& angleDelta)
{
    const QPoint numDegrees = angleDelta / 8;
    if (0 != pixelDelta.x()) {
        return pixelDelta.x();
    } else if (0 != pixelDelta.y()) {
        return pixelDelta.y();
    } else if (0 != numDegrees.x()) {
        return numDegrees.x();
    } else if (0 != numDegrees.y()) {
        return numDegrees.y();
    }
    return 0;
}

/**
 * \if ENGLISH
 * @brief Scale the base wheel step by how fast the wheel is turning (pure)
 * @param baseStep configured step (widgets default 400)
 * @param wheelDelta delta chosen by wheelScrollDelta()
 * @return signed step, doubled above 60 and halved below 20 (widgets parity)
 * \endif
 *
 * \if CHINESE
 * @brief 按滚轮速度缩放基础步长（纯函数）
 * @param baseStep 配置的步长（widgets 默认 400）
 * @param wheelDelta wheelScrollDelta() 选出的增量
 * @return 带符号步长，绝对值大于 60 翻倍、小于 20 减半（与 widgets 一致）
 * \endif
 */
inline int scaledWheelStep(int baseStep, int wheelDelta)
{
    if (0 == wheelDelta) {
        return baseStep;  // widgets parity: an event carrying no delta leaves the base step untouched
    }
    int step = (wheelDelta < 0) ? -baseStep : baseStep;
    const int absDelta = qAbs(wheelDelta);
    if (absDelta > 60) {
        step *= 2;
    } else if (absDelta < 20) {
        step /= 2;
    }
    return step;
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
