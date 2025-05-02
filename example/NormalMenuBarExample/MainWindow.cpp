#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent) : SARibbonMainWindow(parent, SARibbonMainWindowStyleFlag::UseNativeMenuBar)
{
    if (!isUseRibbon()) {
        ui = new Ui::MainWindow;
        ui->setupUi(this);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}
