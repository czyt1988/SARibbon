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
    // QML-only apps never run the widgets theme manager, so the core palette
    // arrives empty and every token would fall back to the flat application
    // palette: load the current theme's default palette once, up front
    if (coreData()->palette().variables().isEmpty()) {
        applyThemePalette(coreData()->theme());
    }
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

void RibbonTheme::applyThemePalette(SARibbonTheme theme)
{
    // the theme -> palette mapping lives in core (SARibbonThemeData), so the QML
    // front end and the widgets front end can never drift apart; an empty path
    // (RibbonThemeUserDefine) keeps whatever palette is currently loaded
    const QString path = SARibbon::Core::SARibbonThemeData::themePalettePath(theme);
    if (path.isEmpty()) {
        return;
    }
    SA::SARibbonThemePalette pal;
    if (pal.loadFromFile(path)) {
        coreData()->setPalette(pal);  // Q_EMITs paletteChanged -> all bindings refresh
    }
}

int RibbonTheme::currentTheme() const
{
    return int(coreData()->theme());
}

void RibbonTheme::setCurrentTheme(int theme)
{
    // mirror applyRibbonTheme's non-QSS half: switch the palette together
    // with the theme enum so the tokens follow (the QSS half has no QML
    // equivalent — rendering here is token-driven, not stylesheet-driven)
    const SARibbonTheme t = SARibbonTheme(theme);
    applyThemePalette(t);
    coreData()->setTheme(t);
}

bool RibbonTheme::isRtl() const
{
    return QGuiApplication::layoutDirection() == Qt::RightToLeft;
}

void RibbonTheme::setRtl(bool on)
{
    // the core engines read the app layout direction through SA::saIsRTL();
    // flipping it relayouts every host on the next polish pass (widgets
    // "Switch to RTL" parity)
    const Qt::LayoutDirection dir = on ? Qt::RightToLeft : Qt::LeftToRight;
    if (QGuiApplication::layoutDirection() == dir) {
        return;
    }
    QGuiApplication::setLayoutDirection(dir);
    Q_EMIT rtlChanged();
}

bool RibbonTheme::isDark() const
{
    return coreData()->palette().isDark();
}

QColor RibbonTheme::paletteColor(const char* tokenName) const
{
    const QString raw = coreData()->palette().rawValue(tokenName);
    QColor c(raw);
    if (c.isValid()) {
        return c;
    }
    // QML-only apps never run the widgets theme manager, so the core palette
    // can stay empty: fall back to the application palette for the core token
    // names, keeping the leaves literal-color-free (plan-04 S3 theme rule)
    if (QGuiApplication::instance()) {
        const QPalette pal = QGuiApplication::palette();
        const QString name = QLatin1String(tokenName);
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

QColor RibbonTheme::tokenColor(const QString& name) const
{
    return paletteColor(name.toUtf8().constData());
}

QColor RibbonTheme::accent() const
{
    return paletteColor("accent");
}

QColor RibbonTheme::textColor() const
{
    return paletteColor("text-color");
}

QColor RibbonTheme::contentBg() const
{
    return paletteColor("content-bg");
}

QColor RibbonTheme::contentHoverBg() const
{
    return paletteColor("content-hover-bg");
}

QColor RibbonTheme::contentPressedBg() const
{
    return paletteColor("content-pressed-bg");
}

QColor RibbonTheme::subtitle() const
{
    return paletteColor("subtitle");
}

QColor RibbonTheme::separator() const
{
    return paletteColor("separator");
}

QColor RibbonTheme::borderColor() const
{
    return paletteColor("border-color");
}

QColor RibbonTheme::tabAccent() const
{
    return paletteColor("tab-accent");
}

QColor RibbonTheme::tabAccentHover() const
{
    return paletteColor("tab-accent-hover");
}

QColor RibbonTheme::accentHover() const
{
    return paletteColor("accent-hover");
}

QColor RibbonTheme::accentPressed() const
{
    return paletteColor("accent-pressed");
}

QColor RibbonTheme::menuBorder() const
{
    return paletteColor("menu-border");
}

QColor RibbonTheme::inputBorder() const
{
    return paletteColor("input-border");
}

QColor RibbonTheme::inputFocus() const
{
    return paletteColor("input-focus");
}

QColor RibbonTheme::selectionBg() const
{
    return paletteColor("selection-bg");
}

QColor RibbonTheme::sysButtonHover() const
{
    return paletteColor("sys-button-hover");
}

QColor RibbonTheme::sysButtonPressed() const
{
    return paletteColor("sys-button-pressed");
}

}
