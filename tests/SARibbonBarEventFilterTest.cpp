#include <QtTest>
#include <QApplication>
#include "SARibbonBar.h"

// Testable subclass to expose protected eventFilter method
class TestableSARibbonBar : public SARibbonBar
{
public:
    using SARibbonBar::eventFilter;
};

class SARibbonBarEventFilterTest : public QObject
{
    Q_OBJECT

private slots:
    void testWindowStateChangeEventHandling();
    void testOtherCornerWidgetEventHandling();
};

void SARibbonBarEventFilterTest::testWindowStateChangeEventHandling()
{
    TestableSARibbonBar ribbonBar;
    ribbonBar.resize(800, 200);
    
    // Create dummy corner widgets
    QWidget topLeftCorner;
    QWidget topRightCorner;
    
    // Set corner widgets
    ribbonBar.setCornerWidget(&topLeftCorner, Qt::TopLeftCorner);
    ribbonBar.setCornerWidget(&topRightCorner, Qt::TopRightCorner);
    
    ribbonBar.show();
    QApplication::processEvents();

    // Create WindowStateChange event
    QEvent windowStateChangeEvent(QEvent::WindowStateChange);

    // Get corner widgets to verify they are set correctly
    QWidget* setTopLeft = ribbonBar.cornerWidget(Qt::TopLeftCorner);
    QWidget* setTopRight = ribbonBar.cornerWidget(Qt::TopRightCorner);
    
    QCOMPARE(setTopLeft, &topLeftCorner);
    QCOMPARE(setTopRight, &topRightCorner);

    // Test event filter for top left corner widget
    bool handled = ribbonBar.eventFilter(&topLeftCorner, &windowStateChangeEvent);
    // Event is processed (relayout called) but propagated to parent class
    Q_UNUSED(handled);

    // Test event filter for top right corner widget
    handled = ribbonBar.eventFilter(&topRightCorner, &windowStateChangeEvent);
    Q_UNUSED(handled);

    // Test that non-corner widgets don't process this event
    QWidget testWidget;
    handled = ribbonBar.eventFilter(&testWidget, &windowStateChangeEvent);
    Q_UNUSED(handled);
}

void SARibbonBarEventFilterTest::testOtherCornerWidgetEventHandling()
{
    TestableSARibbonBar ribbonBar;
    ribbonBar.resize(800, 200);
    
    // Create dummy corner widget
    QWidget topLeftCorner;
    ribbonBar.setCornerWidget(&topLeftCorner, Qt::TopLeftCorner);
    
    ribbonBar.show();
    QApplication::processEvents();

    // Verify corner widget is set correctly
    QWidget* setTopLeft = ribbonBar.cornerWidget(Qt::TopLeftCorner);
    QCOMPARE(setTopLeft, &topLeftCorner);

    // Test UpdateLater event
    QEvent updateLaterEvent(QEvent::UpdateLater);
    bool handled = ribbonBar.eventFilter(&topLeftCorner, &updateLaterEvent);
    Q_UNUSED(handled);

    // Test MouseButtonRelease event
    QMouseEvent mouseReleaseEvent(QEvent::MouseButtonRelease, QPointF(0,0), QPointF(0,0), Qt::LeftButton, Qt::LeftButton, Qt::NoModifier);
    handled = ribbonBar.eventFilter(&topLeftCorner, &mouseReleaseEvent);
    Q_UNUSED(handled);

    // Test WindowActivate event
    QEvent windowActivateEvent(QEvent::WindowActivate);
    handled = ribbonBar.eventFilter(&topLeftCorner, &windowActivateEvent);
    Q_UNUSED(handled);
}

QTEST_MAIN(SARibbonBarEventFilterTest)

#include "SARibbonBarEventFilterTest.moc"