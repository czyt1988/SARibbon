#include "MainWindow.h"
#include "./ui_MainWindow.h"
#include "SARibbonBar.h"
MainWindow::MainWindow(QWidget* parent) : SARibbonMainWindow(parent), ui(new Ui::MainWindow)
{
    //! ui文件里有QMenubar，在setupUi函数里会创建menubar,
    //! 实际应该把ui界面里的menubar删除，避免没有必要的创建
    ui->setupUi(this);

    //! setRibbonBar函数会把原来的menubar删除，重新设置RibbonBar
    //! 如果直接把ui文件里的menubar删除，不需要再次创建RibbonBar
    SARibbonBar* bar = new SARibbonBar();
    auto panel       = bar->addCategoryPage("Main")->addPanel("panel");
    panel->addAction(ui->actionFile);
    setRibbonBar(bar);
}

MainWindow::~MainWindow()
{
    delete ui;
}
