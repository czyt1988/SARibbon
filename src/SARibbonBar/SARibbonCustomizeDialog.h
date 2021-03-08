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
 *
 * 此功能依赖于@ref SARibbonActionsManager ，SARibbonActionsManager建议作为mianwindow的成员变量，
 * SARibbonActionsManager可以快速绑定所有QAction，详细见SARibbonActionsManager的说明
 *
 *
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

    //从xml中加载QList<SARibbonCustomizeData>，对于基于配置文件的设置，对话框显示前建议调用此函数，保证叠加设置的正确记录
    void fromXml(QXmlStreamReader *xml);
    void fromXml(const QString& xmlpath);

    //返回SARibbonCustomizeWidget窗口指针
    SARibbonCustomizeWidget *customizeWidget() const;

private:
    void initConnection();

    SARibbonCustomizeDialogUi *ui;
};

#endif // SARIBBONCUSTOMIZEDIALOG_H
