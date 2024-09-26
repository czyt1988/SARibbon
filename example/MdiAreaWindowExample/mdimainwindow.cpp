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
	// 如果你用qwk，你需要安装mdi窗口的最大最小化按钮到frameless框架中，你需要绑定此槽函数
	connect(ui->mdiArea, &QMdiArea::subWindowActivated, this, &MainWindow::onMdiAreaSubWindowActivated);
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
	sub->setAttribute(Qt::WA_DeleteOnClose);
	sub->show();
	// 如果你用qwk，你需要安装mdi窗口的最大最小化按钮到frameless框架中，你需要绑定此槽函数
	connect(sub, &QMdiSubWindow::windowStateChanged, this, &MainWindow::onSubWindowStateChanged);
}

/**
 * @brief 窗口激活后对cornerwidget的绑定
 *
 * @note mdi窗口的状态变化会导致原来的cornerwidget删除，因此，subWindowActivated和windowStateChanged这两个都要绑定，
 * 防止任意一种状态变化没有监测到，例如最大化状态关闭所有窗口，再新建一个窗口，这时候它不会触发windowStateChanged，但会触发subWindowActivated
 * @param window
 */
void MainWindow::onMdiAreaSubWindowActivated(QMdiSubWindow* window)
{
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
	if (window) {
		SARibbonBar* ribbon = ribbonBar();
		QWidget* cw         = ribbon->cornerWidget(Qt::TopRightCorner);
		if (cw && cw != m_mdiCornerWidget) {
			qDebug() << "set mdi corner widget in frameless|onMdiAreaSubWindowActivated";
			setFramelessHitTestVisible(cw);
			m_mdiCornerWidget = cw;
		}
	}
#endif
}

/**
 * @brief 子窗口变化对cornerwidget的绑定
 * @param oldState
 * @param newState
 */
void MainWindow::onSubWindowStateChanged(Qt::WindowStates oldState, Qt::WindowStates newState)
{
#if SARIBBON_USE_3RDPARTY_FRAMELESSHELPER
	if (newState.testFlag(Qt::WindowMaximized)) {
		SARibbonBar* ribbon = ribbonBar();
		QWidget* cw         = ribbon->cornerWidget(Qt::TopRightCorner);
		if (cw && cw != m_mdiCornerWidget) {
			qDebug() << "set mdi corner widget in frameless|onSubWindowStateChanged";
			setFramelessHitTestVisible(cw);
			m_mdiCornerWidget = cw;
		}
	}
#endif
}
