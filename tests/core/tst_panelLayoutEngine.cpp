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

class FakeCategoryItem : public SARibbon::Core::SARibbonAbstractCategoryItem
{
public:
    explicit FakeCategoryItem(int panelWidth) : mPanelHint(panelWidth, 90)
    {
    }
    QSize sizeHint() const override { return mPanelHint; }
    bool isHidden() const override { return mHidden; }
    Qt::Orientations expandingDirections() const override { return Qt::Orientations(); }
    void applyGeometry(const QRect& rect) override { mApplied = rect; }
    QString debugName() const override { return mName; }

    QSize mPanelHint;
    bool mHidden = false;
    QString mName;
    QRect mApplied;
};

// Mirrors SARibbonCategoryLayout::PrivateData::collectSizeHints() so the engine
// Input carries exactly what the widgets adapter would feed it (separator width 1,
// margins added once into totalWidth)
SARibbon::Core::SARibbonCategorySizeHints collectFakeCategoryHints(const QVector< SARibbon::Core::SARibbonAbstractCategoryItem* >& items,
                                                                   const QMargins& margins)
{
    SARibbon::Core::SARibbonCategorySizeHints hints;
    hints.panelSizes.resize(items.size());
    hints.separatorSizes.resize(items.size());
    if (!margins.isNull()) {
        hints.totalWidth += (margins.left() + margins.right());
    }
    for (int i = 0; i < items.size(); ++i) {
        if (items[ i ]->isHidden()) {
            continue;
        }
        hints.panelSizes[ i ]     = items[ i ]->sizeHint();
        hints.separatorSizes[ i ] = QSize(1, 90);
        hints.totalWidth += hints.panelSizes[ i ].width();
        hints.totalWidth += hints.separatorSizes[ i ].width();
    }
    return hints;
}

}  // namespace

class TestPanelLayoutEngine : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void threeRowMixed();
    void hiddenItemSkipped();
    void categoryScrollFlagsAndClamp();
    void categoryLayoutKeepsScrollBaseWhenScrolling();
    void categoryLayoutResetsScrollBaseWhenContentFits();
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

/**
 * @brief Regression: the scrolling branch must carry input.xBase through Result::newXBase
 * @details 2.x never wrote mXBase when scrolling was needed, and the placement origin was
 *          computed from the live value. After the move into the core engine the widgets
 *          adapter unconditionally writes r.newXBase back, so a default 0 turned every
 *          geometry recalculation (resize, arrow show/hide invalidation) into a silent
 *          scroll reset while the placed geometries stayed offset — the dev-3.0 middle-
 *          wheel overflow blank / cannot-scroll-back-to-left bug. newXBase and the
 *          placement origin must be the very same value.
 */
void TestPanelLayoutEngine::categoryLayoutKeepsScrollBaseWhenScrolling()
{
    using namespace SARibbon::Core;
    SARibbonCategoryLayoutEngine engine;

    QVector< SARibbonAbstractCategoryItem* > items;
    items << new FakeCategoryItem(120) << new FakeCategoryItem(120) << new FakeCategoryItem(120) << new FakeCategoryItem(120);

    SARibbonCategoryLayoutEngine::Input input;
    input.categoryWidth = 300;
    input.height        = 100;
    input.y             = 0;
    input.margins       = QMargins(1, 1, 1, 1);
    input.alignment     = SARibbonAlignment::AlignLeft;
    input.isRTL         = false;
    input.xBase         = -200;
    input.sizeHints     = collectFakeCategoryHints(items, input.margins);

    // total = 4 panels + 4 separators + margins = 484+2+2 > 300 -> needs scrolling
    QVERIFY(input.sizeHints.totalWidth > input.categoryWidth);

    auto r = engine.layout(items, input);
    // the scrolled base survives the layout pass (2.x: the scrolling branch never wrote it)
    QCOMPARE(r.newXBase, -200);
    // placement origin derives from the SAME base: first panel sits at xBase + margin.left()
    QCOMPARE(items[ 0 ]->resultGeometry.x(), -200 + 1);
    // moving the base shifts the placement, not the state
    input.xBase = -100;
    r           = engine.layout(items, input);
    QCOMPARE(r.newXBase, -100);
    QCOMPARE(items[ 0 ]->resultGeometry.x(), -100 + 1);

    qDeleteAll(items);
}

/**
 * @brief Regression: the non-scrolling branch resets the base to 0 BEFORE placement
 * @details 2.x resets mXBase inside the !needsScrolling branch and the placement origin
 *          is computed after it, so content that no longer overflows never stays visually
 *          stuck at the old offset. The reset and the placement origin must be the same
 *          value, otherwise state and placed geometries desynchronize.
 */
void TestPanelLayoutEngine::categoryLayoutResetsScrollBaseWhenContentFits()
{
    using namespace SARibbon::Core;
    SARibbonCategoryLayoutEngine engine;

    QVector< SARibbonAbstractCategoryItem* > items;
    items << new FakeCategoryItem(60) << new FakeCategoryItem(60);

    SARibbonCategoryLayoutEngine::Input input;
    input.categoryWidth = 400;
    input.height        = 100;
    input.y             = 0;
    input.margins       = QMargins(1, 1, 1, 1);
    input.alignment     = SARibbonAlignment::AlignLeft;
    input.isRTL         = false;
    input.xBase         = -200;  // stale offset from a previously wider content
    input.sizeHints     = collectFakeCategoryHints(items, input.margins);

    // total = 2 panels + 2 separators + margins = 124 < 400 -> fits
    QVERIFY(input.sizeHints.totalWidth <= input.categoryWidth);

    auto r = engine.layout(items, input);
    // the stale base is reset in the state...
    QCOMPARE(r.newXBase, 0);
    // ...and the placement origin derives from the reset value, not the stale one
    QCOMPARE(items[ 0 ]->resultGeometry.x(), 0 + 1);
    QVERIFY(!r.scrollFlags.showLeft && !r.scrollFlags.showRight);

    qDeleteAll(items);
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
