#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "SARibbonMainWindow.h"
namespace Ui
{
class MainWindow;
}
class SARibbonCategory;
class MainWindow : public SARibbonMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void on_actionadd_window_triggered();

private:
    void buildMainPage(SARibbonCategory* mainPage);

private:
    Ui::MainWindow* ui;
};

#endif  // MAINWINDOW_H
