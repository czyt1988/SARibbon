#include "SARibbonThemePalette.h"
#include <QJsonDocument>
#include <QJsonObject>

namespace {
QColor deriveColor(const QColor& base, const QString& fn, int amount, bool isDark)
{
    if (isDark) {
        // Reverse direction for dark themes
        if (fn == "darken") {
            return base.lighter(100 + amount);
        } else if (fn == "lighten") {
            return base.darker(100 + amount);
        }
    } else {
        if (fn == "darken") {
            return base.darker(100 + amount);
        } else if (fn == "lighten") {
            return base.lighter(100 + amount);
        }
    }
    return base;
}
} // anonymous namespace

namespace SA {

SARibbonThemePalette::SARibbonThemePalette() = default;

bool SARibbonThemePalette::loadFromJson(const QByteArray& json)
{
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(json, &error);
    if (error.error != QJsonParseError::NoError) {
        return false;
    }

    QJsonObject obj = doc.object();
    m_isDark = obj.value("isDark").toBool(false);

    QJsonObject keyColors = obj.value("keyColors").toObject();
    for (auto it = keyColors.begin(); it != keyColors.end(); ++it) {
        m_keyColors.insert(it.key(), QColor(it.value().toString()));
    }

    // Parse derived colors
    QJsonObject derived = obj.value("derived").toObject();
    for (auto it = derived.begin(); it != derived.end(); ++it) {
        QJsonObject rule = it.value().toObject();
        QString fn = rule.value("fn").toString();
        QString baseName = rule.value("base").toString();
        int amount = rule.value("amount").toInt(10);

        if (m_keyColors.contains(baseName)) {
            QColor base = m_keyColors.value(baseName);
            QColor derivedC = ::deriveColor(base, fn, amount, m_isDark);
            m_derivedColors.insert(it.key(), derivedC);
        }
    }

    // Parse fixed colors
    QJsonObject fixed = obj.value("fixed").toObject();
    for (auto it = fixed.begin(); it != fixed.end(); ++it) {
        m_fixedColors.insert(it.key(), QColor(it.value().toString()));
    }

    return true;
}

QColor SARibbonThemePalette::color(const QString& tokenName) const
{
    if (m_derivedColors.contains(tokenName)) {
        return m_derivedColors.value(tokenName);
    }
    if (m_keyColors.contains(tokenName)) {
        return m_keyColors.value(tokenName);
    }
    if (m_fixedColors.contains(tokenName)) {
        return m_fixedColors.value(tokenName);
    }
    return QColor();
}

QHash<QString, QString> SARibbonThemePalette::variables() const
{
    QHash<QString, QString> vars;
    for (auto it = m_keyColors.cbegin(); it != m_keyColors.cend(); ++it) {
        vars.insert(it.key(), it.value().name());
    }
    for (auto it = m_derivedColors.cbegin(); it != m_derivedColors.cend(); ++it) {
        vars.insert(it.key(), it.value().name());
    }
    for (auto it = m_fixedColors.cbegin(); it != m_fixedColors.cend(); ++it) {
        vars.insert(it.key(), it.value().name());
    }
    return vars;
}

bool SARibbonThemePalette::isDark() const
{
    return m_isDark;
}

void SARibbonThemePalette::setAccentColor(const QColor& color)
{
    m_keyColors.insert("accent", color);
}

void SARibbonThemePalette::setContentBgColor(const QColor& color)
{
    m_keyColors.insert("content-bg", color);
}

void SARibbonThemePalette::setTextColor(const QColor& color)
{
    m_keyColors.insert("text-color", color);
}

} // namespace SA
