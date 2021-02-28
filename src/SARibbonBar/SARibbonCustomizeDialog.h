#ifndef SARIBBONCUSTOMIZEDIALOG_H
#define SARIBBONCUSTOMIZEDIALOG_H
#include "SARibbonGlobal.h"
#include <QDialog>
#include "SARibbonCustomizeWidget.h"
class SARibbonActionsManager;
class SARibbonCustomizeDialogUi;

/**
 * @brief SARibbonCustomizeWidget的对话框封装
 */
class SA_RIBBON_EXPORT SARibbonCustomizeDialog : public QDialog
{
    Q_OBJECT
public:
    SARibbonCustomizeDialog(SARibbonMainWindow *ribbonWindow, QWidget *p = nullptr, Qt::WindowFlags f = Qt::WindowFlags());
    //设置action管理器
    void setupActionsManager(SARibbonActionsManager *mgr);
    bool applys();

private:
    void initConnection();

    SARibbonCustomizeDialogUi *ui;
};

#endif // SARIBBONCUSTOMIZEDIALOG_H
