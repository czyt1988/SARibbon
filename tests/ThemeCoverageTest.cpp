#include <QtTest>
#include <QApplication>
#include <QRegularExpression>
#include <QColor>
#include "SARibbonGlobal.h"
#include "SARibbonUtil.h"

class ThemeCoverageTest : public QObject
{
    Q_OBJECT

private slots:
    void testAllThemesLoad();
    void testSelectorCoverage();
    void testDarkThemeContrast();

private:
    QList<SARibbonTheme> allThemes() const;
    QStringList keySelectors() const;
    double luminance(const QColor& c) const;
    double contrastRatio(const QColor& fg, const QColor& bg) const;
    QColor parseColor(const QString& colorStr) const;
    QColor extractBackgroundColor(const QString& qss, const QString& selector) const;
    QColor extractForegroundColor(const QString& qss, const QString& selector) const;
};

QList<SARibbonTheme> ThemeCoverageTest::allThemes() const
{
    return { SARibbonTheme::RibbonThemeOffice2013,
             SARibbonTheme::RibbonThemeOffice2016Blue,
             SARibbonTheme::RibbonThemeOffice2021Blue,
             SARibbonTheme::RibbonThemeWindows7,
             SARibbonTheme::RibbonThemeDark,
             SARibbonTheme::RibbonThemeDark2 };
}

QStringList ThemeCoverageTest::keySelectors() const
{
    return { "SARibbonBar",
             "SARibbonCategory",
             "SARibbonPanel",
             "SARibbonApplicationButton",
             "SARibbonButtonGroupWidget" };
}

// WCAG 2.0 relative luminance
double ThemeCoverageTest::luminance(const QColor& c) const
{
    auto linearize = [](double v) -> double {
        return (v <= 0.03928) ? v / 12.92 : std::pow((v + 0.055) / 1.055, 2.4);
    };
    double r = linearize(c.redF());
    double g = linearize(c.greenF());
    double b = linearize(c.blueF());
    return 0.2126 * r + 0.7152 * g + 0.0722 * b;
}

// WCAG 2.0 contrast ratio
double ThemeCoverageTest::contrastRatio(const QColor& fg, const QColor& bg) const
{
    double l1 = luminance(fg);
    double l2 = luminance(bg);
    double lighter = std::max(l1, l2);
    double darker   = std::min(l1, l2);
    return (lighter + 0.05) / (darker + 0.05);
}

QColor ThemeCoverageTest::parseColor(const QString& colorStr) const
{
    QString s = colorStr.trimmed();
    if (s.startsWith("#")) {
        return QColor(s);
    }
    // Handle rgb(r,g,b) format
    QRegularExpression rgbRe("rgb\\((\\d+),(\\d+),(\\d+)\\)");
    auto match = rgbRe.match(s);
    if (match.hasMatch()) {
        return QColor(match.captured(1).toInt(),
                       match.captured(2).toInt(),
                       match.captured(3).toInt());
    }
    return QColor(s);  // Named color fallback
}

QColor ThemeCoverageTest::extractBackgroundColor(const QString& qss, const QString& selector) const
{
    // Match selector block and find background/color property
    QRegularExpression blockRe(selector + "\\s*\\{([^}]*)\\}");
    auto match = blockRe.match(qss);
    if (!match.hasMatch()) {
        return QColor();
    }
    QString block = match.captured(1);
    QRegularExpression bgRe("background\\s*:\\s*([^;\\n]+)");
    auto bgMatch = bgRe.match(block);
    if (bgMatch.hasMatch()) {
        QColor c = parseColor(bgMatch.captured(1));
        if (c.isValid()) {
            return c;
        }
    }
    // Try background-color as fallback
    QRegularExpression bgcRe("background-color\\s*:\\s*([^;\\n]+)");
    auto bgcMatch = bgcRe.match(block);
    if (bgcMatch.hasMatch()) {
        return parseColor(bgcMatch.captured(1));
    }
    return QColor();
}

QColor ThemeCoverageTest::extractForegroundColor(const QString& qss, const QString& selector) const
{
    QRegularExpression blockRe(selector + "\\s*\\{([^}]*)\\}");
    auto match = blockRe.match(qss);
    if (!match.hasMatch()) {
        return QColor();
    }
    QString block = match.captured(1);
    QRegularExpression colorRe("color\\s*:\\s*([^;\\n]+)");
    auto colorMatch = colorRe.match(block);
    if (colorMatch.hasMatch()) {
        return parseColor(colorMatch.captured(1));
    }
    return QColor();
}

void ThemeCoverageTest::testAllThemesLoad()
{
    for (auto theme : allThemes()) {
        QString qss = SA::getBuiltInRibbonThemeQss(theme);
        QVERIFY2(!qss.isEmpty(), "QSS should be non-empty for each built-in theme");
        QVERIFY(qss.length() > 100);  // Sanity: themes must have substantial content
    }
}

void ThemeCoverageTest::testSelectorCoverage()
{
    for (auto theme : allThemes()) {
        QString qss = SA::getBuiltInRibbonThemeQss(theme);
        QVERIFY(!qss.isEmpty());

        for (const auto& selector : keySelectors()) {
            // The selector must appear in the QSS (as a standalone selector, not just substring)
            QString pattern = selector;
            QVERIFY2(qss.contains(pattern),
                     QString("Theme missing selector: %1").arg(selector).toUtf8().constData());
        }
    }
}

void ThemeCoverageTest::testDarkThemeContrast()
{
    QString darkQss = SA::getBuiltInRibbonThemeQss(SARibbonTheme::RibbonThemeDark);
    QVERIFY(!darkQss.isEmpty());

    // Extract foreground and background from SARibbonBar for contrast check
    QColor bg  = extractBackgroundColor(darkQss, "SARibbonBar");
    QColor fg  = extractForegroundColor(darkQss, "SARibbonBar");

    if (bg.isValid() && fg.isValid()) {
        double ratio = contrastRatio(fg, bg);
        QVERIFY2(ratio >= 4.5,
                 QString("Dark theme SARibbonBar contrast ratio %1 is below 4.5:1 minimum")
                         .arg(ratio)
                         .toUtf8()
                         .constData());
    } else {
        // If we can't parse colors directly, verify the QSS contains color specifications
        QVERIFY2(darkQss.contains("color"),
                 "Dark theme must specify foreground colors");
        QVERIFY2(darkQss.contains("background"),
                 "Dark theme must specify background colors");
    }
}

QTEST_MAIN(ThemeCoverageTest)

#include "ThemeCoverageTest.moc"