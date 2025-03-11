#include "ApplicationWidget.h"
#include "ui_ApplicationWidget.h"

ApplicationWidget::ApplicationWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ApplicationWidget)
{
    ui->setupUi(this);
}

ApplicationWidget::~ApplicationWidget()
{
    delete ui;
}
