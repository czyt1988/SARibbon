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
    void createQuickAccessBar();
    void createRightButtonGroup();
    void createWindowButtonGroupBar();
    // 创建ActionsManager，实现actions的管理以及SARibbonBar的自定义
    void createActionsManager();
    QAction* createAction(const QString& text, const QString& iconurl, const QString& objName);
    QAction* createAction(const QString& text, const QString& iconurl);
private Q_SLOTS:
    void onMenuButtonPopupCheckableTest(bool b);
    void onInstantPopupCheckableTest(bool b);
    void onDelayedPopupCheckableTest(bool b);
    void onMenuButtonPopupCheckabletriggered(bool b);
    void onInstantPopupCheckabletriggered(bool b);
    void onDelayedPopupCheckabletriggered(bool b);
    void onShowContextCategory(bool on);
    void onStyleClicked(int id);
    void onActionCustomizeTriggered();
    void onActionCustomizeAndSaveTriggered();
    void onActionCustomizeAndSaveWithApplyTriggered();
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
    // 居中对齐checkbox的槽
    void onCheckBoxAlignmentCenterClicked(bool checked);
    //
    void onSpinBoxRibbonTitleHeightChanged(int h);
    void onSpinBoxRibbonTabHeightChanged(int h);
    void onSpinBoxRibbonCategoryHeightChanged(int h);
    void onSpinBoxRibbonPanelTtitleHeightChanged(int h);
    void onSpinBoxRibbonPanelSpacingChanged(int h);
    void onSpinBoxPanelSmallIconSizeChanged(int h);
    void onSpinBoxPanelLargeIconSizeChanged(int h);
    void onDoubleSpinBoxToolbuttonMaximumAspectRatioChanged(double h);
    // 此函数用来演示标题栏颜色改变
    void onTitleBackgroundBrushChangedTimeout();
    // 改变颜色
    void onChangedTitleTimeout();

private:
    // 创建其它actions，这些actions并不在SARibbonBar管理
    void createOtherActions();

protected:
    void closeEvent(QCloseEvent* e) override;

private:
    SARibbonContextCategory* mContextCategory { nullptr };
    SARibbonContextCategory* mContextCategory2 { nullptr };
    SARibbonCustomizeWidget* mWidgetForCustomize { nullptr };
    QTextEdit* mTextedit { nullptr };
    SARibbonActionsManager* mActionsManager { nullptr };
    QMenu* mMenuApplicationBtn { nullptr };
    QComboBox* mComboboxRibbonTheme { nullptr };
    QLineEdit* mSearchEditor { nullptr };
    SARibbonApplicationWidget* mAppWidget { nullptr };
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
    SARibbonPanel* mPanelVisbileExample { nullptr };
    //
    QAction* mActionVisibleAll { nullptr };    ///< 控制所有action是否可见的开关
    bool mHasApplyCustomizeXmlFile { false };  ///< 程序启动后是否应用上次修改
    int mTagForActionText;
    int mTagForActionIcon;
    QTimer mChangeTitleBkColorTimer;  ///< 用于周期改变标题颜色的一个定时器
};

#endif  // MAINWINDOW_H
