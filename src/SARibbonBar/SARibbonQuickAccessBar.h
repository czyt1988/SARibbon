#ifndef SARIBBONQUICKACCESSBAR_H
#define SARIBBONQUICKACCESSBAR_H
#include "SARibbonGlobal.h"
#include "SARibbonCtrlContainer.h"
#include <QMenu>
#include <QToolButton>
class SARibbonToolButton;

///
/// \brief ribbon左上顶部的快速响应栏
///
class SA_RIBBON_EXPORT SARibbonQuickAccessBar : public SARibbonCtrlContainer
{
    Q_OBJECT
    SA_RIBBON_DECLARE_PRIVATE(SARibbonQuickAccessBar)
public:
    SARibbonQuickAccessBar(QWidget* parent = 0);
    ~SARibbonQuickAccessBar();
    void addSeparator();
    void addAction(QAction* act);
    void addWidget(QWidget* w);
    void addMenu(QMenu* m, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
};

#endif  // SARIBBONQUICKACCESSBAR_H
