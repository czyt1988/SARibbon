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
	// 创建main菜单示例
	void createCategoryMain(SARibbonCategory* page);
	void createCategoryOther(SARibbonCategory* page);
	void createContextCategory();
	void createQuickAccessBar();
	void createRightButtonGroup();
	void createWindowButtonGroupBar();
	QAction* createAction(const QString& text, const QString& iconurl, const QString& objName);
	QAction* createAction(const QString& text, const QString& iconurl);
private Q_SLOTS:
	void onMenuButtonPopupCheckableTest(bool b);
	void onDelayedPopupCheckableTest(bool b);
	void onMenuButtonPopupCheckabletriggered(bool b);
	void onInstantPopupCheckabletriggered(bool b);
	void onDelayedPopupCheckabletriggered(bool b);
	void onShowContextCategory(bool on);
	void onStyleClicked(int id);
	void onActionHelpTriggered();
	void onActionWordWrapTriggered(bool b);
	// 切换所有action是否可见
	void onActionVisibleAllTriggered(bool on);
	// 居中对齐checkbox的槽
	void onCheckBoxAlignmentCenterClicked(bool checked);

protected:
	void closeEvent(QCloseEvent* e) override;

private:
	SARibbonContextCategory* mContextCategory { nullptr };
	QTextEdit* mTextedit { nullptr };
	int mTagForActionText;
	int mTagForActionIcon;
	QMenu* mMenuApplicationBtn { nullptr };
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
	QTimer mChangeTitleBkColorTimer;           ///< 用于周期改变标题颜色的一个定时器
};

#endif  // MAINWINDOW_H
