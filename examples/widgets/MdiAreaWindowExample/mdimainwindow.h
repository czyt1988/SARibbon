#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "SARibbonMainWindow.h"
namespace Ui
{
class MainWindow;
}
class QMdiSubWindow;
class SARibbonCategory;
class MainWindow : public SARibbonMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget* parent = nullptr);
	~MainWindow();

private slots:
	void on_actionadd_window_triggered();
	void onMdiAreaSubWindowActivated(QMdiSubWindow* window);
	void onSubWindowStateChanged(Qt::WindowStates oldState, Qt::WindowStates newState);

private:
	void buildMainPage(SARibbonCategory* mainPage);

private:
	Ui::MainWindow* ui;
	QWidget* m_mdiCornerWidget { nullptr };
};

#endif  // MAINWINDOW_H
