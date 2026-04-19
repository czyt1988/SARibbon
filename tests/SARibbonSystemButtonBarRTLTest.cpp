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
    QAbstractButton *minBtn = buttonBar.minimizeButton();
    QAbstractButton *maxBtn = buttonBar.maximizeButton();
    QAbstractButton *closeBtn = buttonBar.closeButton();
    QVERIFY(minBtn && maxBtn && closeBtn); // Min, Max, Close exist
    for (QAbstractButton* btn : {minBtn, maxBtn, closeBtn}) {
        QVERIFY(btn->geometry().right() <= buttonBar.width());
        QVERIFY(btn->geometry().left() > buttonBar.width() - 150); // Right area
    }

    // Test RTL: buttons should be on the left edge
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    for (QAbstractButton* btn : {minBtn, maxBtn, closeBtn}) {
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

    QAbstractButton *minBtn = buttonBar.minimizeButton();
    QAbstractButton *maxBtn = buttonBar.maximizeButton();
    QAbstractButton *closeBtn = buttonBar.closeButton();
    QVERIFY(minBtn && maxBtn && closeBtn);

    // Get button positions in LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QList<int> ltrLeftPositions;
    for (QAbstractButton* btn : {minBtn, maxBtn, closeBtn}) {
        ltrLeftPositions.append(btn->geometry().left());
    }
    std::sort(ltrLeftPositions.begin(), ltrLeftPositions.end());
    // Order in LTR: min (leftmost) -> max -> close (rightmost)

    // Get button positions in RTL
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    QList<int> rtlRightPositions;
    for (QAbstractButton* btn : {minBtn, maxBtn, closeBtn}) {
        rtlRightPositions.append(btn->geometry().right());
    }
    std::sort(rtlRightPositions.begin(), rtlRightPositions.end());
    // Order in RTL: min (rightmost of left group) -> max -> close (leftmost of left group)
    // So the order of buttons themselves is preserved

    // Verify order is same (button positions relative to each other unchanged)

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
