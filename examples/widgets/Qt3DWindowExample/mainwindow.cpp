#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QLabel>
#include <QPushButton>
#include <QScreen>
#include <QTextStream>
#include <QTimer>
#include <QVBoxLayout>
#include <QWindow>
#include <Qt3DExtras/Qt3DWindow>
#include <Qt3DRender/QCamera>
#include <QDebug>
#include "SARibbonBar.h"

MainWindow::MainWindow(QWidget* par, SARibbonMainWindowStyles style) : SARibbonMainWindow(par, style)
{
    setWindowTitle(QStringLiteral("Qt3DWindowExample - #105"));
    resize(1000, 700);

    // 中心区：上侧信息与刷新按钮，下侧 Qt3D container
    QWidget* central = new QWidget(this);
    QVBoxLayout* lay = new QVBoxLayout(central);
    mInfoLabel       = new QLabel(QStringLiteral("启动后自动打印几何数据，也可点击刷新"), central);
    mInfoLabel->setWordWrap(true);
    lay->addWidget(mInfoLabel);

    QPushButton* btnRefresh = new QPushButton(QStringLiteral("刷新对照数据"), central);
    lay->addWidget(btnRefresh);
    connect(btnRefresh, &QPushButton::clicked, this, [this]() { dumpGeometry(QStringLiteral("manual-refresh")); });

    // 嵌入 Qt3D 渲染窗口（原生子窗口）
    Qt3DExtras::Qt3DWindow* window3d = new Qt3DExtras::Qt3DWindow();
    mContainer                       = QWidget::createWindowContainer(window3d, central);
    mContainer->setMinimumSize(200, 200);
    lay->addWidget(mContainer, 1);
    setCentralWidget(central);

    // show 之后打印对照数据（容器几何在窗口显示后才有效）；
    // --dump 模式：延迟打印并自动退出，用于命令行采集（WIN32 子系统无控制台，输出走 DebugView/重定向）
    const bool dumpMode = QCoreApplication::arguments().contains(QStringLiteral("--dump"));
    QTimer::singleShot(dumpMode ? 1500 : 0, this, [this, dumpMode]() {
        dumpGeometry(QStringLiteral("startup"));
        if (dumpMode) {
            QTimer::singleShot(200, qApp, &QCoreApplication::quit);
        }
    });
}

void MainWindow::showEvent(QShowEvent* e)
{
    SARibbonMainWindow::showEvent(e);
    // showEvent 之后延迟一拍再打印，确保布局完成
    QTimer::singleShot(100, this, [this]() { dumpGeometry(QStringLiteral("after-showEvent")); });
}

void MainWindow::dumpGeometry(const QString& tag)
{
    QWidget* central = centralWidget();
    if (!central || !mContainer) {
        return;
    }
    const QMargins cm     = contentsMargins();
    const QRect winGeo    = geometry();
    const QRect frameGeo  = frameGeometry();
    const QRect centralGeo = central->geometry();
    const QPoint centralGlobal = central->mapToGlobal(QPoint(0, 0));
    const QRect contGeo    = mContainer->geometry();
    const QPoint contGlobal = mContainer->mapToGlobal(QPoint(0, 0));
    const qreal dpr        = windowHandle() ? windowHandle()->devicePixelRatio() : qApp->devicePixelRatio();

    // container 在中心区内的期望位置（垂直布局：在 infoLabel 与按钮之下）
    const QPoint expectedInCentral = mContainer->mapTo(central, QPoint(0, 0));
    // 左偏量 = 期望落点 - 实际全局落点（正值表示向左偏）
    const int offsetX = contGlobal.x() - centralGlobal.x();

    qInfo().noquote() << "[QT3D] =====" << tag << "=====";
    qInfo().noquote().nospace() << "[QT3D] mainWindow: geometry=" << winGeo << " frameGeometry=" << frameGeo
                                << " contentsMargins=" << cm;
    qInfo().noquote().nospace() << "[QT3D] centralWidget: geometry=" << centralGeo
                                << " mapToGlobal=" << centralGlobal;
    qInfo().noquote().nospace() << "[QT3D] container: geometry=" << contGeo << " mapToGlobal=" << contGlobal
                                << " posInCentral=" << expectedInCentral;
    if (QWindow* wh = mContainer->windowHandle()) {
        qInfo().noquote().nospace() << "[QT3D] container native window: geometry=" << wh->geometry()
                                    << " frameGeometry=" << wh->frameGeometry();
    } else {
        qInfo().noquote() << "[QT3D] container native window: windowHandle() == nullptr";
    }
    qInfo().noquote().nospace() << "[QT3D] offset: containerGlobalX-centralGlobalX=" << offsetX
                                << " (logical px), dpr=" << dpr;
    qInfo().noquote().nospace() << "[QT3D] screen: " << (windowHandle() && windowHandle()->screen()
                                                             ? windowHandle()->screen()->name()
                                                             : QString("null"));
    qInfo().noquote() << "[QT3D] ===== end" << tag << "=====";

    if (mInfoLabel) {
        mInfoLabel->setText(QStringLiteral("container 左偏量（相对中心区左缘）: %1 逻辑像素（DPR=%2）\n"
                                           "详细数据见 qt3d-geometry.log 与控制台 [QT3D] 日志")
                                .arg(offsetX)
                                .arg(dpr));
    }
    // 同步写入可执行文件旁的 qt3d-geometry.log（WIN32 子系统无控制台时的采集通道）
    static QFile logFile(QCoreApplication::applicationDirPath() + QStringLiteral("/qt3d-geometry.log"));
    if (!logFile.isOpen()) {
        logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text);
    }
    if (logFile.isOpen()) {
        QString nativeLine = QStringLiteral("[QT3D] container native window: windowHandle() == nullptr\n");
        if (QWindow* wh = mContainer->windowHandle()) {
            const QRect ng = wh->geometry();
            nativeLine     = QStringLiteral("[QT3D] container native window: geometry=%1,%2 %3x%4 frameMargins=%5,%6\n")
                         .arg(QString::number(ng.x()), QString::number(ng.y()), QString::number(ng.width()),
                              QString::number(ng.height()), QString::number(wh->frameMargins().left()),
                              QString::number(wh->frameMargins().top()));
        }
        const QString content = QStringLiteral("[QT3D] =====%1=====\n").arg(tag)
            + QStringLiteral("[QT3D] mainWindow: geometry=%1 frameGeometry=%2 contentsMargins=%3\n")
                  .arg(QString::number(winGeo.x()) + "," + QString::number(winGeo.y()) + " "
                           + QString::number(winGeo.width()) + "x" + QString::number(winGeo.height()),
                       QString::number(frameGeo.x()) + "," + QString::number(frameGeo.y()) + " "
                           + QString::number(frameGeo.width()) + "x" + QString::number(frameGeo.height()),
                       QString::number(cm.left()) + "," + QString::number(cm.top()) + ","
                           + QString::number(cm.right()) + "," + QString::number(cm.bottom()))
            + QStringLiteral("[QT3D] centralWidget: geometry=%1,%2 %3x%4 mapToGlobal=%5,%6\n")
                  .arg(QString::number(centralGeo.x()), QString::number(centralGeo.y()),
                       QString::number(centralGeo.width()), QString::number(centralGeo.height()),
                       QString::number(centralGlobal.x()), QString::number(centralGlobal.y()))
            + QStringLiteral("[QT3D] container: geometry=%1,%2 %3x%4 mapToGlobal=%5,%6 posInCentral=%7,%8\n")
                  .arg(QString::number(contGeo.x()), QString::number(contGeo.y()), QString::number(contGeo.width()),
                       QString::number(contGeo.height()), QString::number(contGlobal.x()),
                       QString::number(contGlobal.y()), QString::number(expectedInCentral.x()),
                       QString::number(expectedInCentral.y()))
            + QStringLiteral("[QT3D] offset: containerGlobalX-centralGlobalX=%1 (logical px), dpr=%2\n")
                  .arg(QString::number(offsetX), QString::number(dpr))
            + nativeLine
            + QStringLiteral("[QT3D] =====end %1=====\n").arg(tag);
        logFile.write(content.toUtf8());
        logFile.flush();
    }
}
