#include "SARibbonQuickAccessBar.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonSeparatorWidget.h"
#include <QGuiApplication>

//===================================================
// SARibbonQuickAccessBar
//===================================================
SARibbonQuickAccessBar::SARibbonQuickAccessBar(QWidget* parent) : QToolBar(parent)
{
    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground);
    setOrientation(Qt::Horizontal);
    setMovable(false);    // 禁止移动
    setFloatable(false);  // 禁止浮动
}

SARibbonQuickAccessBar::~SARibbonQuickAccessBar()
{
}

/**
 * @brief Add a menu QAction to the quick access toolbar/在快速访问工具栏中添加一个带菜单的 QAction
 *
 * This function is used to add a QAction that has been associated with a menu to the toolbar and set its popup
 * mode.The popup mode of the tool button determines how users interact with the menu.
 *
 * 此函数用于向工具栏添加一个已经关联了菜单的 QAction，并设置其弹出模式。工具按钮的弹出模式决定了用户如何与菜单进行交互。
 *
 * @param menuAction QAction object that has been set with a menu/已经设置了菜单的 QAction 对象
 * @param popupMode Popup mode of the tool button, default is InstantPopup/工具按钮的弹出模式，默认为 InstantPopup
 *
 * @note If menuAction is not associated with a menu, this function will not perform any special processing/如果
 * menuAction 没有关联菜单，此函数不会进行任何特殊处理
 *
 * @note This function will automatically find the corresponding QToolButton and set the popup
 * mode/此函数会自动查找对应的 QToolButton 并设置弹出模式
 *
 * @par Example:/示例:
 * @code
 * QMenu *fileMenu = new QMenu("File");
 * fileMenu->addAction("New");
 * fileMenu->addAction("Open");
 *
 * QAction *fileAction = new QAction("File");
 * fileAction->setMenu(fileMenu);
 *
 * // Add to quick access toolbar/添加到快速访问工具栏
 * quickAccessBar->addMenuAction(fileAction, QToolButton::InstantPopup);
 * @endcode
 *
 * @see QToolButton::ToolButtonPopupMode
 */
void SARibbonQuickAccessBar::addMenuAction(QAction* menuAction, QToolButton::ToolButtonPopupMode popupMode)
{
    if (!menuAction) {
        return;
    }

    // 添加动作到工具栏
    addAction(menuAction);

    // 如果动作有关联菜单，则设置工具按钮的弹出模式
    if (menuAction->menu()) {
        QToolButton* toolButton = qobject_cast< QToolButton* >(widgetForAction(menuAction));
        if (toolButton) {
            toolButton->setPopupMode(popupMode);
        }
    }
}

/**
 * @brief Create and add a menu action to the quick access toolbar/在快速访问工具栏中创建并添加一个带菜单的动作
 *
 * This function creates a new QAction, associates it with the specified menu,adds it to the toolbar,
 * and sets the corresponding popup mode.
 *
 * 此函数会创建一个新的 QAction，将其与指定的菜单关联，添加到工具栏中，并设置相应的弹出模式。
 *
 * @param text Button text/按钮文本
 * @param icon Button icon/按钮图标
 * @param menu Menu object to associate/要关联的菜单对象
 * @param popupMode Popup mode of the tool button, default is InstantPopup/工具按钮的弹出模式，默认为 InstantPopup
 * @return Returns the newly created QAction object, or nullptr if the parameter is invalid/返回新创建的 QAction 对象，如果参数无效则返回 nullptr
 *
 * @note The caller does not need to manually manage the lifecycle of the returned QAction,it will be automatically managed
 * by the toolbar/调用者不需要手动管理返回的 QAction 的生命周期，它会由工具栏自动管理
 * @note If the menu has a title, it will be automatically used as the QAction's text/如果菜单有标题，会自动用作 QAction 的文本
 *
 * @par Example:/示例:
 * @code
 * // Create menu/创建菜单
 * QMenu *editMenu = new QMenu("Edit");
 * editMenu->addAction("Cut");
 * editMenu->addAction("Copy");
 * editMenu->addAction("Paste");
 *
 * // Directly add menu to quick access toolbar/直接添加菜单到快速访问工具栏
 * QAction *editAction = quickAccessBar->addMenuAction("Edit",QIcon(":/icons/edit.png"),editMenu, QToolButton::MenuButtonPopup);
 * // You can further configure the returned action/可以进一步配置返回的动作
 * @endcode
 *
 * @see QToolButton::ToolButtonPopupMode
 */
QAction* SARibbonQuickAccessBar::addMenuAction(const QString& text,
                                               const QIcon& icon,
                                               QMenu* menu,
                                               QToolButton::ToolButtonPopupMode popupMode)
{
    if (!menu) {
        return nullptr;
    }

    // 创建新的 QAction
    QAction* menuAction = new QAction(icon, text, this);
    menuAction->setMenu(menu);

    // 添加到工具栏并设置弹出模式
    addMenuAction(menuAction, popupMode);

    return menuAction;
}

/**
 * @brief Create and add a menu action to the quick access toolbar (simplified version)/在快速访问工具栏中创建并添加一个带菜单的动作（简化版本）
 *
 * This function creates a new QAction based on the menu's title and icon,
 * associates it with the specified menu, adds it to the toolbar, and sets the popup mode.
 *
 * 此函数根据菜单的标题和图标创建一个新的 QAction，将其与指定菜单关联，添加到工具栏并设置弹出模式。
 *
 * @param menu Menu object to associate/要关联的菜单对象
 * @param popupMode Popup mode of the tool button, default is InstantPopup/工具按钮的弹出模式，默认为 InstantPopup
 * @return Returns the newly created QAction object, or nullptr if the parameter is invalid/返回新创建的 QAction 对象，如果参数无效则返回 nullptr
 *
 * @note The caller does not need to manually manage the lifecycle of the returned QAction,it will be automatically managed
 * by the toolbar/调用者不需要手动管理返回的 QAction 的生命周期，它会由工具栏自动管理
 *
 * @par Example:/示例:
 * @code
 * // Create menu/创建菜单
 * QMenu *viewMenu = new QMenu("View");
 * viewMenu->addAction("Toolbars");
 * viewMenu->addAction("Status Bar");
 *
 * // Add menu to quick access toolbar using menu's title and icon/使用菜单的标题和图标添加到快速访问工具栏
 * QAction *viewAction = quickAccessBar->addMenuAction(viewMenu, QToolButton::InstantPopup);
 * @endcode
 *
 * @see QToolButton::ToolButtonPopupMode
 */
QAction* SARibbonQuickAccessBar::addMenuAction(const QString& text, QMenu* menu, QToolButton::ToolButtonPopupMode popupMode)
{
    if (!menu) {
        return nullptr;
    }
    return addMenuAction(text, menu->icon(), menu, popupMode);
}

/**
 * @brief Create and add a menu action to the quick access toolbar (text only
 * version)/在快速访问工具栏中创建并添加一个带菜单的动作（仅文本版本）
 *
 * This function creates a new QAction based on the specified text,
 * associates it with the specified menu, adds it to the toolbar, and sets the popup mode.
 *
 * 此函数根据指定的文本创建一个新的 QAction，将其与指定菜单关联，添加到工具栏并设置弹出模式。
 *
 * @param text Button text/按钮文本
 * @param menu Menu object to associate/要关联的菜单对象
 * @param popupMode Popup mode of the tool button, default is InstantPopup/工具按钮的弹出模式，默认为 InstantPopup
 * @return Returns the newly created QAction object, or nullptr if the parameter is invalid/返回新创建的 QAction
 * 对象，如果参数无效则返回 nullptr
 *
 * @note The caller does not need to manually manage the lifecycle of the returned QAction,it will be automatically
 * managed by the toolbar/调用者不需要手动管理返回的 QAction 的生命周期， 它会由工具栏自动管理
 *
 * @par Example:/示例:
 * @code
 * // Create menu/创建菜单
 * QMenu *helpMenu = new QMenu("Help");
 * helpMenu->addAction("Contents");
 * helpMenu->addAction("About");
 *
 * // Add menu to quick access toolbar with custom text/使用自定义文本添加菜单到快速访问工具栏
 * QAction *helpAction = quickAccessBar->addMenuAction("Help", helpMenu, QToolButton::MenuButtonPopup);
 * @endcode
 *
 * @see QToolButton::ToolButtonPopupMode
 */
QAction* SARibbonQuickAccessBar::addMenuAction(QMenu* menu, QToolButton::ToolButtonPopupMode popupMode)
{
    if (!menu) {
        return nullptr;
    }
    return addMenuAction(menu->title(), menu->icon(), menu, popupMode);
}
