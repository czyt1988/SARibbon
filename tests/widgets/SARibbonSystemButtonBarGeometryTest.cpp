#include <QtTest>
#include <QApplication>
#include "SARibbonSystemButtonBar.h"
#include "SARibbonMainWindow.h"

/**
 * @brief 系统按钮栏几何回归测试（防复发 #65 / #118 / #119）
 *
 * 只固化既有几何行为，不引入行为变更：
 * - 不同窗口宽度下按钮不越界、锚定边正确（LTR 右缘 / RTL 左缘）
 * - 极窄宽度下允许裁剪，但锚定边与相对顺序必须保持
 * - 最大化/还原后锚定边保持
 * - setButtonWidthStretch 的宽度比例生效
 */
namespace {
constexpr int kAnchorTolerance = 2;  // DPI 取整与像素度量带来的误差容差

bool isInBounds(const QAbstractButton* btn, int barWidth)
{
    return btn->geometry().left() >= 0 && btn->geometry().right() < barWidth;
}

// LTR：close 贴右缘，顺序 min.x < max.x < close.x
bool checkLTRAnchors(SARibbonSystemButtonBar& bar, QString* why)
{
    QAbstractButton* minBtn  = bar.minimizeButton();
    QAbstractButton* maxBtn  = bar.maximizeButton();
    QAbstractButton* closeBtn = bar.closeButton();
    if (!(minBtn && maxBtn && closeBtn)) {
        *why = "buttons missing";
        return false;
    }
    if (qAbs(closeBtn->geometry().right() - (bar.width() - 1)) > kAnchorTolerance) {
        *why = QString("close not anchored to right edge: right=%1, barWidth=%2")
                   .arg(closeBtn->geometry().right())
                   .arg(bar.width());
        return false;
    }
    if (!(minBtn->geometry().left() < maxBtn->geometry().left()
          && maxBtn->geometry().left() < closeBtn->geometry().left())) {
        *why = "button order broken (expect min.x < max.x < close.x)";
        return false;
    }
    return true;
}

// RTL：min 贴左缘，顺序按左缘仍是 min → max → close
bool checkRTLAnchors(SARibbonSystemButtonBar& bar, QString* why)
{
    QAbstractButton* minBtn   = bar.minimizeButton();
    QAbstractButton* maxBtn   = bar.maximizeButton();
    QAbstractButton* closeBtn = bar.closeButton();
    if (!(minBtn && maxBtn && closeBtn)) {
        *why = "buttons missing";
        return false;
    }
    if (qAbs(minBtn->geometry().left()) > kAnchorTolerance) {
        *why = QString("min not anchored to left edge: left=%1").arg(minBtn->geometry().left());
        return false;
    }
    if (!(minBtn->geometry().left() < maxBtn->geometry().left()
          && maxBtn->geometry().left() < closeBtn->geometry().left())) {
        *why = "button order broken (expect min.x < max.x < close.x)";
        return false;
    }
    return true;
}
}  // namespace

class SARibbonSystemButtonBarGeometryTest : public QObject
{
    Q_OBJECT
private slots:
    void testNormalWidthLTR();
    void testNarrowWidth();
    void testVeryNarrowWidth();
    void testRTLAnchoring();
    void testMaximizeRestore();
    void testButtonWidthStretch();
    void testScreenChangeEventRestoresGeometry();
};

void SARibbonSystemButtonBarGeometryTest::testNormalWidthLTR()
{
    SARibbonSystemButtonBar bar(nullptr);
    bar.resize(400, 30);
    bar.show();
    QApplication::processEvents();

    QString why;
    QVERIFY2(checkLTRAnchors(bar, &why), qPrintable(why));
    for (QAbstractButton* btn : { bar.minimizeButton(), bar.maximizeButton(), bar.closeButton() }) {
        QVERIFY2(isInBounds(btn, bar.width()), "button out of bounds at normal width");
        QVERIFY(btn->geometry().height() > 0);
    }
    bar.hide();
}

void SARibbonSystemButtonBarGeometryTest::testNarrowWidth()
{
    SARibbonSystemButtonBar bar(nullptr);
    bar.resize(160, 30);
    bar.show();
    QApplication::processEvents();

    QString why;
    QVERIFY2(checkLTRAnchors(bar, &why), qPrintable(why));
    for (QAbstractButton* btn : { bar.minimizeButton(), bar.maximizeButton(), bar.closeButton() }) {
        QVERIFY2(isInBounds(btn, bar.width()), "button out of bounds at narrow width");
    }
    bar.hide();
}

void SARibbonSystemButtonBarGeometryTest::testVeryNarrowWidth()
{
    // 60px 小于三按钮宽度之和（默认约 104px），裁剪/重叠是既有可接受行为；
    // 只断言不崩溃、锚定边正确、相对顺序不翻转
    SARibbonSystemButtonBar bar(nullptr);
    bar.resize(60, 30);
    bar.show();
    QApplication::processEvents();
    QVERIFY(bar.closeButton() != nullptr);

    QString why;
    QVERIFY2(checkLTRAnchors(bar, &why), qPrintable(why));

    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();
    QVERIFY2(checkRTLAnchors(bar, &why), qPrintable(why));
    QApplication::setLayoutDirection(Qt::LeftToRight);
    bar.hide();
}

void SARibbonSystemButtonBarGeometryTest::testRTLAnchoring()
{
    SARibbonSystemButtonBar bar(nullptr);
    bar.resize(400, 30);
    bar.show();
    QApplication::setLayoutDirection(Qt::RightToLeft);
    QApplication::processEvents();

    QString why;
    QVERIFY2(checkRTLAnchors(bar, &why), qPrintable(why));
    for (QAbstractButton* btn : { bar.minimizeButton(), bar.maximizeButton(), bar.closeButton() }) {
        QVERIFY2(isInBounds(btn, bar.width()), "button out of bounds in RTL");
    }

    QApplication::setLayoutDirection(Qt::LeftToRight);
    QApplication::processEvents();
    QVERIFY2(checkLTRAnchors(bar, &why), qPrintable(why));
    bar.hide();
}

void SARibbonSystemButtonBarGeometryTest::testMaximizeRestore()
{
    SARibbonMainWindow w;
    w.resize(800, 600);
    w.show();
    QVERIFY(QTest::qWaitForWindowExposed(&w));
    QApplication::processEvents();

    SARibbonSystemButtonBar* bar = w.windowButtonBar();
    QVERIFY(bar != nullptr);

    auto checkAnchored = [bar](const char* stage) {
        QString why;
        QVERIFY2(checkLTRAnchors(*bar, &why),
                 qPrintable(QString("%1: %2").arg(stage, why)));
        // 默认 stretch 4:3:3 → close 不小于 max/max，close 严格大于 min
        QVERIFY2(bar->closeButton()->width() >= bar->maximizeButton()->width(),
                 qPrintable(QString("%1: close width %2 < max width %3")
                                .arg(stage)
                                .arg(bar->closeButton()->width())
                                .arg(bar->maximizeButton()->width())));
        QVERIFY2(bar->closeButton()->width() > bar->minimizeButton()->width(),
                 qPrintable(QString("%1: close width %2 <= min width %3")
                                .arg(stage)
                                .arg(bar->closeButton()->width())
                                .arg(bar->minimizeButton()->width())));
    };

    checkAnchored("normal");

    w.showMaximized();
    QApplication::processEvents();
    checkAnchored("maximized");

    w.showNormal();
    QApplication::processEvents();
    checkAnchored("restored");

    w.hide();
}

void SARibbonSystemButtonBarGeometryTest::testButtonWidthStretch()
{
    SARibbonSystemButtonBar bar(nullptr);
    bar.resize(400, 30);
    bar.show();
    QApplication::processEvents();

    // 默认 4:3:3
    const int defClose = bar.closeButton()->width();
    const int defMax   = bar.maximizeButton()->width();
    const int defMin   = bar.minimizeButton()->width();
    QVERIFY(defClose > defMax);
    QCOMPARE(defMax, defMin);

    // 改为 4:5:6：setButtonWidthStretch 不触发重排，需通过 resize 事件重算
    bar.setButtonWidthStretch(4, 5, 6);
    bar.resize(402, 30);  // 尺寸变化触发 resizeEvent -> resizeElement
    QApplication::processEvents();

    const int newClose = bar.closeButton()->width();
    const int newMax   = bar.maximizeButton()->width();
    const int newMin   = bar.minimizeButton()->width();
    QVERIFY2(newMin > newMax, qPrintable(QString("expect min(%1) > max(%2) under 4:5:6").arg(newMin).arg(newMax)));
    QVERIFY2(newMax > newClose, qPrintable(QString("expect max(%1) > close(%2) under 4:5:6").arg(newMax).arg(newClose)));

    // 锚定边在比例变化后仍然正确
    QString why;
    QVERIFY2(checkLTRAnchors(bar, &why), qPrintable(why));
    bar.hide();
}

void SARibbonSystemButtonBarGeometryTest::testScreenChangeEventRestoresGeometry()
{
    // 复现 issue #118 的失效机制：屏幕/DPI 变化时窗口尺寸可能不变，
    // 系统按钮栏不会收到 resizeEvent；若事件分支不显式重算，被打乱的几何不会复位
    SARibbonMainWindow w;
    w.resize(800, 600);
    w.show();
    QVERIFY(QTest::qWaitForWindowExposed(&w));
    QApplication::processEvents();

    SARibbonSystemButtonBar* bar = w.windowButtonBar();
    QVERIFY(bar != nullptr);

    // 记录正确几何，然后手工打乱内部按钮位置（模拟跨屏后停留在旧位置）
    const QRect correctClose = bar->closeButton()->geometry();
    const QRect correctMin   = bar->minimizeButton()->geometry();
    bar->closeButton()->setGeometry(0, 0, 10, 10);
    bar->minimizeButton()->setGeometry(20, 0, 10, 10);
    QApplication::processEvents();

    // 直接触发屏幕变化事件（Qt 5.14+ 才有 ScreenChangeInternal）
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QEvent ev(QEvent::ScreenChangeInternal);
    QCoreApplication::sendEvent(&w, &ev);
    QApplication::processEvents();

    // 即使窗口尺寸未变，内部按钮几何也必须被复位
    QString why;
    QVERIFY2(checkLTRAnchors(*bar, &why), qPrintable(why));
    QCOMPARE(bar->closeButton()->geometry(), correctClose);
    QCOMPARE(bar->minimizeButton()->geometry(), correctMin);
#endif
    w.hide();
}

QTEST_MAIN(SARibbonSystemButtonBarGeometryTest)

#include "SARibbonSystemButtonBarGeometryTest.moc"
