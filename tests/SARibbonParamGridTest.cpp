#include <QtTest>
#include <QApplication>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QFontMetrics>
#include <QSignalSpy>
#include "SARibbonParamGrid.h"
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPanel.h"

namespace
{
// Combo box with a stable minimum width so the field column width is predictable
QComboBox* makeField(QWidget* parent, int minWidth = 90)
{
    QComboBox* c = new QComboBox(parent);
    c->setMinimumWidth(minWidth);
    c->addItem(QStringLiteral("item"));
    return c;
}

// Fill a grid with count rows, each row is "Row<i>" + combo
void addRows(SARibbonParamGrid& grid, int count)
{
    for (int i = 0; i < count; ++i) {
        grid.addRow(QStringLiteral("Row%1").arg(i), makeField(&grid));
    }
}

int expectedAutoLabelWidth(const QWidget& w, const QString& text)
{
    const int textW = QFontMetrics(w.font()).horizontalAdvance(text);
    return qBound(0, textW + 10, 200);
}
}  // namespace

class SARibbonParamGridTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void testAutoLabelColumnWidth();
    void testLabelSpacing();
    void testRowsDoNotStretchOutsidePanel();
    void testLabelColumnWidthOverride();
    void testLeadingAlignment();
    void testTrailingAlignment();
    void testBothSidesAlignment();
    void testBalancedFlow();
    void testColumnFirstFlow();
    void testSpanRowWidth();
    void testNoVerticalOverlap();
    void testTextChangeRecomputesWidth();
    void testHeightDerivedBudgetWithoutPanel();
    void testRowBudgetFollowsPanel();
};

void SARibbonParamGridTest::testAutoLabelColumnWidth()
{
    SARibbonParamGrid grid;
    QComboBox* field = makeField(&grid);
    const int rowId = grid.addRow(QStringLiteral("Channel"), field);

    QVERIFY(rowId >= 1);
    QCOMPARE(grid.rowCount(), 1);
    QCOMPARE(grid.labelColumnWidth(), expectedAutoLabelWidth(grid, QStringLiteral("Channel")));

    // Short labels must not be padded up to a wide fixed column: the automatic width stays tight
    SARibbonParamGrid shortGrid;
    shortGrid.addRow(QStringLiteral("AB"), makeField(&shortGrid));
    QCOMPARE(shortGrid.labelColumnWidth(), expectedAutoLabelWidth(shortGrid, QStringLiteral("AB")));
    QVERIFY(shortGrid.labelColumnWidth() < 60);
}

void SARibbonParamGridTest::testLabelSpacing()
{
    SARibbonParamGrid grid;
    grid.setMaxRows(3);
    QComboBox* field = makeField(&grid);
    grid.addRow(QStringLiteral("Channel"), field);
    grid.resize(400, 60);

    const int before = field->x();
    QCOMPARE(grid.labelSpacing(), 4);
    grid.setLabelSpacing(20);
    QCOMPARE(grid.labelSpacing(), 20);
    QCOMPARE(field->x(), before + 16);
}

void SARibbonParamGridTest::testRowsDoNotStretchOutsidePanel()
{
    SARibbonParamGrid grid;
    grid.setMaxRows(0);
    QComboBox* field = makeField(&grid);
    grid.addRow(QStringLiteral("Row"), field);
    // A very tall grid outside a ribbon panel keeps the natural row height instead of stretching the row
    grid.resize(400, 300);
    QVERIFY(field->height() <= field->sizeHint().height() + 1);
    QVERIFY(field->height() < 100);
}

void SARibbonParamGridTest::testLabelColumnWidthOverride()
{
    SARibbonParamGrid grid;
    QComboBox* field = makeField(&grid);
    grid.addRow(QStringLiteral("Channel"), field);

    grid.setLabelColumnWidth(150);
    QCOMPARE(grid.labelColumnWidth(), 150);

    grid.setLabelColumnWidth(-1);
    QCOMPARE(grid.labelColumnWidth(), expectedAutoLabelWidth(grid, QStringLiteral("Channel")));

    // A longer label widens the automatic column, the clamp keeps it inside the configured range
    grid.setLabelColumnWidthRange(40, 120);
    grid.setRowText(1, QStringLiteral("A very very very long label text"));
    QCOMPARE(grid.labelColumnWidth(), 120);
}

void SARibbonParamGridTest::testLeadingAlignment()
{
    SARibbonParamGrid grid;
    grid.setMaxRows(3);
    QComboBox* a = makeField(&grid);
    QComboBox* b = makeField(&grid);
    grid.addRow(QStringLiteral("Channel"), a);
    grid.addRow(QStringLiteral("Resolution"), b);
    grid.resize(600, 80);

    QCOMPARE(a->x(), b->x());
    QCOMPARE(grid.leadingLabel(1)->x(), grid.leadingLabel(2)->x());
    QCOMPARE(grid.leadingLabel(1)->width(), grid.labelColumnWidth());
    QVERIFY(a->x() > grid.leadingLabel(1)->x());
}

void SARibbonParamGridTest::testTrailingAlignment()
{
    SARibbonParamGrid grid;
    grid.setMaxRows(3);
    QDoubleSpinBox* spin = new QDoubleSpinBox(&grid);
    spin->setMinimumWidth(90);
    spin->setSuffix(QStringLiteral(" Hz"));
    grid.addRow(spin, SARibbonParamGrid::LabelTrailing, QString(), QStringLiteral("Hz"));
    grid.resize(400, 40);

    QCOMPARE(grid.labelColumnWidth(), 0);
    QCOMPARE(spin->x(), 0);
    QLabel* trailing = grid.trailingLabel(1);
    QVERIFY(trailing != nullptr);
    QVERIFY(!trailing->isHidden());
    QCOMPARE(trailing->x(), spin->x() + spin->width() + 4);
}

void SARibbonParamGridTest::testBothSidesAlignment()
{
    SARibbonParamGrid grid;
    grid.setMaxRows(3);
    QDoubleSpinBox* spin = new QDoubleSpinBox(&grid);
    spin->setMinimumWidth(90);
    grid.addRow(QStringLiteral("Window"), spin, QStringLiteral("ms"));
    grid.resize(400, 40);

    QVERIFY(grid.labelColumnWidth() > 0);
    QCOMPARE(spin->x(), grid.leadingLabel(1)->x() + grid.leadingLabel(1)->width() + 4);
    QCOMPARE(grid.trailingLabel(1)->x(), spin->x() + spin->width() + 4);
    // Leading and trailing columns are sized independently from their own texts
    QCOMPARE(grid.trailingLabel(1)->width(), expectedAutoLabelWidth(grid, QStringLiteral("ms")));
}

void SARibbonParamGridTest::testBalancedFlow()
{
    SARibbonParamGrid grid;
    grid.setMaxRows(3);
    addRows(grid, 4);
    grid.resize(800, 80);

    QCOMPARE(grid.currentColumns(), 2);
    QCOMPARE(grid.currentRows(), 2);

    QWidget* r1 = grid.fieldAt(1);
    QWidget* r2 = grid.fieldAt(2);
    QWidget* r3 = grid.fieldAt(3);
    QWidget* r4 = grid.fieldAt(4);
    QCOMPARE(r1->x(), r2->x());
    QCOMPARE(r3->x(), r4->x());
    QVERIFY(r1->x() != r3->x());
}

void SARibbonParamGridTest::testColumnFirstFlow()
{
    SARibbonParamGrid grid;
    grid.setMaxRows(3);
    grid.setFlowPolicy(SARibbonParamGrid::ColumnFirstFlow);
    addRows(grid, 4);
    grid.resize(800, 80);

    QCOMPARE(grid.currentColumns(), 2);
    QCOMPARE(grid.currentRows(), 3);
    QCOMPARE(grid.fieldAt(1)->x(), grid.fieldAt(2)->x());
    QCOMPARE(grid.fieldAt(2)->x(), grid.fieldAt(3)->x());
    QVERIFY(grid.fieldAt(4)->x() != grid.fieldAt(3)->x());
}

void SARibbonParamGridTest::testSpanRowWidth()
{
    SARibbonParamGrid grid;
    grid.setMaxRows(3);
    QLabel* hint = new QLabel(QStringLiteral("delta f = 12.5 Hz"), &grid);
    grid.addSpanRow(hint);
    grid.resize(400, 40);

    QCOMPARE(grid.currentColumns(), 1);
    QCOMPARE(hint->width(), grid.sizeHint().width());
}

void SARibbonParamGridTest::testNoVerticalOverlap()
{
    SARibbonParamGrid grid;
    grid.setMaxRows(2);
    addRows(grid, 4);
    grid.resize(800, 90);

    QCOMPARE(grid.currentColumns(), 2);
    QCOMPARE(grid.currentRows(), 2);
    QWidget* row1 = grid.fieldAt(1);
    QWidget* row2 = grid.fieldAt(2);
    QWidget* row3 = grid.fieldAt(3);
    QWidget* row4 = grid.fieldAt(4);
    QVERIFY(row1->y() + row1->height() <= row2->y());
    QVERIFY(row3->y() + row3->height() <= row4->y());
    QVERIFY(row1->height() > 0);
}

void SARibbonParamGridTest::testTextChangeRecomputesWidth()
{
    SARibbonParamGrid grid;
    grid.setMaxRows(3);
    const int rowId = grid.addRow(QStringLiteral("A"), makeField(&grid));
    const int before = grid.labelColumnWidth();
    grid.setRowText(rowId, QStringLiteral("Channel name"));
    const int after = grid.labelColumnWidth();
    QVERIFY(after >= before);
    QCOMPARE(after, expectedAutoLabelWidth(grid, QStringLiteral("Channel name")));
}

void SARibbonParamGridTest::testHeightDerivedBudgetWithoutPanel()
{
    SARibbonParamGrid grid;
    grid.setMaxRows(0);
    addRows(grid, 4);
    grid.resize(800, 300);
    QVERIFY(grid.currentColumns() >= 1);
    QVERIFY(grid.currentRows() >= 1);
}

void SARibbonParamGridTest::testRowBudgetFollowsPanel()
{
    SARibbonBar bar;
    SARibbonCategory* category = bar.addCategoryPage(QStringLiteral("Main"));
    QVERIFY(category != nullptr);
    SARibbonPanel* panel = category->addPanel(QStringLiteral("Spectrum"));
    QVERIFY(panel != nullptr);

    SARibbonParamGrid* grid = new SARibbonParamGrid(panel);
    for (int i = 0; i < 4; ++i) {
        grid->addRow(QStringLiteral("Row%1").arg(i), makeField(grid));
    }
    panel->addLargeWidget(grid);

    // Three-row mode: 4 rows spread over two blocks of two
    bar.setRibbonStyle(SARibbonBar::RibbonStyleLooseThreeRow);
    QCoreApplication::processEvents();
    QCOMPARE(grid->currentRows(), 2);
    QCOMPARE(grid->currentColumns(), 2);

    // Two-row mode: same 2 + 2 layout
    bar.setRibbonStyle(SARibbonBar::RibbonStyleLooseTwoRow);
    QCoreApplication::processEvents();
    QCOMPARE(grid->currentRows(), 2);
    QCOMPARE(grid->currentColumns(), 2);

    // Single-row mode: one row per block, so four blocks
    bar.setRibbonStyle(SARibbonBar::RibbonStyleLooseSingleRow);
    QCoreApplication::processEvents();
    QCOMPARE(grid->currentRows(), 1);
    QCOMPARE(grid->currentColumns(), 4);
}

QTEST_MAIN(SARibbonParamGridTest)

#include "SARibbonParamGridTest.moc"