#include <QtTest>
#include <QApplication>
#include <QAction>
#include <QFontMetrics>
#include <QLayoutItem>
#include "SARibbonPanel.h"
#include "SARibbonPanelLayout.h"
#include "SARibbonToolButton.h"

/**
 * @brief 大按钮宽高比（buttonMaximumAspectRatio）生效时机测试
 *
 * 约定：大按钮的宽度上限是「panel 的大按钮高度 × buttonMaximumAspectRatio」，
 * 单行文字超过这个上限时必须换成两行显示。
 *
 * 该结论必须在 panel 拿到真实几何之后依然成立，即使 sizeHint 在 panel 还是任意尺寸时
 * 就已经被算出来并缓存过。典型场景（MainWindowExample 的 Other 标签页就是这样）：
 * category / panel 先以无父对象的方式创建（顶层窗口默认 640x480）并填充完 action，
 * 之后才 addPanel / addCategoryPage 进 SARibbonBar。此时按 460 多的高度算出来的
 * 「不用换行 + 单行全宽」结论不能一直沿用下去。
 */
namespace
{
/// 长文本：单行宽度远大于「大按钮高度 * 1.4」，必须触发换行
const char* kLongText1 = "remove application button";
const char* kLongText2 = "show very long text in a button,balabalabala etc";

/// 让 panel 以指定尺寸完成一次真实布局，模拟 category 给 panel 的几何
void relayoutPanel(SARibbonPanel* panel, int width, int height)
{
    panel->resize(width, height);
    panel->layout()->setGeometry(QRect(0, 0, width, height));
}

/// 遍历 panel 内的大按钮
QList< SARibbonToolButton* > largeButtonsOf(SARibbonPanel* panel)
{
    QList< SARibbonToolButton* > buttons;
    SARibbonPanelLayout* lay = panel->panelLayout();
    for (int i = 0; lay && (i < lay->count()); ++i) {
        QLayoutItem* item = lay->itemAt(i);
        if (SARibbonToolButton* btn = qobject_cast< SARibbonToolButton* >(item ? item->widget() : nullptr)) {
            if (btn->isLargeRibbonButton()) {
                buttons << btn;
            }
        }
    }
    return buttons;
}

/// 给 panel 填两个长文本大按钮
void fillLongTextButtons(SARibbonPanel* panel)
{
    panel->addLargeAction(new QAction(QLatin1String(kLongText1), panel));
    panel->addLargeAction(new QAction(QLatin1String(kLongText2), panel));
}
}  // namespace

class SARibbonLargeButtonAspectRatioTest : public QObject
{
    Q_OBJECT
private:
    /// 校验所有大按钮都按 panel 当前高度重新算过宽高比
    static void verifyAspectRatioApplied(SARibbonPanel* panel, const char* where)
    {
        const int largeHeight = panel->largeButtonHeight();
        QVERIFY2(largeHeight > 10, qPrintable(QStringLiteral("%1: panel not laid out").arg(where)));
        const QList< SARibbonToolButton* > buttons = largeButtonsOf(panel);
        QCOMPARE(buttons.size(), 2);
        for (SARibbonToolButton* btn : buttons) {
            const QFontMetrics fm(btn->font());
            const int ratioCap        = qRound(largeHeight * btn->buttonMaximumAspectRatio());
            const int singleLineWidth = fm.size(Qt::TextShowMnemonic, btn->text()).width();
            // sizeHint 必须是按当前 panel 高度算出来的，不能沿用 panel 还是任意尺寸时的旧值
            const int heightLimit = qMax(largeHeight, qRound(fm.lineSpacing() * 2.2)) + 2;
            QVERIFY2(btn->sizeHint().height() <= heightLimit,
                     qPrintable(QStringLiteral("%1: \"%2\" sizeHint height=%3 stale (panel large height=%4)")
                                    .arg(where)
                                    .arg(btn->text())
                                    .arg(btn->sizeHint().height())
                                    .arg(largeHeight)));
            if (singleLineWidth <= ratioCap) {
                // 单行本来就放得进宽高比上限，不要求换行
                continue;
            }
            // 单行文字宽度超过了宽高比上限，换行必须生效：布局宽度小于单行完整文字宽度
            QVERIFY2(btn->width() < singleLineWidth,
                     qPrintable(QStringLiteral("%1: \"%2\" width=%3 not wrapped (single line needs %4, cap=%5)")
                                    .arg(where)
                                    .arg(btn->text())
                                    .arg(btn->width())
                                    .arg(singleLineWidth)
                                    .arg(ratioCap)));
        }
    }

private Q_SLOTS:
    /// panel 先以顶层窗口尺寸算过 sizeHint，拿到真实几何后必须按真实高度重算
    void testStaleHintNotReusedAfterPanelGetsRealGeometry();
    /// 之后每次高度变化都要重新按新高度生效
    void testHintRecomputedOnEveryHeightChange();
};

void SARibbonLargeButtonAspectRatioTest::testStaleHintNotReusedAfterPanelGetsRealGeometry()
{
    // panel 无父对象 → 顶层窗口默认 640x480，此时大按钮高度是 460 多
    SARibbonPanel panel(QStringLiteral("panel two"));
    fillLongTextButtons(&panel);
    panel.layout()->invalidate();
    panel.layout()->sizeHint();  // 在错误高度下算出 sizeHint 并被按钮与 panel 布局两层缓存
    QVERIFY2(panel.largeButtonHeight() > 200,
             "fixture broken: panel should still have the bogus top-level large button height");

    // panel 被放进 ribbon 后拿到真实几何
    relayoutPanel(&panel, 400, 100);
    verifyAspectRatioApplied(&panel, "after real geometry");
}

void SARibbonLargeButtonAspectRatioTest::testHintRecomputedOnEveryHeightChange()
{
    SARibbonPanel panel(QStringLiteral("panel two"));
    fillLongTextButtons(&panel);

    relayoutPanel(&panel, 400, 100);
    verifyAspectRatioApplied(&panel, "height=100");

    relayoutPanel(&panel, 400, 140);
    verifyAspectRatioApplied(&panel, "height=140");

    relayoutPanel(&panel, 400, 80);
    verifyAspectRatioApplied(&panel, "height=80");
}

QTEST_MAIN(SARibbonLargeButtonAspectRatioTest)
#include "SARibbonLargeButtonAspectRatioTest.moc"
