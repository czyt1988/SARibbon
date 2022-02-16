#pragma once

#ifdef USE_OCV_DNN

#include "BaseDetector.h"

#include <opencv2/dnn.hpp>
#include <opencv2/dnn/shape_utils.hpp>

///
/// \brief The OCVDNNDetector class
///
class OCVDNNDetector final : public BaseDetector
{
public:
    OCVDNNDetector(const cv::UMat& colorFrame);
    ~OCVDNNDetector(void) = default;

    bool Init(const config_t& config);

    void Detect(const cv::UMat& colorFrame);

    bool CanGrayProcessing() const
    {
        return false;
    }

private:
    cv::dnn::Net m_net;

    void DetectInCrop(const cv::UMat& colorFrame, const cv::Rect& crop, regions_t& tmpRegions);

    int m_inWidth = 608;
    int m_inHeight = 608;

    float m_WHRatio = 1.f;
    float m_inScaleFactor = 0.003921f;
    float m_meanVal = 0.f;
    float m_confidenceThreshold = 0.24f;
    float m_nmsThreshold = 0.4f;
    bool m_swapRB = false;
    float m_maxCropRatio = 2.0f;
    std::vector<std::string> m_classNames;
    std::vector<cv::String> m_outNames;
    std::vector<int> m_outLayers;
    std::string m_outLayerType;
    cv::UMat m_inputBlob;
};

#endif
