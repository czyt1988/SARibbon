#include <QtTest>
#include <QApplication>
#include <QFile>
#include <QTextStream>
#include <QWidget>
#include "SARibbonPanel.h"
#include "SARibbonPanelLayout.h"
#include "SARibbonPanelItem.h"
#include "SARibbonToolButton.h"

/**
 * @brief Panel geometry golden test (plan-02 S5.0)
 * @details Locks the SARibbonPanelLayout::updateGeomArray input->output mapping
 * as a text blob so the engine extraction (Step A/B) must keep it byte-identical.
 * The blob contains per-item inputs (sizeHint etc. as recorded) and outputs
 * (itemWillSetGeometry / rowIndex / columnIndex) plus panel-level results
 * (sizeHint / columnCount / largeHeight / title geometry / option button geometry).
 * Regenerate with SARIBBON_GOLDEN_REGEN=1 when recording a new baseline.
 */
namespace {

QString dumpPanelCase(const QString& name, SARibbonPanel* panel, const QSize& panelSize)
{
    SARibbonPanelLayout* lay = qobject_cast< SARibbonPanelLayout* >(panel->layout());
    QString out;
    QTextStream s(&out);
    panel->resize(panelSize);
    panel->show();
    QApplication::processEvents();
    lay->updateGeomArray();
    s << "=== case: " << name << " size=" << panelSize.width() << "x" << panelSize.height() << "\n";
    for (int i = 0; i < lay->count(); ++i) {
        SARibbonPanelItem* item = dynamic_cast< SARibbonPanelItem* >(lay->itemAt(i));
        if (!item) {
            continue;
        }
        s << "item[" << i << "] rp=" << int(item->rowProportion)
          << " hidden=" << (item->isEmpty() ? 1 : 0)
          << " hint=" << (item->widget() ? item->widget()->sizeHint().width() : 0)
          << "x" << (item->widget() ? item->widget()->sizeHint().height() : 0)
          << " maxW=" << (item->widget() ? item->widget()->maximumWidth() : 0)
          << " -> geom=" << item->itemWillSetGeometry.x() << "," << item->itemWillSetGeometry.y()
          << "," << item->itemWillSetGeometry.width() << "," << item->itemWillSetGeometry.height()
          << " row=" << item->rowIndex << " col=" << item->columnIndex
          << " expand=" << (item->isExpandItem ? 1 : 0) << "\n";
    }
    s << "panel sizeHint=" << lay->sizeHint().width() << "x" << lay->sizeHint().height() << "\n";
    return out;
}

SARibbonPanel* buildPanel(const QFont& font, SARibbonPanel::PanelLayoutMode mode)
{
    SARibbonPanel* panel = new SARibbonPanel();
    panel->setFont(font);
    panel->setPanelLayoutMode(mode);
    // Actions without icons: SARibbonToolButton sizeHint then depends on text+font only
    QAction* a1 = new QAction("AA");
    QAction* a2 = new QAction("BB");
    QAction* a3 = new QAction("CC");
    QAction* a4 = new QAction("DD");
    QAction* a5 = new QAction("EE");
    panel->addLargeAction(a1);
    panel->addSmallAction(a2);
    panel->addSmallAction(a3);
    panel->addMediumAction(a4);
    panel->addSmallAction(a5);
    return panel;
}

}  // namespace

class TestPanelLayoutGolden : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void goldenGeometry();
};

void TestPanelLayoutGolden::goldenGeometry()
{
    const QFont font("SimSun", 9);
    QApplication::setFont(font, "SARibbonToolButton");
    // plan-02 S5.0-2: the golden values are recorded on Windows + Qt6 (SimSun 9
    // via the system font database). Other platforms / Qt majors resolve different
    // fonts and metrics, so the blob comparison is meaningless there -- the
    // font-independent engine-level golden test (core_PanelLayoutEngine) is the
    // CI gate; this test is the recording/replay fidelity check on the recording
    // environment family. (QFontInfo cannot be used as the guard: it returns
    // localized family names, e.g. SimSun resolves to the localized name.)
#if !defined(Q_OS_WIN) || (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    QSKIP("widget-level golden blob is recorded on Windows/Qt6; see core_PanelLayoutEngine for the platform-independent golden gate");
#endif

    QString blob;
    {
        QTextStream s(&blob);
        s << "font=" << font.family() << " " << font.pointSize() << "\n";
    }
    // three-row / two-row / single-row, three panel sizes each
    const QVector< QPair< QString, SARibbonPanel::PanelLayoutMode > > modes = {
        { "threerow", SARibbonPanel::ThreeRowMode },
        { "tworow", SARibbonPanel::TwoRowMode },
        { "singlerow", SARibbonPanel::SingleRowMode },
    };
    for (const auto& m : modes) {
        for (const QSize& sz : { QSize(200, 120), QSize(500, 160), QSize(80, 90) }) {
            SARibbonPanel* panel = buildPanel(font, m.second);
            blob += dumpPanelCase(m.first, panel, sz);
            delete panel;
        }
    }

    const QString goldenPath = QStringLiteral(QT_TESTCASE_SOURCEDIR) + "/golden_panel_geometry.txt";
    if (qEnvironmentVariableIsSet("SARIBBON_GOLDEN_REGEN")) {
        QFile f(goldenPath);
        QVERIFY(f.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text));
        f.write(blob.toUtf8());
        f.close();
    }
    QFile f(goldenPath);
    QVERIFY2(f.open(QIODevice::ReadOnly | QIODevice::Text),
             qPrintable("golden file missing: " + goldenPath + " (run with SARIBBON_GOLDEN_REGEN=1)"));
    const QString expected = QString::fromUtf8(f.readAll());
    QCOMPARE(blob.trimmed(), expected.trimmed());
}

QTEST_MAIN(TestPanelLayoutGolden)
#include "tst_panelLayoutGolden.moc"
