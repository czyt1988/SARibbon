#include <QtTest>
#include "SARibbonThemePalette.h"
#include "SARibbonUtil.h"

class TestThemePalette : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testLoadFromJson();
    void testDerivation();
    void testDarkDerivation();
    void testTokenReplacement();
    void testOpacityModifier();
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

QTEST_MAIN(TestThemePalette)
#include "tst_themepalette.moc"
