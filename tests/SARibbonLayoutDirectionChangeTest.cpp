#include <QtTest>
#include <QApplication>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPanel.h"
#include "SARibbonToolButton.h"

class SARibbonLayoutDirectionChangeTest : public QObject
{
    Q_OBJECT

private slots:
    void testRibbonBarLayoutDirectionChange();
    void testCategoryLayoutDirectionChange();
    void testPanelLayoutDirectionChange();
    void testToolButtonLayoutDirectionChange();
};

void SARibbonLayoutDirectionChangeTest::testRibbonBarLayoutDirectionChange()
{
    SARibbonBar ribbonBar;
    ribbonBar.resize(800, 200);
    ribbonBar.show();
    QApplication::processEvents();

    // Test LTR baseline
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QVERIFY(ribbonBar.layoutDirection() == Qt::LeftToRight);
    QSize ltrSize = ribbonBar.size();
    QVERIFY(ltrSize.isValid());

    // Change to RTL
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    QVERIFY(ribbonBar.layoutDirection() == Qt::RightToLeft);
    QSize rtlSize = ribbonBar.size();
    QVERIFY(rtlSize.isValid());
    QCOMPARE(ltrSize, rtlSize); // Size should remain the same

    // Change back to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QApplication::processEvents();
    QVERIFY(ribbonBar.layoutDirection() == Qt::LeftToRight);

    ribbonBar.hide();
}

void SARibbonLayoutDirectionChangeTest::testCategoryLayoutDirectionChange()
{
    SARibbonBar ribbonBar;
    SARibbonCategory* category = ribbonBar.addCategory("Test Category");
    ribbonBar.resize(800, 200);
    ribbonBar.show();
    QApplication::processEvents();

    // Test LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QRect ltrGeometry = category->geometry();
    QVERIFY(ltrGeometry.isValid());

    // Test RTL
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    QRect rtlGeometry = category->geometry();
    QVERIFY(rtlGeometry.isValid());
    QCOMPARE(ltrGeometry.size(), rtlGeometry.size());

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QApplication::processEvents();

    ribbonBar.hide();
}

void SARibbonLayoutDirectionChangeTest::testPanelLayoutDirectionChange()
{
    SARibbonBar ribbonBar;
    SARibbonCategory* category = ribbonBar.addCategory("Test Category");
    SARibbonPanel* panel = category->addPanel("Test Panel");
    ribbonBar.resize(800, 200);
    ribbonBar.show();
    QApplication::processEvents();

    // Test LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QRect ltrGeometry = panel->geometry();
    QVERIFY(ltrGeometry.isValid());

    // Test RTL
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    QRect rtlGeometry = panel->geometry();
    QVERIFY(rtlGeometry.isValid());
    QCOMPARE(ltrGeometry.size(), rtlGeometry.size());

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QApplication::processEvents();

    ribbonBar.hide();
}

void SARibbonLayoutDirectionChangeTest::testToolButtonLayoutDirectionChange()
{
    SARibbonToolButton button;
    button.setText("Test Button");
    button.resize(100, 50);
    button.show();
    QApplication::processEvents();

    // Test LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QRect ltrGeometry = button.geometry();
    QVERIFY(ltrGeometry.isValid());

    // Test RTL
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    QRect rtlGeometry = button.geometry();
    QVERIFY(rtlGeometry.isValid());
    QCOMPARE(ltrGeometry.size(), rtlGeometry.size());

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QApplication::processEvents();

    button.hide();
}

QTEST_MAIN(SARibbonLayoutDirectionChangeTest)

#include "SARibbonLayoutDirectionChangeTest.moc"
