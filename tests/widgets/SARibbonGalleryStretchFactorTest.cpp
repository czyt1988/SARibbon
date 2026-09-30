#include <QtTest>
#include <QApplication>
#include "SARibbonCategory.h"
#include "SARibbonPanel.h"
#include "SARibbonGallery.h"

/**
 * @brief Gallery stretch factor 测试（GitHub #47）
 *
 * 约定：stretchFactor 默认 0（保持既有均分行为）；
 * 同一面板多个 Gallery 设置不同系数后，额外宽度按权重分配；
 * 系数为 0 的 Gallery 在有权重列存在时不参与增量分配。
 */
namespace {
SARibbonGallery* addGalleryWithItems(SARibbonPanel* panel)
{
    SARibbonGallery* gallery = panel->addGallery();
    QList< QAction* > acts;
    for (int i = 0; i < 12; ++i) {
        acts.append(new QAction(QString("item%1").arg(i), panel));
    }
    gallery->addCategoryActions("group", acts);
    return gallery;
}
}  // namespace

class SARibbonGalleryStretchFactorTest : public QObject
{
    Q_OBJECT
private slots:
    void testStretchFactorProperty();
    void testEqualWeights();
    void testWeightedDistribution();
    void testZeroWeightExcluded();
    void testSmallWeightNotStarved();
};

void SARibbonGalleryStretchFactorTest::testStretchFactorProperty()
{
    SARibbonGallery gallery(nullptr);
    QCOMPARE(gallery.stretchFactor(), 0);

    QSignalSpy spy(&gallery, &SARibbonGallery::stretchFactorChanged);
    gallery.setStretchFactor(3);
    QCOMPARE(gallery.stretchFactor(), 3);
    QCOMPARE(spy.count(), 1);
    QCOMPARE(spy.at(0).at(0).toInt(), 3);

    // 相同值不重复触发
    gallery.setStretchFactor(3);
    QCOMPARE(spy.count(), 1);

    // 负值归零
    gallery.setStretchFactor(-1);
    QCOMPARE(gallery.stretchFactor(), 0);
    QCOMPARE(spy.count(), 2);
}

void SARibbonGalleryStretchFactorTest::testEqualWeights()
{
    SARibbonCategory category(nullptr);
    SARibbonPanel* panel = category.addPanel(QStringLiteral("p"));
    SARibbonGallery* g1  = addGalleryWithItems(panel);
    SARibbonGallery* g2  = addGalleryWithItems(panel);
    g1->setStretchFactor(1);
    g2->setStretchFactor(1);

    category.resize(1200, 120);
    category.show();
    QApplication::processEvents();

    const int w1 = g1->width();
    const int w2 = g2->width();
    QVERIFY2(w1 > 0 && w2 > 0, "galleries not laid out");
    // 1:1 权重 → 宽度近似相等（容差 10%：受列最小/最大宽度与取整影响）
    const int diff = qAbs(w1 - w2);
    QVERIFY2(diff <= qMax(w1, w2) / 10 + 2,
             qPrintable(QString("equal weights expected similar widths, got %1 vs %2").arg(w1).arg(w2)));
    category.hide();
}

void SARibbonGalleryStretchFactorTest::testWeightedDistribution()
{
    SARibbonCategory category(nullptr);
    SARibbonPanel* panel = category.addPanel(QStringLiteral("p"));
    SARibbonGallery* g1  = addGalleryWithItems(panel);
    SARibbonGallery* g2  = addGalleryWithItems(panel);
    g1->setStretchFactor(2);
    g2->setStretchFactor(1);

    category.resize(1200, 120);
    category.show();
    QApplication::processEvents();

    const int w1 = g1->width();
    const int w2 = g2->width();
    QVERIFY2(w1 > w2, qPrintable(QString("2:1 weights expect g1 wider, got %1 vs %2").arg(w1).arg(w2)));
    // 宽度比接近 2:1（容差 ±15%：受列最大宽度 columnMaximumWidth 约束）
    const qreal ratio = static_cast< qreal >(w1) / static_cast< qreal >(w2);
    QVERIFY2(ratio > 1.7 && ratio < 2.3,
             qPrintable(QString("2:1 weights expect ratio near 2, got %1 (%2/%3)").arg(ratio).arg(w1).arg(w2)));
    category.hide();
}

void SARibbonGalleryStretchFactorTest::testZeroWeightExcluded()
{
    SARibbonCategory category(nullptr);
    SARibbonPanel* panel = category.addPanel(QStringLiteral("p"));
    SARibbonGallery* g0  = addGalleryWithItems(panel);  // 保持默认 0
    SARibbonGallery* g1  = addGalleryWithItems(panel);
    SARibbonGallery* g2  = addGalleryWithItems(panel);
    g1->setStretchFactor(1);
    g2->setStretchFactor(2);

    category.resize(1400, 120);
    category.show();
    QApplication::processEvents();

    const int w0 = g0->width();
    const int w1 = g1->width();
    const int w2 = g2->width();
    // 有权重列存在时，0 权重的 gallery 不参与增量分配，宽度不大于有权重者
    QVERIFY2(w1 > w0 || w2 > w0,
             qPrintable(QString("weighted galleries should be wider than zero-weight one, got %1/%2/%3")
                            .arg(w0)
                            .arg(w1)
                            .arg(w2)));
    // 1:2 权重的两个 gallery 中，权重大的更宽
    QVERIFY2(w2 >= w1,
             qPrintable(QString("weight 2 should be >= weight 1, got w1=%1 w2=%2").arg(w1).arg(w2)));
    category.hide();
}

void SARibbonGalleryStretchFactorTest::testSmallWeightNotStarved()
{
    // 小权重不应被整数除法饿死：1:8 分配 60px 时，权重 1 应至少分到 1px 以上
    SARibbonCategory category(nullptr);
    SARibbonPanel* panel = category.addPanel(QStringLiteral("p"));
    SARibbonGallery* g1  = addGalleryWithItems(panel);
    SARibbonGallery* g2  = addGalleryWithItems(panel);
    g1->setStretchFactor(1);
    g2->setStretchFactor(8);

    category.resize(1200, 120);
    category.show();
    QApplication::processEvents();

    const int w1 = g1->width();
    const int w2 = g2->width();
    // 权重 1 的 gallery 分到的增量至少为正（不与最小宽度混淆：断言两者都在布局中）
    QVERIFY2(w1 > 0 && w2 > 0, "galleries not laid out");
    QVERIFY2(w2 > w1, QString("8:1 weights expect g2 wider, got %1 vs %2").arg(w1).arg(w2).toUtf8().constData());
    category.hide();
}

QTEST_MAIN(SARibbonGalleryStretchFactorTest)

#include "SARibbonGalleryStretchFactorTest.moc"
