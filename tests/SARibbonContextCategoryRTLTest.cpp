#include <QtTest>
#include <QApplication>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"

class SARibbonContextCategoryRTLTest : public QObject
{
    Q_OBJECT

private slots:
    void testContextCategoryTitleRect();
};

void SARibbonContextCategoryRTLTest::testContextCategoryTitleRect()
{
    SARibbonBar ribbonBar;
    ribbonBar.resize(800, 200);

    // Add context category
    SARibbonCategory* ctxCat1 = ribbonBar.addCategory("Context 1");
    SARibbonCategory* ctxCat2 = ribbonBar.addCategory("Context 2");
    QColor ctxColor = QColor(Qt::red);
    ribbonBar.addContextCategory("Test Context", ctxColor, QList<SARibbonCategory*>() << ctxCat1 << ctxCat2);
    ribbonBar.showContextCategory("Test Context");
    ribbonBar.show();
    QApplication::processEvents();

    // Test LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    QRect ltrTitleRect = ribbonBar.calcContextCategoryTitleRect("Test Context");
    QVERIFY(ltrTitleRect.isValid());
    QVERIFY(ltrTitleRect.left() < ctxCat2->geometry().right());
    QVERIFY(ltrTitleRect.right() > ctxCat1->geometry().left());
    int ltrWidth = ltrTitleRect.width();

    // Test RTL
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    QRect rtlTitleRect = ribbonBar.calcContextCategoryTitleRect("Test Context");
    QVERIFY(rtlTitleRect.isValid());
    QCOMPARE(rtlTitleRect.width(), ltrWidth); // Width remains same
    // Verify rect spans both context categories correctly in mirrored position
    QVERIFY(rtlTitleRect.left() < ctxCat1->geometry().right());
    QVERIFY(rtlTitleRect.right() > ctxCat2->geometry().left());

    // Reset to LTR
    QApplication::setLayoutDirection(Qt::LeftToRight);
    ribbonBar.hide();
}

QTEST_MAIN(SARibbonContextCategoryRTLTest)

#include "SARibbonContextCategoryRTLTest.moc"
