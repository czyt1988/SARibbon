#ifndef QT3DWINDOWEXAMPLE_MAINWINDOW_H
#define QT3DWINDOWEXAMPLE_MAINWINDOW_H
#include "SARibbonMainWindow.h"

/**
 * @brief Qt3DWindow 嵌入位置左偏复现工程（GitHub #105）
 *
 * 用 QWidget::createWindowContainer(new Qt3DWindow) 把 Qt3D 渲染窗口嵌入
 * SARibbonMainWindow 中心区，量化 container 的实际落点与期望落点之差。
 */
class QLabel;
class QPushButton;
class QWidget;

class MainWindow : public SARibbonMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* par                   = nullptr,
               SARibbonMainWindowStyles style = { static_cast< int >(SARibbonMainWindowStyleFlag::UseRibbonFrame)
                                                 | static_cast< int >(SARibbonMainWindowStyleFlag::UseRibbonMenuBar) });

protected:
    void showEvent(QShowEvent* e) override;

private:
    // 打印主窗口/中心区/container 的几何对照数据（前缀 [QT3D]）
    void dumpGeometry(const QString& tag);

private:
    QWidget* mContainer { nullptr };
    QLabel* mInfoLabel { nullptr };
};
#endif  // QT3DWINDOWEXAMPLE_MAINWINDOW_H
