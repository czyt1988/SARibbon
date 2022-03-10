#pragma once
#include <QObject>
#include <opencv2/opencv.hpp>

enum States { Running, Stopped, Break, NotInitialized };

class VideoExampleTest : public QObject
{
	Q_OBJECT

public slots:
	void Start();
	void Break();
	void Pause();
	void Continue();
	void SetInFile(std::string inFile) { m_inFile = inFile; }

signals:
	void FrameChanged(const cv::Mat& frame);

public:
	void SyncProcess();
	VideoExampleTest(std::string inFile);

private:
	std::string m_inFile;
	virtual bool OpenCapture(cv::VideoCapture& capture);
	States currentState = States::NotInitialized;
	void Detection();
	void Tracking();


};