#include "SARibbonThemeManager.h"
#include "SARibbonBar.h"
#include <SARibbonCore/SARibbonThemeData.h>
#include "SARibbonTabBar.h"
#include "SARibbonUtil.h"
#include "SARibbonThemePalette.h"
#include <QMargins>
#include <QColor>
#include <QFile>
#include <QIODevice>
#include <QDir>
#include <QCoreApplication>
#include <QDebug>
#include <QHash>

namespace SA
{

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
// ===================================================
// QSS resource text cache
// ===================================================

/**
 * \if ENGLISH
 * @brief Load text content from a Qt resource path with static caching
 * @details The file content is cached in a static QHash for the lifetime of the process.
 *          Repeated calls with the same path return the cached content without disk I/O.
 *          Qt resources are memory-mapped, so the initial read is already fast, but this
 *          avoids redundant string allocations on repeated theme switches.
 * @param path Qt resource path (e.g. ":/SARibbonTheme/resource/theme-base.qss")
 * @return File content as a UTF-8 string, or an empty string if the file cannot be opened
 * \endif
 *
 * \if CHINESE
 * @brief 从 Qt 资源路径加载文本内容，带有静态缓存
 * @details 文件内容缓存在静态 QHash 中，生命周期为整个进程。
 *          使用相同路径的重复调用返回缓存内容，无需磁盘 I/O。
 *          Qt 资源文件是内存映射的，因此首次读取已经很快，但此函数
 *          避免了重复主题切换时的冗余字符串分配。
 * @param path Qt 资源路径（如 ":/SARibbonTheme/resource/theme-base.qss"）
 * @return 文件内容的 UTF-8 字符串，若无法打开则返回空字符串
 * \endif
 */
static QString loadResourceText(const QString& path)
{
    static QHash< QString, QString > cache;
    auto it = cache.constFind(path);
    if (it != cache.constEnd()) {
        return it.value();
    }
    QFile f(path);
    if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        cache[path] = QString::fromUtf8(f.readAll());
        return cache[path];
    }
    return QString();
}

void applyRibbonTheme(QWidget* w, SARibbonBar* bar, SARibbonTheme theme)
{
    SARibbonThemePalette palette;
    QString palettePath = SARibbon::Core::SARibbonThemeData::themePalettePath(theme);
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
        QString baseQss = loadResourceText(":/SARibbonTheme/resource/theme-base.qss");

        // Load the QSS template
        QString templateQss = loadResourceText(templatePath);
        if (!templateQss.isEmpty()) {
            // Replace {{token}} placeholders with palette colors
            QString processedQss = SA::replaceQssTokens(templateQss, palette);
            w->setStyleSheet(baseQss + "\n" + processedQss);
        } else {
            // Template file not found, fall back to base QSS
            qWarning() << "applyRibbonTheme: template not found:" << templatePath
                       << "- falling back to base QSS";
            w->setStyleSheet(loadResourceText(":/SARibbonTheme/resource/theme-base.qss"));
        }
    } else if (w) {
        // Empty palette — fall back to base QSS only
        w->setStyleSheet(loadResourceText(":/SARibbonTheme/resource/theme-base.qss"));
    }

    if (!bar) {
        return;
    }

    // 2. Adjust tab bar margins to match the theme's QSS layout (data from core, plan-02 S2)
    if (SARibbonTabBar* tab = bar->ribbonTabBar()) {
        tab->setTabMargin(SARibbon::Core::SARibbonThemeData::themeMargins(theme));
    }

    // 3. Set context category color list (data from core, plan-02 S2)
    {
        bar->setContextCategoryColorList(SARibbon::Core::SARibbonThemeData::themeContextColorList(theme));
    }

    // 4. Register context category highlight function (data from core, plan-02 S2)
    {
        bar->setContextCategoryColorHighLight(SARibbon::Core::SARibbonThemeData::themeContextHighlight(theme));
    }

    // 5. Set tab bar baseline color (data from core, plan-02 S2)
    {
        bar->setTabBarBaseLineColor(SARibbon::Core::SARibbonThemeData::themeBaselineColor(theme));
    }
}

}