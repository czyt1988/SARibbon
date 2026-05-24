#ifndef SARIBBONTHEMEPALETTE_H
#define SARIBBONTHEMEPALETTE_H
#include "SARibbonGlobal.h"
#include <QColor>
#include <QHash>
#include <QString>
#include <QByteArray>

namespace SA {

class SA_RIBBON_EXPORT SARibbonThemePalette
{
public:
    SARibbonThemePalette();

    bool loadFromJson(const QByteArray& json);

    void setAccentColor(const QColor& color);
    void setContentBgColor(const QColor& color);
    void setTextColor(const QColor& color);

    QColor color(const QString& tokenName) const;
    QHash<QString, QString> variables() const;
    bool isDark() const;

private:
    QHash<QString, QColor> m_keyColors;
    QHash<QString, QColor> m_derivedColors;
    QHash<QString, QColor> m_fixedColors;
    bool m_isDark { false };
};

} // namespace SA
#endif // SARIBBONTHEMEPALETTE_H
