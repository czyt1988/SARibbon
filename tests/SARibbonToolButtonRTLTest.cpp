#include <QtTest>
#include <QApplication>
#include "SARibbonToolButton.h"

class SARibbonToolButtonRTLTest : public QObject
{
    Q_OBJECT

private slots:
    void testSmallButtonSizeHintConsistency();
    void testLargeButtonSizeHintConsistency();
    void testButtonPaintInRTL();
};

void SARibbonToolButtonRTLTest::testSmallButtonSizeHintConsistency()
{
    SARibbonToolButton button;
    button.setButtonType(SARibbonToolButton::SmallButton);
    button.setPopupMode(QToolButton::MenuButtonPopup);
    button.setText("Test");

    QApplication::setLayoutDirection(Qt::LeftToRight);
    button.invalidateSizeHint();
    QSize ltrSize = button.sizeHint();

    QApplication::setLayoutDirection(Qt::RightToLeft);
    button.invalidateSizeHint();
    QSize rtlSize = button.sizeHint();

    QVERIFY(qAbs(ltrSize.width() - rtlSize.width()) <= 15);
    QVERIFY(qAbs(ltrSize.height() - rtlSize.height()) <= 15);
    QApplication::setLayoutDirection(Qt::LeftToRight);
}

void SARibbonToolButtonRTLTest::testLargeButtonSizeHintConsistency()
{
    SARibbonToolButton button;
    button.setButtonType(SARibbonToolButton::LargeButton);
    button.setPopupMode(QToolButton::MenuButtonPopup);
    button.setText("Test");

    QApplication::setLayoutDirection(Qt::LeftToRight);
    button.invalidateSizeHint();
    QSize ltrSize = button.sizeHint();

    QApplication::setLayoutDirection(Qt::RightToLeft);
    button.invalidateSizeHint();
    QSize rtlSize = button.sizeHint();

    QVERIFY(qAbs(ltrSize.width() - rtlSize.width()) <= 15);
    QVERIFY(qAbs(ltrSize.height() - rtlSize.height()) <= 15);
    QApplication::setLayoutDirection(Qt::LeftToRight);
}

void SARibbonToolButtonRTLTest::testButtonPaintInRTL()
{
    SARibbonToolButton button;
    button.setButtonType(SARibbonToolButton::LargeButton);
    button.setPopupMode(QToolButton::MenuButtonPopup);
    button.setText("Test");
    button.resize(80, 60);

    QApplication::setLayoutDirection(Qt::RightToLeft);
    button.show();
    QApplication::processEvents();
    QVERIFY(button.isVisible());

    // Trigger a repaint — should not crash in RTL
    button.update();
    QApplication::processEvents();

    button.hide();
    QApplication::setLayoutDirection(Qt::LeftToRight);
}

QTEST_MAIN(SARibbonToolButtonRTLTest)

#include "SARibbonToolButtonRTLTest.moc"
