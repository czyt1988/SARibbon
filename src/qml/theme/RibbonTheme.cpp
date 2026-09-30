#include "RibbonTheme.h"
#include <QGuiApplication>
#include <QPalette>

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
    if (c.isValid()) {
        return c;
    }
    // QML-only apps never run the widgets theme manager, so the core palette
    // stays empty: fall back to the application palette for the core token
    // names, keeping the leaves literal-color-free (plan-04 S3 theme rule)
    if (QGuiApplication::instance()) {
        const QPalette pal = QGuiApplication::palette();
        if (name == QLatin1String("text-color")) {
            return pal.color(QPalette::Active, QPalette::Text);
        }
        if (name == QLatin1String("content-bg")) {
            return pal.color(QPalette::Active, QPalette::Window);
        }
        if (name == QLatin1String("accent")) {
            return pal.color(QPalette::Active, QPalette::Highlight);
        }
        if (name == QLatin1String("accent-text")) {
            return pal.color(QPalette::Active, QPalette::HighlightedText);
        }
    }
    return QColor();
}

}
