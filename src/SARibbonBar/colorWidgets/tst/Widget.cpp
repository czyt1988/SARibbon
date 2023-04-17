#include "Widget.h"
#include "ui_Widget.h"

Widget::Widget(QWidget* parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->toolButtonIconOnlyNoIcon->setColor(Qt::blue);
    ui->toolButtonIconOnlyHaveIcon->setColor(Qt::blue);
    ui->toolButtonIconOnlyHaveIconSmall->setColor(Qt::blue);
    ui->toolButtonIconOnlyHaveIconLarge->setColor(Qt::blue);
}

Widget::~Widget()
{
    delete ui;
}
