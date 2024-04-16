#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QComboBox>
QT_BEGIN_NAMESPACE
namespace Ui
{
class Widget;
}
QT_END_NAMESPACE

class SARibbonBar;
class Widget : public QWidget
{
	Q_OBJECT

public:
	Widget(QWidget* parent = nullptr);
	~Widget();

private:
	void buildRibbon(SARibbonBar* bar);
	QAction* createAction(const QString& text, const QString& iconurl);
private slots:
	void onRibbonThemeComboBoxCurrentIndexChanged(int index);

private:
	Ui::Widget* ui;
	SARibbonBar* mRibbonBar { nullptr };
	QComboBox* mComboTheme { nullptr };
};
#endif  // WIDGET_H
