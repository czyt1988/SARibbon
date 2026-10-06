#ifndef SARIBBONBARGEOMETRYENGINE_H
#define SARIBBONBARGEOMETRYENGINE_H
#include <SARibbonCore/SARibbonCoreGlobal.h>
#include <QMargins>
#include <QRect>
#include <QSize>

namespace SARibbon
{
namespace Core
{

/**
 * \if ENGLISH
 * @brief Bar geometry engine (plan-02 S7, D6-reduced scope)
 * @details Scope per plan-02 S7-3: the metric part already lives in
 * SARibbonMetrics (S3); this engine carries the layoutTitleRect pure geometry
 * (four branches: RTL/LTR x Compact/Loose, moved verbatim from
 * SARibbonBarLayout.cpp). The two big widget-placement functions
 * (resizeInLooseStyle / resizeInCompactStyle) stay in widgets — they are pure
 * widget placement per v2 section 3.4.4.
 * \endif
 *
 * \if CHINESE
 * @brief Bar 几何引擎（计划 02 S7，D6 降级范围）
 * @details 范围按计划 02 S7-3：度量部分已在 SARibbonMetrics（S3）；本引擎承载
 * layoutTitleRect 的纯几何（RTL/LTR × 紧凑/宽松 四分支，自
 * SARibbonBarLayout.cpp 纯 move）。两大 widget 摆放函数
 * （resizeInLooseStyle / resizeInCompactStyle）留 widgets——v2 §3.4.4 明确
 * 属 widget 摆放主体。
 * \endif
 */
class SA_RIBBON_CORE_EXPORT SARibbonBarGeometryEngine
{
public:
    struct TitleRectInput
    {
        bool isRTL { false };
        bool isCompactStyle { false };
        int ribbonWidth { 0 };
        QMargins border;
        int validTitleBarHeight { 0 };
        QRect tabBarGeometry;             ///< current tab bar geometry
        bool hasQuickAccessBar { false };  ///< whether a quick access bar exists
        QRect quickAccessBarGeometry;     ///< its geometry (valid only when hasQuickAccessBar)
        QSize systemButtonSize;           ///< close/max/min button group size
        bool hasContextTabs { false };    ///< whether context category tabs are visible
        QRect contextFirstTabRect;        ///< first visible context tab rect (offset by tabX already)
        QRect contextLastTabRect;         ///< last visible context tab rect (offset by tabX already)
    };

    /// Minimum window width inputs; the row-zone widths come pre-composed from
    /// the owning layout (front-end placement constants stay out of core)
    struct MinimumWidthInput
    {
        bool isCompactStyle { false };  ///< compact: tab row rides the title row
        int tabRowWidth { 0 };       ///< no-title minimum of the tab row content zone (compact: the whole shared row)
        int titleRowWidth { 0 };     ///< no-title minimum of the title row content zone (loose only, 0 in compact)
        int titleTextWidth { 0 };    ///< window title text width to reserve
        int screenAvailableWidth { 0 };  ///< <= 0 = unknown, the screen rules are skipped
    };

    // Pure geometry of the window-title free area (four branches moved verbatim).
    // Returns an empty QRect when the title area is too small to display.
    static QRect layoutTitleRect(const TitleRectInput& input);

    // Minimum window width so the title-row / tab-row controls never overlap.
    // Composition: loose = max(tab row, title row + title text); compact =
    // shared row + title text. Screen rules (screenAvailableWidth > 0):
    // 1. the title-inclusive width may never exceed 2/3 of the screen (hence
    //    never the screen itself);
    // 2. over that cap the title is sacrificed first (dropped from the
    //    reservation) to keep shrink room for the user;
    // 3. still over the cap without the title, overlap is allowed and the
    //    minimum is capped at 2/3 of the screen.
    static int calcMinimumWidth(const MinimumWidthInput& input);
};

}
}

#endif  // SARIBBONBARGEOMETRYENGINE_H
