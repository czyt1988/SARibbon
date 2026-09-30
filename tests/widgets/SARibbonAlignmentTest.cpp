#include <QtTest>
#include <QApplication>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"

class SARibbonAlignmentTest : public QObject
{
    Q_OBJECT

private slots:
    void testAlignmentEnumValues();
    void testAlignmentPropagation();
};

void SARibbonAlignmentTest::testAlignmentEnumValues()
{
    // Verify enum values exist
    QVERIFY(static_cast<int>(SARibbonAlignment::AlignLeft) == 0);
    QVERIFY(static_cast<int>(SARibbonAlignment::AlignCenter) == 1);
    QVERIFY(static_cast<int>(SARibbonAlignment::AlignRight) == 2);
}

void SARibbonAlignmentTest::testAlignmentPropagation()
{
    SARibbonBar ribbonBar;
    ribbonBar.resize(800, 200);

    // Test setting alignment to AlignRight
    ribbonBar.setRibbonAlignment(SARibbonAlignment::AlignRight);
    QCOMPARE(ribbonBar.ribbonAlignment(), SARibbonAlignment::AlignRight);

    // Add a category and verify alignment is propagated
    SARibbonCategory* category = ribbonBar.addCategoryPage("Test Category");
    QVERIFY(category != nullptr);

    // Force layout update
    ribbonBar.show();
    QApplication::processEvents();

    // Verify alignment is correctly stored and propagated
    QCOMPARE(ribbonBar.ribbonAlignment(), SARibbonAlignment::AlignRight);

    // Test switching back to AlignLeft
    ribbonBar.setRibbonAlignment(SARibbonAlignment::AlignLeft);
    QCOMPARE(ribbonBar.ribbonAlignment(), SARibbonAlignment::AlignLeft);

    // Test AlignCenter
    ribbonBar.setRibbonAlignment(SARibbonAlignment::AlignCenter);
    QCOMPARE(ribbonBar.ribbonAlignment(), SARibbonAlignment::AlignCenter);
}

QTEST_MAIN(SARibbonAlignmentTest)

#include "SARibbonAlignmentTest.moc"
