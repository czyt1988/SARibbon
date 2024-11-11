#ifndef SARIBBONMENU_H
#define SARIBBONMENU_H
#include "SARibbonGlobal.h"
#include <QMenu>

///
/// \brief 用在ribbon的menu
/// 可以通过StyleSheet设置样式而不影响QMenu
///
class SA_RIBBON_EXPORT SARibbonMenu : public QMenu
{
    Q_OBJECT
public:
    explicit SARibbonMenu(QWidget* parent = nullptr);
    explicit SARibbonMenu(const QString& title, QWidget* parent = nullptr);
    ~SARibbonMenu();
    QAction* addRibbonMenu(SARibbonMenu* menu);
    SARibbonMenu* addRibbonMenu(const QString& title);
    SARibbonMenu* addRibbonMenu(const QIcon& icon, const QString& title);
    QAction* addWidget(QWidget* w);
};

#endif  // SARIBBONMENU_H
