#include <QtTest>
#include <QApplication>
#include "SARibbonGlobal.h"
#include "SARibbonUtil.h"
#include "SARibbonWidget.h"

/**
 * @brief 验证系统暗色模式自动切换开关 (SA::setEnableSystemDarkModeAutoSwitch) 的行为
 *
 * 覆盖点：
 * 1. 开关默认处于开启状态
 * 2. 设置/查询接口往返一致
 * 3. 关闭开关后，无论系统处于何种颜色模式，构造出的窗口保持默认主题 Office2021Blue
 * 4. 开启开关时，构造行为跟随系统颜色模式（暗色模式切换为 Dark，浅色保持默认）
 */
class SARibbonThemeAutoSwitchTest : public QObject
{
    Q_OBJECT

private slots:
    // 每个测试函数结束后恢复默认状态，避免测试间相互影响
    void cleanup();

    void testDefaultEnabled();
    void testSetterGetterRoundTrip();
    void testDisabledKeepsDefaultTheme();
    void testEnabledFollowsSystemDarkMode();
};

void SARibbonThemeAutoSwitchTest::cleanup()
{
    SA::setEnableSystemDarkModeAutoSwitch(true);
}

void SARibbonThemeAutoSwitchTest::testDefaultEnabled()
{
    // 自动开关默认开启，保持既有行为
    QVERIFY(SA::isEnableSystemDarkModeAutoSwitch());
}

void SARibbonThemeAutoSwitchTest::testSetterGetterRoundTrip()
{
    SA::setEnableSystemDarkModeAutoSwitch(false);
    QVERIFY(!SA::isEnableSystemDarkModeAutoSwitch());

    SA::setEnableSystemDarkModeAutoSwitch(true);
    QVERIFY(SA::isEnableSystemDarkModeAutoSwitch());
}

void SARibbonThemeAutoSwitchTest::testDisabledKeepsDefaultTheme()
{
    SA::setEnableSystemDarkModeAutoSwitch(false);
    // 关闭开关后构造，即使系统处于暗色模式，主题也应保持默认的 Office2021Blue
    SARibbonWidget w;
    QCOMPARE(w.ribbonTheme(), SARibbonTheme::RibbonThemeOffice2021Blue);
}

void SARibbonThemeAutoSwitchTest::testEnabledFollowsSystemDarkMode()
{
    SA::setEnableSystemDarkModeAutoSwitch(true);
    SARibbonWidget w;
    if (SA::isOperatingSystemInDarkMode()) {
        // 系统为暗色模式：默认主题被自动切换为 Dark
        QCOMPARE(w.ribbonTheme(), SARibbonTheme::RibbonThemeDark);
    } else {
        // 系统为浅色模式：默认主题保持不变
        QCOMPARE(w.ribbonTheme(), SARibbonTheme::RibbonThemeOffice2021Blue);
    }
}

QTEST_MAIN(SARibbonThemeAutoSwitchTest)

#include "SARibbonThemeAutoSwitchTest.moc"
