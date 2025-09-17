#ifndef SARIBBONQUICKACCESSBAR_H
#define SARIBBONQUICKACCESSBAR_H
#include "SARibbonGlobal.h"
#include "SARibbonButtonGroupWidget.h"
/**
 * @brief Ribbon quick access toolbar in the top left corner/ribbon左上顶部的快速响应栏
 *
 * The SARibbonQuickAccessBar class provides a specialized toolbar that is typically placedin the top-left corner of the
 * ribbon interface. It contains frequently used commands that users can access quickly without navigating through ribbon tabs.
 *
 * This toolbar supports adding regular actions as well as menu actions with different popup modes. The menu actions can
 * be configured with various popup behaviors such as InstantPopup, MenuButtonPopup, or DelayedPopup.
 *
 * 此类提供了一个专门的工具栏，通常放置在ribbon界面的左上角。它包含常用的命令用户可以快速访问这些命令，而无需浏览ribbon选项卡。
 *
 * 此工具栏支持添加常规动作以及具有不同弹出模式的菜单动作。菜单动作可以配置各种弹出行为，如InstantPopup、MenuButtonPopup或DelayedPopup。
 *
 * @par Features:/特点:
 * - Quick access to frequently used commands/快速访问常用命令
 * - Support for menu actions with customizable popup modes/支持具有可自定义弹出模式的菜单动作
 * - Easy integration with ribbon interface/与ribbon界面轻松集成
 * - Automatic management of action lifecycles/自动管理动作生命周期
 *
 * @par Usage:/用法:
 * @code
 * // Create quick access bar/创建快速访问栏
 * SARibbonQuickAccessBar *quickAccessBar = ribbonBar()->quickAccessBar();
 *
 * // Add regular actions/添加常规动作
 * quickAccessBar->addAction(saveAction);
 * quickAccessBar->addAction(undoAction);
 *
 * // Add menu actions/添加菜单动作
 * QMenu *fileMenu = new QMenu("File");
 * fileMenu->addAction(newAction);
 * fileMenu->addAction(openAction);
 * quickAccessBar->addMenuAction(fileMenu, QToolButton::InstantPopup);
 * @endcode
 *
 * @note This class inherits from QToolBar, so all standard QToolBar features are available/此类继承自QToolBar，因此所有标准QToolBar功能都可用
 * @note Menu actions are automatically configured with appropriate tool button popup modes/菜单动作会自动配置适当的工具按钮弹出模式
 *
 * @see SARibbonBar
 */
class SA_RIBBON_EXPORT SARibbonQuickAccessBar : public SARibbonButtonGroupWidget
{
    Q_OBJECT
public:
    explicit SARibbonQuickAccessBar(QWidget* parent = nullptr);
    ~SARibbonQuickAccessBar();
};

#endif  // SARIBBONQUICKACCESSBAR_H
