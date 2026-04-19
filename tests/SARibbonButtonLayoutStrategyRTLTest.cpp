#include <QtTest>
#include <QApplication>
#include "SARibbonButtonLayoutStrategy.h"
#include "SARibbonToolButton.h"

class SARibbonButtonLayoutStrategyRTLTest : public QObject
{
    Q_OBJECT

private slots:
    void testSmallButtonStrategyIndicator();
    void testLargeButtonStrategyIndicator();
    void testStrategySizeHintConsistency();
};

void SARibbonButtonLayoutStrategyRTLTest::testSmallButtonStrategyIndicator()
{
    SARibbonToolButton button;
    button.setButtonType(SARibbonToolButton::SmallButton);
    button.setPopupMode(QToolButton::MenuButtonPopup);
    button.setText("Test");
    button.resize(80, 30);

    SARibbonSmallButtonLayoutStrategy smallStrategy(&button);

    // Test LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    SARibbonButtonLayoutStrategy::DrawRects ltrRects = smallStrategy.calcDrawRects(button.rect());
    QVERIFY(ltrRects.indicatorRect.isValid());
    QVERIFY(ltrRects.indicatorRect.left() > button.width() / 2); // Right side

    // Test RTL
    QApplication::setLayoutDirection(Qt::RightToLeft);
    SARibbonButtonLayoutStrategy::DrawRects rtlRects = smallStrategy.calcDrawRects(button.rect());
    QVERIFY(rtlRects.indicatorRect.isValid());
    QVERIFY(rtlRects.indicatorRect.right() < button.width() / 2); // Left side

    // Icon should be mirrored
    QCOMPARE(ltrRects.iconRect.size(), rtlRects.iconRect.size());

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
}

void SARibbonButtonLayoutStrategyRTLTest::testLargeButtonStrategyIndicator()
{
    SARibbonToolButton button;
    button.setButtonType(SARibbonToolButton::LargeButton);
    button.setPopupMode(QToolButton::MenuButtonPopup);
    button.setText("Test");
    button.resize(80, 60);

    SARibbonLargeButtonLayoutStrategy largeStrategy(&button);

    // Test LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    SARibbonButtonLayoutStrategy::DrawRects ltrRects = largeStrategy.calcDrawRects(button.rect());
    QVERIFY(ltrRects.indicatorRect.isValid());
    QVERIFY(ltrRects.indicatorRect.left() > button.width() / 2); // Right side
    QVERIFY(ltrRects.indicatorRect.top() > button.height() / 2); // Bottom side

    // Test RTL
    QApplication::setLayoutDirection(Qt::RightToLeft);
    SARibbonButtonLayoutStrategy::DrawRects rtlRects = largeStrategy.calcDrawRects(button.rect());
    QVERIFY(rtlRects.indicatorRect.isValid());
    QVERIFY(rtlRects.indicatorRect.right() < button.width() / 2); // Left side
    QVERIFY(rtlRects.indicatorRect.top() > button.height() / 2); // Bottom side

    // Icon should be mirrored
    QCOMPARE(ltrRects.iconRect.size(), rtlRects.iconRect.size());

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
}

void SARibbonButtonLayoutStrategyRTLTest::testStrategySizeHintConsistency()
{
    SARibbonToolButton button;
    button.setButtonType(SARibbonToolButton::SmallButton);
    button.setText("Test");

    SARibbonSmallButtonLayoutStrategy smallStrategy(&button);
    SARibbonLargeButtonLayoutStrategy largeStrategy(&button);

    // Test small button size hint same in LTR and RTL
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QSize ltrSmallSize = smallStrategy.sizeHint(Qt::MinimumSize);
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QSize rtlSmallSize = smallStrategy.sizeHint(Qt::MinimumSize);
    QCOMPARE(ltrSmallSize, rtlSmallSize);

    // Test large button size hint same in LTR and RTL
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QSize ltrLargeSize = largeStrategy.sizeHint(Qt::MinimumSize);
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QSize rtlLargeSize = largeStrategy.sizeHint(Qt::MinimumSize);
    QCOMPARE(ltrLargeSize, rtlLargeSize);

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
}

QTEST_MAIN(SARibbonButtonLayoutStrategyRTLTest)

#include "SARibbonButtonLayoutStrategyRTLTest.moc"
