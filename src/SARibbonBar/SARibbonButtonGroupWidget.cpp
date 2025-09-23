#include "SARibbonButtonGroupWidget.h"
#include <QHBoxLayout>
#include <QDebug>
#include <QMargins>
#include <QChildEvent>
#include <QActionEvent>
#include <QWidgetAction>
#include <QApplication>
#include <QStyle>

//===================================================
// SARibbonButtonGroupWidget
//===================================================

SARibbonButtonGroupWidget::SARibbonButtonGroupWidget(QWidget* parent) : QToolBar(parent)
{
    setAutoFillBackground(false);
    setAttribute(Qt::WA_NoSystemBackground);
    setOrientation(Qt::Horizontal);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    setMovable(false);    // 禁止移动
    setFloatable(false);  // 禁止浮动
    setContentsMargins(0, 0, 0, 0);
    const int smallIconSize = QApplication::style()->pixelMetric(QStyle::PM_SmallIconSize);
    setIconSize(QSize(smallIconSize, smallIconSize));
}

SARibbonButtonGroupWidget::~SARibbonButtonGroupWidget()
{
}

/**
 * @brief Add a menu QAction to the button group widget/在按钮栏中添加一个带菜单的 QAction
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
 * // Add to button group widget/添加到按钮栏
 * buttongroup->addMenuAction(fileAction, QToolButton::InstantPopup);
 * @endcode
 *
 * @see QToolButton::ToolButtonPopupMode
 */
void SARibbonButtonGroupWidget::addMenuAction(QAction* menuAction, QToolButton::ToolButtonPopupMode popupMode)
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
 * @brief Create and add a menu action to the button group widget (text only
 * version)/在按钮栏中创建并添加一个带菜单的动作（仅文本版本）
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
 * // Add menu to button group widget with custom text/使用自定义文本添加菜单到按钮栏
 * QAction *helpAction = buttongroup->addMenuAction("Help", helpMenu, QToolButton::MenuButtonPopup);
 * @endcode
 *
 * @see QToolButton::ToolButtonPopupMode
 */
QAction* SARibbonButtonGroupWidget::addMenuAction(QMenu* menu, QToolButton::ToolButtonPopupMode popupMode)
{
    if (!menu) {
        return nullptr;
    }
    addMenuAction(menu->menuAction(), popupMode);
    return menu->menuAction();
}
