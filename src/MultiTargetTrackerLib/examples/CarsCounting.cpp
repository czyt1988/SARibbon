#include "CarsCounting.h"

///
/// \brief CarsCounting::CarsCounting
/// \param parser
///
CarsCounting::CarsCounting(const cv::CommandLineParser& parser)
    : VideoExample(parser)
{
#ifdef _WIN32
	std::string pathToModel = "../../data/";
#else
	std::string pathToModel = "../data/";
#endif

	m_drawHeatMap = parser.get<int>("heat_map") != 0;

	m_weightsFile = parser.get<std::string>("weights");
	m_configFile = parser.get<std::string>("config");
	m_namesFile = parser.get<std::string>("names");
	if (m_weightsFile.empty() && m_configFile.empty())
	{
		m_weightsFile = pathToModel + "yolov4.weights";
		m_configFile = pathToModel + "yolov4.cfg";
	}
	if (m_namesFile.empty())
		m_namesFile = pathToModel + "coco.names";

	std::map<std::string, tracking::Detectors> infMap;
	infMap.emplace("darknet", tracking::Detectors::Yolo_Darknet);
    infMap.emplace("tensorrt", tracking::Detectors::Yolo_TensorRT);
	infMap.emplace("ocvdnn", tracking::Detectors::DNN_OCV);
	std::string inference = parser.get<std::string>("inference");
	auto infType = infMap.find(inference);
	if (infType != std::end(infMap))
		m_detectorType = infType->second;
	else
		m_detectorType = tracking::Detectors::Yolo_Darknet;

	std::cout << "Inference framework set " << inference << " used " << m_detectorType << ", weights: " << m_weightsFile << ", config: " << m_configFile << ", names: " << m_namesFile << std::endl;
}

///
/// \brief CarsCounting::DrawTrack
/// \param frame
/// \param track
/// \param drawTrajectory
/// \param framesCounters
///
void CarsCounting::DrawTrack(cv::Mat frame, const TrackingObject& track, bool drawTrajectory, int framesCounter)
{
    cv::Rect brect = track.m_rrect.boundingRect();

    m_resultsLog.AddTrack(framesCounter, track.m_ID, brect, track.m_type, track.m_confidence);
	m_resultsLog.AddRobustTrack(track.m_ID);

    if (track.m_isStatic)
    {
#if (CV_VERSION_MAJOR >= 4)
        cv::rectangle(frame, brect, cv::Scalar(255, 0, 255), 2, cv::LINE_AA);
#else
        cv::rectangle(frame, brect, cv::Scalar(255, 0, 255), 2, CV_AA);
#endif
    }
    else
    {
#if (CV_VERSION_MAJOR >= 4)
        cv::rectangle(frame, brect, cv::Scalar(0, 255, 0), 1, cv::LINE_AA);
#else
        cv::rectangle(frame, brect, cv::Scalar(0, 255, 0), 1, CV_AA);
#endif

		if (!m_geoParams.Empty())
		{
            int traceSize = static_cast<int>(track.m_trace.size());
            int period = std::min(2 * cvRound(m_fps), traceSize);
            const auto& from = m_geoParams.Pix2Geo(track.m_trace[traceSize - period]);
            const auto& to = m_geoParams.Pix2Geo(track.m_trace[traceSize - 1]);
			auto dist = DistanceInMeters(from, to);

			std::stringstream label;
			if (period >= cvRound(m_fps) / 4)
			{
				auto velocity = (3.6f * dist * m_fps) / period;
				//std::cout << TypeConverter::Type2Str(track.m_type) << ": distance " << std::fixed << std::setw(2) << std::setprecision(2) << dist << " on time " << (period / m_fps) << " with velocity " << velocity << " km/h: " << track.m_confidence << std::endl;
				if (velocity < 1.f || std::isnan(velocity))
					velocity = 0;
				//label << TypeConverter::Type2Str(track.m_type) << " " << std::fixed << std::setw(2) << std::setprecision(2) << velocity << " km/h";
				label << TypeConverter::Type2Str(track.m_type) << " " << cvRound(velocity) << " km/h";

				int baseLine = 0;
				double fontScale = 0.5;
				cv::Size labelSize = cv::getTextSize(label.str(), cv::FONT_HERSHEY_SIMPLEX, fontScale, 1, &baseLine);

				if (brect.x < 0)
				{
					brect.width = std::min(brect.width, frame.cols - 1);
					brect.x = 0;
				}
				else if (brect.x + brect.width >= frame.cols)
				{
					brect.x = std::max(0, frame.cols - brect.width - 1);
					brect.width = std::min(brect.width, frame.cols - 1);
				}
				if (brect.y - labelSize.height < 0)
				{
					brect.height = std::min(brect.height, frame.rows - 1);
					brect.y = labelSize.height;
				}
				else if (brect.y + brect.height >= frame.rows)
				{
					brect.y = std::max(0, frame.rows - brect.height - 1);
					brect.height = std::min(brect.height, frame.rows - 1);
				}
				cv::rectangle(frame, cv::Rect(cv::Point(brect.x, brect.y - labelSize.height), cv::Size(labelSize.width, labelSize.height + baseLine)), cv::Scalar(200, 200, 200), cv::FILLED);
				cv::putText(frame, label.str(), brect.tl(), cv::FONT_HERSHEY_SIMPLEX, fontScale, cv::Scalar(0, 0, 0));

				if (velocity > 3)
					AddToHeatMap(brect);
			}
		}
    }

    if (drawTrajectory)
    {
        cv::Scalar cl = m_colors[track.m_ID.ID2Module(m_colors.size())];

        for (size_t j = 0; j < track.m_trace.size() - 1; ++j)
        {
            const TrajectoryPoint& pt1 = track.m_trace.at(j);
            const TrajectoryPoint& pt2 = track.m_trace.at(j + 1);
#if (CV_VERSION_MAJOR >= 4)
            cv::line(frame, pt1.m_prediction, pt2.m_prediction, cl, 1, cv::LINE_AA);
#else
			cv::line(frame, pt1.m_prediction, pt2.m_prediction, cl, 1, CV_AA);
#endif
            if (!pt2.m_hasRaw)
            {
#if (CV_VERSION_MAJOR >= 4)
                cv::circle(frame, pt2.m_prediction, 4, cl, 1, cv::LINE_AA);
#else
				cv::circle(frame, pt2.m_prediction, 4, cl, 1, CV_AA);
#endif
            }
        }
    }
}

///
/// \brief CarsCounting::InitDetector
/// \param frame
///
bool CarsCounting::InitDetector(cv::UMat frame)
{
    config_t config;

#if 1
	switch (m_detectorType)
	{
	case tracking::Detectors::Yolo_Darknet:
		break;

	case tracking::Detectors::DNN_OCV:
#if 1
		config.emplace("dnnTarget", "DNN_TARGET_CPU");
		config.emplace("dnnBackend", "DNN_BACKEND_OPENCV");
#else
		config.emplace("dnnTarget", "DNN_TARGET_CUDA");
		config.emplace("dnnBackend", "DNN_BACKEND_CUDA");
#endif
		break;

	default:
		break;
	}

	config.emplace("modelConfiguration", m_configFile);
	config.emplace("modelBinary", m_weightsFile);
	config.emplace("classNames", m_namesFile);
	config.emplace("confidenceThreshold", "0.5");
	config.emplace("nmsThreshold", "0.4");
	config.emplace("swapRB", "0");
    config.emplace("maxCropRatio", "-1");

	config.emplace("white_list", std::to_string((objtype_t)ObjectTypes::obj_person));
	config.emplace("white_list", std::to_string((objtype_t)ObjectTypes::obj_car));
	config.emplace("white_list", std::to_string((objtype_t)ObjectTypes::obj_bicycle));
	config.emplace("white_list", std::to_string((objtype_t)ObjectTypes::obj_motorbike));
	config.emplace("white_list", std::to_string((objtype_t)ObjectTypes::obj_bus));
	config.emplace("white_list", std::to_string((objtype_t)ObjectTypes::obj_truck));
    config.emplace("white_list", std::to_string((objtype_t)ObjectTypes::obj_vehicle));

	m_detector = CreateDetector(m_detectorType, config, frame);

#else // Background subtraction

#if 1
    config.emplace("history", std::to_string(cvRound(10 * minStaticTime * m_fps)));
    config.emplace("varThreshold", "16");
    config.emplace("detectShadows", "1");
    m_detector = CreateDetector(tracking::Detectors::Motion_MOG2, config, frame);
#else
    config.emplace("minPixelStability", "15");
    config.emplace("maxPixelStability", "900");
    config.emplace("useHistory", "1");
    config.emplace("isParallel", "1");
    m_detector = CreateDetector(tracking::Detectors::Motion_CNT, config, m_useLocalTracking, frame);
#endif

#endif

	return m_detector.operator bool();
}

///
/// \brief CarsCounting::InitTracker
/// \param grayFrame
///
bool CarsCounting::InitTracker(cv::UMat frame)
{
	if (m_drawHeatMap)
    {
        if (frame.channels() == 3)
            m_keyFrame = frame.getMat(cv::ACCESS_READ).clone();
        else
            cv::cvtColor(frame, m_keyFrame, cv::COLOR_GRAY2BGR);
        m_heatMap = cv::Mat(m_keyFrame.size(), CV_32FC1, cv::Scalar::all(0));
    }

    const int minStaticTime = 5;

    TrackerSettings settings;
    settings.SetDistance(tracking::DistJaccard);
    settings.m_kalmanType = tracking::KalmanLinear;
    settings.m_filterGoal = tracking::FilterCenter;
    settings.m_lostTrackType = tracking::TrackCSRT; // Use KCF tracker for collisions resolving. Used if m_filterGoal == tracking::FilterRect
    settings.m_matchType = tracking::MatchHungrian;
    settings.m_dt = 0.3f;                           // Delta time for Kalman filter
    settings.m_accelNoiseMag = 0.2f;                // Accel noise magnitude for Kalman filter
    settings.m_distThres = 0.7f;                    // Distance threshold between region and object on two frames
    settings.m_minAreaRadiusPix = frame.rows / 20.f;
    settings.m_maximumAllowedSkippedFrames = cvRound(2 * m_fps); // Maximum allowed skipped frames

    settings.AddNearTypes(ObjectTypes::obj_car, ObjectTypes::obj_bus, false);
    settings.AddNearTypes(ObjectTypes::obj_car, ObjectTypes::obj_truck, false);
    settings.AddNearTypes(ObjectTypes::obj_person, ObjectTypes::obj_bicycle, true);
    settings.AddNearTypes(ObjectTypes::obj_person, ObjectTypes::obj_motorbike, true);

    settings.m_useAbandonedDetection = false;
    if (settings.m_useAbandonedDetection)
    {
        settings.m_minStaticTime = minStaticTime;
        settings.m_maxStaticTime = 60;
        settings.m_maximumAllowedSkippedFrames = cvRound(settings.m_minStaticTime * m_fps); // Maximum allowed skipped frames
        settings.m_maxTraceLength = 2 * settings.m_maximumAllowedSkippedFrames;        // Maximum trace length
    }
    else
    {
        settings.m_maximumAllowedSkippedFrames = cvRound(10 * m_fps); // Maximum allowed skipped frames
        settings.m_maxTraceLength = cvRound(4 * m_fps);              // Maximum trace length
    }

    m_tracker = BaseTracker::CreateTracker(settings);

#if 0
#if 0
    std::vector<cv::Point> framePoints{ cv::Point(420, 348), cv::Point(509, 283), cv::Point(731, 281), cv::Point(840, 343) };
    std::vector<cv::Point2f> geoPoints{ cv::Point2f(45.526646, 5.974535), cv::Point2f(45.527566, 5.973849), cv::Point2f(45.527904, 5.974135), cv::Point2f(45.526867, 5.974826) };
#else
    std::vector<cv::Point> framePoints{ cv::Point(1665, 746), cv::Point(246, 521), cv::Point(570, 282), cv::Point(1773, 378) };
    std::vector<cv::Point2f> geoPoints{ cv::Point2f(30.258855, 60.006536), cv::Point2f(30.258051, 60.006855), cv::Point2f(30.258080, 60.007414), cv::Point2f(30.259066, 60.007064) };
#endif
    m_geoParams.SetKeyPoints(framePoints, geoPoints);
#endif
    return true;
}

///
/// \brief CarsCounting::DrawData
/// \param frame
///
void CarsCounting::DrawData(cv::Mat frame, const std::vector<TrackingObject>& tracks, int framesCounter, int currTime)
{
    if (m_showLogs)
        std::cout << "Frame " << framesCounter << ": tracks = " << tracks.size() << ", time = " << currTime << std::endl;

	if (!m_geoParams.Empty())
	{
		std::vector<cv::Point> points = m_geoParams.GetFramePoints();
		for (size_t i = 0; i < points.size(); ++i)
		{
			cv::line(frame, points[i % points.size()], points[(i + 1) % points.size()], cv::Scalar(255, 255, 255), 1, cv::LINE_AA);
		}
	}
	
    for (const auto& track : tracks)
    {
        if (track.m_isStatic)
        {
            DrawTrack(frame, track, true, framesCounter);
        }
        else
        {
            if (track.IsRobust(cvRound(m_fps / 4),          // Minimal trajectory size
                                0.8f,                        // Minimal ratio raw_trajectory_points / trajectory_lenght
                                cv::Size2f(0.1f, 8.0f))      // Min and max ratio: width / height
                    )
            {
                DrawTrack(frame, track, true, framesCounter);

                CheckLinesIntersection(track, static_cast<float>(frame.cols), static_cast<float>(frame.rows));
            }
        }
    }
    //m_detector->CalcMotionMap(frame);

    for (const auto& rl : m_lines)
    {
        rl.Draw(frame);
    }

	cv::Mat heatMap = DrawHeatMap();
	if (!heatMap.empty())
		cv::imshow("Heat map", heatMap);
}

///
/// \brief CarsCounting::AddLine
/// \param newLine
///
void CarsCounting::AddLine(const RoadLine& newLine)
{
    m_lines.push_back(newLine);
}

///
/// \brief CarsCounting::GetLine
/// \param lineUid
/// \return
///
bool CarsCounting::GetLine(unsigned int lineUid, RoadLine& line)
{
    for (const auto& rl : m_lines)
    {
        if (rl.m_uid == lineUid)
        {
            line = rl;
            return true;
        }
    }
    return false;
}

///
/// \brief CarsCounting::RemoveLine
/// \param lineUid
/// \return
///
bool CarsCounting::RemoveLine(unsigned int lineUid)
{
    for (auto it = std::begin(m_lines); it != std::end(m_lines);)
    {
        if (it->m_uid == lineUid)
            it = m_lines.erase(it);
        else
            ++it;
    }
    return false;
}

///
/// \brief CarsCounting::CheckLinesIntersection
/// \param track
///
void CarsCounting::CheckLinesIntersection(const TrackingObject& track, float xMax, float yMax)
{
    auto Pti2f = [&](cv::Point pt) -> cv::Point2f
    {
        return cv::Point2f(pt.x / xMax, pt.y / yMax);
    };

	constexpr size_t minTrack = 5;
	if (track.m_trace.size() >= minTrack)
	{
		for (auto& rl : m_lines)
		{
            rl.IsIntersect(track.m_ID, Pti2f(track.m_trace[track.m_trace.size() - minTrack]), Pti2f(track.m_trace[track.m_trace.size() - 1]));
		}
	}
}

///
/// \brief CarsCounting::DrawHeatMap
///
cv::Mat CarsCounting::DrawHeatMap()
{
	cv::Mat res;
	if (!m_heatMap.empty())
	{
		cv::normalize(m_heatMap, m_normHeatMap, 255, 0, cv::NORM_MINMAX, CV_8UC1);
		cv::applyColorMap(m_normHeatMap, m_colorMap, cv::COLORMAP_HOT);
		cv::bitwise_or(m_keyFrame, m_colorMap, res);
	}
	return res;
}

///
/// \brief CarsCounting::AddToHeatMap
///
void CarsCounting::AddToHeatMap(const cv::Rect& rect)
{
	if (m_heatMap.empty())
		return;

	constexpr float w = 0.001f;
	for (int y = 0; y < rect.height; ++y)
	{
		float* heatPtr = m_heatMap.ptr<float>(rect.y + y) + rect.x;
		for (int x = 0; x < rect.width; ++x)
		{
			heatPtr[x] += w;
		}
	}
}
