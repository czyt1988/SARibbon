#ifndef SARIBBONQUICKACCESSBAR_H
#define SARIBBONQUICKACCESSBAR_H
#include "SARibbonGlobal.h"
#include "SARibbonButtonGroupWidget.h"
/**
 * \if ENGLISH
 * @brief Ribbon quick access toolbar in the top left corner
 * @details The SARibbonQuickAccessBar class provides a specialized toolbar that is typically placed in the top-left corner of the
 * @details ribbon interface. It contains frequently used commands that users can access quickly without navigating through ribbon tabs.
 * @details This toolbar supports adding regular actions as well as menu actions with different popup modes. The menu actions can
 * @details be configured with various popup behaviors such as InstantPopup, MenuButtonPopup, or DelayedPopup.
 * @details @par Features:
 * @details - Quick access to frequently used commands
 * @details - Support for menu actions with customizable popup modes
 * @details - Easy integration with ribbon interface
 * @details - Automatic management of action lifecycles
 * @details @par Usage:
 * @details @code
 * @details // Create quick access bar
 * @details SARibbonQuickAccessBar *quickAccessBar = ribbonBar()->quickAccessBar();
 * @details 
 * @details // Add regular actions
 * @details quickAccessBar->addAction(saveAction);
 * @details quickAccessBar->addAction(undoAction);
 * @details 
 * @details // Add menu actions
 * @details QMenu *fileMenu = new QMenu("File");
 * @details fileMenu->addAction(newAction);
 * @details fileMenu->addAction(openAction);
 * @details quickAccessBar->addMenuAction(fileMenu, QToolButton::InstantPopup);
 * @details @endcode
 * @details @note This class inherits from QToolBar, so all standard QToolBar features are available
 * @details @note Menu actions are automatically configured with appropriate tool button popup modes
 * @details @see SARibbonBar
 * \endif
 *
 * \if CHINESE
 * @brief ribbon左上顶部的快速响应栏
 * @details 此类提供了一个专门的工具栏，通常放置在ribbon界面的左上角。它包含常用的命令用户可以快速访问这些命令，而无需浏览ribbon选项卡。
 * @details 此工具栏支持添加常规动作以及具有不同弹出模式的菜单动作。菜单动作可以配置各种弹出行为，如InstantPopup、MenuButtonPopup或DelayedPopup。
 * @details @par 特点:
 * @details - 快速访问常用命令
 * @details - 支持具有可自定义弹出模式的菜单动作
 * @details - 与ribbon界面轻松集成
 * @details - 自动管理动作生命周期
 * @details @par 用法:
 * @details @code
 * @details // 创建快速访问栏
 * @details SARibbonQuickAccessBar *quickAccessBar = ribbonBar()->quickAccessBar();
 * @details 
 * @details // 添加常规动作
 * @details quickAccessBar->addAction(saveAction);
 * @details quickAccessBar->addAction(undoAction);
 * @details 
 * @details // 添加菜单动作
 * @details QMenu *fileMenu = new QMenu("File");
 * @details fileMenu->addAction(newAction);
 * @details fileMenu->addAction(openAction);
 * @details quickAccessBar->addMenuAction(fileMenu, QToolButton::InstantPopup);
 * @details @endcode
 * @details @note 此类继承自QToolBar，因此所有标准QToolBar功能都可用
 * @details @note 菜单动作会自动配置适当的工具按钮弹出模式
 * @details @see SARibbonBar
 * \endif
 */
class SA_RIBBON_EXPORT SARibbonQuickAccessBar : public SARibbonButtonGroupWidget
{
    Q_OBJECT
public:
    /// Constructor for SARibbonQuickAccessBar
    explicit SARibbonQuickAccessBar(QWidget* parent = nullptr);
    /// Destructor for SARibbonQuickAccessBar
    ~SARibbonQuickAccessBar();
};

#endif  // SARIBBONQUICKACCESSBAR_H
