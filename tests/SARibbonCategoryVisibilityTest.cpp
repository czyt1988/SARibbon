#include <QtTest>
#include <QApplication>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonTabBar.h"

class SARibbonCategoryVisibilityTest : public QObject
{
    Q_OBJECT

private slots:
    void testSingleCategoryHideShow();
    void testMultipleCategoryHideShow();
    void testBatchSetCategoriesVisible();
    void testTabBarPresenceAfterVisibilityChange();
};

void SARibbonCategoryVisibilityTest::testSingleCategoryHideShow()
{
    SARibbonBar ribbonBar;
    ribbonBar.resize(800, 200);

    // Add 3 categories
    SARibbonCategory* cat1 = ribbonBar.addCategoryPage("Category 1");
    SARibbonCategory* cat2 = ribbonBar.addCategoryPage("Category 2");
    SARibbonCategory* cat3 = ribbonBar.addCategoryPage("Category 3");
    
    QVERIFY(cat1 != nullptr);
    QVERIFY(cat2 != nullptr);
    QVERIFY(cat3 != nullptr);
    
    // Force layout update
    ribbonBar.show();
    QApplication::processEvents();
    
    // Initial state: all visible
    QVERIFY(ribbonBar.isCategoryVisible(cat1));
    QVERIFY(ribbonBar.isCategoryVisible(cat2));
    QVERIFY(ribbonBar.isCategoryVisible(cat3));
    
    // Hide category 2
    ribbonBar.hideCategory(cat2);
    QApplication::processEvents();
    
    QVERIFY(ribbonBar.isCategoryVisible(cat1));
    QVERIFY(!ribbonBar.isCategoryVisible(cat2));
    QVERIFY(ribbonBar.isCategoryVisible(cat3));
    
    // Show category 2 again
    ribbonBar.showCategory(cat2);
    QApplication::processEvents();
    
    QVERIFY(ribbonBar.isCategoryVisible(cat1));
    QVERIFY(ribbonBar.isCategoryVisible(cat2));
    QVERIFY(ribbonBar.isCategoryVisible(cat3));
}

void SARibbonCategoryVisibilityTest::testMultipleCategoryHideShow()
{
    SARibbonBar ribbonBar;
    ribbonBar.resize(800, 200);

    // Add 4 categories
    SARibbonCategory* cat1 = ribbonBar.addCategoryPage("Category 1");
    SARibbonCategory* cat2 = ribbonBar.addCategoryPage("Category 2");
    SARibbonCategory* cat3 = ribbonBar.addCategoryPage("Category 3");
    SARibbonCategory* cat4 = ribbonBar.addCategoryPage("Category 4");
    
    // Force layout update
    ribbonBar.show();
    QApplication::processEvents();
    
    // Hide 2 categories
    ribbonBar.hideCategory(cat1);
    ribbonBar.hideCategory(cat3);
    QApplication::processEvents();
    
    QVERIFY(!ribbonBar.isCategoryVisible(cat1));
    QVERIFY(ribbonBar.isCategoryVisible(cat2));
    QVERIFY(!ribbonBar.isCategoryVisible(cat3));
    QVERIFY(ribbonBar.isCategoryVisible(cat4));
    
    // Hide another one
    ribbonBar.hideCategory(cat4);
    QApplication::processEvents();
    
    QVERIFY(!ribbonBar.isCategoryVisible(cat1));
    QVERIFY(ribbonBar.isCategoryVisible(cat2));
    QVERIFY(!ribbonBar.isCategoryVisible(cat3));
    QVERIFY(!ribbonBar.isCategoryVisible(cat4));
    
    // Show all hidden
    ribbonBar.showCategory(cat1);
    ribbonBar.showCategory(cat3);
    ribbonBar.showCategory(cat4);
    QApplication::processEvents();
    
    QVERIFY(ribbonBar.isCategoryVisible(cat1));
    QVERIFY(ribbonBar.isCategoryVisible(cat2));
    QVERIFY(ribbonBar.isCategoryVisible(cat3));
    QVERIFY(ribbonBar.isCategoryVisible(cat4));
}

void SARibbonCategoryVisibilityTest::testBatchSetCategoriesVisible()
{
    SARibbonBar ribbonBar;
    ribbonBar.resize(800, 200);

    // Add 4 categories
    SARibbonCategory* cat1 = ribbonBar.addCategoryPage("Category 1");
    SARibbonCategory* cat2 = ribbonBar.addCategoryPage("Category 2");
    SARibbonCategory* cat3 = ribbonBar.addCategoryPage("Category 3");
    SARibbonCategory* cat4 = ribbonBar.addCategoryPage("Category 4");
    
    QList<SARibbonCategory*> cats = {cat1, cat3, cat4};
    
    // Force layout update
    ribbonBar.show();
    QApplication::processEvents();
    
    // Batch hide 3 categories
    ribbonBar.setCategoriesVisible(cats, false);
    QApplication::processEvents();
    
    QVERIFY(!ribbonBar.isCategoryVisible(cat1));
    QVERIFY(ribbonBar.isCategoryVisible(cat2));
    QVERIFY(!ribbonBar.isCategoryVisible(cat3));
    QVERIFY(!ribbonBar.isCategoryVisible(cat4));
    
    // Batch show them again
    ribbonBar.setCategoriesVisible(cats, true);
    QApplication::processEvents();
    
    QVERIFY(ribbonBar.isCategoryVisible(cat1));
    QVERIFY(ribbonBar.isCategoryVisible(cat2));
    QVERIFY(ribbonBar.isCategoryVisible(cat3));
    QVERIFY(ribbonBar.isCategoryVisible(cat4));
    
    // Batch hide all
    QList<SARibbonCategory*> allCats = ribbonBar.categoryPages();
    ribbonBar.setCategoriesVisible(allCats, false);
    QApplication::processEvents();
    
    QVERIFY(!ribbonBar.isCategoryVisible(cat1));
    QVERIFY(!ribbonBar.isCategoryVisible(cat2));
    QVERIFY(!ribbonBar.isCategoryVisible(cat3));
    QVERIFY(!ribbonBar.isCategoryVisible(cat4));
    
    // Batch show all
    ribbonBar.setCategoriesVisible(allCats, true);
    QApplication::processEvents();
    
    QVERIFY(ribbonBar.isCategoryVisible(cat1));
    QVERIFY(ribbonBar.isCategoryVisible(cat2));
    QVERIFY(ribbonBar.isCategoryVisible(cat3));
    QVERIFY(ribbonBar.isCategoryVisible(cat4));
}

void SARibbonCategoryVisibilityTest::testTabBarPresenceAfterVisibilityChange()
{
    SARibbonBar ribbonBar;
    ribbonBar.resize(800, 200);
    SARibbonTabBar* tabBar = ribbonBar.ribbonTabBar();
    
    // Add 3 categories
    SARibbonCategory* cat1 = ribbonBar.addCategoryPage("Category 1");
    SARibbonCategory* cat2 = ribbonBar.addCategoryPage("Category 2");
    SARibbonCategory* cat3 = ribbonBar.addCategoryPage("Category 3");
    
    // Force layout update
    ribbonBar.show();
    QApplication::processEvents();
    
    // Initial tab count: 3
    QCOMPARE(tabBar->count(), 3);
    
    // Hide category 2
    ribbonBar.hideCategory(cat2);
    QApplication::processEvents();
    QCOMPARE(tabBar->count(), 2);
    // Verify remaining tabs are cat1 and cat3
    QCOMPARE(tabBar->tabText(0), "Category 1");
    QCOMPARE(tabBar->tabText(1), "Category 3");
    
    // Show category 2 again
    ribbonBar.showCategory(cat2);
    QApplication::processEvents();
    QCOMPARE(tabBar->count(), 3);
    QCOMPARE(tabBar->tabText(0), "Category 1");
    QCOMPARE(tabBar->tabText(1), "Category 2");
    QCOMPARE(tabBar->tabText(2), "Category 3");
    
    // Batch hide cat1 and cat3
    QList<SARibbonCategory*> cats = {cat1, cat3};
    ribbonBar.setCategoriesVisible(cats, false);
    QApplication::processEvents();
    QCOMPARE(tabBar->count(), 1);
    QCOMPARE(tabBar->tabText(0), "Category 2");
    
    // Batch show all
    QList<SARibbonCategory*> allCats = ribbonBar.categoryPages();
    ribbonBar.setCategoriesVisible(allCats, true);
    QApplication::processEvents();
    QCOMPARE(tabBar->count(), 3);
}

QTEST_MAIN(SARibbonCategoryVisibilityTest)

#include "SARibbonCategoryVisibilityTest.moc"
