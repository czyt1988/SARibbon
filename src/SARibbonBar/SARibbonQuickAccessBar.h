#ifndef SARIBBONQUICKACCESSBAR_H
#define SARIBBONQUICKACCESSBAR_H
#include "SARibbonGlobal.h"
#include "SARibbonCtrlContainer.h"
#include <QMenu>
#include <QToolButton>
class SARibbonToolButton;
class SARibbonButtonGroupWidget;
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
    void addAction(QAction* act, Qt::ToolButtonStyle buttonStyle = Qt::ToolButtonIconOnly, QToolButton::ToolButtonPopupMode popMode = QToolButton::DelayedPopup);
    void addWidget(QWidget* w);
    void addMenu(QMenu* m, Qt::ToolButtonStyle buttonStyle = Qt::ToolButtonIconOnly, QToolButton::ToolButtonPopupMode popMode = QToolButton::InstantPopup);
    //获取内部管理的ButtonGroupWidget
    SARibbonButtonGroupWidget* buttonGroupWidget();
    const SARibbonButtonGroupWidget* buttonGroupWidget() const;
    // icon 尺寸设置 - 注意，如果要自定义iconsize，需要设置
    void setIconSize(const QSize& s);
    QSize iconSize() const;
};

#endif  // SARIBBONQUICKACCESSBAR_H
