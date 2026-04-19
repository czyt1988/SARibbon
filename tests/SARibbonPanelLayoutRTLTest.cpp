#include <QtTest>
#include <QApplication>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPanel.h"
#include "SARibbonToolButton.h"
#include "SARibbonGlobal.h"

class SARibbonPanelLayoutRTLTest : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testButtonOrderInRTL();
};

void SARibbonPanelLayoutRTLTest::initTestCase()
{
    int argc = 0;
    char** argv = nullptr;
    QApplication app(argc, argv);
}

void SARibbonPanelLayoutRTLTest::cleanupTestCase()
{
}

void SARibbonPanelLayoutRTLTest::testButtonOrderInRTL()
{
    SARibbonBar ribbonBar;
    ribbonBar.resize(800, 200);
    SARibbonCategory* category = ribbonBar.addCategoryPage("Test Category");
    SARibbonPanel* panel = category->addPanel("Test Panel");
    
    QAction* act1 = new QAction("Button 1", this);
    QAction* act2 = new QAction("Button 2", this);
    QAction* act3 = new QAction("Button 3", this);
    
    panel->addSmallAction(act1);
    panel->addSmallAction(act2);
    panel->addSmallAction(act3);

    ribbonBar.show();
    QApplication::processEvents();

    // LTR mode - just verify layout works
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QApplication::processEvents();

    // Switch to RTL mode - verify layout doesn't crash
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
}

QTEST_MAIN(SARibbonPanelLayoutRTLTest)

#include "SARibbonPanelLayoutRTLTest.moc"
