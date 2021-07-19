#include "SARibbonMenu.h"
#include <QWidgetAction>

SARibbonMenu::SARibbonMenu(QWidget *parent) : QMenu(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


SARibbonMenu::SARibbonMenu(const QString& title, QWidget *parent) : QMenu(title, parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


QAction *SARibbonMenu::addRibbonMenu(SARibbonMenu *menu)
{
    return (QMenu::addMenu(menu));
}


SARibbonMenu *SARibbonMenu::addRibbonMenu(const QString& title)
{
    SARibbonMenu *menu = new SARibbonMenu(title, this);

    return (menu);
}


SARibbonMenu *SARibbonMenu::addRibbonMenu(const QIcon& icon, const QString& title)
{
    SARibbonMenu *menu = new SARibbonMenu(title, this);

    menu->setIcon(icon);
    return (menu);
}


QAction *SARibbonMenu::addWidget(QWidget *w)
{
    QWidgetAction *action = new QWidgetAction(this);

    action->setDefaultWidget(w);
    addAction(action);
    return (action);
}
