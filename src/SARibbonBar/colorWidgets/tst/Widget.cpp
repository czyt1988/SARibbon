#include "Widget.h"
#include "ui_Widget.h"
#include <QMenu>
Widget::Widget(QWidget* parent) : QWidget(parent), ui(new Ui::Widget)
{
    ui->setupUi(this);
    ui->toolButtonIconOnlyNoIcon->setColor(Qt::blue);
    ui->toolButtonIconOnlyNoIcon2->setColor(Qt::blue);
    ui->toolButtonIconOnlyHaveIcon->setColor(Qt::blue);
    ui->toolButtonIconOnlyHaveIcon2->setColor(Qt::blue);
    ui->toolButtonIconOnlyHaveIconSmall->setColor(Qt::blue);
    ui->toolButtonIconOnlyHaveIconSmall2->setColor(Qt::blue);
    ui->toolButtonIconOnlyHaveIconLarge->setColor(Qt::blue);
    ui->toolButtonIconOnlyHaveIconLarge2->setColor(Qt::blue);
    QMenu* menu = new QMenu(this);
    menu->addAction("1");
    menu->addAction("2");
    menu->addAction("3");
    menu->addAction("4");
    menu->addAction("5");
    ui->toolButtonIconOnlyHaveIconMenuPopupMode->setColor(Qt::blue);
    ui->toolButtonIconOnlyHaveIconMenuPopupMode->setMenu(menu);
    ui->toolButtonIconOnlyHaveIconMenuPopupMode2->setColor(Qt::blue);
    ui->toolButtonIconOnlyHaveIconMenuPopupMode2->setMenu(menu);
}

Widget::~Widget()
{
    delete ui;
}
