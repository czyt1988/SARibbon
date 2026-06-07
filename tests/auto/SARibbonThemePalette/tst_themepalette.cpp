#include <QtTest>
#include "SARibbonThemePalette.h"
#include "SARibbonUtil.h"
#include "SARibbonGlobal.h"
#include <QFile>
#include <QRegularExpression>
#include <QSet>

class TestThemePalette : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testLoadFromJson();
    void testDerivation();
    void testDarkDerivation();
    void testTokenReplacement();
    void testOpacityModifier();
    void testQssTokenCoverage();
    void testPaletteAutoLoading();
    void testPaletteAutoLoadingWin7();
    void testAllPalettesLoad();
    void testNewPalettesContainAllTokens();
    void testWin7AndOffice2013TokenCoverage();
    void testSetterRecalculateDerived();
    void testDarkSetterRecalculate();
    void testVariablesPriority();
};

void TestThemePalette::testLoadFromJson()
{
    SA::SARibbonThemePalette palette;
    QByteArray json = R"({
        "name": "test-blue",
        "isDark": false,
        "keyColors": {
            "accent": "#225497",
            "content-bg": "#f1f1f1"
        }
    })";

    bool success = palette.loadFromJson(json);
    QVERIFY(success);
    QCOMPARE(palette.color("accent").name(), QColor("#225497").name());
    QCOMPARE(palette.color("content-bg").name(), QColor("#f1f1f1").name());
    QCOMPARE(palette.isDark(), false);
}

void TestThemePalette::testDerivation()
{
    SA::SARibbonThemePalette palette;
    QByteArray json = R"({
        "name": "test",
        "isDark": false,
        "keyColors": {
            "accent": "#225497"
        },
        "derived": {
            "accent-hover": {"fn": "lighten", "base": "accent", "amount": 15}
        }
    })";

    palette.loadFromJson(json);
    QColor hover = palette.color("accent-hover");
    QVERIFY(hover.isValid());
    QVERIFY(hover.lightness() > QColor("#225497").lightness());
}

void TestThemePalette::testDarkDerivation()
{
    SA::SARibbonThemePalette palette;
    QByteArray json = R"({
        "name": "test-dark",
        "isDark": true,
        "keyColors": {
            "accent": "#225497"
        },
        "derived": {
            "accent-hover": {"fn": "lighten", "base": "accent", "amount": 15}
        }
    })";

    palette.loadFromJson(json);
    QColor hover = palette.color("accent-hover");
    // In dark mode, lighten becomes darken
    QVERIFY(hover.lightness() < QColor("#225497").lightness());
}

void TestThemePalette::testTokenReplacement()
{
    SA::SARibbonThemePalette palette;
    QByteArray json = R"({
        "name": "test",
        "keyColors": {
            "accent": "#225497",
            "content-bg": "#f1f1f1"
        }
    })";
    palette.loadFromJson(json);

    QString templateQss = "SARibbonBar { background: {{accent}}; }";
    QString result        = SA::replaceQssTokens(templateQss, palette);
    QCOMPARE(result, QString("SARibbonBar { background: #225497; }"));
}

void TestThemePalette::testOpacityModifier()
{
    SA::SARibbonThemePalette palette;
    QByteArray json = R"({
        "name": "test",
        "keyColors": {"accent": "#225497"}
    })";
    palette.loadFromJson(json);

    QString templateQss = "QToolButton { bg: {{accent|opacity(0.5)}}; }";
    QString result        = SA::replaceQssTokens(templateQss, palette);
    QCOMPARE(result, QString("QToolButton { bg: #80225497; }"));
}

void TestThemePalette::testQssTokenCoverage()
{
    // Each palette maps to a template QSS; after token replacement, no {{ should remain
    struct PaletteTemplate {
        QString palettePath;
        QString templatePath;
    };

    QVector<PaletteTemplate> cases = {
        { ":/SARibbonTheme/resource/palettes/office2016-blue.json",
          ":/SARibbonTheme/resource/templates/office2016.qss" },
        { ":/SARibbonTheme/resource/palettes/office2016-green.json",
          ":/SARibbonTheme/resource/templates/office2016.qss" },
        { ":/SARibbonTheme/resource/palettes/office2016-dark.json",
          ":/SARibbonTheme/resource/templates/office2016.qss" },
        { ":/SARibbonTheme/resource/palettes/office2021-blue.json",
          ":/SARibbonTheme/resource/templates/office2021.qss" },
        { ":/SARibbonTheme/resource/palettes/office2021-green.json",
          ":/SARibbonTheme/resource/templates/office2021.qss" },
        { ":/SARibbonTheme/resource/palettes/office2021-dark.json",
          ":/SARibbonTheme/resource/templates/office2021.qss" },
        { ":/SARibbonTheme/resource/palettes/dark-default.json",
          ":/SARibbonTheme/resource/templates/dark.qss" },
        { ":/SARibbonTheme/resource/palettes/dark2-default.json",
          ":/SARibbonTheme/resource/templates/dark2.qss" },
        { ":/SARibbonTheme/resource/palettes/win7-default.json",
          ":/SARibbonTheme/resource/templates/win7.qss" },
        { ":/SARibbonTheme/resource/palettes/office2013-default.json",
          ":/SARibbonTheme/resource/templates/office2013.qss" },
    };

    for (const auto& tc : cases) {
        SA::SARibbonThemePalette palette;
        QVERIFY2(palette.loadFromFile(tc.palettePath),
                 ("Failed to load palette: " + tc.palettePath).toUtf8().constData());

        QFile templateFile(tc.templatePath);
        QVERIFY2(templateFile.open(QIODevice::ReadOnly | QIODevice::Text),
                 ("Failed to open template: " + tc.templatePath).toUtf8().constData());
        QString templateQss = QString::fromUtf8(templateFile.readAll());

        QString result = SA::replaceQssTokens(templateQss, palette);
        QVERIFY2(!result.contains("{{"),
                 ("Unreplaced tokens in palette: " + tc.palettePath).toUtf8().constData());
    }
}

void TestThemePalette::testPaletteAutoLoading()
{
    // Verify that template-supported themes have palette resource files at the expected paths
    struct ThemePath {
        SARibbonTheme theme;
        QString path;
    };
    QVector<ThemePath> supported = {
        { SARibbonTheme::RibbonThemeOffice2016Blue,
          ":/SARibbonTheme/resource/palettes/office2016-blue.json" },
        { SARibbonTheme::RibbonThemeOffice2016Green,
          ":/SARibbonTheme/resource/palettes/office2016-green.json" },
        { SARibbonTheme::RibbonThemeOffice2016Dark,
          ":/SARibbonTheme/resource/palettes/office2016-dark.json" },
        { SARibbonTheme::RibbonThemeOffice2021Blue,
          ":/SARibbonTheme/resource/palettes/office2021-blue.json" },
        { SARibbonTheme::RibbonThemeOffice2021Green,
          ":/SARibbonTheme/resource/palettes/office2021-green.json" },
        { SARibbonTheme::RibbonThemeOffice2021Dark,
          ":/SARibbonTheme/resource/palettes/office2021-dark.json" },
        { SARibbonTheme::RibbonThemeDark,
          ":/SARibbonTheme/resource/palettes/dark-default.json" },
        { SARibbonTheme::RibbonThemeDark2,
          ":/SARibbonTheme/resource/palettes/dark2-default.json" },
        { SARibbonTheme::RibbonThemeWindows7,
          ":/SARibbonTheme/resource/palettes/win7-default.json" },
        { SARibbonTheme::RibbonThemeOffice2013,
          ":/SARibbonTheme/resource/palettes/office2013-default.json" }
    };

    for (const auto& tp : supported) {
        QFile f(tp.path);
        QVERIFY2(f.exists(),
                 QString("Palette resource for theme %1 should exist at %2")
                     .arg(static_cast<int>(tp.theme))
                     .arg(tp.path)
                     .toUtf8()
                     .constData());
    }
}

void TestThemePalette::testPaletteAutoLoadingWin7()
{
    // Win7 and Office2013 now have palette and template support
    QVector<QString> supportedPaths = {
        ":/SARibbonTheme/resource/palettes/win7-default.json",
        ":/SARibbonTheme/resource/palettes/office2013-default.json",
        ":/SARibbonTheme/resource/templates/win7.qss",
        ":/SARibbonTheme/resource/templates/office2013.qss"
    };

    for (const auto& path : supportedPaths) {
        QFile f(path);
        QVERIFY2(f.exists(),
                 QString("Palette/template resource should exist at %1")
                     .arg(path)
                     .toUtf8()
                     .constData());
    }
}

void TestThemePalette::testAllPalettesLoad()
{
    // Verify all 8 palette JSONs can be loaded successfully by SARibbonThemePalette::loadFromFile
    QVector<QString> palettePaths = {
        ":/SARibbonTheme/resource/palettes/office2016-blue.json",
        ":/SARibbonTheme/resource/palettes/office2016-green.json",
        ":/SARibbonTheme/resource/palettes/office2016-dark.json",
        ":/SARibbonTheme/resource/palettes/office2021-blue.json",
        ":/SARibbonTheme/resource/palettes/office2021-green.json",
        ":/SARibbonTheme/resource/palettes/office2021-dark.json",
        ":/SARibbonTheme/resource/palettes/dark-default.json",
        ":/SARibbonTheme/resource/palettes/dark2-default.json",
        ":/SARibbonTheme/resource/palettes/win7-default.json",
        ":/SARibbonTheme/resource/palettes/office2013-default.json"
    };

    for (const auto& path : palettePaths) {
        SA::SARibbonThemePalette palette;
        QVERIFY2(palette.loadFromFile(path),
                 QString("Failed to load palette from %1").arg(path).toUtf8().constData());
    }
}

void TestThemePalette::testNewPalettesContainAllTokens()
{
    // Extract all token names from the office2021.qss template
    QFile templateFile(":/SARibbonTheme/resource/templates/office2021.qss");
    QVERIFY2(templateFile.open(QIODevice::ReadOnly | QIODevice::Text),
             "Failed to open office2021.qss template");
    QString templateQss = QString::fromUtf8(templateFile.readAll());

    QRegularExpression tokenRe("\\{\\{([^}|]+)");
    QSet<QString> requiredTokens;
    QRegularExpressionMatchIterator it = tokenRe.globalMatch(templateQss);
    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        requiredTokens.insert(match.captured(1));
    }
    QVERIFY2(requiredTokens.size() > 0, "Template should contain at least one token");

    // Verify office2021-green and office2021-dark contain all required tokens
    QVector<QString> newPalettePaths = {
        ":/SARibbonTheme/resource/palettes/office2021-green.json",
        ":/SARibbonTheme/resource/palettes/office2021-dark.json"
    };

    for (const auto& path : newPalettePaths) {
        SA::SARibbonThemePalette palette;
        QVERIFY2(palette.loadFromFile(path),
                 QString("Failed to load palette: %1").arg(path).toUtf8().constData());

        QHash<QString, QString> vars = palette.variables();
        for (const QString& token : requiredTokens) {
            QVERIFY2(vars.contains(token),
                     QString("Palette %1 missing required token '%2'")
                         .arg(path)
                         .arg(token)
                         .toUtf8()
                         .constData());
        }
    }
}

void TestThemePalette::testWin7AndOffice2013TokenCoverage()
{
    struct TemplatePalette {
        QString templatePath;
        QString palettePath;
    };

    QVector< TemplatePalette > cases = {
        { ":/SARibbonTheme/resource/templates/win7.qss",
          ":/SARibbonTheme/resource/palettes/win7-default.json" },
        { ":/SARibbonTheme/resource/templates/office2013.qss",
          ":/SARibbonTheme/resource/palettes/office2013-default.json" }
    };

    QRegularExpression tokenRe("\\{\\{([^}|]+)");

    for (const auto& tc : cases) {
        QFile templateFile(tc.templatePath);
        QVERIFY2(templateFile.open(QIODevice::ReadOnly | QIODevice::Text),
                 QString("Failed to open template: %1").arg(tc.templatePath).toUtf8().constData());
        QString templateQss = QString::fromUtf8(templateFile.readAll());

        QSet< QString > requiredTokens;
        QRegularExpressionMatchIterator it = tokenRe.globalMatch(templateQss);
        while (it.hasNext()) {
            QRegularExpressionMatch match = it.next();
            requiredTokens.insert(match.captured(1));
        }
        QVERIFY2(requiredTokens.size() > 0,
                 QString("Template %1 should contain at least one token").arg(tc.templatePath).toUtf8().constData());

        SA::SARibbonThemePalette palette;
        QVERIFY2(palette.loadFromFile(tc.palettePath),
                 QString("Failed to load palette: %1").arg(tc.palettePath).toUtf8().constData());

        QHash< QString, QString > vars = palette.variables();
        for (const QString& token : requiredTokens) {
            QVERIFY2(vars.contains(token),
                     QString("Palette %1 missing token '%2' required by template %3")
                         .arg(tc.palettePath)
                         .arg(token)
                         .arg(tc.templatePath)
                         .toUtf8()
                         .constData());
        }
    }
}

void TestThemePalette::testSetterRecalculateDerived()
{
    SA::SARibbonThemePalette palette;
    QByteArray json = R"({
        "name": "test-recalc",
        "isDark": false,
        "keyColors": {
            "accent": "#225497"
        },
        "derived": {
            "accent-hover": {"fn": "lighten", "base": "accent", "amount": 15},
            "accent-pressed": {"fn": "darken", "base": "accent", "amount": 10}
        }
    })";

    palette.loadFromJson(json);
    QString originalHover = palette.color("accent-hover").name();
    QString originalPressed = palette.color("accent-pressed").name();

    // Change accent color
    palette.setAccentColor(QColor("#ff0000"));

    // Derived colors should be recalculated
    QString newHover = palette.color("accent-hover").name();
    QString newPressed = palette.color("accent-pressed").name();

    QVERIFY2(newHover != originalHover,
             "accent-hover should change after setAccentColor");
    QVERIFY2(newPressed != originalPressed,
             "accent-pressed should change after setAccentColor");

    // Verify derived colors are based on new accent
    QColor newAccent("#ff0000");
    QColor expectedHover = newAccent.lighter(115);  // lighten 15%
    QCOMPARE(palette.color("accent-hover").name(), expectedHover.name());

    QColor expectedPressed = newAccent.darker(110);  // darken 10%
    QCOMPARE(palette.color("accent-pressed").name(), expectedPressed.name());
}

void TestThemePalette::testDarkSetterRecalculate()
{
    SA::SARibbonThemePalette palette;
    QByteArray json = R"({
        "name": "test-dark-recalc",
        "isDark": true,
        "keyColors": {
            "accent": "#225497"
        },
        "derived": {
            "accent-hover": {"fn": "lighten", "base": "accent", "amount": 15}
        }
    })";

    palette.loadFromJson(json);

    // In dark mode, lighten becomes darken
    QColor originalHover = palette.color("accent-hover");
    QVERIFY2(originalHover.lightness() < QColor("#225497").lightness(),
             "Dark mode: lighten should actually darken");

    palette.setAccentColor(QColor("#ff0000"));

    QColor newHover = palette.color("accent-hover");
    // Dark mode reversal: lighten -> darker
    QVERIFY2(newHover.lightness() < QColor("#ff0000").lightness(),
             "Dark mode: after setter, lighten should still darken");
    QVERIFY2(newHover != originalHover,
             "Derived color should change after setter");
}

void TestThemePalette::testVariablesPriority()
{
    // Test that variables() priority matches color() lookup order:
    // derived > key > fixed
    SA::SARibbonThemePalette palette;
    QByteArray json = R"({
        "name": "priority-test",
        "isDark": false,
        "keyColors": {
            "dupe-token": "#ff0000",
            "key-only": "#00ff00"
        },
        "derived": {
            "dupe-token": {"fn": "lighten", "base": "key-only", "amount": 10}
        },
        "fixed": {
            "dupe-token": "#0000ff",
            "fixed-only": "#808080"
        }
    })";

    palette.loadFromJson(json);

    // "dupe-token" exists in all three layers
    // color() should return derived value (lighten of key-only)
    QColor colorResult = palette.color("dupe-token");
    QString varResult   = palette.variables().value("dupe-token");

    // Both should agree
    QCOMPARE(varResult, colorResult.name());

    // Verify derived wins: dupe-token should be lighten(#00ff00, 10), NOT #ff0000 or #0000ff
    QVERIFY(colorResult.lightness() > QColor("#00ff00").lightness());

    // "key-only" exists only in key layer
    QCOMPARE(palette.color("key-only").name(), QColor("#00ff00").name());
    QCOMPARE(palette.variables().value("key-only"), QColor("#00ff00").name());

    // "fixed-only" exists only in fixed layer
    QCOMPARE(palette.color("fixed-only").name(), QColor("#808080").name());
    QCOMPARE(palette.variables().value("fixed-only"), QColor("#808080").name());
}

QTEST_MAIN(TestThemePalette)
#include "tst_themepalette.moc"
