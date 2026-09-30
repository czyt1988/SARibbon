#include <QtTest>
#include <QApplication>
#include "SARibbonMainWindow.h"

/**
 * @brief 标题栏可拖拽区命中测试（GitHub #31）
 *
 * SA::isTitleBarDragArea 是 nativeEvent 中 WM_NCHITTEST -> HTCAPTION 判定的纯函数，
 * 表驱动覆盖：正常命中、区域外、最大化/全屏、排除区（系统按钮/QA栏/tab栏）。
 */
class SARibbonTitleBarHitTestTest : public QObject
{
    Q_OBJECT
private slots:
    void testNormalTitleBar_data();
    void testNormalTitleBar();
    void testOutsideTitleBar();
    void testMaximizedAndFullscreen();
    void testExcludedRects();
    void testRtlExcludedRects_data();
    void testRtlExcludedRects();
};

void SARibbonTitleBarHitTestTest::testNormalTitleBar_data()
{
    QTest::addColumn< QPoint >("localPos");
    QTest::addColumn< bool >("expected");

    const QRect win(0, 0, 800, 600);
    Q_UNUSED(win)
    // titleHeight = 30
    QTest::newRow("left edge of titlebar") << QPoint(5, 10) << true;
    QTest::newRow("center of titlebar") << QPoint(400, 15) << true;
    QTest::newRow("right edge inside") << QPoint(795, 25) << true;
    QTest::newRow("just below titlebar") << QPoint(400, 35) << false;
    QTest::newRow("negative y") << QPoint(400, -5) << false;
}

void SARibbonTitleBarHitTestTest::testNormalTitleBar()
{
    QFETCH(QPoint, localPos);
    QFETCH(bool, expected);
    const QRect windowRect(0, 0, 800, 600);
    // 正常窗口、无排除区
    QCOMPARE(SA::isTitleBarDragArea(localPos, windowRect, 30, {}, false), expected);
}

void SARibbonTitleBarHitTestTest::testOutsideTitleBar()
{
    const QRect windowRect(0, 0, 800, 600);
    // 窗口外
    QVERIFY(!SA::isTitleBarDragArea(QPoint(900, 10), windowRect, 30, {}, false));
    // 标题栏高度为 0（异常防御）
    QVERIFY(!SA::isTitleBarDragArea(QPoint(400, 10), windowRect, 0, {}, false));
    // 标题栏高度为负
    QVERIFY(!SA::isTitleBarDragArea(QPoint(400, 10), windowRect, -1, {}, false));
}

void SARibbonTitleBarHitTestTest::testMaximizedAndFullscreen()
{
    const QRect windowRect(0, 0, 800, 600);
    // 最大化/全屏时标题栏不返回 HTCAPTION，避免"最大化状态下拖动窗口"的怪异行为
    QVERIFY(!SA::isTitleBarDragArea(QPoint(400, 15), windowRect, 30, {}, true));
    QVERIFY(SA::isTitleBarDragArea(QPoint(400, 15), windowRect, 30, {}, false));
}

void SARibbonTitleBarHitTestTest::testExcludedRects()
{
    const QRect windowRect(0, 0, 800, 600);
    // 模拟系统按钮区（右上角）与 tab 栏区（标题栏下方）
    const QList< QRect > excluded = { QRect(650, 0, 150, 30),  // 系统按钮
                                      QRect(300, 5, 200, 25) };  // 假设的 tab 区域
    // 命中系统按钮区 → 不拖拽（按钮可点击）
    QVERIFY(!SA::isTitleBarDragArea(QPoint(700, 15), windowRect, 30, excluded, false));
    // 命中 tab 区 → 不拖拽
    QVERIFY(!SA::isTitleBarDragArea(QPoint(400, 15), windowRect, 30, excluded, false));
    // 标题栏其他位置 → 拖拽
    QVERIFY(SA::isTitleBarDragArea(QPoint(200, 15), windowRect, 30, excluded, false));
    // 无效矩形（0 尺寸）不参与排除
    const QList< QRect > invalidExcluded = { QRect() };
    QVERIFY(SA::isTitleBarDragArea(QPoint(400, 15), windowRect, 30, invalidExcluded, false));
}

void SARibbonTitleBarHitTestTest::testRtlExcludedRects_data()
{
    // RTL 下系统按钮位于左上角，排除矩形在左侧，判定逻辑相同（纯函数按传入矩形计算）
    QTest::addColumn< QPoint >("localPos");
    QTest::addColumn< bool >("expected");
    QTest::newRow("rtl system buttons area") << QPoint(30, 15) << false;
    QTest::newRow("rtl other titlebar area") << QPoint(500, 15) << true;
}

void SARibbonTitleBarHitTestTest::testRtlExcludedRects()
{
    QFETCH(QPoint, localPos);
    QFETCH(bool, expected);
    const QRect windowRect(0, 0, 800, 600);
    const QList< QRect > excludedRTL = { QRect(0, 0, 150, 30) };  // RTL：系统按钮在左上角
    QCOMPARE(SA::isTitleBarDragArea(localPos, windowRect, 30, excludedRTL, false), expected);
}

QTEST_MAIN(SARibbonTitleBarHitTestTest)

#include "SARibbonTitleBarHitTestTest.moc"
