#include "Widget.h"
#include "ui_Widget.h"
#include <QMenu>
Widget::Widget(QWidget* parent) : QWidget(parent), ui(new Ui::Widget)
{
    QColor clr(Qt::blue);
    ui->setupUi(this);
    ui->toolButtonIconOnlyNoIcon->setColor(clr);
    ui->toolButtonIconOnlyNoIcon2->setColor(clr);
    ui->toolButtonIconOnlyHaveIcon->setColor(clr);
    ui->toolButtonIconOnlyHaveIcon2->setColor(clr);
    ui->toolButtonIconOnlyHaveIconSmall->setColor(clr);
    ui->toolButtonIconOnlyHaveIconSmall2->setColor(clr);
    ui->toolButtonIconOnlyHaveIconLarge->setColor(clr);
    ui->toolButtonIconOnlyHaveIconLarge2->setColor(clr);
    QMenu* menu = new QMenu(this);
    menu->addAction("1");
    menu->addAction("2");
    menu->addAction("3");
    menu->addAction("4");
    menu->addAction("5");
    ui->toolButtonIconOnlyHaveIconMenuPopupMode->setColor(clr);
    ui->toolButtonIconOnlyHaveIconMenuPopupMode->setMenu(menu);
    ui->toolButtonIconOnlyHaveIconMenuPopupMode2->setColor(clr);
    ui->toolButtonIconOnlyHaveIconMenuPopupMode2->setMenu(menu);
    ui->toolButtonIconOnlyHaveIconInstantPop->setColor(clr);
    ui->toolButtonIconOnlyHaveIconInstantPop->setMenu(menu);
    ui->toolButtonIconOnlyHaveIconInstantPop2->setColor(clr);
    ui->toolButtonIconOnlyHaveIconInstantPop2->setMenu(menu);
    ui->toolButtonIconOnlyHaveIconDelayedPop->setColor(clr);
    ui->toolButtonIconOnlyHaveIconDelayedPop->setMenu(menu);
    ui->toolButtonIconOnlyHaveIconDelayedPop2->setColor(clr);
    ui->toolButtonIconOnlyHaveIconDelayedPop2->setMenu(menu);

    // Text Only
    ui->toolButtonIconOnlyText->setColor(clr);
    ui->toolButtonIconOnlyText_1->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIcon->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIcon_1->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIconSmall->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIconSmall_1->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIconLarge->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIconLarge_1->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIconMenuPopupMode->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIconMenuPopupMode->setMenu(menu);
    ui->toolButtonIconOnlyTextHaveIconMenuPopupMode_1->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIconMenuPopupMode_1->setMenu(menu);
    ui->toolButtonIconOnlyTextHaveIconDelayedPop->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIconDelayedPop->setMenu(menu);
    ui->toolButtonIconOnlyTextHaveIconDelayedPop_1->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIconDelayedPop_1->setMenu(menu);
    ui->toolButtonIconOnlyTextHaveIconInstantPop->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIconInstantPop->setMenu(menu);
    ui->toolButtonIconOnlyTextHaveIconInstantPop_1->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIconInstantPop_1->setMenu(menu);

    // Text beside
    ui->toolButtonIconTextBeside->setColor(clr);
    ui->toolButtonIconTextBeside1->setColor(clr);
    ui->toolButtonTextBesideHaveIcon->setColor(clr);
    ui->toolButtonTextBesideHaveIcon1->setColor(clr);
    ui->toolButtonTextBesideHaveIconSmall->setColor(clr);
    ui->toolButtonTextBesideHaveIconSmall1->setColor(clr);
    ui->toolButtonTextBesideHaveIconLarge->setColor(clr);
    ui->toolButtonTextBesideHaveIconLarge1->setColor(clr);
    ui->toolButtonTextBesideHaveIconInstantPop->setColor(clr);
    ui->toolButtonTextBesideHaveIconInstantPop1->setColor(clr);
    ui->toolButtonTextBesideHaveIconDelayedPop->setColor(clr);
    ui->toolButtonTextBesideHaveIconDelayedPop1->setColor(clr);
    ui->toolButtonTextBesideMenuPopupMode->setColor(clr);
    ui->toolButtonTextBesideMenuPopupMode1->setColor(clr);
}

Widget::~Widget()
{
    delete ui;
}
