#ifndef SARIBBONBUTTONGROUPWIDGET_H
#define SARIBBONBUTTONGROUPWIDGET_H
#include "SARibbonGlobal.h"
#include <QFrame>
#include <QAbstractButton>
class SARibbonButtonGroupWidgetPrivate;
///
/// \brief 用于管理一组按钮的控件
///
class SA_RIBBON_EXPORT SARibbonButtonGroupWidget : public QFrame
{
public:
    SARibbonButtonGroupWidget(QWidget* parent);
    ~SARibbonButtonGroupWidget();
    void addButton(QAbstractButton* btn);
    QSize sizeHint() const Q_DECL_OVERRIDE;
    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
private:
    SARibbonButtonGroupWidgetPrivate* m_d;
};

#endif // SARIBBONBUTTONGROUPWIDGET_H
