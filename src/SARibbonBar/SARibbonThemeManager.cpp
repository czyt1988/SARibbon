#include "SARibbonThemeManager.h"
#include "SARibbonBar.h"
#include "SARibbonTabBar.h"
#include "SARibbonUtil.h"
#include <map>
#include <QMargins>
#include <QColor>

namespace SA
{

// ===================================================
// Static theme data maps
// ===================================================

/// Tab margin per theme (affects SARibbonContextCategory drawing)
static const std::map< SARibbonTheme, QMargins > s_themeMargins = {
    { SARibbonTheme::RibbonThemeWindows7, QMargins(5, 0, 0, 0) },
    { SARibbonTheme::RibbonThemeOffice2013, QMargins(5, 0, 0, 0) },
    { SARibbonTheme::RibbonThemeOffice2016Blue, QMargins(5, 0, 0, 0) },
    { SARibbonTheme::RibbonThemeDark, QMargins(5, 0, 0, 0) },
    { SARibbonTheme::RibbonThemeDark2, QMargins(5, 0, 0, 0) },
    { SARibbonTheme::RibbonThemeOffice2021Blue, QMargins(5, 0, 5, 0) }
};

/// Highlight function: produce a darker variant of the context category color
static const SARibbonBar::FpContextCategoryHighlight s_csDarkerHighlight = [](const QColor& c) -> QColor {
    return c.darker();
};

/// Highlight function: produce a more vibrant variant of the context category color
static const SARibbonBar::FpContextCategoryHighlight s_csVibrantHighlight = [](const QColor& c) -> QColor {
    return SA::makeColorVibrant(c);
};

/// Context category highlight function per theme
static const std::map< SARibbonTheme, SARibbonBar::FpContextCategoryHighlight > s_themeContextHighlights = {
    { SARibbonTheme::RibbonThemeWindows7, s_csVibrantHighlight },
    { SARibbonTheme::RibbonThemeOffice2013, s_csVibrantHighlight },
    { SARibbonTheme::RibbonThemeDark, s_csVibrantHighlight },
    { SARibbonTheme::RibbonThemeOffice2016Blue, s_csDarkerHighlight },
    { SARibbonTheme::RibbonThemeOffice2021Blue, [](const QColor&) -> QColor { return QColor(39, 96, 167); } },
    { SARibbonTheme::RibbonThemeDark2, s_csVibrantHighlight }
};

/// Context category color list per theme
static const std::map< SARibbonTheme, QList< QColor > > s_themeContextColorLists = {
    { SARibbonTheme::RibbonThemeWindows7, {} },
    { SARibbonTheme::RibbonThemeOffice2013, {} },
    { SARibbonTheme::RibbonThemeDark, {} },
    { SARibbonTheme::RibbonThemeOffice2016Blue, { QColor(18, 64, 120) } },
    { SARibbonTheme::RibbonThemeOffice2021Blue, { QColor(209, 207, 209) } },
    { SARibbonTheme::RibbonThemeDark2, { QColor(42, 141, 181) } }
};

/// Tab bar baseline color per theme (only Office2013 has a visible baseline)
static const std::map< SARibbonTheme, QColor > s_themeBaselineColors = {
    { SARibbonTheme::RibbonThemeWindows7, QColor() },
    { SARibbonTheme::RibbonThemeOffice2013, QColor(186, 201, 219) },
    { SARibbonTheme::RibbonThemeOffice2016Blue, QColor() },
    { SARibbonTheme::RibbonThemeOffice2021Blue, QColor() },
    { SARibbonTheme::RibbonThemeDark, QColor() },
    { SARibbonTheme::RibbonThemeDark2, QColor() }
};

// ===================================================
// applyRibbonTheme
// ===================================================

/**
 * \if ENGLISH
 * @brief Apply a built-in ribbon theme to the widget and configure the ribbon bar's theme-dependent properties.
 *
 * This performs the full theme-switch sequence:
 * 1. Loads the QSS stylesheet for the theme onto @p w
 * 2. Adjusts the tab bar margins to match the theme's QSS layout
 * 3. Sets the context category color list for context tabs
 * 4. Registers the context category highlight function (controls tab color when selected)
 * 5. Sets the tab bar baseline color (visible only in Office2013)
 *
 * @param w The widget that receives the QSS stylesheet
 * @param bar The SARibbonBar whose theme-dependent properties are configured (may be null)
 * @param theme The built-in ribbon theme to apply
 * \endif
 *
 * \if CHINESE
 * @brief 对窗口部件应用内置ribbon主题，并配置ribbon栏的主题依赖属性。
 *
 * 此函数执行完整的主题切换序列：
 * 1. 将主题的 QSS 样式表加载到 @p w 上
 * 2. 根据主题调整标签栏边距以匹配 QSS 布局
 * 3. 设置上下文标签的颜色列表
 * 4. 注册上下文标签高亮函数（控制标签选中时的颜色）
 * 5. 设置标签栏基线颜色（仅在 Office2013 主题中可见）
 *
 * @param w 接收 QSS 样式表的窗口部件
 * @param bar 需要配置主题依赖属性的 SARibbonBar（可为 null）
 * @param theme 要应用的内置 ribbon 主题
 * \endif
 */
void applyRibbonTheme(QWidget* w, SARibbonBar* bar, SARibbonTheme theme)
{
    // 1. Load the QSS stylesheet for the theme
    SA::setBuiltInRibbonTheme(w, theme);

    if (!bar) {
        return;
    }

    // 2. Adjust tab bar margins to match the theme's QSS layout
    if (SARibbonTabBar* tab = bar->ribbonTabBar()) {
        auto itMargins = s_themeMargins.find(theme);
        if (itMargins != s_themeMargins.end()) {
            tab->setTabMargin(itMargins->second);
        }
    }

    // 3. Set context category color list
    {
        auto itColors = s_themeContextColorLists.find(theme);
        if (itColors != s_themeContextColorLists.end()) {
            bar->setContextCategoryColorList(itColors->second);
        }
    }

    // 4. Register context category highlight function
    {
        auto itHighlight = s_themeContextHighlights.find(theme);
        if (itHighlight != s_themeContextHighlights.end()) {
            bar->setContextCategoryColorHighLight(itHighlight->second);
        }
    }

    // 5. Set tab bar baseline color
    {
        auto itBaseline = s_themeBaselineColors.find(theme);
        if (itBaseline != s_themeBaselineColors.end()) {
            bar->setTabBarBaseLineColor(itBaseline->second);
        }
    }
}

}