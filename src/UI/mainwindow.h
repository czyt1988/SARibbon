#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "SARibbonMainWindow.h"
#include "VideoExampleTest.h"
#include <QLabel>;
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
private slots:
    void onShowContextCategory(bool on);
    void onStyleClicked(int id);
	void onFrameChanged(const cv::Mat& frame) 
	{
		std::cout << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
		QCoreApplication::processEvents();

		QImage img((uchar*)frame.data, frame.cols, frame.rows, frame.step, QImage::Format_RGB888);
		QSize size = this->size();
		int width = size.width();
		int height = size.height();
		m_label->setPixmap(QPixmap::fromImage(img).scaled(width, height, Qt::KeepAspectRatio));
	}


private:
    void createCategoryMain(SARibbonCategory *page);
	void createCategoryVideo(SARibbonCategory *page);
    void createCategoryOther(SARibbonCategory *page);
    void createCategoryDelete(SARibbonCategory *page);
    void createContextCategoryPage1(SARibbonCategory *page);
    void createContextCategoryPage2(SARibbonCategory *page);
    void addSomeOtherAction();

private slots:
    void onMenuButtonPopupCheckableTest(bool b);
    void onInstantPopupCheckableTest(bool b);
    void onDelayedPopupCheckableTest(bool b);

    void onMenuButtonPopupCheckabletriggered(bool b);
    void onInstantPopupCheckabletriggered(bool b);
    void onDelayedPopupCheckabletriggered(bool b);

private:
    SARibbonContextCategory *m_contextCategory;
    SARibbonContextCategory *m_contextCategory2;
    SARibbonCustomizeWidget *m_customizeWidget;
    QTextEdit *m_edit;
	QLabel *m_label;
    SARibbonActionsManager *m_actMgr;
    int m_actionTagText;
    int m_actionTagWithIcon;
	std::unique_ptr<VideoExampleTest> m_videoExampleTest;
};

#endif // MAINWINDOW_H
