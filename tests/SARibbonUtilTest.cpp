#include <QtTest>
#include <QApplication>
#include "SARibbonGlobal.h"
#include "SARibbonUtil.h"

class SARibbonUtilTest : public QObject
{
    Q_OBJECT

private slots:
    void testSaIsRTL();
    void testSaMirrorX();
};

void SARibbonUtilTest::testSaIsRTL()
{
    // Test LTR mode (default)
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QVERIFY(!SA::saIsRTL());

    // Test RTL mode
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QVERIFY(SA::saIsRTL());

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
}

void SARibbonUtilTest::testSaMirrorX()
{
    // Test LTR mode
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QCOMPARE(SA::saMirrorX(10, 100, 20), 10);
    QCOMPARE(SA::saMirrorX(0, 100, 10), 0);
    QCOMPARE(SA::saMirrorX(70, 100, 30), 70);

    // Test RTL mode
    QApplication::setLayoutDirection(Qt::RightToLeft);
    // Formula: containerWidth - x - elementWidth
    QCOMPARE(SA::saMirrorX(10, 100, 20), 100 - 10 - 20); // 70
    QCOMPARE(SA::saMirrorX(0, 100, 10), 100 - 0 - 10); // 90
    QCOMPARE(SA::saMirrorX(70, 100, 30), 100 - 70 - 30); // 0

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
}

QTEST_MAIN(SARibbonUtilTest)

#include "SARibbonUtilTest.moc"
