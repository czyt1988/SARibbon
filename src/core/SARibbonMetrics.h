#ifndef SARIBBONMETRICS_H
#define SARIBBONMETRICS_H
#include <SARibbonCore/SARibbonCoreGlobal.h>
#include <QFontMetrics>
#include <QSize>
#include <optional>

namespace SARibbon
{
namespace Core
{

/**
 * \if ENGLISH
 * @brief Metric collection and derivation for the ribbon bar (plan 02 S3)
 * @details Gathers the size constants and row-height derivations that used to
 * live inside SARibbonBarLayout::PrivateData. Core never queries any widget:
 * style-derived pixel metrics and the font metrics are inputs supplied by the
 * widgets adapter (v2 section 3.3), so all derivation formulas are pure
 * functions of the stored state. Formula bodies were moved verbatim from
 * SARibbonBarLayout.cpp (calcDefaultTabBarHeight / calcDefaultTitleBarHeight /
 * calcCategoryHeight / calcMainBarHeight).
 * \endif
 *
 * \if CHINESE
 * @brief Ribbon 栏的度量收口类（计划 02 S3）
 * @details 收口原 SARibbonBarLayout::PrivateData 中的尺寸常量与行高推导。
 * core 不查询任何控件：style 派生的 pixelMetric 与字体度量均由 widgets
 * 适配器作为输入传入（v2 §3.3），全部推导公式是所存状态的纯函数。
 * 公式体自 SARibbonBarLayout.cpp 纯 move（calcDefaultTabBarHeight /
 * calcDefaultTitleBarHeight / calcCategoryHeight / calcMainBarHeight）。
 * @note 2.9.5 默认值：titleBarHeight=30、tabBarHeight=28、categoryHeight=60、
 * panelTitleHeight=15（首值会被 estimateSizeHint() 的推导结果覆写）
 * \endif
 */
class SA_RIBBON_CORE_EXPORT SARibbonMetrics
{
public:
    SARibbonMetrics();
    SARibbonMetrics(const QFontMetrics& fm, qreal devicePixelRatio);

    // ---- inputs (adapter collects; core never queries QStyle/widget) ----
    void setFontMetrics(const QFontMetrics& fm);
    void setDevicePixelRatio(qreal dpr);
    // Style pixel metrics: PM_TabBarBaseHeight / PM_TabBarTabHSpace /
    // PM_TabBarTabOverlap / PM_TitleBarHeight (QStyle is QtWidgets, core forbids)
    void setStylePixelMetrics(int pmTabBarBaseHeight, int pmTabBarTabHSpace, int pmTabBarTabOverlap, int pmTitleBarHeight);

    // ---- derivation formulas (moved verbatim, SARibbonBarLayout.cpp) ----

    // System tab bar height: sum of the three style pixel metrics
    int systemTabBarHeight() const;

    // Calculate default tab bar height (SARibbonBarLayout.cpp calcDefaultTabBarHeight)
    int calcDefaultTabBarHeight() const;

    // Calculate default title bar height (SARibbonBarLayout.cpp calcDefaultTitleBarHeight)
    int calcDefaultTitleBarHeight() const;

    // Calculate category height (SARibbonBarLayout.cpp calcCategoryHeight);
    // isThreeRowStyle/isSingleRowStyle replace the ribbonBar->isXxxStyle() queries
    int calcCategoryHeight(bool isThreeRowStyle, bool isSingleRowStyle) const;

    // Calculate main bar height (static pure, SARibbonBarLayout.cpp calcMainBarHeight);
    // minimumMode replaces the SARibbonBar::RibbonMode comparison
    static int calcMainBarHeight(int tabBarHeight, int titleHeight, int categoryHeight, bool tabOnTitle, bool minimumMode);

    // ---- estimate & actual (moved from SARibbonBarLayout.cpp estimateSizeHint / getActual*) ----

    // Recalculate the default heights from current font/style inputs
    void estimateSizeHint(bool isThreeRowStyle, bool isSingleRowStyle);

    // User-defined overrides win over the (estimated) defaults
    int getActualTitleBarHeight() const;
    int getActualTabBarHeight() const;
    int getActualCategoryHeight() const;

    void setTitleBarHeight(int h);
    void setTabBarHeight(int h);
    void setCategoryHeight(int h);

    // ---- fields (2.9.5 defaults; panelTitleHeight participates in calcCategoryHeight) ----
    int titleBarHeight { 30 };    ///< Title bar height
    int tabBarHeight { 28 };      ///< Tab bar height
    int panelTitleHeight { 15 };  ///< Panel title default height
    int categoryHeight { 60 };    ///< Category height

    int maxMinWidth { 1000 };  ///< Maximum minimum width, usually 0.8 of screen width
    int minWidth { 500 };
    int minHeight { 0 };

    QFontMetrics fontMetrics() const;
    qreal devicePixelRatio() const;

private:
    QFontMetrics mFontMetrics;
    qreal mDevicePixelRatio { 1.0 };
    int mPmTabBarBaseHeight { 0 };
    int mPmTabBarTabHSpace { 0 };
    int mPmTabBarTabOverlap { 0 };
    int mPmTitleBarHeight { 0 };
    std::optional< int > mUserDefTitleBarHeight;
    std::optional< int > mUserDefTabBarHeight;
    std::optional< int > mUserDefCategoryHeight;
};

}
}

#endif  // SARIBBONMETRICS_H
