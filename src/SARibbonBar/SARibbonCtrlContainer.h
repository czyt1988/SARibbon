#ifndef SARIBBONCTROLCONTAINER_H
#define SARIBBONCTROLCONTAINER_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QScopedPointer>
class QStyleOption;
class SARibbonCtrlContainerPrivate;

/**
 * @brief 用于装载一个窗体的容器，这个窗体会布满整个SARibbonCtrlContainer，但会预留空间显示icon或者title
 */
class SA_RIBBON_EXPORT SARibbonCtrlContainer : public QWidget
{
    Q_OBJECT
public:
    SARibbonCtrlContainer(QWidget* containerWidget, QWidget* parent = Q_NULLPTR);
    ~SARibbonCtrlContainer();
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QWidget* containerWidget();
    const QWidget* containerWidget() const;
    void setEnableShowIcon(bool b);
    void setEnableShowTitle(bool b);
    //判断是否存在容器窗口
    bool hasContainerWidget() const;

protected:
    void setContainerWidget(QWidget* w);
    void paintEvent(QPaintEvent* e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent* e) Q_DECL_OVERRIDE;
    virtual void initStyleOption(QStyleOption* opt) = 0;

private:
    SARibbonCtrlContainerPrivate* m_d;
};

#endif  // SARIBBONCTROLCONTAINER_H
