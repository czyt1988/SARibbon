#ifndef APPLICATIONWIDGET_H
#define APPLICATIONWIDGET_H

#include <QWidget>
#include "SARibbonApplicationWidget.h"
class SARibbonMainWindow;
namespace Ui
{
class ApplicationWidget;
}

class ApplicationWidget : public SARibbonApplicationWidget
{
    Q_OBJECT

public:
    explicit ApplicationWidget(SARibbonMainWindow* parent = nullptr);
    ~ApplicationWidget();

private:
    Ui::ApplicationWidget* ui;
};

#endif  // APPLICATIONWIDGET_H
