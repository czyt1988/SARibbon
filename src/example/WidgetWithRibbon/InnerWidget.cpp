#include "InnerWidget.h"
#include "ui_InnerWidget.h"

InnerWidget::InnerWidget(QWidget* parent) : QWidget(parent), ui(new Ui::InnerWidget)
{
    ui->setupUi(this);
}

InnerWidget::~InnerWidget()
{
    delete ui;
}

void InnerWidget::appendText(const QString& t)
{
    ui->plainTextEdit->appendPlainText(t);
}
