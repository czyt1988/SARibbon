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
};

#endif  // MAINWINDOW_H
