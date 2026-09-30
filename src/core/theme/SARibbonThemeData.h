#ifndef SARIBBONTHEMEDATA_H
#define SARIBBONTHEMEDATA_H
#include <SARibbonCore/SARibbonCoreGlobal.h>
#include <SARibbonCore/SARibbonThemePalette.h>
#include <SARibbonCore/SARibbonEnums.h>
#include <QObject>
#include <functional>
#include <QMargins>
#include <QColor>
#include <QList>

namespace SARibbon
{
namespace Core
{

/// Context category highlight function type (lifted from SARibbonBar::FpContextCategoryHighlight,
/// plan 02 S2.1-2; the widgets-side using alias forwards to it)
using SARibbonFpContextCategoryHighlight = std::function< QColor(const QColor&) >;

// SARibbonThemePalette lives in namespace SA (2.x spelling kept, plan 02 S2)
using SA::SARibbonThemePalette;

/**
 * \if ENGLISH
 * @brief Theme data holder shared by both front ends (widgets / QML)
 * @details Holds the current SARibbonTheme and SARibbonThemePalette plus the static
 * theme data tables (margins / context colors / highlights / baseline colors) that
 * used to live as file-local maps inside SARibbonThemeManager.cpp. The tables were
 * moved verbatim; QSS loading and rendering stay in widgets (v2 section 3.2).
 * \endif
 *
 * \if CHINESE
 * @brief 双前端共享的主题数据持有者（widgets / QML）
 * @details 持有当前 SARibbonTheme 与 SARibbonThemePalette，以及原先散落在
 * SARibbonThemeManager.cpp 文件内的静态主题表（边距/上下文颜色/高亮/基线色）。
 * 表为纯 move；QSS 加载与渲染留 widgets（v2 §3.2）。
 * @note 单例为 Meyers 函数内静态对象（C++11 magic static，线程安全初始化）；
 * 析构不得发射信号、不得触碰 QCoreApplication（静态析构期 app 已亡）。
 * \endif
 */
class SA_RIBBON_CORE_EXPORT SARibbonThemeData : public QObject
{
    Q_OBJECT
public:
    explicit SARibbonThemeData(QObject* parent = nullptr);
    ~SARibbonThemeData() override;

    // Singleton (Meyers function-local static; see class note)
    static SARibbonThemeData* instance();

    // Current theme
    SARibbonTheme theme() const;
    void setTheme(SARibbonTheme theme);

    // Current palette
    SARibbonThemePalette palette() const;
    void setPalette(const SARibbonThemePalette& palette);

    // ---- Static theme data tables (moved verbatim from SARibbonThemeManager.cpp) ----

    // Tab margin per theme (affects SARibbonContextCategory drawing)
    static QMargins themeMargins(SARibbonTheme theme);

    // Context category color list per theme
    static QList< QColor > themeContextColorList(SARibbonTheme theme);

    // Context category highlight function per theme
    static SARibbonFpContextCategoryHighlight themeContextHighlight(SARibbonTheme theme);

    // Tab bar baseline color per theme (only Office2013 has a visible baseline)
    static QColor themeBaselineColor(SARibbonTheme theme);

Q_SIGNALS:
    void themeChanged(SARibbonTheme theme);
    void paletteChanged();

private:
    SARibbonTheme mTheme     = SARibbonTheme::RibbonThemeOffice2021Blue;
    SARibbonThemePalette mPalette;
};

}
}

#endif  // SARIBBONTHEMEDATA_H
