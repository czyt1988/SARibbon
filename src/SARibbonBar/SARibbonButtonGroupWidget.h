#ifndef SARIBBONBUTTONGROUPWIDGET_H
#define SARIBBONBUTTONGROUPWIDGET_H
#include "SARibbonGlobal.h"
#include "SARibbonToolButton.h"
#include <QFrame>
#include <QAbstractButton>
class SARibbonButtonGroupWidgetPrivate;
///
/// \brief 用于管理一组按钮的控件
///
class SA_RIBBON_EXPORT SARibbonButtonGroupWidget : public QFrame
{
    Q_OBJECT
public:
    SARibbonButtonGroupWidget(QWidget* parent=Q_NULLPTR);
    ~SARibbonButtonGroupWidget();
    void addButton(QAbstractButton* btn);
    SARibbonToolButton* addButton(QAction* action);
    void addWidget(QWidget* w);
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
private:
    SARibbonButtonGroupWidgetPrivate* m_d;
};

#endif // SARIBBONBUTTONGROUPWIDGET_H
