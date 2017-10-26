#ifndef SARIBBONCTROLCONTAINER_H
#define SARIBBONCTROLCONTAINER_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QScopedPointer>
class QStyleOption;
class SARibbonCtrolContainerPrivate;
class SA_RIBBON_EXPORT SARibbonCtrolContainer : public QWidget
{
public:
    SARibbonCtrolContainer(QWidget *containerWidget,QWidget *parent = Q_NULLPTR);
    ~SARibbonCtrolContainer();
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QWidget* containerWidget();
    const QWidget* containerWidget() const;
protected:
    void setContainerWidget(QWidget* w);
    void paintEvent(QPaintEvent *e) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *e ) Q_DECL_OVERRIDE;
    virtual void initStyleOption(QStyleOption* opt) = 0;
private:
    SARibbonCtrolContainerPrivate*  m_d;
};

#endif // SARIBBONCTROLCONTAINER_H
