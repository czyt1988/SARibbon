#include "mainwindow.h"
#include <QFile>
#include <QTextEdit>
#include <QAbstractButton>
#include <QCoreApplication>
#include "SARibbonBar.h"
#include "SARibbonCategory.h"
#include <QPushButton>
#include "SARibbonPannel.h"
#include "SARibbonToolButton.h"
#include <QAction>
#include <QMenu>
#include <QStatusBar>
#include <QDebug>
#include <QElapsedTimer>
#include <QRadioButton>
#include <QButtonGroup>
#include "SARibbonMenu.h"
#include "SARibbonComboBox.h"
#include "SARibbonLineEdit.h"
#include "SARibbonGallery.h"
#include "SARibbonCheckBox.h"
#include "SARibbonQuickAccessBar.h"
#include "SARibbonButtonGroupWidget.h"
#include "SARibbonApplicationButton.h"
#include "SARibbonCustomizeWidget.h"
#include <QCalendarWidget>
#include "SARibbonCustomizeDialog.h"
#include <QXmlStreamWriter>
#include <QTextStream>
#include "SAFramelessHelper.h"
#include "examples.h"
#include <QObject>
#include <QFileDialog>
#include <QString>


#define PRINT_COST(ElapsedTimer, LastTime, STR)					      \
    do{									      \
        int ___TMP_INT = ElapsedTimer.elapsed();			      \
        qDebug() << STR << ___TMP_INT - LastTime << "(" << ___TMP_INT << ")"; \
        LastTime = ___TMP_INT;						      \
    }while(0)

MainWindow::MainWindow(QWidget *par) : SARibbonMainWindow(par)
, m_customizeWidget(nullptr)
{
	// helper is there for shrinking or enlarging the GUI
	SAFramelessHelper *helper = framelessHelper();
	
	helper->setRubberBandOnResize(false);

	setWindowTitle(("Balltracking and prediction"));
	SARibbonBar *ribbon = ribbonBar();

	SARibbonCategory *categoryVideo = new SARibbonCategory();

	categoryVideo->setCategoryName(("Video"));
	categoryVideo->setObjectName(("categoryVideo"));
	ribbon->addCategoryPage(categoryVideo);
	createCategoryVideo(categoryVideo);


	SARibbonCategory *categoryAI = new SARibbonCategory();

	categoryAI->setCategoryName(("AI"));
	categoryAI->setObjectName(("AI"));
	ribbon->addCategoryPage(categoryAI);
	createCategoryAI(categoryAI);
	setMinimumWidth(500);
	//
	showMaximized();
	//
}

void MainWindow::createCategoryAI(SARibbonCategory *page)
{

}

void MainWindow::createCategoryVideo(SARibbonCategory *page)
{

	SARibbonPannel *pannel = page->addPannel(("File"));

	QAction* act = new QAction(this);
	act->setObjectName(("OpenFile"));
	act->setIcon(QIcon(":/icon/icon/folder.png"));
	act->setText(("Open File"));
	act->setCheckable(false);
	pannel->addLargeAction(act);
	

	act = new QAction(this);
	act->setObjectName(("Play"));
	act->setIcon(QIcon(":/icon/icon/folder.png"));
	act->setText(("Play"));
	act->setCheckable(false);
	pannel->addLargeAction(act);



	act = new QAction(this);
	act->setObjectName(("Pause"));
	act->setIcon(QIcon(":/icon/icon/folder.png"));
	act->setText(("Pause"));
	act->setCheckable(false);
	pannel->addLargeAction(act);

	act = new QAction(this);
	act->setObjectName(("Continue"));
	act->setIcon(QIcon(":/icon/icon/folder.png"));
	act->setText(("Continue"));
	act->setCheckable(false);
	pannel->addLargeAction(act);

	act = new QAction(this);
	act->setObjectName(("Break"));
	act->setIcon(QIcon(":/icon/icon/folder.png"));
	act->setText(("Break"));
	act->setCheckable(false);
	pannel->addLargeAction(act);

	act = new QAction(this);
	act->setObjectName(("Green"));
	act->setIcon(QIcon(":/icon/icon/folder.png"));
	act->setText(("Green"));
	act->setCheckable(false);
	pannel->addLargeAction(act);
	//connect(act, &QAction::triggered, this, [=]() {m_videoExampleTest->ChangeBoundingBoxColor(cv::Scalar(0, 255, 0));});

	act = new QAction(this);
	act->setObjectName(("Blue"));
	act->setIcon(QIcon(":/icon/icon/folder.png"));
	act->setText(("Blue"));
	act->setCheckable(false);
	pannel->addLargeAction(act);
	//connect(act, &QAction::triggered, this, [=]() {m_videoExample->ChangeBoundingBoxColor(cv::Scalar(255, 0, 0));});

	act = new QAction(this);
	act->setObjectName(("Red"));
	act->setIcon(QIcon(":/icon/icon/folder.png"));
	act->setText(("Red"));
	act->setCheckable(false);
	pannel->addLargeAction(act);
	//connect(act, &QAction::triggered, this, [=]() {m_videoExample->ChangeBoundingBoxColor(cv::Scalar(0, 0, 255));});
}
