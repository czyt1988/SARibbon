#ifndef SARIBBONSTACKEDWIDGET_H
#define SARIBBONSTACKEDWIDGET_H
#include <QStackedWidget>
#include "SARibbonGlobal.h"
class SA_RIBBON_EXPORT SARibbonStackedWidget : public QStackedWidget
{
    Q_OBJECT
public:
    SARibbonStackedWidget(QWidget* parent);
};

#endif // SARIBBONSTACKEDWIDGET_H
