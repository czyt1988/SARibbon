#include "SARibbonThemeData.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QMargins>
#include <QString>
#include <map>

namespace SARibbon
{
namespace Core
{

// ===================================================
// Static theme data maps (moved verbatim from SARibbonThemeManager.cpp, values unchanged)
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
static const SARibbonFpContextCategoryHighlight s_csDarkerHighlight = [](const QColor& c) -> QColor {
    return c.darker();
};

// Highlight function: produce a more vibrant variant of the context category color
static const SARibbonFpContextCategoryHighlight s_csVibrantHighlight = [](const QColor& c) -> QColor {
    return SA::makeColorVibrant(c);
};

// Context category highlight function per theme
static const std::map< SARibbonTheme, SARibbonFpContextCategoryHighlight > s_themeContextHighlights = {
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

SARibbonThemeData::SARibbonThemeData(QObject* parent) : QObject(parent)
{
}

SARibbonThemeData::~SARibbonThemeData()
{
    // 静态析构期 QCoreApplication 已亡：不得发射信号、不得触碰 app 对象
}

SARibbonThemeData* SARibbonThemeData::instance()
{
    static SARibbonThemeData s_themeData;  // C++11 magic static, thread-safe init
    return &s_themeData;
}

SARibbonTheme SARibbonThemeData::theme() const
{
    return mTheme;
}

void SARibbonThemeData::setTheme(SARibbonTheme theme)
{
    if (mTheme == theme) {
        return;
    }
    mTheme = theme;
    Q_EMIT themeChanged(theme);
}

SARibbonThemePalette SARibbonThemeData::palette() const
{
    return mPalette;
}

void SARibbonThemeData::setPalette(const SARibbonThemePalette& palette)
{
    mPalette = palette;
    Q_EMIT paletteChanged();
}

/**
 * \if ENGLISH
 * @brief Resolve the built-in palette resource path of a theme
 * @details The single source of truth for the theme -> palette JSON mapping.
 *          It used to be duplicated verbatim in four translation units
 *          (SARibbonMainWindow.cpp / SARibbonThemeManager.cpp / SARibbonUtil.cpp
 *          in widgets and RibbonTheme.cpp in QML); both front ends now call this
 *          one function, so the resource prefix ":/SARibbonTheme/resource" is
 *          guaranteed identical everywhere. RibbonThemeUserDefine deliberately
 *          returns an empty string: a user-defined theme has no built-in
 *          palette, and the caller must keep whatever palette the user loaded
 *          instead of silently falling back to the previous theme's colors.
 * \endif
 *
 * \if CHINESE
 * @brief 解析主题对应的内置调色板资源路径
 * @details 主题 -> 调色板 JSON 映射的唯一来源。原先该 switch 在四个编译单元里
 *          逐字重复（widgets 的 SARibbonMainWindow.cpp / SARibbonThemeManager.cpp
 *          / SARibbonUtil.cpp 与 QML 的 RibbonTheme.cpp），现两个前端都调用本函数，
 *          资源前缀 ":/SARibbonTheme/resource" 因此处处一致。
 *          RibbonThemeUserDefine 刻意返回空串：自定义主题没有内置调色板，调用方
 *          必须保留用户已加载的调色板，而不是静默沿用上一个主题的色值。
 * \endif
 */
QString SARibbonThemeData::themePalettePath(SARibbonTheme theme)
{
    switch (theme) {
    case SARibbonTheme::RibbonThemeOffice2016Blue:
        return QStringLiteral(":/SARibbonTheme/resource/palettes/office2016-blue.json");
    case SARibbonTheme::RibbonThemeOffice2016Green:
        return QStringLiteral(":/SARibbonTheme/resource/palettes/office2016-green.json");
    case SARibbonTheme::RibbonThemeOffice2016Dark:
        return QStringLiteral(":/SARibbonTheme/resource/palettes/office2016-dark.json");
    case SARibbonTheme::RibbonThemeOffice2021Blue:
        return QStringLiteral(":/SARibbonTheme/resource/palettes/office2021-blue.json");
    case SARibbonTheme::RibbonThemeOffice2021Green:
        return QStringLiteral(":/SARibbonTheme/resource/palettes/office2021-green.json");
    case SARibbonTheme::RibbonThemeOffice2021Dark:
        return QStringLiteral(":/SARibbonTheme/resource/palettes/office2021-dark.json");
    case SARibbonTheme::RibbonThemeDark:
        return QStringLiteral(":/SARibbonTheme/resource/palettes/dark-default.json");
    case SARibbonTheme::RibbonThemeDark2:
        return QStringLiteral(":/SARibbonTheme/resource/palettes/dark2-default.json");
    case SARibbonTheme::RibbonThemeWindows7:
        return QStringLiteral(":/SARibbonTheme/resource/palettes/win7-default.json");
    case SARibbonTheme::RibbonThemeOffice2013:
        return QStringLiteral(":/SARibbonTheme/resource/palettes/office2013-default.json");
    case SARibbonTheme::RibbonThemeUserDefine:
    default:
        return QString();
    }
}

QMargins SARibbonThemeData::themeMargins(SARibbonTheme theme)
{
    auto it = s_themeMargins.find(theme);
    return (it != s_themeMargins.end()) ? it->second : QMargins();
}

QList< QColor > SARibbonThemeData::themeContextColorList(SARibbonTheme theme)
{
    auto it = s_themeContextColorLists.find(theme);
    return (it != s_themeContextColorLists.end()) ? it->second : QList< QColor >();
}

SARibbonFpContextCategoryHighlight SARibbonThemeData::themeContextHighlight(SARibbonTheme theme)
{
    auto it = s_themeContextHighlights.find(theme);
    return (it != s_themeContextHighlights.end())
               ? it->second
               : SARibbonFpContextCategoryHighlight();
}

QColor SARibbonThemeData::themeBaselineColor(SARibbonTheme theme)
{
    auto it = s_themeBaselineColors.find(theme);
    return (it != s_themeBaselineColors.end()) ? it->second : QColor();
}

}
}
