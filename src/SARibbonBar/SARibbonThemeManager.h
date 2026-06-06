#ifndef SARIBBONTHEMEMANAGER_H
#define SARIBBONTHEMEMANAGER_H
#include "SARibbonGlobal.h"

class QWidget;
class SARibbonBar;
enum class SARibbonTheme;

namespace SA
{

class SARibbonThemePalette;

// Apply a built-in ribbon theme with fixed QSS to the widget and configure ribbon bar properties
SA_RIBBON_EXPORT void applyRibbonTheme(QWidget* w, SARibbonBar* bar, SARibbonTheme theme);

// Apply a ribbon theme with custom color palette, falling back to fixed QSS if palette is empty or no template found
SA_RIBBON_EXPORT void applyRibbonTheme(QWidget* w, SARibbonBar* bar, SARibbonTheme theme,
                                       const SARibbonThemePalette& palette);

}

#endif  // SARIBBONTHEMEMANAGER_H