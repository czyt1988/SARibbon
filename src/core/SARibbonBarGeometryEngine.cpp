#include <SARibbonCore/SARibbonBarGeometryEngine.h>

// 计划 02 S7（D6 范围）：layoutTitleRect 四分支自 SARibbonBarLayout.cpp 纯 move。
// 机械替换表（其余一字不改）：
//   SA::saIsRTL()                    -> input.isRTL
//   isCompactStyle()                 -> input.isCompactStyle
//   ribbon->width()                  -> input.ribbonWidth
//   d_ptr->contentsMargins()         -> input.border
//   d_ptr->getActualTitleBarHeight() -> input.validTitleBarHeight
//   ribbonTabBar->geometry()         -> input.tabBarGeometry
//   quickAccessBar (含 geometry)     -> input.hasQuickAccessBar + input.quickAccessBarGeometry
//   d_ptr->systemButtonSize          -> input.systemButtonSize
//   ribbon->currentVisibleContextCategoryTabIndexs()
//                                    -> input.hasContextTabs + contextFirst/LastTabRect
//   ribbonTabBar->tabRect(idx) + tabX-> 适配器预先算好的 contextFirst/LastTabRect
//   d_ptr->titleRect（状态写入）      -> 返回值（适配器回写）
namespace SARibbon
{
namespace Core
{

QRect SARibbonBarGeometryEngine::layoutTitleRect(const TitleRectInput& input)
{
    const QMargins border         = input.border;
    const int validTitleBarHeight = input.validTitleBarHeight;

    // 计算标题栏区域
    if (input.isRTL) {
        // RTL mode: title rect positions are mirrored
        if (input.isCompactStyle) {
            // 紧凑模式 RTL: title bar in the remaining space left of tabbar
            // In RTL, titleEnd is the left edge of tabbar (mirrored from right)
            int titleEnd   = input.tabBarGeometry.left();
            int titleWidth = input.hasQuickAccessBar
                                 ? (titleEnd - input.quickAccessBarGeometry.right())
                                 : (titleEnd - border.left());
            if (titleWidth > 10) {
                return QRect(titleEnd - titleWidth, border.top(), titleWidth, validTitleBarHeight);
            } else {
                return QRect();
            }
        } else {
            // 三行宽松模式 RTL
            const int tabX = input.tabBarGeometry.x();
            // In RTL, contextRegionLeft becomes the leftmost (visual rightmost) tab boundary
            // and contextRegionRight becomes the rightmost (visual leftmost) tab boundary
            // We swap the variable semantics to reflect the mirrored geometry
            int contextRegionLeft  = -1;             // In RTL: the left boundary of context region (visual rightmost)
            int contextRegionRight = input.ribbonWidth;  // In RTL: the right boundary of context region (visual leftmost)

            if (input.hasContextTabs) {
                // In RTL, first context tab rect's left() is visually the rightmost edge
                int edgeVal = input.contextFirstTabRect.left() + tabX;
                if (edgeVal > contextRegionLeft) {
                    contextRegionLeft = edgeVal;
                }
                // In RTL, last context tab rect's right() is visually the leftmost edge
                edgeVal = input.contextLastTabRect.right() + tabX;
                if (edgeVal < contextRegionRight) {
                    contextRegionRight = edgeVal;
                }
            }

            // In RTL, x1 is the right side of the quickAccessBar (which is now on the right)
            int x1 = border.left();
            if (input.hasQuickAccessBar) {
                // In RTL, quickAccessBar is at right side, so title area starts from its left edge
                x1 = input.quickAccessBarGeometry.x();
            }
            // In RTL, x2 is the left side (system buttons are at left)
            int x2 = border.left() + input.systemButtonSize.width();

            if (contextRegionLeft < 0) {
                // No context category: title between system buttons (left) and quickAccessBar (right)
                return QRect(QPoint(x2, border.top()), QPoint(x1, validTitleBarHeight + border.top()));
            } else {
                int leftwidth  = contextRegionRight - x2;
                int rightwidth = x1 - contextRegionLeft;
                if (leftwidth > rightwidth) {
                    return QRect(QPoint(x2, border.top()), QPoint(contextRegionRight, validTitleBarHeight + border.top()));
                } else {
                    return QRect(QPoint(contextRegionLeft, border.top()), QPoint(x1, validTitleBarHeight + border.top()));
                }
            }
        }
    } else {
        // LTR mode: original title rect logic unchanged
        if (input.isCompactStyle) {
            // 紧凑模式,紧凑模式的标题栏在tabbar的剩余空间中
            int titleStart = input.tabBarGeometry.right();
            int titleWidth = input.hasQuickAccessBar
                                 ? (input.quickAccessBarGeometry.x() - titleStart)
                                 : (input.ribbonWidth - titleStart - input.systemButtonSize.width());
            if (titleWidth > 10) {
                return QRect(titleStart, border.top(), titleWidth, validTitleBarHeight);
            } else {
                // 标题栏过小，就不显示
                return QRect();
            }
        } else {
            const int tabX = input.tabBarGeometry.x();
            // 三行宽松模式
            int contextRegionLeft  = input.ribbonWidth;
            int contextRegionRight = -1;

            // 使用上下文标签的视觉数据
            // 上下文标签会占用宽松模式下的标题栏位置，因此，要计算此时标题栏应该在哪里显示
            if (input.hasContextTabs) {
                int edgeVal = input.contextFirstTabRect.left() + tabX;
                if (edgeVal < contextRegionLeft) {
                    contextRegionLeft = edgeVal;
                }
                edgeVal = input.contextLastTabRect.right() + tabX;
                if (edgeVal > contextRegionRight) {
                    contextRegionRight = edgeVal;
                }
            }

            int x1 = border.left();
            if (input.hasQuickAccessBar) {
                x1 = input.quickAccessBarGeometry.right() + 1;
            }
            int x2 = input.ribbonWidth - input.systemButtonSize.width() - border.right();

            if (contextRegionRight < 0) {
                // 说明没有上下文标签，那么标题直接放在quickAccessBar到systembar之间
                return QRect(QPoint(x1, border.top()), QPoint(x2, validTitleBarHeight + border.top()));
            } else {
                int leftwidth  = contextRegionLeft - x1;
                int rightwidth = x2 - contextRegionRight;
                if (rightwidth > leftwidth) {
                    return QRect(QPoint(contextRegionRight, border.top()), QPoint(x2, validTitleBarHeight + border.top()));
                } else {
                    return QRect(QPoint(x1, border.top()), QPoint(contextRegionLeft, validTitleBarHeight + border.top()));
                }
            }
        }
    }
}

}
}
