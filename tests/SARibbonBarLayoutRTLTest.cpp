#include <QtTest>
#include <QApplication>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonGlobal.h"
#include "SARibbonUtil.h"
#include "SARibbonTabBar.h"

class SARibbonBarLayoutRTLTest : public QObject
{
    Q_OBJECT

private slots:
    void testBarLayoutDirectionChange();
    void testRTLLayoutMirroring();
};

void SARibbonBarLayoutRTLTest::testBarLayoutDirectionChange()
{
    SARibbonBar ribbonBar;
    ribbonBar.resize(800, 200);
    ribbonBar.addCategoryPage("Category 1");
    ribbonBar.addCategoryPage("Category 2");
    ribbonBar.addCategoryPage("Category 3");

    ribbonBar.show();
    QApplication::processEvents();

    SARibbonTabBar* tabBar = ribbonBar.ribbonTabBar();
    QVERIFY(tabBar != nullptr);
    QVERIFY(tabBar->count() >= 3);

    // Test LTR mode
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QVERIFY(!SA::saIsRTL());
    QApplication::processEvents();
    QRect tab1RectLTR = tabBar->tabRect(0);
    QRect tab2RectLTR = tabBar->tabRect(1);
    QRect tab3RectLTR = tabBar->tabRect(2);

    // Verify tabs are ordered left to right
    QVERIFY(tab1RectLTR.left() < tab2RectLTR.left());
    QVERIFY(tab2RectLTR.left() < tab3RectLTR.left());

    // Switch to RTL mode
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QVERIFY(SA::saIsRTL());
    QApplication::processEvents();
    QRect tab1RectRTL = tabBar->tabRect(0);
    QRect tab2RectRTL = tabBar->tabRect(1);
    QRect tab3RectRTL = tabBar->tabRect(2);

    // Verify tabs are ordered right to left
    QVERIFY(tab1RectRTL.left() > tab2RectRTL.left());
    QVERIFY(tab2RectRTL.left() > tab3RectRTL.left());

    // Verify tab widths are preserved (allow tolerance for platform style differences)
    QVERIFY(qAbs(tab1RectLTR.width() - tab1RectRTL.width()) <= 15);
    QVERIFY(qAbs(tab2RectLTR.width() - tab2RectRTL.width()) <= 15);
    QVERIFY(qAbs(tab3RectLTR.width() - tab3RectRTL.width()) <= 15);

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
