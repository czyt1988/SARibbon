#ifndef SARIBBONTHEMEMANAGER_H
#define SARIBBONTHEMEMANAGER_H
#include "SARibbonGlobal.h"

class QWidget;
class SARibbonBar;
enum class SARibbonTheme;

namespace SA
{

class SARibbonThemePalette;

/**
 * @brief Apply a built-in ribbon theme to the widget and configure the ribbon bar's theme-dependent properties
 *
 * This overload uses the fixed QSS stylesheet for the specified theme.
 *
 * @param w The widget that receives the QSS stylesheet
 * @param bar The SARibbonBar whose theme-dependent properties are configured (may be null)
 * @param theme The built-in ribbon theme to apply
 */
SA_RIBBON_EXPORT void applyRibbonTheme(QWidget* w, SARibbonBar* bar, SARibbonTheme theme);

/**
 * @brief Apply a ribbon theme with custom color palette
 *
 * This overload loads the QSS template for the theme and replaces color tokens
 * using the provided palette. If the palette is empty or no template is found,
 * it falls back to the default behavior (equivalent to the 3-arg overload).
 *
 * @param w The widget that receives the QSS stylesheet
 * @param bar The SARibbonBar whose theme-dependent properties are configured (may be null)
 * @param theme The built-in ribbon theme to apply
 * @param palette The color palette for token replacement
 */
SA_RIBBON_EXPORT void applyRibbonTheme(QWidget* w, SARibbonBar* bar, SARibbonTheme theme,
                                       const SARibbonThemePalette& palette);

}

#endif  // SARIBBONTHEMEMANAGER_H