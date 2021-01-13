#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "SARibbonMainWindow.h"
class SARibbonCategory;
class SARibbonContextCategory;
class QTextEdit;
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
    void createContextCategoryPage1(SARibbonCategory *page);
    void createContextCategoryPage2(SARibbonCategory *page);

private slots:
    void onMenuButtonPopupCheckableTest(bool b);
    void onInstantPopupCheckableTest(bool b);
    void onDelayedPopupCheckableTest(bool b);

    void onMenuButtonPopupCheckabletriggered(bool b);
    void onInstantPopupCheckabletriggered(bool b);
    void onDelayedPopupCheckabletriggered(bool b);

private:
    SARibbonContextCategory *m_contextCategory;
    QTextEdit *m_edit;
};

#endif // MAINWINDOW_H
