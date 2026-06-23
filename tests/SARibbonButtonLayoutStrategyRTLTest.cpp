#include <QtTest>
#include <QApplication>
#include <QFontMetrics>
#include <QStyleOptionToolButton>
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

    SARibbonSmallButtonLayoutStrategy smallStrategy;
    SARibbonButtonLayoutContext ctx(&button);

    QStyleOptionToolButton opt;
    opt.rect = button.rect();
    opt.state |= QStyle::State_Enabled;
    opt.features |= QStyleOptionToolButton::MenuButtonPopup;
    opt.text            = QStringLiteral("Test");
    opt.toolButtonStyle = Qt::ToolButtonTextOnly;
    opt.fontMetrics     = QFontMetrics(button.font());

    // LTR: indicator on the right
    QApplication::setLayoutDirection(Qt::LeftToRight);
    SARibbonButtonLayoutRects ltrRects;
    smallStrategy.calculateDrawRects(opt, ltrRects, ctx);
    QVERIFY(ltrRects.indicatorRect.isValid());
    QVERIFY(ltrRects.indicatorRect.left() > button.width() / 2);

    // RTL: indicator on the left
    QApplication::setLayoutDirection(Qt::RightToLeft);
    SARibbonButtonLayoutRects rtlRects;
    smallStrategy.calculateDrawRects(opt, rtlRects, ctx);
    QVERIFY(rtlRects.indicatorRect.isValid());
    QVERIFY(rtlRects.indicatorRect.right() < button.width() / 2);

    // Icon rect size should be identical in both directions (allow tolerance for platform style differences)
    QVERIFY(qAbs(ltrRects.iconRect.width() - rtlRects.iconRect.width()) <= 15);
    QVERIFY(qAbs(ltrRects.iconRect.height() - rtlRects.iconRect.height()) <= 15);

    QApplication::setLayoutDirection(Qt::LeftToRight);
}

void SARibbonButtonLayoutStrategyRTLTest::testLargeButtonStrategyIndicator()
{
    SARibbonToolButton button;
    button.setButtonType(SARibbonToolButton::LargeButton);
    button.setPopupMode(QToolButton::MenuButtonPopup);
    button.setText("Test");
    button.resize(80, 60);

    SARibbonLargeButtonLayoutStrategy largeStrategy;
    SARibbonButtonLayoutContext ctx(&button);
    ctx.enableWordWrap = false;  // single-line mode for compact indicator placement

    QStyleOptionToolButton opt;
    opt.rect = button.rect();
    opt.state |= QStyle::State_Enabled;
    opt.features |= QStyleOptionToolButton::MenuButtonPopup;
    opt.text            = QStringLiteral("Test");
    opt.toolButtonStyle = Qt::ToolButtonTextUnderIcon;
    opt.fontMetrics     = QFontMetrics(button.font());

    // LTR: indicator at bottom-right
    QApplication::setLayoutDirection(Qt::LeftToRight);
    SARibbonButtonLayoutRects ltrRects;
    largeStrategy.calculateDrawRects(opt, ltrRects, ctx);
    QVERIFY(ltrRects.indicatorRect.isValid());
    QVERIFY(ltrRects.indicatorRect.left() > button.width() / 2);
    QVERIFY(ltrRects.indicatorRect.top() > button.height() / 2);

    // RTL: indicator at bottom-left
    QApplication::setLayoutDirection(Qt::RightToLeft);
    SARibbonButtonLayoutRects rtlRects;
    largeStrategy.calculateDrawRects(opt, rtlRects, ctx);
    QVERIFY(rtlRects.indicatorRect.isValid());
    QVERIFY(rtlRects.indicatorRect.right() < button.width() / 2);
    QVERIFY(rtlRects.indicatorRect.top() > button.height() / 2);

    // Icon rect size should be identical in both directions (allow tolerance for platform style differences)
    QVERIFY(qAbs(ltrRects.iconRect.width() - rtlRects.iconRect.width()) <= 15);
    QVERIFY(qAbs(ltrRects.iconRect.height() - rtlRects.iconRect.height()) <= 15);

    QApplication::setLayoutDirection(Qt::LeftToRight);
}

void SARibbonButtonLayoutStrategyRTLTest::testStrategySizeHintConsistency()
{
    SARibbonToolButton button;
    button.setButtonType(SARibbonToolButton::SmallButton);
    button.setText("Test");

    SARibbonSmallButtonLayoutStrategy smallStrategy;
    SARibbonLargeButtonLayoutStrategy largeStrategy;
    SARibbonButtonLayoutContext ctx(&button);

    QStyleOptionToolButton opt;
    opt.state |= QStyle::State_Enabled;
    opt.text            = QStringLiteral("Test");
    opt.toolButtonStyle = Qt::ToolButtonTextBesideIcon;
    opt.fontMetrics     = QFontMetrics(button.font());

    // Small button size hint should be the same in LTR and RTL
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QSize ltrSmallSize = smallStrategy.calculateSizeHint(opt, ctx);
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QSize rtlSmallSize = smallStrategy.calculateSizeHint(opt, ctx);
    QVERIFY(qAbs(ltrSmallSize.width() - rtlSmallSize.width()) <= 15);
    QVERIFY(qAbs(ltrSmallSize.height() - rtlSmallSize.height()) <= 15);

    // Large button size hint should be the same in LTR and RTL
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QSize ltrLargeSize = largeStrategy.calculateSizeHint(opt, ctx);
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QSize rtlLargeSize = largeStrategy.calculateSizeHint(opt, ctx);
    QVERIFY(qAbs(ltrLargeSize.width() - rtlLargeSize.width()) <= 15);
    QVERIFY(qAbs(ltrLargeSize.height() - rtlLargeSize.height()) <= 15);

    QApplication::setLayoutDirection(Qt::LeftToRight);
}

QTEST_MAIN(SARibbonButtonLayoutStrategyRTLTest)

#include "SARibbonButtonLayoutStrategyRTLTest.moc"
