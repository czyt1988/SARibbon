#ifndef RIBBONWIDGET_H
#define RIBBONWIDGET_H

#include "SARibbonWidget.h"
#include <QComboBox>

class SARibbonBar;
class RibbonWidget : public SARibbonWidget
{
	Q_OBJECT

public:
    RibbonWidget(QWidget* parent = nullptr);
    ~RibbonWidget();

private:
	void buildRibbon(SARibbonBar* bar);
	QAction* createAction(const QString& text, const QString& iconurl);
private slots:
	void onRibbonThemeComboBoxCurrentIndexChanged(int index);

private:
	QComboBox* mComboTheme { nullptr };
};
#endif  // WIDGET_H
