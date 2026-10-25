#include <QtTest>
#include <QApplication>
#include <QCursor>
#include <QImage>
#include <QPalette>
#include <QWidget>
#include "SARibbonToolButton.h"

/**
 * @brief SARibbonToolButton 前景色回归测试（GitHub #101）
 *
 * 稳定层：向控件 QPalette 的 foregroundRole 注入前景色，验证文字绘制真的取该角色的颜色；
 * QSS 层：验证 QSS 的 color 是否生效。
 *
 * 这两层都只依赖 QWidget::grab() 的离屏渲染，不 show 窗口、不移动光标：无头 CI
 * （QT_QPA_PLATFORM=offscreen）上没有可用窗口系统，创建真实窗口并让它获得焦点会在
 * Qt5 上触发 QStyleSheetStyle 析构期间的崩溃（见 GitHub Actions 日志）。
 *
 * 伪状态层（:hover）依赖真实光标，仅在桌面平台手动验证；无头平台直接跳过。
 */
namespace {
// 红/蓝判定使用松阈值并要求数量下限：文字抗锯齿的边缘是混合色，不能做精确颜色相等
bool isRedDominant(const QRgb& rgb)
{
    return qRed(rgb) > 150 && qGreen(rgb) < 100 && qBlue(rgb) < 100;
}

bool isBlueDominant(const QRgb& rgb)
{
    return qBlue(rgb) > 150 && qRed(rgb) < 100 && qGreen(rgb) < 100;
}

int countPixels(const QImage& img, bool (*pred)(const QRgb&))
{
    int count = 0;
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            if (pred(img.pixel(x, y))) {
                ++count;
            }
        }
    }
    return count;
}

bool isHeadlessPlatform()
{
    const QString name = QGuiApplication::platformName();
    return name == QLatin1String("offscreen") || name == QLatin1String("minimal");
}
}  // namespace

class SARibbonToolButtonColorTest : public QObject
{
    Q_OBJECT
private slots:
    void testForegroundRoleColor();
    void testStyleSheetColor();
    void testHoverStyleSheetColor();
};

void SARibbonToolButtonColorTest::testForegroundRoleColor()
{
    // 不 show：grab() 离屏渲染即可，避免无头环境下窗口/焦点带来的平台差异
    SARibbonToolButton btn;
    // 与 SARibbonPanelLayout 创建按钮时的配置一致：按钮不参与焦点链，
    // 避免无头平台上窗口获得焦点后样式析构触发 Qt5 的崩溃路径
    btn.setFocusPolicy(Qt::NoFocus);
    btn.setText(u8"ColorText");
    btn.resize(150, 40);

    // 前景色注入为红色
    QPalette pal = btn.palette();
    pal.setColor(btn.foregroundRole(), Qt::red);
    btn.setPalette(pal);
    QApplication::processEvents();
    const QImage redImg = btn.grab().toImage();

    // 前景色换为蓝色
    pal = btn.palette();
    pal.setColor(btn.foregroundRole(), Qt::blue);
    btn.setPalette(pal);
    QApplication::processEvents();
    const QImage blueImg = btn.grab().toImage();

    const int redInRedImg   = countPixels(redImg, isRedDominant);
    const int blueInRedImg  = countPixels(redImg, isBlueDominant);
    const int blueInBlueImg = countPixels(blueImg, isBlueDominant);
    const int redInBlueImg  = countPixels(blueImg, isRedDominant);

    QVERIFY2(redInRedImg >= 10, qPrintable(QString("red text pixels: %1").arg(redInRedImg)));
    QVERIFY2(blueInBlueImg >= 10, qPrintable(QString("blue text pixels: %1").arg(blueInBlueImg)));
    // 差分断言：排除 ClearType 亚像素抗锯齿产生的彩色边缘干扰
    QVERIFY2(redInRedImg > blueInRedImg + 10, "text color should follow foregroundRole (red case)");
    QVERIFY2(blueInBlueImg > redInBlueImg + 10, "text color should follow foregroundRole (blue case)");
}

// 静态 QSS 的 color（非伪状态）不依赖鼠标，可稳定验证 QStyleSheetStyle 路径
void SARibbonToolButtonColorTest::testStyleSheetColor()
{
    SARibbonToolButton btn;
    btn.setFocusPolicy(Qt::NoFocus);
    btn.setText(u8"QssText");
    btn.setStyleSheet("SARibbonToolButton { color: red; }");
    btn.resize(150, 40);
    QApplication::processEvents();
    const QImage img = btn.grab().toImage();

    const int redCount = countPixels(img, isRedDominant);
    QVERIFY2(redCount >= 10, qPrintable(QString("red text pixels: %1").arg(redCount)));
}

void SARibbonToolButtonColorTest::testHoverStyleSheetColor()
{
    if (isHeadlessPlatform()) {
        QSKIP("hover 伪状态需要真实窗口系统与光标，无头平台无法验证，转为手动验证项");
    }

    QWidget host;
    host.resize(300, 200);
    SARibbonToolButton btn(&host);
    btn.setFocusPolicy(Qt::NoFocus);
    btn.setText(u8"HoverText");
    btn.setStyleSheet("SARibbonToolButton:hover { color: red; }");
    btn.resize(150, 40);
    btn.move(75, 80);
    host.show();
    QVERIFY(QTest::qWaitForWindowExposed(&host));
    host.activateWindow();
    // 把真实光标移到 host 之外（避免 hover 状态污染像素断言）
    QCursor::setPos(host.mapToGlobal(QPoint(5, host.height() - 5)));
    QTest::qWait(50);
    QVERIFY(!btn.underMouse());
    QApplication::processEvents();
    const QImage noHoverImg = btn.grab().toImage();

    // 触发 hover（QTest::mouseMove 在 Qt5.15 等价于移动真实光标）
    QTest::mouseMove(&btn, btn.rect().center());
    QTest::qWait(200);
    if (!btn.underMouse()) {
        QSKIP("hover 状态未能触发（环境限制），:hover 伪状态颜色验证转为手动验证项");
    }
    const QImage hoverImg = btn.grab().toImage();

    const int redNoHover = countPixels(noHoverImg, isRedDominant);
    const int redHover   = countPixels(hoverImg, isRedDominant);

    QVERIFY2(redHover > redNoHover + 10,
             qPrintable(QString(":hover color:red not applied, red pixels before=%1 after=%2")
                            .arg(redNoHover)
                            .arg(redHover)));
}

QTEST_MAIN(SARibbonToolButtonColorTest)

#include "SARibbonToolButtonColorTest.moc"
