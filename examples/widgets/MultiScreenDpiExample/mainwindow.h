#ifndef MULTI_SCREENDPI_MAINWINDOW_H
#define MULTI_SCREENDPI_MAINWINDOW_H
#include "SARibbonMainWindow.h"
#include <QTimer>

/**
 * @brief 多屏 DPI 最大化异常诊断工程（GitHub #109）
 *
 * 目标环境：1080p 主屏 + 4K 外接屏（不同缩放比）。
 * 工程提供四个操作入口，每个操作后自动输出结构化诊断日志，
 * 报告者把日志回贴到 issue 即可用于归因。
 */
class MainWindow : public SARibbonMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* par                   = nullptr,
               SARibbonMainWindowStyles style = { static_cast< int >(SARibbonMainWindowStyleFlag::UseRibbonFrame)
                                                 | static_cast< int >(SARibbonMainWindowStyleFlag::UseRibbonMenuBar) });

private:
    // 打印所有屏幕信息与窗口几何（统一前缀 [MSDPI]，便于回贴）
    void dumpState(const QString& tag);
    // 延迟打印：DPI 切换 / screenChanged 是异步的，立即读到的是旧值
    void dumpStateDelayed(const QString& tag, int delayMs = 300);

private Q_SLOTS:
    void onPrintStateClicked();
    void onToggleMaximizeClicked();
    void onMoveToNextScreenClicked();
    void onPingPongClicked();
    void onPingPongStep();
    void onScreenChanged(QScreen* screen);

private:
    QTimer mPingPongTimer;   // 两侧横跳定时器
    QList<QScreen*> mScreensForPingPong;
    int mPingPongIndex { 0 };
    int mPingPongCount { 0 };
};
#endif  // MULTI_SCREENDPI_MAINWINDOW_H
