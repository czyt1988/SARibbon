#include <QtTest>
#include <QApplication>
#include <QSignalSpy>
#include "SARibbonMainWindow.h"

/**
 * @brief 无边框窗口系统阴影测试（GitHub #129）
 *
 * 可自动化的部分：属性默认值/往返/信号；开启阴影后窗口正常显示、不崩溃；
 * 钩子对未处理消息不吞（nativeEvent 对无关消息返回 false 走默认链）。
 * 阴影的视觉效果依赖 DWM 合成器与真实窗口管理器，属手动验证项。
 */
class SARibbonFrameShadowTest : public QObject
{
    Q_OBJECT
private slots:
    void testDefaultsAndSignals();
    void testEnableShadowWindowAlive();
};

void SARibbonFrameShadowTest::testDefaultsAndSignals()
{
    SARibbonMainWindow w;
    QVERIFY(!w.isFrameShadowEnabled());  // 默认关闭，不改变既有行为
    QSignalSpy spy(&w, &SARibbonMainWindow::frameShadowEnabledChanged);
    w.setFrameShadowEnabled(true);
    QVERIFY(w.isFrameShadowEnabled());
    QCOMPARE(spy.count(), 1);
    w.setFrameShadowEnabled(true);
    QCOMPARE(spy.count(), 1);  // 相同值不重复触发
    w.setFrameShadowEnabled(false);
    QVERIFY(!w.isFrameShadowEnabled());
    QCOMPARE(spy.count(), 2);
}

void SARibbonFrameShadowTest::testEnableShadowWindowAlive()
{
    SARibbonMainWindow w;
    w.resize(800, 600);
    w.show();
    QVERIFY(QTest::qWaitForWindowExposed(&w));
    QApplication::processEvents();

    // 运行时开关阴影：窗口不得崩溃、保持可见、几何不变
    const QRect geoBefore = w.geometry();
    w.setFrameShadowEnabled(true);
    QApplication::processEvents();
    QVERIFY(w.isVisible());
    w.setFrameShadowEnabled(false);
    QApplication::processEvents();
    QVERIFY(w.isVisible());
    QCOMPARE(w.geometry().size(), geoBefore.size());

    // 构造期开启（show 之前）：showEvent 路径应用
    SARibbonMainWindow w2;
    w2.setFrameShadowEnabled(true);
    w2.resize(600, 400);
    w2.show();
    QVERIFY(QTest::qWaitForWindowExposed(&w2));
    QApplication::processEvents();
    QVERIFY(w2.isVisible());
    QVERIFY(w2.isFrameShadowEnabled());
    // 最大化/还原不崩溃
    w2.showMaximized();
    QApplication::processEvents();
    w2.showNormal();
    QApplication::processEvents();
    QVERIFY(w2.isVisible());
    w2.hide();
    w.hide();
}

QTEST_MAIN(SARibbonFrameShadowTest)

#include "SARibbonFrameShadowTest.moc"
