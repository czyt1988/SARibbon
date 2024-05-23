#include "MainWidget.h"
#include "ui_MainWidget.h"
#include "RibbonWidget.h"
MainWidget::MainWidget(QWidget* parent) : QWidget(parent), ui(new Ui::MainWidget)
{
    ui->setupUi(this);
    ui->tabWidget->addTab(new RibbonWidget(), "ribbon tab 1");
    ui->tabWidget->addTab(new RibbonWidget(), "ribbon tab 2");
    ui->tabWidget->addTab(new RibbonWidget(), "ribbon tab 3");
}

MainWidget::~MainWidget()
{
    delete ui;
}
