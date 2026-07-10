#include "SARibbonThemeManager.h"
#include "SARibbonBar.h"
#include "SARibbonTabBar.h"
#include "SARibbonUtil.h"
#include "SARibbonThemePalette.h"
#include <map>
#include <QMargins>
#include <QColor>
#include <QFile>
#include <QIODevice>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>

namespace SA
{

// ===================================================
// Static theme data maps
// ===================================================

// Tab margin per theme (affects SARibbonContextCategory drawing)
static const std::map< SARibbonTheme, QMargins > s_themeMargins = {
    { SARibbonTheme::RibbonThemeWindows7, QMargins(5, 0, 0, 0) },
    { SARibbonTheme::RibbonThemeOffice2013, QMargins(5, 0, 0, 0) },
    { SARibbonTheme::RibbonThemeOffice2016Blue, QMargins(5, 0, 0, 0) },
    { SARibbonTheme::RibbonThemeOffice2016Green, QMargins(5, 0, 0, 0) },
    { SARibbonTheme::RibbonThemeOffice2016Dark, QMargins(5, 0, 0, 0) },
    { SARibbonTheme::RibbonThemeDark, QMargins(5, 0, 0, 0) },
    { SARibbonTheme::RibbonThemeDark2, QMargins(5, 0, 0, 0) },
    { SARibbonTheme::RibbonThemeOffice2021Blue, QMargins(5, 0, 5, 0) },
    { SARibbonTheme::RibbonThemeOffice2021Green, QMargins(5, 0, 5, 0) },
    { SARibbonTheme::RibbonThemeOffice2021Dark, QMargins(5, 0, 5, 0) }
};

// Highlight function: produce a darker variant of the context category color
static const SARibbonBar::FpContextCategoryHighlight s_csDarkerHighlight = [](const QColor& c) -> QColor {
    return c.darker();
};

// Highlight function: produce a more vibrant variant of the context category color
static const SARibbonBar::FpContextCategoryHighlight s_csVibrantHighlight = [](const QColor& c) -> QColor {
    return SA::makeColorVibrant(c);
};

// Context category highlight function per theme
static const std::map< SARibbonTheme, SARibbonBar::FpContextCategoryHighlight > s_themeContextHighlights = {
    { SARibbonTheme::RibbonThemeWindows7, s_csVibrantHighlight },
    { SARibbonTheme::RibbonThemeOffice2013, s_csVibrantHighlight },
    { SARibbonTheme::RibbonThemeDark, s_csVibrantHighlight },
    { SARibbonTheme::RibbonThemeOffice2016Blue, s_csDarkerHighlight },
    { SARibbonTheme::RibbonThemeOffice2016Green, s_csDarkerHighlight },
    { SARibbonTheme::RibbonThemeOffice2016Dark, s_csDarkerHighlight },
    { SARibbonTheme::RibbonThemeOffice2021Blue, [](const QColor&) -> QColor { return QColor(39, 96, 167); } },
    { SARibbonTheme::RibbonThemeDark2, s_csVibrantHighlight },
    { SARibbonTheme::RibbonThemeOffice2021Green, s_csVibrantHighlight },
    { SARibbonTheme::RibbonThemeOffice2021Dark, s_csVibrantHighlight }
};

// Context category color list per theme
static const std::map< SARibbonTheme, QList< QColor > > s_themeContextColorLists = {
    { SARibbonTheme::RibbonThemeWindows7, {} },
    { SARibbonTheme::RibbonThemeOffice2013, {} },
    { SARibbonTheme::RibbonThemeDark, {} },
    { SARibbonTheme::RibbonThemeOffice2016Blue, { QColor(18, 64, 120) } },
    { SARibbonTheme::RibbonThemeOffice2016Green, { QColor(24, 96, 48) } },
    { SARibbonTheme::RibbonThemeOffice2016Dark, { QColor(60, 60, 60) } },
    { SARibbonTheme::RibbonThemeOffice2021Blue, { QColor(209, 207, 209) } },
    { SARibbonTheme::RibbonThemeDark2, { QColor(42, 141, 181) } },
    { SARibbonTheme::RibbonThemeOffice2021Green, { QColor(180, 200, 180) } },
    { SARibbonTheme::RibbonThemeOffice2021Dark, { QColor(80, 80, 80) } }
};

// Tab bar baseline color per theme (only Office2013 has a visible baseline)
static const std::map< SARibbonTheme, QColor > s_themeBaselineColors = {
    { SARibbonTheme::RibbonThemeWindows7, QColor() },
    { SARibbonTheme::RibbonThemeOffice2013, QColor(186, 201, 219) },
    { SARibbonTheme::RibbonThemeOffice2016Blue, QColor() },
    { SARibbonTheme::RibbonThemeOffice2016Green, QColor() },
    { SARibbonTheme::RibbonThemeOffice2016Dark, QColor() },
    { SARibbonTheme::RibbonThemeOffice2021Blue, QColor() },
    { SARibbonTheme::RibbonThemeDark, QColor() },
    { SARibbonTheme::RibbonThemeDark2, QColor() },
    { SARibbonTheme::RibbonThemeOffice2021Green, QColor() },
    { SARibbonTheme::RibbonThemeOffice2021Dark, QColor() }
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
// Forward declaration — defined later in this file
static QString themeToPalettePath(SARibbonTheme theme);

void applyRibbonTheme(QWidget* w, SARibbonBar* bar, SARibbonTheme theme)
{
    SARibbonThemePalette palette;
    QString palettePath = themeToPalettePath(theme);
    if (!palettePath.isEmpty()) {
        if (palette.loadFromFile(palettePath)) {
            applyRibbonTheme(w, bar, theme, palette);
            return;
        }
        qWarning() << "applyRibbonTheme: failed to load palette" << palettePath
                     << "for theme" << static_cast<int>(theme) << "- falling back to built-in QSS";
    }
    applyRibbonTheme(w, bar, theme, SARibbonThemePalette());
}

/**
 * \if ENGLISH
 * @brief Map SARibbonTheme enum to the corresponding QSS template resource path
 * @return Resource path of the QSS template for the given theme, or an empty string if no template exists
 * \endif
 *
 * \if CHINESE
 * @brief 将 SARibbonTheme 枚举映射到对应的 QSS 模板资源路径
 * @return 给定主题对应的 QSS 模板资源路径，若该主题无模板则返回空字符串
 * \endif
 */
static QString themeToTemplatePath(SARibbonTheme theme)
{
    switch (theme) {
    case SARibbonTheme::RibbonThemeOffice2016Blue:
    case SARibbonTheme::RibbonThemeOffice2016Green:
    case SARibbonTheme::RibbonThemeOffice2016Dark:
        return ":/SARibbonTheme/resource/templates/office2016.qss";
    case SARibbonTheme::RibbonThemeOffice2021Blue:
    case SARibbonTheme::RibbonThemeOffice2021Green:
    case SARibbonTheme::RibbonThemeOffice2021Dark:
        return ":/SARibbonTheme/resource/templates/office2021.qss";
    case SARibbonTheme::RibbonThemeDark:
        return ":/SARibbonTheme/resource/templates/dark.qss";
    case SARibbonTheme::RibbonThemeDark2:
        return ":/SARibbonTheme/resource/templates/dark2.qss";
    case SARibbonTheme::RibbonThemeWindows7:
        return ":/SARibbonTheme/resource/templates/win7.qss";
    case SARibbonTheme::RibbonThemeOffice2013:
        return ":/SARibbonTheme/resource/templates/office2013.qss";
    default:
        return QString();
    }
}

/**
 * \if ENGLISH
 * @brief Map SARibbonTheme enum to the corresponding default palette JSON resource path
 * @return Resource path of the default palette JSON for the given theme, or an empty string if no palette exists
 * \endif
 *
 * \if CHINESE
 * @brief 将 SARibbonTheme 枚举映射到对应的默认调色板 JSON 资源路径
 * @return 给定主题对应的默认调色板 JSON 资源路径，若该主题无调色板则返回空字符串
 * \endif
 */
static QString themeToPalettePath(SARibbonTheme theme)
{
    switch (theme) {
    case SARibbonTheme::RibbonThemeOffice2016Blue:
        return ":/SARibbonTheme/resource/palettes/office2016-blue.json";
    case SARibbonTheme::RibbonThemeOffice2016Green:
        return ":/SARibbonTheme/resource/palettes/office2016-green.json";
    case SARibbonTheme::RibbonThemeOffice2016Dark:
        return ":/SARibbonTheme/resource/palettes/office2016-dark.json";
    case SARibbonTheme::RibbonThemeOffice2021Blue:
        return ":/SARibbonTheme/resource/palettes/office2021-blue.json";
    case SARibbonTheme::RibbonThemeOffice2021Green:
        return ":/SARibbonTheme/resource/palettes/office2021-green.json";
    case SARibbonTheme::RibbonThemeOffice2021Dark:
        return ":/SARibbonTheme/resource/palettes/office2021-dark.json";
    case SARibbonTheme::RibbonThemeDark:
        return ":/SARibbonTheme/resource/palettes/dark-default.json";
    case SARibbonTheme::RibbonThemeDark2:
        return ":/SARibbonTheme/resource/palettes/dark2-default.json";
    case SARibbonTheme::RibbonThemeWindows7:
        return ":/SARibbonTheme/resource/palettes/win7-default.json";
    case SARibbonTheme::RibbonThemeOffice2013:
        return ":/SARibbonTheme/resource/palettes/office2013-default.json";
    default:
        return QString();
    }
}

/**
 * \if ENGLISH
 * @brief Apply a ribbon theme with custom color palette
 *
 * This overload loads the QSS template for the specified theme and replaces color tokens
 * using the provided palette. If the palette is empty or no template is found for the theme,
 * it falls back to loading the base QSS only (without theme-specific colors).
 *
 * After applying the stylesheet, the function also configures theme-dependent ribbon bar
 * properties: tab bar margins, context category colors, context category highlight function,
 * and tab bar baseline color.
 *
 * @param w The widget that receives the QSS stylesheet
 * @param bar The SARibbonBar whose theme-dependent properties are configured (may be null)
 * @param theme The built-in ribbon theme to apply
 * @param palette The color palette for token replacement in the QSS template
 * \endif
 *
 * \if CHINESE
 * @brief 使用自定义调色板应用ribbon主题
 *
 * 此重载加载指定主题的QSS模板，并使用提供的调色板替换颜色标记。
 * 如果调色板为空或主题没有对应的模板，则回退到仅加载基础QSS（不含主题特定颜色）。
 *
 * 应用样式表后，此函数还会配置主题依赖的ribbon栏属性：标签栏边距、上下文标签颜色、
 * 上下文标签高亮函数以及标签栏基线颜色。
 *
 * @param w 接收QSS样式表的窗口部件
 * @param bar 需要配置主题依赖属性的SARibbonBar（可为null）
 * @param theme 要应用的内置ribbon主题
 * @param palette 用于QSS模板中标记替换的调色板
 * \endif
 */
void applyRibbonTheme(QWidget* w, SARibbonBar* bar, SARibbonTheme theme,
                      const SARibbonThemePalette& palette)
{
    // If palette is provided and a template exists, use template-based approach
    QString templatePath = themeToTemplatePath(theme);
    if (palette.variables().size() > 0 && !templatePath.isEmpty() && w) {
        QFile baseFile(":/SARibbonTheme/resource/theme-base.qss");
        QString baseQss;
        if (baseFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            baseQss = QString::fromUtf8(baseFile.readAll());
        }

        // Load the QSS template
        QFile templateFile(templatePath);
        if (templateFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString templateQss = QString::fromUtf8(templateFile.readAll());
            // Replace {{token}} placeholders with palette colors
            QString processedQss = SA::replaceQssTokens(templateQss, palette);
            w->setStyleSheet(baseQss + "\n" + processedQss);
        } else {
            // Template file not found, fall back to base QSS
            qWarning() << "applyRibbonTheme: template not found:" << templatePath
                       << "- falling back to base QSS";
            QFile baseOnly(":/SARibbonTheme/resource/theme-base.qss");
            if (baseOnly.open(QIODevice::ReadOnly | QIODevice::Text)) {
                w->setStyleSheet(QString::fromUtf8(baseOnly.readAll()));
            }
        }
    } else if (w) {
        // Empty palette — fall back to base QSS only
        QFile baseQss(":/SARibbonTheme/resource/theme-base.qss");
        if (baseQss.open(QIODevice::ReadOnly | QIODevice::Text)) {
            w->setStyleSheet(QString::fromUtf8(baseQss.readAll()));
        }
    }

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