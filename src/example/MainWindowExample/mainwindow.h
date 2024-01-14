#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "SARibbonMainWindow.h"
class SARibbonCategory;
class SARibbonContextCategory;
class SARibbonCustomizeWidget;
class SARibbonActionsManager;
class SARibbonQuickAccessBar;
class SARibbonButtonGroupWidget;
class SARibbonPannel;
class QTextEdit;
class QComboBox;
class QCloseEvent;
class QLineEdit;

class MainWindow : public SARibbonMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget* par = nullptr);

private:
    // 创建ribbon-application菜单示例
    void createRibbonApplicationButton();
    // 创建main菜单示例
    void createCategoryMain(SARibbonCategory* page);
    void createCategoryOther(SARibbonCategory* page);
    void createCategoryDelete(SARibbonCategory* page);
    void createCategorySize(SARibbonCategory* page);
    void createCategoryColor(SARibbonCategory* page);
    void createContextCategory1();
    void createContextCategory2();
    void createContextCategoryPage1(SARibbonCategory* page);
    void createContextCategoryPage2(SARibbonCategory* page);
    void createQuickAccessBar(SARibbonQuickAccessBar* quickAccessBar);
    void createRightButtonGroup();
    // 创建ActionsManager，实现actions的管理以及SARibbonBar的自定义
    void createActionsManager();
    QAction* createAction(const QString& text, const QString& iconurl, const QString& objName);
    QAction* createAction(const QString& text, const QString& iconurl);
private slots:
    void onMenuButtonPopupCheckableTest(bool b);
    void onInstantPopupCheckableTest(bool b);
    void onDelayedPopupCheckableTest(bool b);
    void onMenuButtonPopupCheckabletriggered(bool b);
    void onInstantPopupCheckabletriggered(bool b);
    void onDelayedPopupCheckabletriggered(bool b);
    void onShowContextCategory(bool on);
    void onStyleClicked(int id);
    void onActionCustomizeTriggered(bool b);
    void onActionCustomizeAndSaveTriggered(bool b);
    void onActionCustomizeAndSaveWithApplyTriggered(bool b);
    void onActionHelpTriggered();
    void onActionRemoveAppBtnTriggered(bool b);
    void onActionUseQssTriggered();
    void onActionLoadCustomizeXmlFileTriggered();
    void onActionWindowFlagNormalButtonTriggered(bool b);

    void onFontComWidgetCurrentFontChanged(const QFont& f);
    void onActionFontLargerTriggered();
    void onActionFontSmallerTriggered();
    void onActionWordWrapTriggered(bool b);
    void onButtonGroupActionTriggered(QAction* act);
    void onColorButtonColorClicked(const QColor& c, bool on);
    void onRibbonThemeComboBoxCurrentIndexChanged(int index);
    void onActionHideActionTriggered(bool on);
    // 切换所有action是否可见
    void onActionVisibleAllTriggered(bool on);
    //居中对齐checkbox的槽
    void onCheckBoxAlignmentCenterClicked(bool checked);

private:
    // 创建其它actions，这些actions并不在SARibbonBar管理
    void createOtherActions();

protected:
    void closeEvent(QCloseEvent* e) override;

private:
    SARibbonContextCategory* mContextCategory;
    SARibbonContextCategory* mContextCategory2;
    SARibbonCustomizeWidget* mWidgetForCustomize;
    QTextEdit* mTextedit;
    SARibbonActionsManager* mActionsManager;
    int mTagForActionText;
    int mTagForActionIcon;
    QMenu* mMenuApplicationBtn;
    QComboBox* mComboboxRibbonTheme;
    QLineEdit* mSearchEditor { nullptr };
    //
    QAction* mActionWordWrap { nullptr };
    QAction* mActionDisable { nullptr };
    QAction* mActionUnlock { nullptr };
    QAction* mActionSetTextTest { nullptr };
    QAction* mActionShowTest { nullptr };
    QAction* mActionHideAction2 { nullptr };
    QAction* mActionHideAction4 { nullptr };
    QAction* mActionHideShowTextAct2 { nullptr };
    QAction* mActionHideShowTextAct3 { nullptr };
    QAction* mActionHideShowTextAct4 { nullptr };
    //
    QAction* mOtherAction1 { nullptr };
    QAction* mOtherAction2 { nullptr };
    QAction* mOtherAction3 { nullptr };
    QAction* mOtherAction4 { nullptr };
    QAction* mOtherAction5 { nullptr };
    QAction* mOtherActionIcon1 { nullptr };
    SARibbonPannel* mPannelVisbileExample { nullptr };
    //
    QAction* mActionVisibleAll { nullptr };  ///< 控制所有action是否可见的开关
    bool mHasApplyCustomizeXmlFile {false}; ///程序启动后是否应用上次修改
};

#endif  // MAINWINDOW_H
