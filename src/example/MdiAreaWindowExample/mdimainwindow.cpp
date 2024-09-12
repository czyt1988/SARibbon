#include "mdimainwindow.h"
#include "ui_MainWindow.h"
#include <QTextEdit>
#include <QMdiSubWindow>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include "SARibbonPannel.h"
MainWindow::MainWindow(QWidget* parent) : SARibbonMainWindow(parent), ui(new Ui::MainWindow)
{
	// 注意，你必须在ui文件中删除菜单栏，否则会拿到空的ribbon指针
	ui->setupUi(this);
	SARibbonBar* ribbon        = ribbonBar();
	SARibbonCategory* mainPage = new SARibbonCategory("Main");
	ribbon->addCategoryPage(mainPage);
	buildMainPage(mainPage);
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::buildMainPage(SARibbonCategory* mainPage)
{
	SARibbonPannel* mainPannel = new SARibbonPannel("operate");
	mainPannel->addAction(ui->actionadd_window);
	mainPage->addPannel(mainPannel);
}

void MainWindow::on_actionadd_window_triggered()
{
	QMdiSubWindow* sub = ui->mdiArea->addSubWindow(new QTextEdit());
	sub->show();
}
