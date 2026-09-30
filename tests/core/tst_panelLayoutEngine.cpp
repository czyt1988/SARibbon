#include <QtTest>
#include <QGuiApplication>
#include <SARibbonCore/SARibbonPanelLayoutEngine.h>
#include <SARibbonCore/SARibbonCategoryLayoutEngine.h>
#include <SARibbonCore/SARibbonBarGeometryEngine.h>

/**
 * @brief Engine-level golden tests with FakeItem (plan-02 S5.2-4 / S8)
 * @details Pure in-memory contract items with EXPLICIT sizeHint inputs — zero
 * font/platform dependency, deterministic on every platform. Locks the engine
 * input->output mapping (FakeItem values hand-fixed, derived from the recorded
 * widgets baseline scenario shape).
 */
namespace {

class FakePanelItem : public SARibbon::Core::SARibbonAbstractLayoutItem
{
public:
    FakePanelItem(const QSize& hint, SARibbon::Core::SARibbonRowProportion rp, Qt::Orientations exp = Qt::Orientations())
        : mHint(hint), mExp(exp)
    {
        // the engine reads the contract field, mirrors createItem in the widgets adapter
        rowProportion = rp;
    }
    QSize sizeHint() const override { return mHint; }
    bool isHidden() const override { return mHidden; }
    Qt::Orientations expandingDirections() const override { return mExp; }
    void applyGeometry(const QRect& rect) override { mApplied = rect; }
    QString debugName() const override { return mName; }

    QSize mHint;
    Qt::Orientations mExp;
    bool mHidden = false;
    QString mName;
    QRect mApplied;
};

}  // namespace

class TestPanelLayoutEngine : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void threeRowMixed();
    void hiddenItemSkipped();
    void categoryScrollFlagsAndClamp();
    void barTitleRectCompactLtr();
};

void TestPanelLayoutEngine::threeRowMixed()
{
    // Same shape as the widgets golden baseline scenario: Large, Small, Small, Medium, Small
    SARibbon::Core::SARibbonPanelLayoutEngine engine;
    SARibbon::Core::SARibbonPanelLayoutEngine::Input input;
    input.rowCount        = 3;
    input.showPanelTitle  = true;
    input.hasTitleLabel   = true;
    input.hasOptionAction = false;
    input.isRTL           = false;
    input.contentsMargins = QMargins(1, 1, 1, 1);
    input.spacing         = 2;
    input.titleTextWidth  = -1;
    input.titleHeight     = 15;
    input.titleSpace      = 2;

    QVector< SARibbon::Core::SARibbonAbstractLayoutItem* > items;
    items << new FakePanelItem(QSize(50, 100), SARibbon::Core::SARibbonRowProportion::Large);
    items << new FakePanelItem(QSize(50, 30), SARibbon::Core::SARibbonRowProportion::Small);
    items << new FakePanelItem(QSize(50, 30), SARibbon::Core::SARibbonRowProportion::Small);
    items << new FakePanelItem(QSize(50, 30), SARibbon::Core::SARibbonRowProportion::Medium);
    items << new FakePanelItem(QSize(50, 30), SARibbon::Core::SARibbonRowProportion::Small);

    auto r = engine.layout(items, QRect(0, 0, 500, 160), input);

    // Derived heights (formulas verbatim from 2.x): largeHeight = 160-1-1-15-2 = 141
    QCOMPARE(r.largeHeight, 141);
    // Box layout mirrors the widgets behavior: col0 = Large, col1 = Small/Small, col2 = Medium+Small
    QCOMPARE(items[ 0 ]->rowIndex, 0);
    QCOMPARE(items[ 0 ]->columnIndex, 0);
    QCOMPARE(items[ 0 ]->resultGeometry, QRect(1, 1, 50, 141));
    QCOMPARE(items[ 1 ]->rowIndex, 0);
    QCOMPARE(items[ 1 ]->columnIndex, 1);
    QCOMPARE(items[ 2 ]->rowIndex, 1);
    QCOMPARE(items[ 2 ]->columnIndex, 1);
    // Small pair occupies the same column: 2.x-derived heights
    const int smallHeight = qMax((141 - 2) / 3, 1);
    QCOMPARE(items[ 1 ]->resultGeometry.height(), smallHeight);
    QCOMPARE(items[ 3 ]->columnIndex, 2);
    QCOMPARE(r.columnCount, 3);
    QVERIFY(r.sizeHint.width() > 0);

    qDeleteAll(items);
}

void TestPanelLayoutEngine::hiddenItemSkipped()
{
    SARibbon::Core::SARibbonPanelLayoutEngine engine;
    SARibbon::Core::SARibbonPanelLayoutEngine::Input input;
    input.rowCount       = 3;
    input.contentsMargins = QMargins(0, 0, 0, 0);
    input.spacing        = 2;
    input.titleHeight    = 0;
    input.titleSpace     = 0;

    QVector< SARibbon::Core::SARibbonAbstractLayoutItem* > items;
    auto* visible = new FakePanelItem(QSize(40, 30), SARibbon::Core::SARibbonRowProportion::Small);
    auto* hidden  = new FakePanelItem(QSize(40, 30), SARibbon::Core::SARibbonRowProportion::Small);
    hidden->mHidden = true;
    items << visible << hidden;

    auto r = engine.layout(items, QRect(0, 0, 200, 60), input);
    // hidden item: rowIndex/columnIndex -1, geometry unchanged (default)
    QCOMPARE(hidden->rowIndex, -1);
    QCOMPARE(hidden->columnIndex, -1);
    QVERIFY(!hidden->resultGeometry.isValid());
    QCOMPARE(visible->rowIndex, 0);
    qDeleteAll(items);
}

void TestPanelLayoutEngine::categoryScrollFlagsAndClamp()
{
    using namespace SARibbon::Core;
    // Converged pure function (2.x dual implementation semantics):
    // no scrolling -> both off
    auto f = scrollButtonFlags(100, 200, 0, false);
    QVERIFY(!f.showLeft && !f.showRight);
    // LTR scrolling at start (xBase 0): right only
    f = scrollButtonFlags(300, 200, 0, false);
    QVERIFY(f.showRight && !f.showLeft);
    // LTR at end (xBase <= 200-300): left only
    f = scrollButtonFlags(300, 200, -100, false);
    QVERIFY(!f.showRight && f.showLeft);
    // LTR in between: both
    f = scrollButtonFlags(300, 200, -50, false);
    QVERIFY(f.showRight && f.showLeft);
    // RTL at start (0): left only
    f = scrollButtonFlags(300, 200, 0, true);
    QVERIFY(!f.showRight && f.showLeft);
    // RTL at end (>= 100): right only
    f = scrollButtonFlags(300, 200, 100, true);
    QVERIFY(f.showRight && !f.showLeft);

    // clamp (2.x semantics): LTR range [viewport-total, 0]; RTL [0, total-viewport]
    QCOMPARE(clampScrollOffset(-999, 300, 200, false), -100);
    QCOMPARE(clampScrollOffset(50, 300, 200, false), 0);
    QCOMPARE(clampScrollOffset(-999, 300, 200, true), 0);
    QCOMPARE(clampScrollOffset(999, 300, 200, true), 100);
    // no scrolling case: LTR collapses to 0
    QCOMPARE(clampScrollOffset(50, 100, 200, false), 0);
}

void TestPanelLayoutEngine::barTitleRectCompactLtr()
{
    SARibbon::Core::SARibbonBarGeometryEngine::TitleRectInput input;
    input.isRTL               = false;
    input.isCompactStyle      = true;
    input.ribbonWidth         = 400;
    input.border              = QMargins(1, 1, 1, 1);
    input.validTitleBarHeight = 30;
    input.tabBarGeometry      = QRect(10, 31, 200, 20);
    input.hasQuickAccessBar   = true;
    input.quickAccessBarGeometry = QRect(250, 1, 80, 28);
    input.systemButtonSize    = QSize(90, 28);
    input.hasContextTabs      = false;

    auto title = SARibbon::Core::SARibbonBarGeometryEngine::layoutTitleRect(input);
    // compact LTR: titleStart = tabbar right (209), width = qab.x() - start = 41
    QCOMPARE(title, QRect(209, 1, 41, 30));

    // too small -> empty
    input.quickAccessBarGeometry = QRect(215, 1, 80, 28);
    title = SARibbon::Core::SARibbonBarGeometryEngine::layoutTitleRect(input);
    QVERIFY(title.isNull());
}

QTEST_MAIN(TestPanelLayoutEngine)
#include "tst_panelLayoutEngine.moc"
