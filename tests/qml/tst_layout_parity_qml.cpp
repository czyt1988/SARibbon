#include <QtTest>
#include <QApplication>
#include <QAction>
#include <QIcon>
#include <QImage>
#include <QQuickItem>
#include <memory>
#include <SARibbonCore/SARibbonToolButtonLayout.h>
#include <SARibbonQml/SARibbonQmlToolButton.h>
#include "SARibbonPanel.h"
#include "SARibbonToolButton.h"

/**
 * @brief QML ↔ widgets 工具按钮布局一致性测试
 * @details 布局算法（尺寸估算、两行/单行判定、icon/text/indicator 三区划分）已下沉
 *          core 的 SARibbonToolButtonLayout，两个前端都只是「喂输入 + 取输出」。
 *          本测试把这条约定钉死：
 *          1. 同一文本、同一大按钮高度、同一字体下，widgets 的 sizeHint() 与 QML
 *             宿主的 sizeHint() 必须逐像素相同（大按钮不依赖当前 rect，小按钮依赖
 *             rect 高度，因此小按钮用例先把宿主高度对齐到 widgets 侧）；
 *          2. QML 宿主对外发布的 iconGeometry/textGeometry/indicatorGeometry 必须
 *             等于 core calcDrawRects 在同一 rect 上的结果——防止将来有人在 QML 里
 *             重新推导一遍布局（铁律：禁止在 QML 重写布局计算）；
 *          3. 大按钮开启 wordWrap 时按两行预算排版（displayText 保留原始换行），
 *             关闭 wordWrap 或小按钮时按单行省略号排版；
 *          4. 布局旋钮（两个文字高度系数、宽高比一对、spacing、两个图标盒）在两端
 *             都是"填进 core Input 的值"，同一组值必须给出同一个 sizeHint。
 */
namespace
{
using Layout = SARibbon::Core::SARibbonToolButtonLayout;

/// core 的 Input 只消费「有没有图标」这个布尔（widgets 侧 !opt.icon.isNull()，
/// QML 侧 !iconSource.isEmpty()），因此这里用一张 1x1 合成图标即可，不依赖真实资源
QIcon makeStubIcon()
{
    QImage img(1, 1, QImage::Format_ARGB32);
    img.fill(Qt::black);
    return QIcon(QPixmap::fromImage(img));
}

/// QML 宿主的图标源是 URL 字符串，测试不建叶子，因此不会真的去加载它
const char* const kStubIconSource = "image://saribbon-stub/icon";

/// 让 panel 以指定尺寸完成一次真实布局，largeButtonHeight() 随之有效
void relayoutPanel(SARibbonPanel* panel, int width, int height)
{
    panel->resize(width, height);
    if (panel->layout()) {
        panel->layout()->setGeometry(QRect(0, 0, width, height));
    }
    QCoreApplication::sendPostedEvents(panel, QEvent::LayoutRequest);
}

SARibbonToolButton* firstButtonOf(SARibbonPanel* panel)
{
    const QList< SARibbonToolButton* > buttons = panel->findChildren< SARibbonToolButton* >();
    return buttons.isEmpty() ? nullptr : buttons.first();
}

/// widgets 侧：建一个只含单个 action 的 panel，返回按钮与 panel 的大按钮高度
struct WidgetsSide
{
    std::unique_ptr< SARibbonPanel > panel;
    SARibbonToolButton* button { nullptr };
    int largeButtonHeight { -1 };
};

WidgetsSide makeWidgetsButton(const QString& text, SARibbonPanelItem::RowProportion proportion, bool wordWrap)
{
    WidgetsSide side;
    side.panel.reset(new SARibbonPanel());
    QAction* act = new QAction(text, side.panel.get());
    switch (proportion) {
    case SARibbonPanelItem::Large:
        side.panel->addLargeAction(act);
        break;
    case SARibbonPanelItem::Small:
        side.panel->addSmallAction(act);
        break;
    default:
        side.panel->addMediumAction(act);
        break;
    }
    // 换行开关影响 sizeHint，必须在 panel 真实布局之前设定
    if (SARibbonToolButton* btn = firstButtonOf(side.panel.get())) {
        btn->setEnableWordWrap(wordWrap);
    }
    relayoutPanel(side.panel.get(), 400, 160);
    side.button            = firstButtonOf(side.panel.get());
    side.largeButtonHeight = side.panel->largeButtonHeight();
    return side;
}

/// QML 侧：独立宿主（不建叶子，sizeHint 全在 C++ 侧由 core 度量推导）
std::unique_ptr< SARibbonQml::RibbonToolButton > makeQmlButton(const QString& text,
                                                               SARibbonQml::RibbonEnums::RowProportion proportion,
                                                               bool wordWrap,
                                                               int largeButtonHeight,
                                                               int heightHint)
{
    std::unique_ptr< SARibbonQml::RibbonToolButton > host(new SARibbonQml::RibbonToolButton());
    host->setProportion(proportion);
    host->setWordWrap(wordWrap);
    if (largeButtonHeight > 0) {
        host->setLargeButtonHeightContext(largeButtonHeight);
    }
    if (heightHint > 0) {
        host->setHeight(heightHint);
    }
    host->setText(text);  // 最后设置：setText 会重算 sizeHint，此时高度/上下文已就位
    return host;
}
}  // namespace

class TestLayoutParityQml : public QObject
{
    Q_OBJECT
private Q_SLOTS:
    void initTestCase();
    void largeButtonSizeHintParity_data();
    void largeButtonSizeHintParity();
    void largeButtonNoWrapSizeHintParity();
    void smallButtonSizeHintParity_data();
    void smallButtonSizeHintParity();
    void publishedGeometryComesFromCore();
    void captionModeParity();
    void layoutKnobsParity();
};

void TestLayoutParityQml::initTestCase()
{
    // 两端字体同源：widgets 侧按钮字体继承应用字体，QML 侧 RibbonMetrics 默认
    // 也取 QGuiApplication::font()，因此无需额外对齐（改动任一侧都会破坏本测试）
    const QFontMetrics fm(QApplication::font());
    QVERIFY(fm.lineSpacing() > 0);
}

void TestLayoutParityQml::largeButtonSizeHintParity_data()
{
    QTest::addColumn< QString >("text");
    QTest::newRow("short") << QStringLiteral("Paste");
    QTest::newRow("two-words-fits") << QStringLiteral("Font Size");
    QTest::newRow("long-needs-wrap") << QStringLiteral("remove application button");
    QTest::newRow("very-long") << QStringLiteral("show very long text in a button,balabalabala etc");
    QTest::newRow("manual-newline") << QStringLiteral("Paste\nSpecial");
    QTest::newRow("single-long-word") << QStringLiteral("Supercalifragilistic");
    QTest::newRow("cjk") << QStringLiteral("粘贴为纯文本");
}

void TestLayoutParityQml::largeButtonSizeHintParity()
{
    QFETCH(QString, text);
    const WidgetsSide w = makeWidgetsButton(text, SARibbonPanelItem::Large, true);
    QVERIFY(w.button);
    QVERIFY(w.largeButtonHeight > 0);

    auto q = makeQmlButton(text, SARibbonQml::RibbonEnums::Large, true, w.largeButtonHeight, 0);
    QCOMPARE(q->sizeHint(), w.button->sizeHint());
}

void TestLayoutParityQml::largeButtonNoWrapSizeHintParity()
{
    const QStringList texts { QStringLiteral("Paste"), QStringLiteral("remove application button"),
                              QStringLiteral("Supercalifragilistic") };
    for (const QString& text : texts) {
        const WidgetsSide w = makeWidgetsButton(text, SARibbonPanelItem::Large, false);
        QVERIFY(w.button);
        auto q = makeQmlButton(text, SARibbonQml::RibbonEnums::Large, false, w.largeButtonHeight, 0);
        QCOMPARE(q->sizeHint(), w.button->sizeHint());
    }
}

void TestLayoutParityQml::smallButtonSizeHintParity_data()
{
    QTest::addColumn< QString >("text");
    QTest::newRow("short") << QStringLiteral("Cut");
    QTest::newRow("long") << QStringLiteral("Select everything in the document");
    QTest::newRow("cjk") << QStringLiteral("剪切");
}

void TestLayoutParityQml::smallButtonSizeHintParity()
{
    QFETCH(QString, text);
    const WidgetsSide w = makeWidgetsButton(text, SARibbonPanelItem::Small, true);
    QVERIFY(w.button);

    // 小按钮的 textDrawRectHeight = rect.height() - 2，两端必须用同一个 rect 高度：
    // widgets 侧显式失效缓存后按指定高度重算，QML 侧把宿主高度设成同一个值
    const int h = 24;
    w.button->resize(w.button->width(), h);
    w.button->invalidateSizeHint();
    const QSize wHint = w.button->sizeHint();
    QVERIFY(wHint.height() > 0);

    auto q = makeQmlButton(text, SARibbonQml::RibbonEnums::Small, true, w.largeButtonHeight, h);
    QCOMPARE(q->sizeHint(), wHint);
}

void TestLayoutParityQml::publishedGeometryComesFromCore()
{
    const WidgetsSide w = makeWidgetsButton(QStringLiteral("remove application button"), SARibbonPanelItem::Large, true);
    QVERIFY(w.button);
    auto q = makeQmlButton(QStringLiteral("remove application button"), SARibbonQml::RibbonEnums::Large, true,
                           w.largeButtonHeight, 0);

    // 把宿主放到 widgets 按钮真实拿到的几何上（面板布局结果）
    const QRect r = w.button->geometry();
    QVERIFY(r.width() > 0 && r.height() > 0);
    q->setSize(r.size());

    const Layout::Input in = [&]() {
        Layout::Input i;
        i.rect            = QRect(QPoint(0, 0), r.size());
        i.hasIcon         = false;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        // 与宿主 layoutInput() 相同：Qt6 无图标时 initStyleOption 降级为 TextOnly
        i.toolButtonStyle = Qt::ToolButtonTextOnly;
#else
        i.toolButtonStyle = Qt::ToolButtonTextBesideIcon;
#endif
        i.isLargeButton          = true;
        i.enableWordWrap         = true;
        i.hasIndicator           = false;
        i.isRTL                  = false;
        i.iconSize               = QSize(22, 22);
        i.largeIconSize          = QSize(32, 32);
        i.text                   = QStringLiteral("remove application button");
        i.fontMetrics            = QFontMetrics(QApplication::font());
        i.spacing                = SARibbon::Core::ToolButtonLayoutConstants::DEFAULT_SPACING;
        i.indicatorLen           = SARibbon::Core::ToolButtonLayoutConstants::DEFAULT_INDICATOR_LEN_LARGE;
        i.panelLargeButtonHeight = w.largeButtonHeight;
        i.maximumWidth           = SARibbon::Core::ToolButtonLayoutConstants::UNLIMITED_WIDTH;
        return i;
    }();

    const Layout::SizeHintResult hint = Layout::calcSizeHint(in);
    const Layout::DrawRectResult ref  = Layout::calcDrawRects(in, hint.isTextNeedWrap);
    // 大按钮 + wordWrap：文本框必须是顶对齐的两行预算高度，而不是整块居中
    QVERIFY(q->textGeometry().height() > 0);
    QVERIFY(q->textGeometry().width() > 0);
    QCOMPARE(q->isTextWordWrap(), true);
    QCOMPARE(q->textGeometry(), QRectF(ref.textRect.isValid() ? ref.textRect : QRect()));
    QCOMPARE(q->iconGeometry(), QRectF(ref.iconRect.isValid() ? ref.iconRect : QRect()));
    QCOMPARE(q->indicatorGeometry(), QRectF(ref.indicatorArrowRect.isValid() ? ref.indicatorArrowRect : QRect()));
    // 无图标时 core 不给 icon 区，宿主必须发布空矩形（叶子据此隐藏 Image）
    QVERIFY(q->iconGeometry().isEmpty());
}

void TestLayoutParityQml::captionModeParity()
{
    const QString text = QStringLiteral("Paste\nSpecial");

    // 大按钮 + wordWrap：保留用户手动换行，交给 QML Text 做两行排版
    const WidgetsSide wl = makeWidgetsButton(text, SARibbonPanelItem::Large, true);
    QVERIFY(wl.button);
    auto ql = makeQmlButton(text, SARibbonQml::RibbonEnums::Large, true, wl.largeButtonHeight, 0);
    QCOMPARE(ql->displayText(), text);
    QCOMPARE(ql->isTextWordWrap(), true);
    QCOMPARE(ql->isLargeType(), true);

    // 大按钮 + 关闭 wordWrap：单行省略
    const WidgetsSide wn = makeWidgetsButton(text, SARibbonPanelItem::Large, false);
    QVERIFY(wn.button);
    auto qn = makeQmlButton(text, SARibbonQml::RibbonEnums::Large, false, wn.largeButtonHeight, 0);
    QCOMPARE(qn->isTextWordWrap(), false);
    QVERIFY(!qn->displayText().contains(QLatin1Char('\n')));

    // 小按钮：任何情况下都是单行省略
    const WidgetsSide ws = makeWidgetsButton(text, SARibbonPanelItem::Small, true);
    QVERIFY(ws.button);
    const int h = ws.button->height() > 0 ? ws.button->height() : ws.button->sizeHint().height();
    auto qs = makeQmlButton(text, SARibbonQml::RibbonEnums::Small, true, ws.largeButtonHeight, h);
    QCOMPARE(qs->isLargeType(), false);
    QCOMPARE(qs->isTextWordWrap(), false);
    QVERIFY(!qs->displayText().contains(QLatin1Char('\n')));
}

/**
 * @brief 布局旋钮跨端一致性（Factors / spacing / icon 尺寸）
 * @details 这些旋钮在 widgets 侧是 SARibbonToolButton 的公开设置函数，在 QML 侧是
 *          宿主属性；两端都只是把值填进 core 的 Input，因此同一组值必须给出同一个
 *          sizeHint。每个块都额外断言"改完之后与默认值不同"，防止旋钮其实没接上
 *          算法而测试空转。
 */
void TestLayoutParityQml::layoutKnobsParity()
{
    const QString longText  = QStringLiteral("remove application button");
    const QString shortText = QStringLiteral("Paste");

    // 宽高比上限放宽：大按钮允许更宽
    {
        WidgetsSide w = makeWidgetsButton(longText, SARibbonPanelItem::Large, true);
        QVERIFY(w.button);
        const QSize defaultHint = w.button->sizeHint();
        w.button->setButtonMaximumAspectRatio(2.5);
        w.button->invalidateSizeHint();
        const QSize wHint = w.button->sizeHint();
        QVERIFY(wHint != defaultHint);

        auto q = makeQmlButton(longText, SARibbonQml::RibbonEnums::Large, true, w.largeButtonHeight, 0);
        q->setButtonMaximumAspectRatio(2.5);
        QCOMPARE(q->sizeHint(), wHint);
    }

    // 大按钮最小宽度比例：minW = round(lineSpacing * LARGE_BUTTON_HEIGHT_FACTOR * ratio)。
    // 默认 0.75 时 minW 小于短词按钮的内容宽度，约束不生效，因此这里把比例调大到
    // 让 minW 反过来决定宽度——这才能证明旋钮真的接进了 core 算法
    {
        WidgetsSide w = makeWidgetsButton(QStringLiteral("Cut"), SARibbonPanelItem::Large, true);
        QVERIFY(w.button);
        const QSize defaultHint = w.button->sizeHint();
        w.button->setLargeButtonMinimumWidthRatio(2.0);
        w.button->invalidateSizeHint();
        const QSize wHint = w.button->sizeHint();
        QVERIFY(wHint.width() > defaultHint.width());
        QCOMPARE(wHint.height(), defaultHint.height());

        auto q = makeQmlButton(QStringLiteral("Cut"), SARibbonQml::RibbonEnums::Large, true, w.largeButtonHeight, 0);
        q->setLargeButtonMinimumWidthRatio(2.0);
        QCOMPARE(q->sizeHint(), wHint);
    }

    // 两行/单行文字高度系数：panel 内大按钮的高度是 panel 给的，textHeight
    // (= lineSpacing * 系数) 只有超过它才会把 sizeHint 顶高，所以按当前字体把系数
    // 放大到必然越界——否则会出现"系数改了但结果没变"的空转断言
    {
        WidgetsSide w = makeWidgetsButton(longText, SARibbonPanelItem::Large, true);
        QVERIFY(w.button);
        const qreal amplify = (qreal(w.largeButtonHeight) / w.button->fontMetrics().lineSpacing()) + 2.0;
        const QSize defaultHint = w.button->sizeHint();
        SARibbonToolButton::LayoutFactor fac = w.button->layoutFactor();
        fac.twoLineHeightFactor              = amplify;
        w.button->setLayoutFactor(fac);
        w.button->invalidateSizeHint();
        const QSize wHint = w.button->sizeHint();
        QVERIFY(wHint.height() > defaultHint.height());

        auto q = makeQmlButton(longText, SARibbonQml::RibbonEnums::Large, true, w.largeButtonHeight, 0);
        q->setTwoLineHeightFactor(amplify);
        QCOMPARE(q->sizeHint(), wHint);
    }
    {
        WidgetsSide w = makeWidgetsButton(longText, SARibbonPanelItem::Large, false);
        QVERIFY(w.button);
        const qreal amplify = (qreal(w.largeButtonHeight) / w.button->fontMetrics().lineSpacing()) + 2.0;
        const QSize defaultHint = w.button->sizeHint();
        SARibbonToolButton::LayoutFactor fac = w.button->layoutFactor();
        fac.oneLineHeightFactor              = amplify;
        w.button->setLayoutFactor(fac);
        w.button->invalidateSizeHint();
        const QSize wHint = w.button->sizeHint();
        QVERIFY(wHint.height() > defaultHint.height());

        auto q = makeQmlButton(longText, SARibbonQml::RibbonEnums::Large, false, w.largeButtonHeight, 0);
        q->setOneLineHeightFactor(amplify);
        QCOMPARE(q->sizeHint(), wHint);
    }

    // 元素间距（大按钮 + 小按钮各一次；小按钮依赖 rect 高度，两端对齐到同一高度）
    {
        WidgetsSide wl = makeWidgetsButton(longText, SARibbonPanelItem::Large, true);
        QVERIFY(wl.button);
        const QSize wlDefault = wl.button->sizeHint();
        wl.button->setSpacing(6);
        wl.button->invalidateSizeHint();
        QVERIFY(wl.button->sizeHint() != wlDefault);
        auto ql = makeQmlButton(longText, SARibbonQml::RibbonEnums::Large, true, wl.largeButtonHeight, 0);
        ql->setSpacing(6);
        QCOMPARE(ql->sizeHint(), wl.button->sizeHint());

        WidgetsSide ws = makeWidgetsButton(shortText, SARibbonPanelItem::Small, true);
        QVERIFY(ws.button);
        const int h = 24;
        const QSize wsDefault = ws.button->sizeHint();
        ws.button->setSpacing(6);
        ws.button->resize(ws.button->width(), h);
        ws.button->invalidateSizeHint();
        const QSize wHint = ws.button->sizeHint();
        QVERIFY(wHint != wsDefault);
        auto qs           = makeQmlButton(shortText, SARibbonQml::RibbonEnums::Small, true, ws.largeButtonHeight, h);
        qs->setSpacing(6);
        QCOMPARE(qs->sizeHint(), wHint);
    }

    // 图标盒尺寸：大按钮的 largeIconSize 只在取消高度比例约束（ratio<=0）时充当最小
    // 宽度下限，所以先关掉比例再放大图标盒才观测得到；小按钮的 smallIconSize 直接
    // 决定 icon 区宽高，放大到明显超过默认 16x16 即可
    {
        WidgetsSide w = makeWidgetsButton(shortText, SARibbonPanelItem::Large, true);
        QVERIFY(w.button);
        w.button->setLargeButtonMinimumWidthRatio(0.0);
        w.button->invalidateSizeHint();
        const QSize defaultHint = w.button->sizeHint();
        w.button->setLargeIconSize(QSize(200, 200));
        w.button->invalidateSizeHint();
        const QSize wHint = w.button->sizeHint();
        QVERIFY(wHint.width() > defaultHint.width());

        auto q = makeQmlButton(shortText, SARibbonQml::RibbonEnums::Large, true, w.largeButtonHeight, 0);
        q->setLargeButtonMinimumWidthRatio(0.0);
        q->setLargeIconSize(QSize(200, 200));
        QCOMPARE(q->sizeHint(), wHint);

        // 小按钮的 iconSize 只在「图标 + 文字」分支被消费（Qt6 下无图标时
        // initStyleOption 降级为 TextOnly，该分支根本不读 iconSize），所以两端都要
        // 先挂上图标，否则旋钮改了也观测不到，断言会空转
        WidgetsSide ws = makeWidgetsButton(shortText, SARibbonPanelItem::Small, true);
        QVERIFY(ws.button);
        ws.button->setIcon(makeStubIcon());
        ws.button->invalidateSizeHint();
        const int h              = 24;
        const QSize smallDefault = ws.button->sizeHint();
        ws.button->setSmallIconSize(QSize(40, 40));
        ws.button->resize(ws.button->width(), h);
        ws.button->invalidateSizeHint();
        QVERIFY(ws.button->sizeHint() != smallDefault);
        auto qs = makeQmlButton(shortText, SARibbonQml::RibbonEnums::Small, true, ws.largeButtonHeight, h);
        qs->setIconSource(QLatin1String(kStubIconSource));
        qs->setSmallIconSize(QSize(40, 40));
        QCOMPARE(qs->sizeHint(), ws.button->sizeHint());
    }
}

QTEST_MAIN(TestLayoutParityQml)
#include "tst_layout_parity_qml.moc"
