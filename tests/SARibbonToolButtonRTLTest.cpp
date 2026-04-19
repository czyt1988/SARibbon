#include <QtTest>
#include <QApplication>
#include "SARibbonToolButton.h"

class SARibbonToolButtonRTLTest : public QObject
{
    Q_OBJECT

private slots:
    void testSmallButtonIndicatorPosition();
    void testLargeButtonIndicatorPosition();
};

void SARibbonToolButtonRTLTest::testSmallButtonIndicatorPosition()
{
    SARibbonToolButton button;
    button.setButtonType(SARibbonToolButton::SmallButton);
    button.setPopupMode(QToolButton::MenuButtonPopup);
    button.setText("Test");
    button.resize(80, 30);
    button.show();
    QApplication::processEvents();

    // Test LTR: indicator should be on the right
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QRect indicatorRect = button.calcSmallButtonDrawRects().indicatorRect;
    QVERIFY(indicatorRect.isValid());
    QVERIFY(indicatorRect.left() > button.width() / 2); // Right side

    // Test RTL: indicator should be on the left
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    indicatorRect = button.calcSmallButtonDrawRects().indicatorRect;
    QVERIFY(indicatorRect.isValid());
    QVERIFY(indicatorRect.right() < button.width() / 2); // Left side

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    button.hide();
}

void SARibbonToolButtonRTLTest::testLargeButtonIndicatorPosition()
{
    SARibbonToolButton button;
    button.setButtonType(SARibbonToolButton::LargeButton);
    button.setPopupMode(QToolButton::MenuButtonPopup);
    button.setText("Test");
    button.resize(80, 60);
    button.show();
    QApplication::processEvents();

    // Test LTR: indicator should be at bottom-right
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QRect indicatorRect = button.calcLargeButtonDrawRects().indicatorRect;
    QVERIFY(indicatorRect.isValid());
    QVERIFY(indicatorRect.left() > button.width() / 2); // Right side
    QVERIFY(indicatorRect.top() > button.height() / 2); // Bottom side

    // Test RTL: indicator should be at bottom-left
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    indicatorRect = button.calcLargeButtonDrawRects().indicatorRect;
    QVERIFY(indicatorRect.isValid());
    QVERIFY(indicatorRect.right() < button.width() / 2); // Left side
    QVERIFY(indicatorRect.top() > button.height() / 2); // Bottom side

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    button.hide();
}

QTEST_MAIN(SARibbonToolButtonRTLTest)

#include "SARibbonToolButtonRTLTest.moc"
