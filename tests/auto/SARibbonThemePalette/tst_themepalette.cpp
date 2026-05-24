#include <QtTest>
#include "SARibbonThemePalette.h"

class TestThemePalette : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void testLoadFromJson();
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

QTEST_MAIN(TestThemePalette)
#include "tst_themepalette.moc"
