#include <QtTest>
#include <QAction>
#include <QFontMetrics>
#include <QLayout>
#include <QLayoutItem>
#include "SARibbonPanel.h"
#include "SARibbonPanelLayout.h"
#include "SARibbonToolButton.h"

/**
 * @brief 大按钮换行宽度估算的回归测试（issue #163）
 *
 * 换行模式下，按钮宽度由 SARibbonToolButton::PrivateData::estimateLargeButtonTextWidth() 用二分查找
 * 估算，目标是「换行后不超过两行」的最小宽度。二分的起点是单行宽度的一半，因此如果只判断行数，
 * 搜索会接受比最长单词还窄的宽度：Qt::TextWordWrap 不在单词内部断行，超宽单词所在行虽然只占
 * 一行高度，但实际宽度超出了试探宽度，最终该单词被截断。
 *
 * 这里的断言都换算到「绘制文字的可用宽度」（sizeHint 宽度减去左右 spacing），避免依赖具体字体：
 * - 换行必须仍然生效（可用宽度小于单行文字宽度）；
 * - 可用宽度不能小于最长单词宽度，否则单词会被截断。
 */
namespace
{
/// 让 panel 以指定尺寸完成一次真实布局
void relayoutPanel(SARibbonPanel* panel, int width, int height)
{
    panel->resize(width, height);
    panel->layout()->setGeometry(QRect(0, 0, width, height));
}

/// 取 panel 内的第一个大按钮
SARibbonToolButton* firstLargeButton(SARibbonPanel* panel)
{
    SARibbonPanelLayout* lay = panel->panelLayout();
    for (int i = 0; lay && (i < lay->count()); ++i) {
        QLayoutItem* item = lay->itemAt(i);
        if (SARibbonToolButton* btn = qobject_cast< SARibbonToolButton* >(item ? item->widget() : nullptr)) {
            if (btn->isLargeRibbonButton()) {
                return btn;
            }
        }
    }
    return nullptr;
}

/// 面板高度取「单行文字宽度 / 宽高比」，保证单行超出宽高比上限，换行一定会被触发
int panelHeightForWrap(const QFontMetrics& fm, int singleLineWidth, qreal aspectRatio)
{
    return qMax(qRound(fm.lineSpacing() * 4.5), qRound(singleLineWidth / aspectRatio) - 2);
}

/// 绘制文字时可用的宽度：sizeHint 宽度减去左右 spacing
int textRectWidthOf(const SARibbonToolButton* button)
{
    return button->sizeHint().width() - 2 * button->spacing();
}

/// 文本中最长的单词宽度
int longestWordWidth(const QFontMetrics& fm, const QString& text)
{
    int longest = 0;
    int start   = 0;
    for (int i = 0; i <= text.size(); ++i) {
        if ((i == text.size()) || text.at(i).isSpace()) {
            if (i > start) {
                longest = qMax(longest, fm.horizontalAdvance(text.mid(start, i - start)));
            }
            start = i + 1;
        }
    }
    return longest;
}
}  // namespace

class SARibbonToolButtonWordWrapTest : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    /// issue #163 的最小复现：长单词不能被截断
    void testLongestWordFitsInWrappedButton();
    /// 单词都不长时必须继续换行，不能被宽度判断逼成单行全宽
    void testShortWordsStillWrap();
    /// 中文按字符换行，不受「最长单词」限制
    void testCjkTextStillWraps();
};

void SARibbonToolButtonWordWrapTest::testLongestWordFitsInWrappedButton()
{
    const QString text        = QStringLiteral("Internationalization options");
    const QString longestWord = QStringLiteral("Internationalization");

    SARibbonPanel panel(QStringLiteral("panel"));
    panel.addLargeAction(new QAction(text, &panel));
    SARibbonToolButton* btn = firstLargeButton(&panel);
    QVERIFY2(btn, "large button was not created");
    btn->setEnableWordWrap(true);

    const QFontMetrics fm(btn->font());
    const int singleLineWidth = fm.size(Qt::TextShowMnemonic, text).width();
    const int wordWidth       = fm.horizontalAdvance(longestWord);
    QVERIFY2(wordWidth * 2 > singleLineWidth, "fixture broken: the long word must be wider than half of the label");

    relayoutPanel(&panel, 400, panelHeightForWrap(fm, singleLineWidth, btn->buttonMaximumAspectRatio()));
    QVERIFY2(btn->isLargeRibbonButton(), "button is no longer a large button");

    const int textWidth = textRectWidthOf(btn);
    QVERIFY2(textWidth < singleLineWidth,
             qPrintable(QStringLiteral("\"%1\": text width=%2, word wrap was not applied (single line needs %3)")
                            .arg(text)
                            .arg(textWidth)
                            .arg(singleLineWidth)));
    QVERIFY2(textWidth >= wordWidth,
             qPrintable(QStringLiteral("\"%1\": text width=%2 is narrower than the longest word \"%3\" (%4), "
                                       "the word will be truncated")
                            .arg(text)
                            .arg(textWidth)
                            .arg(longestWord)
                            .arg(wordWidth)));
}

void SARibbonToolButtonWordWrapTest::testShortWordsStillWrap()
{
    const QString text = QStringLiteral("alpha beta gamma delta epsilon zeta eta theta");

    SARibbonPanel panel(QStringLiteral("panel"));
    panel.addLargeAction(new QAction(text, &panel));
    SARibbonToolButton* btn = firstLargeButton(&panel);
    QVERIFY2(btn, "large button was not created");
    btn->setEnableWordWrap(true);

    const QFontMetrics fm(btn->font());
    const int singleLineWidth = fm.size(Qt::TextShowMnemonic, text).width();
    // 面板高度固定为 100：宽高比上限约 140px，文本单行宽度远大于它，必然触发换行
    if (singleLineWidth <= qRound(100 * btn->buttonMaximumAspectRatio())) {
        QSKIP("fixture not reproducible: the test font is too narrow to exceed the aspect ratio limit");
    }
    relayoutPanel(&panel, 400, 100);

    const int textWidth = textRectWidthOf(btn);
    QVERIFY2(textWidth < singleLineWidth,
             qPrintable(QStringLiteral("\"%1\": text width=%2, word wrap was disabled by the width check "
                                       "(single line needs %3)")
                            .arg(text)
                            .arg(textWidth)
                            .arg(singleLineWidth)));
    QVERIFY2(textWidth >= longestWordWidth(fm, text),
             qPrintable(QStringLiteral("\"%1\": text width=%2 is narrower than its longest word (%3)")
                            .arg(text)
                            .arg(textWidth)
                            .arg(longestWordWidth(fm, text))));
}

void SARibbonToolButtonWordWrapTest::testCjkTextStillWraps()
{
    const QString text = QStringLiteral("国际化选项设置按钮文本测试");

    SARibbonPanel panel(QStringLiteral("panel"));
    panel.addLargeAction(new QAction(text, &panel));
    SARibbonToolButton* btn = firstLargeButton(&panel);
    QVERIFY2(btn, "large button was not created");
    btn->setEnableWordWrap(true);

    const QFontMetrics fm(btn->font());
    if (fm.horizontalAdvance(QStringLiteral("国")) <= 0) {
        QSKIP("no CJK glyphs in the test font");
    }
    const int singleLineWidth = fm.size(Qt::TextShowMnemonic, text).width();
    if (singleLineWidth <= qRound(100 * btn->buttonMaximumAspectRatio())) {
        QSKIP("fixture not reproducible: the CJK label does not exceed the aspect ratio limit");
    }
    relayoutPanel(&panel, 400, 100);

    const int textWidth = textRectWidthOf(btn);
    QVERIFY2(textWidth < singleLineWidth,
             qPrintable(QStringLiteral("\"%1\": text width=%2, CJK text no longer wraps (single line needs %3)")
                            .arg(text)
                            .arg(textWidth)
                            .arg(singleLineWidth)));
    QVERIFY2(textWidth >= fm.horizontalAdvance(QStringLiteral("国")),
             qPrintable(QStringLiteral("\"%1\": text width=%2 is too narrow to draw even one character")
                            .arg(text)
                            .arg(textWidth)));
}

QTEST_MAIN(SARibbonToolButtonWordWrapTest)
#include "SARibbonToolButtonWordWrapTest.moc"
