#include "MainWindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : SARibbonMainWindow(parent, false)
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
