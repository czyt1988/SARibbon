#ifndef SARIBBONCUSTOMIZEDIALOG_H
#define SARIBBONCUSTOMIZEDIALOG_H
#include "SARibbonGlobal.h"
#include <QDialog>
#include "SARibbonCustomizeWidget.h"
class SARibbonActionsManager;
class SARibbonCustomizeDialogUi;
class QXmlStreamWriter;

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

    //应用所有的设定
    bool applys();

    //清除所有动作
    void clear();

    //转换为xml
    bool toXml(QXmlStreamWriter *xml) const;

    //返回SARibbonCustomizeWidget窗口指针
    SARibbonCustomizeWidget *customizeWidget() const;

private:
    void initConnection();

    SARibbonCustomizeDialogUi *ui;
};

#endif // SARIBBONCUSTOMIZEDIALOG_H
