#include <QtTest>
#include <QApplication>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonGlobal.h"
#include "SARibbonUtil.h"

class SARibbonBarLayoutRTLTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testBarLayoutDirectionChange();
    void testRTLLayoutMirroring();
};

void SARibbonBarLayoutRTLTest::initTestCase()
{
    int argc = 0;
    char** argv = nullptr;
    QApplication app(argc, argv);
}

void SARibbonBarLayoutRTLTest::cleanupTestCase()
{
}

void SARibbonBarLayoutRTLTest::testBarLayoutDirectionChange()
{
    SARibbonBar ribbonBar;
    ribbonBar.resize(800, 200);
    SARibbonCategory* category1 = ribbonBar.addCategoryPage("Category 1");
    SARibbonCategory* category2 = ribbonBar.addCategoryPage("Category 2");
    SARibbonCategory* category3 = ribbonBar.addCategoryPage("Category 3");

    ribbonBar.show();
    QApplication::processEvents();

    // Test LTR mode
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QVERIFY(!SA::saIsRTL());
    QApplication::processEvents();
    QRect category1RectLTR = category1->geometry();
    QRect category2RectLTR = category2->geometry();
    QRect category3RectLTR = category3->geometry();

    // Verify categories are ordered left to right
    QVERIFY(category1RectLTR.left() < category2RectLTR.left());
    QVERIFY(category2RectLTR.left() < category3RectLTR.left());

    // Switch to RTL mode
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QVERIFY(SA::saIsRTL());
    QApplication::processEvents();
    QRect category1RectRTL = category1->geometry();
    QRect category2RectRTL = category2->geometry();
    QRect category3RectRTL = category3->geometry();

    // Verify categories are ordered right to left
    QVERIFY(category1RectRTL.left() > category2RectRTL.left());
    QVERIFY(category2RectRTL.left() > category3RectRTL.left());

    // Verify total width is preserved
    QCOMPARE(category1RectLTR.width(), category1RectRTL.width());
    QCOMPARE(category2RectLTR.width(), category2RectRTL.width());
    QCOMPARE(category3RectLTR.width(), category3RectRTL.width());

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QApplication::processEvents();
}

void SARibbonBarLayoutRTLTest::testRTLLayoutMirroring()
{
    SARibbonBar ribbonBar;
    ribbonBar.resize(1000, 200);
    SARibbonCategory* category = ribbonBar.addCategoryPage("Test Category");
    ribbonBar.show();
    QApplication::processEvents();

    // LTR mode
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QApplication::processEvents();
    int categoryLeftLTR = category->geometry().left();

    // RTL mode
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    int categoryLeftRTL = category->geometry().left();

    // Verify category is mirrored
    int expectedLeftRTL = SA::saMirrorX(categoryLeftLTR, ribbonBar.width(), category->width());
    // Allow small tolerance for layout margins
    QVERIFY(qAbs(categoryLeftRTL - expectedLeftRTL) < 10);

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
}

QTEST_MAIN(SARibbonBarLayoutRTLTest)

#include "SARibbonBarLayoutRTLTest.moc"
