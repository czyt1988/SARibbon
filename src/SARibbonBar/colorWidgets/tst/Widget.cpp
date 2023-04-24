#include "Widget.h"
#include "ui_Widget.h"
#include <QMenu>
#include <cstdlib>
#include <iostream>
#include <ctime>
#include <QDebug>
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
    ui->toolButtonIconOnlyHaveIconMenuPopupMode2->setColor(clr);
    ui->toolButtonIconOnlyHaveIconInstantPop->setColor(clr);
    ui->toolButtonIconOnlyHaveIconInstantPop2->setColor(clr);
    ui->toolButtonIconOnlyHaveIconDelayedPop->setColor(clr);
    ui->toolButtonIconOnlyHaveIconDelayedPop2->setColor(clr);

    ui->toolButtonIconOnlyHaveIconMenuPopupMode->setMenu(menu);
    ui->toolButtonIconOnlyHaveIconMenuPopupMode2->setMenu(menu);
    ui->toolButtonIconOnlyHaveIconInstantPop->setMenu(menu);
    ui->toolButtonIconOnlyHaveIconInstantPop2->setMenu(menu);
    ui->toolButtonIconOnlyHaveIconDelayedPop->setMenu(menu);
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
    ui->toolButtonIconOnlyTextHaveIconMenuPopupMode_1->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIconDelayedPop->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIconDelayedPop_1->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIconInstantPop->setColor(clr);
    ui->toolButtonIconOnlyTextHaveIconInstantPop_1->setColor(clr);

    ui->toolButtonIconOnlyTextHaveIconMenuPopupMode->setMenu(menu);
    ui->toolButtonIconOnlyTextHaveIconMenuPopupMode_1->setMenu(menu);
    ui->toolButtonIconOnlyTextHaveIconDelayedPop->setMenu(menu);
    ui->toolButtonIconOnlyTextHaveIconDelayedPop_1->setMenu(menu);
    ui->toolButtonIconOnlyTextHaveIconInstantPop->setMenu(menu);
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

    // TextUnderIcon
    ui->toolButtonTextUnderIcon->setColor(clr);
    ui->toolButtonTextUnderIcon1->setColor(clr);
    ui->toolButtonTextUnderIconHaveIcon->setColor(clr);
    ui->toolButtonTextUnderIconHaveIcon1->setColor(clr);
    ui->toolButtonTextUnderIconHaveIconSmall->setColor(clr);
    ui->toolButtonTextUnderIconHaveIconSmall1->setColor(clr);
    ui->toolButtonTextUnderIconHaveIconLarge->setColor(clr);
    ui->toolButtonTextUnderIconHaveIconLarge1->setColor(clr);
    ui->toolButtonTextUnderIconMenuPopupMode->setColor(clr);
    ui->toolButtonTextUnderIconMenuPopupMode1->setColor(clr);
    ui->toolButtonTextUnderIconInstantPop->setColor(clr);
    ui->toolButtonTextUnderIconInstantPop1->setColor(clr);
    ui->toolButtonTextUnderIconDelayedPop->setColor(clr);
    ui->toolButtonTextUnderIconDelayedPop1->setColor(clr);
    //
    QList< QColor > clrs;
    for (int i = 0; i < 8; ++i) {
        clrs.append(makeRandColor());
    }
    ui->colorGrid1->setColumnCount(0);
    ui->colorGrid1->setColorList(clrs);

    QList< QColor > clrs2;
    for (int i = 0; i < 30; ++i) {
        clrs2.append(makeRandColor());
    }
    ui->colorGrid2->setColumnCount(5);
    ui->colorGrid2->setColorList(clrs2);
}

Widget::~Widget()
{
    delete ui;
}

QColor Widget::makeRandColor() const
{
    QColor res;
    std::srand(std::time(nullptr));
    res.setRed(std::rand() * 255 / RAND_MAX);
    res.setGreen(std::rand() * 255 / RAND_MAX);
    res.setBlue(std::rand() * 255 / RAND_MAX);
    qDebug() << std::rand() << " " << std::rand() << " " << std::rand();
    return res;
}
