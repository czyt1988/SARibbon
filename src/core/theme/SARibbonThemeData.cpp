#include "SARibbonThemeData.h"
#include <SARibbonCore/SARibbonCoreUtil.h>
#include <QMargins>
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
