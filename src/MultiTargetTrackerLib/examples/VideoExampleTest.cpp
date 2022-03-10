#include "VideoExampleTest.h"
#include <thread>
#include <QApplication>

VideoExampleTest::VideoExampleTest(std::string inFile)
{
	m_inFile = inFile;
}

void VideoExampleTest::Start()
{
	if (currentState == States::NotInitialized)
		SyncProcess();
}

void VideoExampleTest::Break()
{
	if (currentState == States::Running || currentState == States::Stopped)
	{
		currentState = States::Break;
	}
}

void VideoExampleTest::Pause()
{
	if (currentState == States::Running)
		currentState = States::Stopped;
}

void VideoExampleTest::Continue()
{
	if (currentState == States::Stopped)
		currentState = States::Running;
}

void VideoExampleTest::SyncProcess()
{
	cv::VideoCapture capture;
	if (!OpenCapture(capture))
	{
		std::cerr << "Can't open " << m_inFile << std::endl;
		return;
	}

	currentState = States::Running;

	cv::Mat frame;

	while (1)
	{
		while (currentState == States::Stopped) {
			std::this_thread::sleep_for(std::chrono::microseconds(50));
			QApplication::processEvents();
		}

		capture >> frame;

		if (currentState == States::Break || frame.empty())
		{
			break;
		}

		emit FrameChanged(frame);
	}

	capture.release();
	currentState = States::NotInitialized;
}

bool VideoExampleTest::OpenCapture(cv::VideoCapture& capture)
{
	if (m_inFile.size() == 1)
		capture.open(std::atoi(m_inFile.c_str()));
	else
		capture.open(m_inFile);

	if (capture.isOpened())
		return true;

	return false;
}