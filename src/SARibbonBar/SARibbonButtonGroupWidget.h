#ifndef SARIBBONBUTTONGROUPWIDGET_H
#define SARIBBONBUTTONGROUPWIDGET_H
#include "SARibbonGlobal.h"
#include <QToolButton>
#include <QMenu>
#include <QToolBar>
/**
 * \if ENGLISH
 * @brief Widget for managing a group of actions, similar to QToolBar
 * @details This class provides a widget to manage a group of actions, similar to QToolBar, but with additional functionality for menu actions.
 * \endif
 *
 * \if CHINESE
 * @brief 用于管理一组Action,类似于QToolBar
 * @details 此类提供一个小部件来管理一组动作，类似于QToolBar，但具有菜单动作的附加功能。
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonButtonGroupWidget : public QToolBar
{
    Q_OBJECT
public:
    /// Constructor for SARibbonButtonGroupWidget
    explicit SARibbonButtonGroupWidget(QWidget* parent = nullptr);
    /// Destructor for SARibbonButtonGroupWidget
    ~SARibbonButtonGroupWidget();

    /// Add a menu QAction to the button group widget
    void addMenuAction(QAction* menuAction, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
    /// Create and add a menu action to the button group widget
    QAction* addMenuAction(QMenu* menu, QToolButton::ToolButtonPopupMode popupMode = QToolButton::InstantPopup);
};

#endif  // SARIBBONBUTTONGROUPWIDGET_H
