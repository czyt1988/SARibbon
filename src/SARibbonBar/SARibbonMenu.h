#ifndef SARIBBONMENU_H
#define SARIBBONMENU_H
#include "SARibbonGlobal.h"
#include <QMenu>

///
/// \brief 用在ribbon的menu
///
class SA_RIBBON_EXPORT SARibbonMenu : public QMenu
{
    Q_OBJECT
public:
    explicit SARibbonMenu(QWidget *parent = Q_NULLPTR);
    explicit SARibbonMenu(const QString& title, QWidget *parent = Q_NULLPTR);
    QAction *addRibbonMenu(SARibbonMenu *menu);
    SARibbonMenu *addRibbonMenu(const QString& title);
    SARibbonMenu *addRibbonMenu(const QIcon& icon, const QString& title);
    QAction *addWidget(QWidget *w);
};

#endif // SARIBBONMENU_H
