#include <QtTest>
#include <QApplication>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonStackedWidget.h"
#include "SARibbonTabBar.h"

class SARibbonCategoryVisibilityTest : public QObject
{
    Q_OBJECT

private slots:
    void testSingleCategoryHideShow();
    void testMultipleCategoryHideShow();
    void testBatchSetCategoriesVisible();
    void testTabBarPresenceAfterVisibilityChange();
    void testAddCategoryAfterHidingCurrent();
    void testShowCategoryAfterHidingAll();
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

/*
 * Issue #160: 添加新 category 前，前面的 category 都被 hideCategory 隐藏，
 * 此时 stacked widget 必须切换到新添加的 category 页面
 */
void SARibbonCategoryVisibilityTest::testAddCategoryAfterHidingCurrent()
{
    SARibbonBar ribbonBar;
    ribbonBar.resize(800, 200);
    ribbonBar.show();
    QApplication::processEvents();

    const QStringList names = {"Main", "Other", "Delete", "Size", "Color"};
    QList<SARibbonCategory*> pages;
    for (const QString& name : names) {
        SARibbonCategory* c = ribbonBar.addCategoryPage(name);
        QVERIFY(c);
        pages.append(c);
        if (name != names.last()) {
            ribbonBar.hideCategory(c);
            QApplication::processEvents();
        }
    }
    QApplication::processEvents();

    QCOMPARE(ribbonBar.ribbonTabBar()->count(), 1);
    QCOMPARE(ribbonBar.ribbonTabBar()->currentIndex(), 0);
    QCOMPARE(ribbonBar.ribbonStackedWidget()->currentWidget(), pages.last());
}

/*
 * 所有 category 隐藏后（tabbar 为空），重新显示一个 category，
 * stacked widget 必须切换到该 category 页面
 */
void SARibbonCategoryVisibilityTest::testShowCategoryAfterHidingAll()
{
    SARibbonBar ribbonBar;
    ribbonBar.resize(800, 200);
    SARibbonCategory* cat1 = ribbonBar.addCategoryPage("Category 1");
    SARibbonCategory* cat2 = ribbonBar.addCategoryPage("Category 2");
    SARibbonCategory* cat3 = ribbonBar.addCategoryPage("Category 3");
    ribbonBar.show();
    QApplication::processEvents();

    ribbonBar.setCategoriesVisible(ribbonBar.categoryPages(), false);
    QApplication::processEvents();
    QCOMPARE(ribbonBar.ribbonTabBar()->count(), 0);

    // showCategory 显示单个分类
    ribbonBar.showCategory(cat3);
    QApplication::processEvents();
    QCOMPARE(ribbonBar.ribbonTabBar()->count(), 1);
    QCOMPARE(ribbonBar.ribbonStackedWidget()->currentWidget(), cat3);

    // 再次全部隐藏，用 setCategoriesVisible 批量显示
    ribbonBar.setCategoriesVisible(ribbonBar.categoryPages(), false);
    QApplication::processEvents();
    QCOMPARE(ribbonBar.ribbonTabBar()->count(), 0);

    ribbonBar.setCategoriesVisible({cat1, cat2}, true);
    QApplication::processEvents();
    QCOMPARE(ribbonBar.ribbonTabBar()->count(), 2);
    QCOMPARE(ribbonBar.ribbonStackedWidget()->currentWidget(), cat1);
}

QTEST_MAIN(SARibbonCategoryVisibilityTest)

#include "SARibbonCategoryVisibilityTest.moc"
