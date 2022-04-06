#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "SARibbonMainWindow.h"
#include <QLabel>
#include <iostream>
#include <chrono>
#include <ctime> 
#include <QApplication>

class SARibbonCategory;
class SARibbonContextCategory;
class SARibbonCustomizeWidget;
class QTextEdit;
class SARibbonActionsManager;
class MainWindow : public SARibbonMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget *par = nullptr);
private:
	void createCategoryVideo(SARibbonCategory *page);
	void createCategoryAI(SARibbonCategory *page);

	SARibbonContextCategory *m_contextCategory;
	SARibbonContextCategory *m_contextCategory2;
	SARibbonCustomizeWidget *m_customizeWidget;
	QTextEdit *m_edit;
	QLabel *m_label;
	SARibbonActionsManager *m_actMgr;
	int m_actionTagText;
	int m_actionTagWithIcon;
};

#endif // MAINWINDOW_H
