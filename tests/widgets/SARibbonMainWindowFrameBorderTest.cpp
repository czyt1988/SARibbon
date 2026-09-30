#include <QtTest>
#include <QApplication>
#include <QImage>
#include <QSignalSpy>
#include "SARibbonMainWindow.h"

/**
 * @brief 主窗口边框属性测试（GitHub #154）
 *
 * 约定：frameBorderEnabled 默认 false（不改变既有渲染）；
 * 开启后边缘 1px 有可见边框；自定义颜色优先于主题色；属性往返与信号正确。
 */
namespace {
bool pixelIsRedish(const QRgb& rgb)
{
    return qRed(rgb) > 150 && qGreen(rgb) < 100 && qBlue(rgb) < 100;
}
}  // namespace

class SARibbonMainWindowFrameBorderTest : public QObject
{
    Q_OBJECT
private slots:
    void testDefaults();
    void testPropertyRoundTripAndSignals();
    void testDisabledByDefaultNoPixelChange();
    void testEnabledBorderVisible();
};

void SARibbonMainWindowFrameBorderTest::testDefaults()
{
    SARibbonMainWindow w;
    QVERIFY(!w.isFrameBorderEnabled());
    QVERIFY(!w.frameBorderColor().isValid());  // 无效色 = 跟随主题
}

void SARibbonMainWindowFrameBorderTest::testPropertyRoundTripAndSignals()
{
    SARibbonMainWindow w;
    QSignalSpy enabledSpy(&w, &SARibbonMainWindow::frameBorderEnabledChanged);
    QSignalSpy colorSpy(&w, &SARibbonMainWindow::frameBorderColorChanged);

    w.setFrameBorderEnabled(true);
    QVERIFY(w.isFrameBorderEnabled());
    QCOMPARE(enabledSpy.count(), 1);
    QCOMPARE(enabledSpy.at(0).at(0).toBool(), true);

    // 相同值不触发
    w.setFrameBorderEnabled(true);
    QCOMPARE(enabledSpy.count(), 1);

    w.setFrameBorderEnabled(false);
    QVERIFY(!w.isFrameBorderEnabled());
    QCOMPARE(enabledSpy.count(), 2);

    const QColor red(Qt::red);
    w.setFrameBorderColor(red);
    QCOMPARE(w.frameBorderColor(), red);
    QCOMPARE(colorSpy.count(), 1);

    w.setFrameBorderColor(red);
    QCOMPARE(colorSpy.count(), 1);

    w.setFrameBorderColor(QColor());  // 无效色 = 跟随主题
    QVERIFY(!w.frameBorderColor().isValid());
    QCOMPARE(colorSpy.count(), 2);
}

void SARibbonMainWindowFrameBorderTest::testDisabledByDefaultNoPixelChange()
{
    SARibbonMainWindow w1;
    w1.resize(400, 300);
    w1.show();
    QVERIFY(QTest::qWaitForWindowExposed(&w1));
    QApplication::processEvents();
    const QImage noBorder = w1.grab().toImage();

    // 显式关闭（与默认等价）后图像一致
    w1.setFrameBorderEnabled(false);
    QApplication::processEvents();
    const QImage stillNoBorder = w1.grab().toImage();
    QCOMPARE(noBorder, stillNoBorder);
    w1.hide();
}

void SARibbonMainWindowFrameBorderTest::testEnabledBorderVisible()
{
    SARibbonMainWindow w;
    w.resize(400, 300);
    w.show();
    QVERIFY(QTest::qWaitForWindowExposed(&w));
    QApplication::processEvents();

    const QImage noBorder = w.grab().toImage();

    // 自定义红色边框
    w.setFrameBorderColor(QColor(Qt::red));
    w.setFrameBorderEnabled(true);
    QApplication::processEvents();
    const QImage withBorder = w.grab().toImage();

    // 边框可见性：左右边（落在 contentsMargins(2,0,2,0) 的边距区）与底边未被遮挡；
    // 顶边被 menuWidget(SARibbonBar) 覆盖属几何事实，不断言
    auto redCountOnRow = [ &withBorder ](int y) {
        int cnt = 0;
        for (int x = 8; x < withBorder.width() - 8; ++x) {
            if (pixelIsRedish(withBorder.pixel(x, y))) {
                ++cnt;
            }
        }
        return cnt;
    };
    auto redCountOnColumn = [ &withBorder ](int x) {
        int cnt = 0;
        for (int y = 8; y < withBorder.height() - 8; ++y) {
            if (pixelIsRedish(withBorder.pixel(x, y))) {
                ++cnt;
            }
        }
        return cnt;
    };
    const int bottomEdgeRed = redCountOnRow(withBorder.height() - 1);
    const int leftEdgeRed   = redCountOnColumn(0);
    const int rightEdgeRed  = redCountOnColumn(withBorder.width() - 1);
    QVERIFY2(bottomEdgeRed >= 10, qPrintable(QString("bottom edge red pixels: %1").arg(bottomEdgeRed)));
    QVERIFY2(leftEdgeRed >= 10, qPrintable(QString("left edge red pixels: %1").arg(leftEdgeRed)));
    QVERIFY2(rightEdgeRed >= 10, qPrintable(QString("right edge red pixels: %1").arg(rightEdgeRed)));

    // 未开启边框时同位置没有红色像素
    int redPixelsBefore = 0;
    for (int x = 8; x < noBorder.width() - 8; ++x) {
        if (pixelIsRedish(noBorder.pixel(x, noBorder.height() - 1))) {
            ++redPixelsBefore;
        }
    }
    QVERIFY(redPixelsBefore < bottomEdgeRed);
    w.hide();
}

QTEST_MAIN(SARibbonMainWindowFrameBorderTest)

#include "SARibbonMainWindowFrameBorderTest.moc"
