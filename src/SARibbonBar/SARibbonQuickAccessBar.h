#ifndef SARIBBONQUICKACCESSBAR_H
#define SARIBBONQUICKACCESSBAR_H
#include "SARibbonGlobal.h"
#include "SARibbonCtrlContainer.h"
#include <QMenu>
#include <QToolButton>
class SARibbonToolButton;
class SARibbonQuickAccessBarPrivate;
///
/// \brief ribbon左上顶部的快速响应栏
///
class SA_RIBBON_EXPORT SARibbonQuickAccessBar : public SARibbonCtrlContainer
{
    Q_OBJECT
public:
    SARibbonQuickAccessBar(QWidget *parent = 0);
    ~SARibbonQuickAccessBar();
    void addSeparator();
    void addAction(QAction *act);
    void addWidget(QWidget *w);
    void addMenu(QMenu *m, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);

protected:
    virtual void initStyleOption(QStyleOption *opt);

private:
    SARibbonQuickAccessBarPrivate *m_d;
};

#endif // SARIBBONQUICKACCESSBAR_H
