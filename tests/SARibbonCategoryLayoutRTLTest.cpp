#include <QtTest>
#include <QApplication>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPanel.h"
#include "SARibbonGlobal.h"

class SARibbonCategoryLayoutRTLTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testPanelOrderInRTL();
    void testScrollButtonsInRTL();
};

void SARibbonCategoryLayoutRTLTest::initTestCase()
{
    int argc = 0;
    char** argv = nullptr;
    QApplication app(argc, argv);
}

void SARibbonCategoryLayoutRTLTest::cleanupTestCase()
{
}

void SARibbonCategoryLayoutRTLTest::testPanelOrderInRTL()
{
    SARibbonBar ribbonBar;
    ribbonBar.resize(800, 200);
    SARibbonCategory* category = ribbonBar.addCategoryPage("Test Category");
    SARibbonPanel* panel1 = category->addPanel("Panel 1");
    SARibbonPanel* panel2 = category->addPanel("Panel 2");
    SARibbonPanel* panel3 = category->addPanel("Panel 3");

    ribbonBar.show();
    QApplication::processEvents();

    // LTR mode
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QApplication::processEvents();
    QRect panel1RectLTR = panel1->geometry();
    QRect panel2RectLTR = panel2->geometry();
    QRect panel3RectLTR = panel3->geometry();

    // Verify panels are ordered left to right
    QVERIFY(panel1RectLTR.left() < panel2RectLTR.left());
    QVERIFY(panel2RectLTR.left() < panel3RectLTR.left());

    // Switch to RTL mode
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    QRect panel1RectRTL = panel1->geometry();
    QRect panel2RectRTL = panel2->geometry();
    QRect panel3RectRTL = panel3->geometry();

    // Verify panels are ordered right to left
    QVERIFY(panel1RectRTL.left() > panel2RectRTL.left());
    QVERIFY(panel2RectRTL.left() > panel3RectRTL.left());

    // Verify panel widths are preserved
    QCOMPARE(panel1RectLTR.width(), panel1RectRTL.width());
    QCOMPARE(panel2RectLTR.width(), panel2RectRTL.width());
    QCOMPARE(panel3RectLTR.width(), panel3RectRTL.width());

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
}

void SARibbonCategoryLayoutRTLTest::testScrollButtonsInRTL()
{
    SARibbonBar ribbonBar;
    ribbonBar.resize(400, 200); // Small width to force scrolling
    SARibbonCategory* category = ribbonBar.addCategoryPage("Test Category");
    
    // Add many panels to force scroll
    for (int i = 0; i < 10; ++i) {
        category->addPanel(QString("Panel %1").arg(i+1));
    }

    ribbonBar.show();
    QApplication::processEvents();

    // LTR mode - scroll buttons should be at right
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QApplication::processEvents();
    QList<QAbstractButton*> scrollButtonsLTR = category->findChildren<QAbstractButton*>();
    QVERIFY(scrollButtonsLTR.size() >= 2); // Left and right scroll buttons
    int rightButtonLeftLTR = scrollButtonsLTR.last()->geometry().left();
    QVERIFY(rightButtonLeftLTR > ribbonBar.width() / 2); // Right button on right side

    // RTL mode - scroll buttons should be at left
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    QList<QAbstractButton*> scrollButtonsRTL = category->findChildren<QAbstractButton*>();
    QVERIFY(scrollButtonsRTL.size() >= 2);
    int rightButtonLeftRTL = scrollButtonsRTL.first()->geometry().left();
    QVERIFY(rightButtonLeftRTL < ribbonBar.width() / 2); // Right button now on left side

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
}

QTEST_MAIN(SARibbonCategoryLayoutRTLTest)

#include "SARibbonCategoryLayoutRTLTest.moc"
