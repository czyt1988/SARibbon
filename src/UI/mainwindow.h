#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "SARibbonMainWindow.h"
class SARibbonCategory;
class SARibbonContextCategory;
class SARibbonCustomizeWidget;
class QTextEdit;
class SARibbonActionsManager;
class MainWindow : public SARibbonMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *par = nullptr);
private slots:
    void onShowContextCategory(bool on);
    void onStyleClicked(int id);


private:
    void createCategoryMain(SARibbonCategory *page);
    void createCategoryOther(SARibbonCategory *page);
    void createCategoryDelete(SARibbonCategory *page);
    void createContextCategoryPage1(SARibbonCategory *page);
    void createContextCategoryPage2(SARibbonCategory *page);
    void addSomeOtherAction();

private slots:
    void onMenuButtonPopupCheckableTest(bool b);
    void onInstantPopupCheckableTest(bool b);
    void onDelayedPopupCheckableTest(bool b);

    void onMenuButtonPopupCheckabletriggered(bool b);
    void onInstantPopupCheckabletriggered(bool b);
    void onDelayedPopupCheckabletriggered(bool b);

private:
    SARibbonContextCategory *m_contextCategory;
    SARibbonContextCategory *m_contextCategory2;
    SARibbonCustomizeWidget *m_customizeWidget;
    QTextEdit *m_edit;
    SARibbonActionsManager *m_actMgr;
    int m_actionTagText;
    int m_actionTagWithIcon;
};

#endif // MAINWINDOW_H
