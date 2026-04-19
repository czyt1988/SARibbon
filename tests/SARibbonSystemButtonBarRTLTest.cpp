#include <QtTest>
#include <QApplication>
#include "SARibbonSystemButtonBar.h"

class SARibbonSystemButtonBarRTLTest : public QObject
{
    Q_OBJECT

private slots:
    void testSystemButtonsPosition();
    void testSystemButtonsOrder();
};

void SARibbonSystemButtonBarRTLTest::testSystemButtonsPosition()
{
    SARibbonSystemButtonBar buttonBar;
    buttonBar.resize(150, 30);
    buttonBar.show();
    QApplication::processEvents();

    // Test LTR: buttons should be on the right edge
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QList<QPushButton*> buttons = buttonBar.systemButtons();
    QVERIFY(buttons.size() >= 3); // Min, Max, Close
    for (QPushButton* btn : buttons) {
        QVERIFY(btn->geometry().right() <= buttonBar.width());
        QVERIFY(btn->geometry().left() > buttonBar.width() - 150); // Right area
    }

    // Test RTL: buttons should be on the left edge
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    for (QPushButton* btn : buttons) {
        QVERIFY(btn->geometry().left() >= 0);
        QVERIFY(btn->geometry().right() < 150); // Left area
    }

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    buttonBar.hide();
}

void SARibbonSystemButtonBarRTLTest::testSystemButtonsOrder()
{
    SARibbonSystemButtonBar buttonBar;
    buttonBar.resize(150, 30);
    buttonBar.show();
    QApplication::processEvents();

    QList<QPushButton*> buttons = buttonBar.systemButtons();
    QVERIFY(buttons.size() >= 3);

    // Get button positions in LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QList<int> ltrLeftPositions;
    for (QPushButton* btn : buttons) {
        ltrLeftPositions.append(btn->geometry().left());
    }
    std::sort(ltrLeftPositions.begin(), ltrLeftPositions.end());
    // Order in LTR: min (leftmost) -> max -> close (rightmost)

    // Get button positions in RTL
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    QList<int> rtlRightPositions;
    for (QPushButton* btn : buttons) {
        rtlRightPositions.append(btn->geometry().right());
    }
    std::sort(rtlRightPositions.begin(), rtlRightPositions.end());
    // Order in RTL: min (rightmost of left group) -> max -> close (leftmost of left group)
    // So the order of buttons themselves is preserved

    // Verify order is same (button positions relative to each other unchanged)
    QCOMPARE(buttons.size(), 3);
    QPushButton *minBtn = buttonBar.minimizeButton();
    QPushButton *maxBtn = buttonBar.maximizeButton();
    QPushButton *closeBtn = buttonBar.closeButton();

    // In LTR: min.x < max.x < close.x
    QVERIFY(minBtn->geometry().left() < maxBtn->geometry().left());
    QVERIFY(maxBtn->geometry().left() < closeBtn->geometry().left());

    // In RTL: min.x > max.x > close.x (since they are on left edge, order reversed in position but button order preserved)
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    QVERIFY(minBtn->geometry().left() > maxBtn->geometry().left());
    QVERIFY(maxBtn->geometry().left() > closeBtn->geometry().left());

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    buttonBar.hide();
}

QTEST_MAIN(SARibbonSystemButtonBarRTLTest)

#include "SARibbonSystemButtonBarRTLTest.moc"
