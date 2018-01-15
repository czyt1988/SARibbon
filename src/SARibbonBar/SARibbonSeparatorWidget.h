#ifndef SARIBBONSEPARATORWIDGET_H
#define SARIBBONSEPARATORWIDGET_H
#include "SARibbonGlobal.h"
#include <QWidget>
#include <QStyleOption>
///
/// \brief 用于显示分割线
///
class SA_RIBBON_EXPORT SARibbonSeparatorWidget : public QWidget
{
    Q_OBJECT
public:
    SARibbonSeparatorWidget(int height,QWidget* parent = nullptr);
    SARibbonSeparatorWidget(QWidget* parent = nullptr);
    virtual QSize sizeHint() const Q_DECL_OVERRIDE;
protected:
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
};

#endif // SARIBBONSEPARATORWIDGET_H
