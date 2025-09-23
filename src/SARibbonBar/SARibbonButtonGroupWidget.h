#ifndef SARIBBONBUTTONGROUPWIDGET_H
#define SARIBBONBUTTONGROUPWIDGET_H
#include "SARibbonGlobal.h"
#include <QToolButton>
#include <QMenu>
#include <QToolBar>
/**
 * @brief 用于管理一组Action,类似于QToolBar
 */
class SA_RIBBON_EXPORT SARibbonButtonGroupWidget : public QToolBar
{
    Q_OBJECT
public:
    explicit SARibbonButtonGroupWidget(QWidget* parent = nullptr);
    ~SARibbonButtonGroupWidget();

    // 在快速访问工具栏中添加一个带菜单的 QAction
    void addMenuAction(QAction* menuAction, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
    QAction* addMenuAction(QMenu* menu, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
};

#endif  // SARIBBONBUTTONGROUPWIDGET_H
