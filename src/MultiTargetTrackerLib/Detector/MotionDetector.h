#pragma once

#include "BaseDetector.h"
#include "BackgroundSubtract.h"

///
/// \brief The MotionDetector class
///
class MotionDetector final : public BaseDetector
{
public:
    MotionDetector(BackgroundSubtract::BGFG_ALGS algType, cv::UMat& gray);
    ~MotionDetector(void) = default;

    bool Init(const config_t& config);

    void Detect(const cv::UMat& gray);

	bool CanGrayProcessing() const
	{
		return true;
	}

	void CalcMotionMap(cv::Mat& frame);

	void ResetModel(const cv::UMat& img, const cv::Rect& roiRect);

private:
    void DetectContour();

    std::unique_ptr<BackgroundSubtract> m_backgroundSubst;

    cv::UMat m_fg;

    BackgroundSubtract::BGFG_ALGS m_algType = BackgroundSubtract::BGFG_ALGS::ALG_MOG2;
    bool m_useRotatedRect = false;
};
