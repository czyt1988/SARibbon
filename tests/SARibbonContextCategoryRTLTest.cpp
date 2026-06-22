#include <QtTest>
#include <QApplication>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonContextCategory.h"
#include "SARibbonTabBar.h"

class SARibbonContextCategoryRTLTest : public QObject
{
    Q_OBJECT

private slots:
    void testContextCategoryRtlLayout();
};

void SARibbonContextCategoryRTLTest::testContextCategoryRtlLayout()
{
    SARibbonBar ribbonBar;
    ribbonBar.resize(800, 200);

    // Add context category with two pages
    QColor ctxColor = QColor(Qt::red);
    SARibbonContextCategory* ctx = ribbonBar.addContextCategory("Test Context", ctxColor);
    ctx->addCategoryPage("Context 1");
    ctx->addCategoryPage("Context 2");
    ribbonBar.showContextCategory(ctx);
    ribbonBar.show();
    QApplication::processEvents();

    SARibbonTabBar* tabBar = ribbonBar.ribbonTabBar();
    QVERIFY(tabBar != nullptr);
    int tabCount = tabBar->count();
    QVERIFY(tabCount >= 2);

    // Test LTR: tabs are left-to-right
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QApplication::processEvents();
    QRect tab1RectLTR = tabBar->tabRect(tabCount - 2);
    QRect tab2RectLTR = tabBar->tabRect(tabCount - 1);
    QVERIFY(tab1RectLTR.isValid());
    QVERIFY(tab2RectLTR.isValid());
    QVERIFY(tab1RectLTR.left() < tab2RectLTR.left());

    // Test RTL: tabs are right-to-left
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    QRect tab1RectRTL = tabBar->tabRect(tabCount - 2);
    QRect tab2RectRTL = tabBar->tabRect(tabCount - 1);
    QVERIFY(tab1RectRTL.isValid());
    QVERIFY(tab2RectRTL.isValid());
    QVERIFY(tab1RectRTL.left() > tab2RectRTL.left());

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    ribbonBar.hide();
}

QTEST_MAIN(SARibbonContextCategoryRTLTest)

#include "SARibbonContextCategoryRTLTest.moc"
