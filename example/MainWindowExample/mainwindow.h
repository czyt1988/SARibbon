#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "SARibbonMainWindow.h"
#include <QTimer>
class SARibbonCategory;
class SARibbonContextCategory;
class SARibbonCustomizeWidget;
class SARibbonActionsManager;
class SARibbonQuickAccessBar;
class SARibbonButtonGroupWidget;
class SARibbonPanel;
class SARibbonApplicationWidget;
class SARibbonGalleryGroup;
class QTextEdit;
class QComboBox;
class QCloseEvent;
class QLineEdit;

namespace Ui
{
class MainWindow;
}

/**
 * \if ENGLISH
 * @brief Main window class demonstrating SARibbon library usage
 * @details This class provides a comprehensive example of how to use the SARibbon library,
 *          including creating ribbon bars, panels, categories, and various ribbon controls.
 *          It serves as a learning resource for developers who want to integrate SARibbon
 *          into their Qt applications.
 *
 *          Key features demonstrated:
 *          - Ribbon bar creation and configuration
 *          - Category and panel management
 *          - Context categories for contextual UI
 *          - Quick access bar customization
 *          - Button group widgets
 *          - Application button and menu
 *          - Actions manager for customization
 *          - Theme and style customization
 *
 * @code
 * // Example usage:
 * MainWindow* window = new MainWindow();
 * window->show();
 * @endcode
 * \endif
 *
 * \if CHINESE
 * @brief 主窗口类，演示SARibbon库的使用方法
 * @details 该类提供了如何使用SARibbon库的综合示例，包括创建Ribbon栏、面板、分类和各种Ribbon控件。
 *          它作为开发者将SARibbon集成到Qt应用程序中的学习资源。
 *
 *          演示的主要功能：
 *          - Ribbon栏的创建和配置
 *          - 分类和面板管理
 *          - 上下文分类用于上下文相关UI
 *          - 快速访问栏自定义
 *          - 按钮组控件
 *          - 应用程序按钮和菜单
 *          - 动作管理器用于自定义
 *          - 主题和样式自定义
 *
 * @code
 * // 使用示例：
 * MainWindow* window = new MainWindow();
 * window->show();
 * @endcode
 * \endif
 */
class MainWindow : public SARibbonMainWindow
{
    Q_OBJECT
public:
    // Constructor that initializes the main window with optional parent widget
    explicit MainWindow(QWidget* par = nullptr);

    // Destructor that cleans up resources
    ~MainWindow();

private:
    // Creates the application button (top-left corner button) with dropdown menu
    void createRibbonApplicationButton();

    // Creates the main category with essential ribbon controls and panels
    void createCategoryMain(SARibbonCategory* page);

    // Creates the "Other" category demonstrating additional ribbon features
    void createCategoryOther(SARibbonCategory* page);

    // Creates the "Delete" category demonstrating dynamic panel/category removal
    void createCategoryDelete(SARibbonCategory* page);

    // Creates the "Size" category for testing ribbon element sizing
    void createCategorySize(SARibbonCategory* page);

    // Creates the "Color" category for theme and color customization
    void createCategoryColor(SARibbonCategory* page);

    // Creates the first context category (appears conditionally based on context)
    void createContextCategory1();

    // Creates the second context category demonstrating multiple context categories
    void createContextCategory2();

    // Creates the first page content for context category 1
    void createContextCategoryPage1(SARibbonCategory* page);

    // Creates the second page content for context category 2
    void createContextCategoryPage2(SARibbonCategory* page);

    // Creates the quick access toolbar (top-left mini toolbar)
    void createQuickAccessBar();

    // Creates the right button group in the ribbon bar title area
    void createRightButtonGroup();

    // Creates window button group bar (minimize, maximize, close buttons area)
    void createWindowButtonGroupBar();

    // Creates the actions manager for ribbon customization support
    void createActionsManager();

    // Creates an action with text, icon, and object name (for action manager registration)
    QAction* createAction(const QString& text, const QString& iconurl, const QString& objName);

    // Creates an action with text and icon (auto-generates object name)
    QAction* createAction(const QString& text, const QString& iconurl);

    // Creates additional actions not directly placed on ribbon
    void createOtherActions();

    // Initializes the complete user interface
    void initUI();

private Q_SLOTS:
    // Test the checkable function in menu button popup mode
    void onMenuButtonPopupCheckableTest(bool checked);

    // Test the checkable function in instant popup mode
    void onInstantPopupCheckableTest(bool checked);

    // Test the checkable function in delayed popup mode
    void onDelayedPopupCheckableTest(bool checked);

    // Handle triggered signal for menu button popup checkable action
    void onMenuButtonPopupCheckabletriggered(bool checked);

    // Handle triggered signal for instant popup checkable action
    void onInstantPopupCheckabletriggered(bool checked);

    // Handle triggered signal for delayed popup checkable action
    void onDelayedPopupCheckabletriggered(bool checked);

    // Show or hide the context category
    void onShowContextCategory(bool visible);

    // Handle ribbon style button click
    void onStyleClicked(int styleId);

    // Open the ribbon customization dialog
    void onActionCustomizeTriggered();

    // Open customization dialog and save settings to file
    void onActionCustomizeAndSaveTriggered();

    // Open customization dialog, save and apply settings
    void onActionCustomizeAndSaveWithApplyTriggered();

    // Open help dialog or documentation
    void onActionHelpTriggered();

    // Remove or restore the application button
    void onActionRemoveAppBtnTriggered(bool removeButton);

    // Load and apply a QSS stylesheet file
    void onActionUseQssTriggered();

    // Load a customization XML file for ribbon layout
    void onActionLoadCustomizeXmlFileTriggered();

    // Toggle normal window flag buttons visibility
    void onActionWindowFlagNormalButtonTriggered(bool showButtons);

    // Handle font change from font combo box
    void onFontComWidgetCurrentFontChanged(const QFont& newFont);

    // Increase the font size
    void onActionFontLargerTriggered();

    // Decrease the font size
    void onActionFontSmallerTriggered();

    // Toggle word wrap mode
    void onActionWordWrapTriggered(bool enableWordWrap);

    // Handle button group action trigger
    void onButtonGroupActionTriggered(QAction* triggeredAction);

    // Handle color button color selection
    void onColorButtonColorClicked(const QColor& selectedColor, bool clicked);

    // Handle ribbon theme combo box selection change
    void onRibbonThemeComboBoxCurrentIndexChanged(int themeIndex);

    // Hide or show specific actions
    void onActionHideActionTriggered(bool hideActions);

    // Make all actions visible
    void onActionVisibleAllTriggered(bool visibleAll);

    // Handle center alignment checkbox click
    void onCheckBoxAlignmentCenterClicked(bool centerAligned);

    // Change the ribbon title height
    void onSpinBoxRibbonTitleHeightChanged(int newHeight);

    // Change the ribbon tab height
    void onSpinBoxRibbonTabHeightChanged(int newHeight);

    // Change the ribbon category height
    void onSpinBoxRibbonCategoryHeightChanged(int newHeight);

    // Change the ribbon panel title height
    void onSpinBoxRibbonPanelTtitleHeightChanged(int newHeight);

    // Change the ribbon panel spacing
    void onSpinBoxRibbonPanelSpacingChanged(int newSpacing);

    // Change the panel small icon size
    void onSpinBoxPanelSmallIconSizeChanged(int newSize);

    // Change the panel large icon size
    void onSpinBoxPanelLargeIconSizeChanged(int newSize);

    // Change the tool button maximum aspect ratio
    void onDoubleSpinBoxToolbuttonMaximumAspectRatioChanged(double newRatio);

    // Timer timeout handler for title background color changes
    void onTitleBackgroundBrushChangedTimeout();

    // Timer timeout handler for title changes
    void onChangedTitleTimeout();

    // Handle save action trigger
    void onSaveActionTriggered();

    // Hide or show the entire ribbon bar
    void onHideRibbonActionTriggered(bool hide);

    // Show or hide the hide button
    void onShowHideButtonActionTriggered(bool show);

    // Show or hide the title icon
    void onShowTitleIconActionTriggered(bool show);

    // Expand or collapse the application button
    void onAppButtonExpandActionTriggered(bool expand);

    // Show or hide context category
    void onShowContextActionTriggered(bool show);

    // Delete the current context category
    void onDeleteContextActionTriggered();

    // Demonstrate word wrap feature in ribbon buttons
    void onWordWrapDemoActionTriggered();

    // Demonstrate word wrap with menu feature
    void onWordWrapWithMenuActionTriggered();

    // Remove a panel from a category
    void onRemovePanelActionTriggered(SARibbonCategory* category, SARibbonPanel* panel);

    // Remove a category from the ribbon bar
    void onRemoveCategoryActionTriggered(SARibbonCategory* category);

    // Insert a panel at the start of a category
    void onInsertPanelAtStartActionTriggered(SARibbonCategory* category);

    // Insert a panel at the end of a category
    void onInsertPanelAtEndActionTriggered(SARibbonCategory* category);

    // Insert a panel at position -1 (demonstrates edge case)
    void onInsertPanelAtNegativeOneActionTriggered(SARibbonCategory* category);

    // Unlock disabled actions for testing
    void onUnlockActionTriggered();

    // Toggle action text visibility
    void onSetTextActionToggled(bool checked);

    // Toggle panel visibility
    void onShowPanelActionToggled(bool checked);

    // Hide action 2 for visibility demonstration
    void onHideAction2Triggered(bool checked);

    // Hide action 4 for visibility demonstration
    void onHideAction4Triggered(bool checked);

    // Delete the specified category
    void onDeleteThisCategoryActionTriggered(SARibbonCategory* category);

    // Handle undo action trigger
    void onUndoActionTriggered();

    // Handle redo action trigger
    void onRedoActionTriggered();

    // Handle search editor editing finished signal
    void onSearchEditorEditingFinished();

    // Handle login action trigger
    void onLoginActionTriggered();

    // Add a new item to the gallery group
    void onAddGalleryItemActionTriggered(SARibbonGalleryGroup* galleryGroup);

    // Change the application language
    void onChangeLanguageActionTriggered(bool toEnglish);

    // Show or hide context category 1
    void onContextCategory1ActionTriggered(bool visible);

    // Show or hide context category 2
    void onContextCategory2ActionTriggered(bool visible);

    // Toggle RTL layout direction
    void onTriggeredLayoutDirection();

    // Set ribbon alignment to left
    void onSetAlignmentLeft();

    // Set ribbon alignment to center
    void onSetAlignmentCenter();

    // Set ribbon alignment to right
    void onSetAlignmentRight();

    // Set ribbon style - Loose Three Row
    void onSetStyleLooseThreeRow();

    // Set ribbon style - Loose Two Row
    void onSetStyleLooseTwoRow();

    // Set ribbon style - Compact Three Row
    void onSetStyleCompactThreeRow();

    // Set ribbon style - Compact Two Row
    void onSetStyleCompactTwoRow();
    // Set ribbon style - Loose Single Row
    void onSetStyleLooseSingleRow();
    // Set ribbon style - Compact Single Row
    void onSetStyleCompactSingleRow();

protected:
    // Handles the close event for the main window
    void closeEvent(QCloseEvent* closeEvent) override;

private:
    SARibbonContextCategory* mContextCategory { nullptr };  ///< First context category for contextual ribbon interface
    SARibbonContextCategory* mContextCategory2 { nullptr };  ///< Second context category demonstrating multiple context categories
    SARibbonCustomizeWidget* mWidgetForCustomize { nullptr };  ///< Widget for ribbon customization dialog
    SARibbonActionsManager* mActionsManager { nullptr };  ///< Manager for all ribbon actions (used in customization)
    QMenu* mMenuApplicationBtn { nullptr };               ///< Menu for the application button (top-left corner)
    QComboBox* mComboboxRibbonTheme { nullptr };          ///< Combo box for selecting ribbon themes
    QLineEdit* mSearchEditor { nullptr };                 ///< Search editor in the ribbon bar
    SARibbonApplicationWidget* mAppWidget { nullptr };    ///< Application widget shown when clicking app button
    QAction* mActionWordWrap { nullptr };                 ///< Action for word wrap toggle
    QAction* mActionDisable { nullptr };                  ///< Action for testing disabled state
    QAction* mActionUnlock { nullptr };                   ///< Action for unlocking disabled actions
    QAction* mActionSetTextTest { nullptr };              ///< Action for testing set text functionality
    QAction* mActionShowTest { nullptr };                 ///< Action for testing show functionality
    QAction* mActionHideAction2 { nullptr };              ///< Action 2 for hide/show demonstration
    QAction* mActionHideAction4 { nullptr };              ///< Action 4 for hide/show demonstration
    QAction* mActionHideShowTextAct2 { nullptr };         ///< Action for hide/show text demonstration 2
    QAction* mActionHideShowTextAct3 { nullptr };         ///< Action for hide/show text demonstration 3
    QAction* mActionHideShowTextAct4 { nullptr };         ///< Action for hide/show text demonstration 4
    QAction* mOtherAction1 { nullptr };                   ///< Other action 1 for demonstration
    QAction* mOtherAction2 { nullptr };                   ///< Other action 2 for demonstration
    QAction* mOtherAction3 { nullptr };                   ///< Other action 3 for demonstration
    QAction* mOtherAction4 { nullptr };                   ///< Other action 4 for demonstration
    QAction* mOtherAction5 { nullptr };                   ///< Other action 5 for demonstration
    QAction* mOtherActionIcon1 { nullptr };               ///< Other action with icon for demonstration
    SARibbonPanel* mPanelVisbileExample { nullptr };      ///< Panel for visibility example demonstration
    QAction* mActionVisibleAll { nullptr };               ///< Action to make all items visible
    bool mHasApplyCustomizeXmlFile { false };             ///< Flag indicating if customize XML file has been applied
    int mTagForActionText;                                ///< Tag for action text in actions manager
    int mTagForActionIcon;                                ///< Tag for action icon in actions manager
    QTimer mChangeTitleBkColorTimer;                      ///< Timer for changing title background color
    QAction* mActionDeleteContext { nullptr };            ///< Action for deleting context category
    QAction* mActionToggleRTL { nullptr };                 ///< Action for toggling RTL layout direction
    QAction* mActionAlignLeft { nullptr };                   ///< Action for setting alignment to left
    QAction* mActionAlignCenter { nullptr };               ///< Action for setting alignment to center
    QAction* mActionAlignRight { nullptr };                ///< Action for setting alignment to right
    QAction* mActionStyleLooseThreeRow { nullptr };        ///< Action for setting style to Loose Three Row
    QAction* mActionStyleLooseTwoRow { nullptr };          ///< Action for setting style to Loose Two Row
    QAction* mActionStyleCompactThreeRow { nullptr };      ///< Action for setting style to Compact Three Row
    QAction* mActionStyleCompactTwoRow { nullptr };        ///< Action for setting style to Compact Two Row
    QAction* mActionStyleLooseSingleRow { nullptr };       ///< Action for setting style to Loose Single Row
    QAction* mActionStyleCompactSingleRow { nullptr };     ///< Action for setting style to Compact Single Row
    Ui::MainWindow* ui;                                   ///< UI form pointer (Qt Designer generated)
};

#endif  // MAINWINDOW_H
