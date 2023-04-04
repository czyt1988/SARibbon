#ifndef SARIBBONCTROLCONTAINER_H
#define SARIBBONCTROLCONTAINER_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QScopedPointer>
class QStyleOption;
class SARibbonCtrlContainerPrivate;

/**
 * @brief 用于装载一个窗体的容器，这个窗体会布满整个SARibbonCtrlContainer，但会预留空间显示icon或者title
 *
 * ----------------------
 * |icon|text|  widget  |
 * ----------------------
 */
class SA_RIBBON_EXPORT SARibbonCtrlContainer : public QWidget
{
    Q_OBJECT
public:
    SARibbonCtrlContainer(QWidget* parent = Q_NULLPTR);
    ~SARibbonCtrlContainer();

    void setEnableShowIcon(bool b);
    void setEnableShowTitle(bool b);
    //判断是否存在容器窗口
    bool hasContainerWidget() const;
    //图标
    void setIcon(const QIcon& i);
    QIcon getIcon() const;
    //图标
    void setText(const QString& t);
    QString getText() const;
    //设置窗口
    void setContainerWidget(QWidget* w);
    QWidget* getContainerWidget();
    const QWidget* getContainerWidget() const;

private:
    SARibbonCtrlContainerPrivate* m_d;
};

#endif  // SARIBBONCTROLCONTAINER_H
