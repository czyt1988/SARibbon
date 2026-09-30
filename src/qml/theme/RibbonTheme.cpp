#include "RibbonTheme.h"

namespace SARibbonQml {

RibbonTheme::RibbonTheme(QObject* parent) : QObject(parent)
{
    // bridge the core signal source: any theme/palette change re-emits both
    // notify signals so every QML binding refreshes (v2 §3.2-1)
    connect(SARibbon::Core::SARibbonThemeData::instance(), &SARibbon::Core::SARibbonThemeData::themeChanged,
            this, [this]() { Q_EMIT currentThemeChanged(); });
    connect(SARibbon::Core::SARibbonThemeData::instance(), &SARibbon::Core::SARibbonThemeData::paletteChanged,
            this, [this]() { Q_EMIT paletteChanged(); Q_EMIT currentThemeChanged(); });
}

RibbonTheme::~RibbonTheme()
{
}

RibbonTheme* RibbonTheme::instance()
{
    static RibbonTheme s_instance;
    return &s_instance;
}

SARibbon::Core::SARibbonThemeData* RibbonTheme::coreData() const
{
    return SARibbon::Core::SARibbonThemeData::instance();
}

int RibbonTheme::currentTheme() const
{
    return int(coreData()->theme());
}

void RibbonTheme::setCurrentTheme(int theme)
{
    coreData()->setTheme(SARibbonTheme(theme));
}

bool RibbonTheme::isDark() const
{
    return coreData()->palette().isDark();
}

QColor RibbonTheme::tokenColor(const QString& name) const
{
    const QString raw = coreData()->palette().rawValue(name);
    QColor c(raw);
    return c.isValid() ? c : QColor();
}

}
