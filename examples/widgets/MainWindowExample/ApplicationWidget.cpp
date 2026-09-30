#include "ApplicationWidget.h"
#include "ui_ApplicationWidget.h"
#include "SARibbonMainWindow.h"

ApplicationWidget::ApplicationWidget(SARibbonMainWindow* parent)
    : SARibbonApplicationWidget(parent), ui(new Ui::ApplicationWidget)
{
    ui->setupUi(this);
}

ApplicationWidget::~ApplicationWidget()
{
    delete ui;
}
