#ifndef SARIBBONTHEMEMANAGER_H
#define SARIBBONTHEMEMANAGER_H
#include "SARibbonGlobal.h"

class QWidget;
class SARibbonBar;
enum class SARibbonTheme;

namespace SA
{
/// Apply a built-in ribbon theme to the widget and configure the ribbon bar's theme-dependent properties
SA_RIBBON_EXPORT void applyRibbonTheme(QWidget* w, SARibbonBar* bar, SARibbonTheme theme);
}

#endif  // SARIBBONTHEMEMANAGER_H