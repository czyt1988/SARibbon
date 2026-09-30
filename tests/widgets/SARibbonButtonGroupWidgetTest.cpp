#include <QtTest>
#include <QApplication>
#include <QComboBox>
#include <QWidgetAction>
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonQuickAccessBar.h"

namespace {
// 返回 actions() 顺序下每个 action 对应 widget 的 x 坐标；widget 缺失记 -1
QList<int> widgetXsInActionOrder(QToolBar* bar)
{
    QList<int> xs;
    const QList<QAction*> acts = bar->actions();
    for (QAction* a : acts) {
        if (QWidget* w = bar->widgetForAction(a)) {
            xs.append(w->x());
        } else {
            xs.append(-1);
        }
    }
    return xs;
}

bool isStrictlyIncreasing(const QList<int>& xs)
{
    for (int i = 1; i < xs.count(); ++i) {
        if (xs.at(i) <= xs.at(i - 1)) {
            return false;
        }
    }
    return true;
}

// 保证按钮不会被收进扩展按钮的前置条件
void ensureWideEnough(QToolBar* bar)
{
    QVERIFY(bar->width() >= bar->sizeHint().width());
}
}  // namespace

class SARibbonButtonGroupWidgetTest : public QObject
{
    Q_OBJECT
private slots:
    void testAppendOrder();
    void testPrependOrder();
    void testInsertMiddleOrder();
    void testRemoveKeepsOrder();
    void testQuickAccessBarOrder();
    void testMoveKeepsCustomWidgetState();
};

void SARibbonButtonGroupWidgetTest::testAppendOrder()
{
    SARibbonButtonGroupWidget bar;
    bar.resize(800, 30);
    QAction* a1 = bar.addAction("A1");
    QAction* a2 = bar.addAction("A2");
    QAction* a3 = bar.addAction("A3");
    bar.show();
    QApplication::processEvents();

    ensureWideEnough(&bar);
    QCOMPARE(bar.actions(), QList<QAction*>({ a1, a2, a3 }));
    const QList<int> xs = widgetXsInActionOrder(&bar);
    QVERIFY(xs.at(0) < xs.at(1));
    QVERIFY(xs.at(1) < xs.at(2));
    bar.hide();
}

void SARibbonButtonGroupWidgetTest::testPrependOrder()
{
    SARibbonButtonGroupWidget bar;
    bar.resize(800, 30);
    QAction* a1 = bar.addAction("A1");
    QAction* a2 = bar.addAction("A2");
    QAction* a3 = bar.addAction("A3");
    QAction* a0 = new QAction("A0", &bar);
    bar.insertAction(a1, a0);  // 插到 A1 之前 → 头插
    bar.show();
    QApplication::processEvents();

    ensureWideEnough(&bar);
    QCOMPARE(bar.actions(), QList<QAction*>({ a0, a1, a2, a3 }));
    const QList<int> xs = widgetXsInActionOrder(&bar);
    QVERIFY(isStrictlyIncreasing(xs));
    bar.hide();
}

void SARibbonButtonGroupWidgetTest::testInsertMiddleOrder()
{
    SARibbonButtonGroupWidget bar;
    bar.resize(800, 30);
    QAction* a0  = bar.addAction("A0");
    QAction* a1  = bar.addAction("A1");
    QAction* a2  = bar.addAction("A2");
    QAction* a3  = bar.addAction("A3");
    QAction* a15 = new QAction("A15", &bar);
    bar.insertAction(a3, a15);  // 插到 A3 之前 → 中间插
    bar.show();
    QApplication::processEvents();

    ensureWideEnough(&bar);
    QCOMPARE(bar.actions(), QList<QAction*>({ a0, a1, a2, a15, a3 }));
    const QList<int> xs = widgetXsInActionOrder(&bar);
    QVERIFY(isStrictlyIncreasing(xs));
    bar.hide();
}

void SARibbonButtonGroupWidgetTest::testRemoveKeepsOrder()
{
    SARibbonButtonGroupWidget bar;
    bar.resize(800, 30);
    QAction* a0  = bar.addAction("A0");
    QAction* a1  = bar.addAction("A1");
    QAction* a2  = bar.addAction("A2");
    QAction* a3  = bar.addAction("A3");
    QAction* a15 = new QAction("A15", &bar);
    bar.insertAction(a3, a15);
    bar.show();
    QApplication::processEvents();

    bar.removeAction(a1);
    QApplication::processEvents();

    ensureWideEnough(&bar);
    QCOMPARE(bar.actions(), QList<QAction*>({ a0, a2, a15, a3 }));
    const QList<int> xs = widgetXsInActionOrder(&bar);
    QVERIFY(isStrictlyIncreasing(xs));
    bar.hide();
}

void SARibbonButtonGroupWidgetTest::testQuickAccessBarOrder()
{
    // SARibbonQuickAccessBar 继承 SARibbonButtonGroupWidget，插入语义应与基类一致
    SARibbonQuickAccessBar bar;
    bar.resize(800, 30);
    QAction* a1 = bar.addAction("A1");
    QAction* a2 = bar.addAction("A2");
    QAction* a3 = bar.addAction("A3");
    QAction* a0 = new QAction("A0", &bar);
    bar.insertAction(a1, a0);  // 头插
    QAction* a15 = new QAction("A15", &bar);
    bar.insertAction(a3, a15);  // 中间插
    bar.show();
    QApplication::processEvents();

    ensureWideEnough(&bar);
    QCOMPARE(bar.actions(), QList<QAction*>({ a0, a1, a2, a15, a3 }));

    bar.removeAction(a2);
    QApplication::processEvents();
    QCOMPARE(bar.actions(), QList<QAction*>({ a0, a1, a15, a3 }));
    const QList<int> xs = widgetXsInActionOrder(&bar);
    QVERIFY(isStrictlyIncreasing(xs));
    bar.hide();
}

void SARibbonButtonGroupWidgetTest::testMoveKeepsCustomWidgetState()
{
    // GitHub #60 API 部分结论验证：move 用 removeAction+insertAction 组合表达时，
    // QWidgetAction 承载的自定义控件状态是否保留（QToolBar 移除时会销毁自动创建的
    // 按钮，再插入会重建；QWidgetAction 的 requestWidget/releaseWidget 应保住 widget 本体）
    SARibbonButtonGroupWidget bar;
    bar.resize(900, 30);
    QAction* a1 = bar.addAction("A1");

    QComboBox* combo    = new QComboBox(&bar);
    combo->setObjectName("myCombo");
    combo->addItems({ "x", "y", "z" });
    combo->setCurrentIndex(2);  // 用户状态
    QWidgetAction* wa = new QWidgetAction(&bar);
    wa->setDefaultWidget(combo);
    bar.addAction(wa);

    QAction* a3 = bar.addAction("A3");
    bar.show();
    QApplication::processEvents();
    ensureWideEnough(&bar);
    QCOMPARE(bar.actions(), QList< QAction* >({ a1, wa, a3 }));

    // move：把 wa 移到 a3 之后（remove + append）
    bar.removeAction(wa);
    bar.addAction(wa);
    QApplication::processEvents();

    QCOMPARE(bar.actions(), QList< QAction* >({ a1, a3, wa }));
    // 自定义 widget 本体保留且状态不丢
    QWidget* w = bar.widgetForAction(wa);
    QVERIFY2(w != nullptr, "custom widget missing after move");
    QCOMPARE(w->objectName(), QStringLiteral("myCombo"));
    QComboBox* moved = qobject_cast< QComboBox* >(w);
    QVERIFY(moved != nullptr);
    QCOMPARE(moved->currentIndex(), 2);
    bar.hide();
}

QTEST_MAIN(SARibbonButtonGroupWidgetTest)

#include "SARibbonButtonGroupWidgetTest.moc"
