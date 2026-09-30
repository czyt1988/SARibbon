#include "mainwindow.h"
#include <QApplication>
#include <QGuiApplication>
#include <QScreen>
#include <QWindow>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QDebug>
#include <cstdio>
#include "SARibbonBar.h"

MainWindow::MainWindow(QWidget* par, SARibbonMainWindowStyles style) : SARibbonMainWindow(par, style)
{
    setObjectName(QStringLiteral("MSDPI-MainWindow"));
    setWindowTitle(QStringLiteral("MultiScreenDpiExample - #109"));

    // 中心区：操作按钮 + 日志窗口
    QWidget* central = new QWidget(this);
    QVBoxLayout* lay = new QVBoxLayout(central);

    QHBoxLayout* btnLay = new QHBoxLayout();
    QPushButton* btnPrint   = new QPushButton(QStringLiteral("打印当前状态"), central);
    QPushButton* btnMax     = new QPushButton(QStringLiteral("最大化 / 还原"), central);
    QPushButton* btnMove    = new QPushButton(QStringLiteral("跨屏移动"), central);
    QPushButton* btnPingPong = new QPushButton(QStringLiteral("反复两侧横跳(5次)"), central);
    btnLay->addWidget(btnPrint);
    btnLay->addWidget(btnMax);
    btnLay->addWidget(btnMove);
    btnLay->addWidget(btnPingPong);
    btnLay->addStretch();
    lay->addLayout(btnLay);

    QLabel* tip = new QLabel(
        QStringLiteral("操作后日志自动输出到下方与控制台（前缀 [MSDPI]），请把全部日志回贴到 issue。\n"
                       "建议操作顺序：打印当前状态 -> 跨屏移动 -> 最大化/还原 -> 反复两侧横跳"),
        central);
    tip->setWordWrap(true);
    lay->addWidget(tip);

    QTextEdit* logView = new QTextEdit(central);
    logView->setReadOnly(true);
    logView->setPlaceholderText(QStringLiteral("[MSDPI] 日志将显示在这里"));
    lay->addWidget(logView, 1);
    setCentralWidget(central);
    resize(900, 600);

    connect(btnPrint, &QPushButton::clicked, this, &MainWindow::onPrintStateClicked);
    connect(btnMax, &QPushButton::clicked, this, &MainWindow::onToggleMaximizeClicked);
    connect(btnMove, &QPushButton::clicked, this, &MainWindow::onMoveToNextScreenClicked);
    connect(btnPingPong, &QPushButton::clicked, this, &MainWindow::onPingPongClicked);

    // 屏幕变化钩子：打印变化的时机（这是归因 #109 的关键数据）
    // 构造时 windowHandle 可能尚未创建，show 之后再连接一次
    QTimer::singleShot(0, this, [this]() {
        if (windowHandle()) {
            connect(windowHandle(), &QWindow::screenChanged, this, &MainWindow::onScreenChanged);
        }
        dumpState(QStringLiteral("startup"));
    });
}

static void msDpiMessageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    // 输出到控制台
    fprintf(stderr, "%s\n", qPrintable(msg));
    fflush(stderr);
    // 镜像到日志窗口（通过 objectName 定位），方便报告者直接复制回贴
    for (QWidget* w : QApplication::topLevelWidgets()) {
        if (w->objectName() == QLatin1String("MSDPI-MainWindow")) {
            if (QTextEdit* te = w->findChild< QTextEdit* >()) {
                te->append(msg);
            }
            break;
        }
    }
    Q_UNUSED(type);
    Q_UNUSED(context);
}

void MainWindow::dumpState(const QString& tag)
{
    qInfo().noquote() << "[MSDPI] =====" << tag << "=====";
    const QList< QScreen* > screens = QGuiApplication::screens();
    qInfo().noquote() << "[MSDPI] screens().size() =" << screens.size();
    for (int i = 0; i < screens.size(); ++i) {
        QScreen* s = screens.at(i);
        qInfo().noquote().nospace() << "[MSDPI] screen[" << i << "] name=" << s->name() << " geometry=" << s->geometry()
                                    << " availableGeometry=" << s->availableGeometry()
                                    << " devicePixelRatio=" << s->devicePixelRatio()
                                    << " logicalDotsPerInch=" << s->logicalDotsPerInch();
    }
    if (QWindow* wh = windowHandle()) {
        const QRect g = geometry();
        const QRect fg = frameGeometry();
        const qreal dpr = wh->devicePixelRatio();
        // 逻辑坐标 -> 物理像素矩形（判断"跨屏后逻辑尺寸被重算导致抖动"的关键数据）
        QRectF physRect(QPointF(g.topLeft()) * dpr, QPointF(g.size().width(), g.size().height()) * dpr);
        qInfo().noquote().nospace()
            << "[MSDPI] window: handleScreen=" << (wh->screen() ? wh->screen()->name() : QString("null"))
            << " handleDpr=" << dpr << " geometry(logical)=" << g << " frameGeometry=" << fg
            << " isMaximized=" << isMaximized() << " windowState=" << static_cast< int >(windowState())
            << " geometry(physical)=" << physRect.toRect();
    } else {
        qInfo().noquote() << "[MSDPI] window: windowHandle() == nullptr";
    }
    qInfo().noquote() << "[MSDPI] screenCount=" << QGuiApplication::screens().size()
                      << " primaryScreen=" << (QGuiApplication::primaryScreen() ? QGuiApplication::primaryScreen()->name() : QString("null"));
    qInfo().noquote() << "[MSDPI] ===== end" << tag << "=====";
}

void MainWindow::dumpStateDelayed(const QString& tag, int delayMs)
{
    QTimer::singleShot(delayMs, this, [this, tag]() { dumpState(tag); });
}

void MainWindow::onPrintStateClicked()
{
    dumpState(QStringLiteral("manual-print"));
}

void MainWindow::onToggleMaximizeClicked()
{
    if (isMaximized()) {
        showNormal();
        qInfo().noquote() << "[MSDPI] action: showNormal()";
    } else {
        showMaximized();
        qInfo().noquote() << "[MSDPI] action: showMaximized()";
    }
    dumpStateDelayed(QStringLiteral("after-maximize-toggle"));
}

void MainWindow::onMoveToNextScreenClicked()
{
    const QList< QScreen* > screens = QGuiApplication::screens();
    if (screens.size() < 2) {
        qInfo().noquote() << "[MSDPI] only" << screens.size() << "screen(s), cannot move across screens";
        dumpState(QStringLiteral("single-screen"));
        return;
    }
    QScreen* current = windowHandle() ? windowHandle()->screen() : nullptr;
    int nextIndex = 0;
    for (int i = 0; i < screens.size(); ++i) {
        if (screens.at(i) == current) {
            nextIndex = (i + 1) % screens.size();
            break;
        }
    }
    QScreen* target = screens.at(nextIndex);
    qInfo().noquote() << "[MSDPI] action: moveToScreen" << target->name();
    // 移动到目标屏可用区域中心（保持窗口尺寸）
    const QRect ag = target->availableGeometry();
    const QSize sz = size();
    setGeometry(ag.center().x() - sz.width() / 2, ag.center().y() - sz.height() / 2, sz.width(), sz.height());
    dumpStateDelayed(QStringLiteral("after-move-") + target->name());
}

void MainWindow::onPingPongClicked()
{
    const QList< QScreen* > screens = QGuiApplication::screens();
    if (screens.size() < 2) {
        qInfo().noquote() << "[MSDPI] only" << screens.size() << "screen(s), ping-pong needs 2+ screens";
        return;
    }
    mScreensForPingPong = screens;
    mPingPongIndex      = 0;
    mPingPongCount      = 0;
    qInfo().noquote() << "[MSDPI] action: ping-pong start," << screens.size() << "screens";
    mPingPongTimer.setInterval(800);
    mPingPongTimer.disconnect();
    connect(&mPingPongTimer, &QTimer::timeout, this, &MainWindow::onPingPongStep);
    mPingPongTimer.start();
    onPingPongStep();
}

void MainWindow::onPingPongStep()
{
    if (mPingPongCount >= 10) {  // 5 次往返 = 10 次移动
        mPingPongTimer.stop();
        qInfo().noquote() << "[MSDPI] action: ping-pong finished";
        dumpState(QStringLiteral("after-pingpong"));
        return;
    }
    QScreen* target = mScreensForPingPong.at(mPingPongIndex % mScreensForPingPong.size());
    ++mPingPongIndex;
    ++mPingPongCount;
    qInfo().noquote() << "[MSDPI] ping-pong step" << mPingPongCount << "->" << target->name();
    const QRect ag = target->availableGeometry();
    const QSize sz = size();
    setGeometry(ag.center().x() - sz.width() / 2, ag.center().y() - sz.height() / 2, sz.width(), sz.height());
    dumpStateDelayed(QStringLiteral("pingpong-step-") + QString::number(mPingPongCount), 400);
}

void MainWindow::onScreenChanged(QScreen* screen)
{
    qInfo().noquote() << "[MSDPI] QWindow::screenChanged ->" << (screen ? screen->name() : QString("null"));
}

void msDpiInstallMessageHandler()
{
    qInstallMessageHandler(msDpiMessageHandler);
}
