#include <iomanip>
#include <ctime>

#include "VideoExample.h"

///
/// \brief VideoExample::VideoExample
/// \param parser
///
VideoExample::VideoExample(const cv::CommandLineParser& parser)
    : m_resultsLog(parser.get<std::string>("res"), parser.get<int>("write_n_frame"))
{
    m_inFile = parser.get<std::string>(0);
    m_outFile = parser.get<std::string>("out");
    m_showLogs = parser.get<int>("show_logs") != 0;
    m_startFrame = parser.get<int>("start_frame");
    m_endFrame = parser.get<int>("end_frame");
    m_finishDelay = parser.get<int>("end_delay");
	m_batchSize = std::max(1, parser.get<int>("batch_size"));

    m_colors.emplace_back(255, 0, 0);
    m_colors.emplace_back(0, 255, 0);
    m_colors.emplace_back(0, 0, 255);
    m_colors.emplace_back(255, 255, 0);
    m_colors.emplace_back(0, 255, 255);
    m_colors.emplace_back(255, 0, 255);
    m_colors.emplace_back(255, 127, 255);
    m_colors.emplace_back(127, 0, 255);
    m_colors.emplace_back(127, 0, 127);

    m_resultsLog.Open();

    std::string settingsFile = parser.get<std::string>("settings");
    m_trackerSettingsLoaded = ParseTrackerSettings(settingsFile, m_trackerSettings);

	if (m_batchSize > 1)
	{
		m_frameInfo[0].SetBatchSize(m_batchSize);
		m_frameInfo[1].SetBatchSize(m_batchSize);
	}
}

///
/// \brief VideoExample::SyncProcess
///
void VideoExample::SyncProcess()
{
    cv::VideoWriter writer;

#ifndef SILENT_WORK
    cv::namedWindow("Video", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
    bool manualMode = false;
#endif

    double freq = cv::getTickFrequency();
    int64 allTime = 0;

    int framesCounter = m_startFrame + 1;

    cv::VideoCapture capture;
    if (!OpenCapture(capture))
    {
        std::cerr << "Can't open " << m_inFile << std::endl;
        return;
    }

	FrameInfo frameInfo(m_batchSize);
	frameInfo.m_frames.resize(frameInfo.m_batchSize);
	frameInfo.m_frameInds.resize(frameInfo.m_batchSize);

    int64 startLoopTime = cv::getTickCount();

    for (;;)
    {
		size_t i = 0;
		for (; i < m_batchSize; ++i)
		{
			capture >> frameInfo.m_frames[i].GetMatBGRWrite();
			if (frameInfo.m_frames[i].empty())
				break;
			frameInfo.m_frameInds[i] = framesCounter;

			++framesCounter;
			if (m_endFrame && framesCounter > m_endFrame)
			{
				std::cout << "Process: riched last " << m_endFrame << " frame" << std::endl;
				break;
			}
		}
		if (i < m_batchSize)
			break;

		if (!m_isDetectorInitialized || !m_isTrackerInitialized)
		{
			cv::UMat ufirst = frameInfo.m_frames[0].GetUMatBGR();
			if (!m_isDetectorInitialized)
			{
				m_isDetectorInitialized = InitDetector(ufirst);
				if (!m_isDetectorInitialized)
				{
					std::cerr << "CaptureAndDetect: Detector initialize error!!!" << std::endl;
					break;
				}
			}
			if (!m_isTrackerInitialized)
			{
				m_isTrackerInitialized = InitTracker(ufirst);
				if (!m_isTrackerInitialized)
				{
					std::cerr << "CaptureAndDetect: Tracker initialize error!!!" << std::endl;
					break;
				}
			}
		}

        int64 t1 = cv::getTickCount();

        regions_t regions;
        Detection(frameInfo);
        Tracking(frameInfo);
        int64 t2 = cv::getTickCount();

        allTime += t2 - t1;
        int currTime = cvRound(1000 * (t2 - t1) / freq);

		for (i = 0; i < m_batchSize; ++i)
		{
			DrawData(frameInfo.m_frames[i].GetMatBGR(), frameInfo.m_tracks[i], frameInfo.m_frameInds[i], currTime);

#ifndef SILENT_WORK
			cv::imshow("Video", frameInfo.m_frames[i].GetMatBGR());

			int waitTime = manualMode ? 0 : 1;// std::max<int>(1, cvRound(1000 / m_fps - currTime));
			int k = cv::waitKey(waitTime);
			if (k == 27)
				break;
			else if (k == 'm' || k == 'M')
				manualMode = !manualMode;
#else
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
#endif

			WriteFrame(writer, frameInfo.m_frames[i].GetMatBGR());
		}
        if (framesCounter % 100 == 0)
            m_resultsLog.Flush();
    }

    int64 stopLoopTime = cv::getTickCount();

    std::cout << "algorithms time = " << (allTime / freq) << ", work time = " << ((stopLoopTime - startLoopTime) / freq) << std::endl;
#ifndef SILENT_WORK
    cv::waitKey(m_finishDelay);
#endif
}

#define SHOW_ASYNC_LOGS 0

///
/// \brief VideoExample::AsyncProcess
///
void VideoExample::AsyncProcess()
{
    std::atomic<bool> stopCapture(false);

    std::thread thCapDet(CaptureAndDetect, this, std::ref(stopCapture));

    cv::VideoWriter writer;

#ifndef SILENT_WORK
    cv::namedWindow("Video", cv::WINDOW_NORMAL | cv::WINDOW_KEEPRATIO);
    bool manualMode = false;
#endif

    double freq = cv::getTickFrequency();

    int64 allTime = 0;
    int64 startLoopTime = cv::getTickCount();
    size_t processCounter = 0;
    for (; !stopCapture.load(); )
    {
        FrameInfo& frameInfo = m_frameInfo[processCounter % 2];
#if SHOW_ASYNC_LOGS
        std::cout << "--- waiting tracking from " << (processCounter % 2) << " ind = " << processCounter << std::endl;
#endif
        {
            std::unique_lock<std::mutex> lock(frameInfo.m_mutex);
            if (!frameInfo.m_cond.wait_for(lock, std::chrono::milliseconds(m_captureTimeOut), [&frameInfo] { return frameInfo.m_captured.load(); }))
            {
                std::cout << "--- Wait frame timeout!" << std::endl;
                break;
            }
        }
#if SHOW_ASYNC_LOGS
        std::cout << "--- tracking from " << (processCounter % 2) << " in progress..." << std::endl;
#endif
        if (!m_isTrackerInitialized)
        {
            m_isTrackerInitialized = InitTracker(frameInfo.m_frames[0].GetUMatBGR());
            if (!m_isTrackerInitialized)
            {
                std::cerr << "--- AsyncProcess: Tracker initialize error!!!" << std::endl;
                frameInfo.m_cond.notify_one();
                break;
            }
        }

        int64 t1 = cv::getTickCount();

        Tracking(frameInfo);

        int64 t2 = cv::getTickCount();

        allTime += t2 - t1 + frameInfo.m_dt;
        int currTime = cvRound(1000 * (t2 - t1 + frameInfo.m_dt) / freq);

#if SHOW_ASYNC_LOGS
        std::cout << "--- Frame " << frameInfo.m_frameInds[0] << ": td = " << (1000 * frameInfo.m_dt / freq) << ", tt = " << (1000 * (t2 - t1) / freq) << std::endl;
#endif

		int key = 0;
		for (size_t i = 0; i < m_batchSize; ++i)
		{
			DrawData(frameInfo.m_frames[i].GetMatBGR(), frameInfo.m_tracks[i], frameInfo.m_frameInds[i], currTime);

			WriteFrame(writer, frameInfo.m_frames[i].GetMatBGR());

#ifndef SILENT_WORK
			cv::imshow("Video", frameInfo.m_frames[i].GetMatBGR());

			int waitTime = manualMode ? 0 : 1;// std::max<int>(1, cvRound(1000 / m_fps - currTime));
			key = cv::waitKey(waitTime);
			if (key == 'm' || key == 'M')
				manualMode = !manualMode;
			else
				break;
#else
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
#endif
		}

#if SHOW_ASYNC_LOGS
        std::cout << "--- tracking m_captured " << (processCounter % 2) << " - captured still " << frameInfo.m_captured.load() << std::endl;
#endif
        assert(frameInfo.m_captured.load());
        frameInfo.m_captured = false;
        frameInfo.m_cond.notify_one();

        if (key == 27)
            break;

        ++processCounter;

        if (processCounter % 100 == 0)
            m_resultsLog.Flush();
    }
    stopCapture = true;

    if (thCapDet.joinable())
        thCapDet.join();

    int64 stopLoopTime = cv::getTickCount();

    std::cout << "--- algorithms time = " << (allTime / freq) << ", work time = " << ((stopLoopTime - startLoopTime) / freq) << std::endl;

#ifndef SILENT_WORK
    cv::waitKey(m_finishDelay);
#endif
}

///
/// \brief VideoExample::CaptureAndDetect
/// \param thisPtr
/// \param stopCapture
///
void VideoExample::CaptureAndDetect(VideoExample* thisPtr, std::atomic<bool>& stopCapture)
{
    cv::VideoCapture capture;
    if (!thisPtr->OpenCapture(capture))
    {
        std::cerr << "+++ Can't open " << thisPtr->m_inFile << std::endl;
        stopCapture = true;
        return;
    }

	int framesCounter = 0;

    const auto localEndFrame = thisPtr->m_endFrame;
    auto localIsDetectorInitialized = thisPtr->m_isDetectorInitialized;
    auto localTrackingTimeOut = thisPtr->m_trackingTimeOut;
    size_t processCounter = 0;
    for (; !stopCapture.load();)
    {
        FrameInfo& frameInfo = thisPtr->m_frameInfo[processCounter % 2];
#if SHOW_ASYNC_LOGS
        std::cout << "+++ waiting capture to " << (processCounter % 2) << " ind = " << processCounter << std::endl;
#endif
        {
            std::unique_lock<std::mutex> lock(frameInfo.m_mutex);
            if (!frameInfo.m_cond.wait_for(lock, std::chrono::milliseconds(localTrackingTimeOut), [&frameInfo] { return !frameInfo.m_captured.load(); }))
            {
                std::cout << "+++ Wait tracking timeout!" << std::endl;
                frameInfo.m_cond.notify_one();
                break;
            }
        }
#if SHOW_ASYNC_LOGS
        std::cout << "+++ capture to " << (processCounter % 2) << " in progress..." << std::endl;
#endif
		if (frameInfo.m_frames.size() < frameInfo.m_batchSize)
		{
			frameInfo.m_frames.resize(frameInfo.m_batchSize);
			frameInfo.m_frameInds.resize(frameInfo.m_batchSize);
		}

        cv::Mat frame;
		size_t i = 0;
		for (; i < frameInfo.m_batchSize; ++i)
		{
			capture >> frame;
			if (frame.empty())
			{
				std::cerr << "+++ CaptureAndDetect: frame is empty!" << std::endl;
				frameInfo.m_cond.notify_one();
				break;
			}
            frameInfo.m_frames[i].GetMatBGRWrite() = frame;
			frameInfo.m_frameInds[i] = framesCounter;
			++framesCounter;

            if (localEndFrame && framesCounter > localEndFrame)
            {
                std::cout << "+++ Process: riched last " << localEndFrame << " frame" << std::endl;
                break;
            }
        }
        if (i < frameInfo.m_batchSize)
            break;

        if (!localIsDetectorInitialized)
        {
            thisPtr->m_isDetectorInitialized = thisPtr->InitDetector(frameInfo.m_frames[0].GetUMatBGR());
            localIsDetectorInitialized = thisPtr->m_isDetectorInitialized;
            if (!thisPtr->m_isDetectorInitialized)
            {
                std::cerr << "+++ CaptureAndDetect: Detector initialize error!!!" << std::endl;
                frameInfo.m_cond.notify_one();
                break;
            }
        }

        int64 t1 = cv::getTickCount();
        thisPtr->Detection(frameInfo);
        int64 t2 = cv::getTickCount();
        frameInfo.m_dt = t2 - t1;

#if SHOW_ASYNC_LOGS
        std::cout << "+++ capture m_captured " << (processCounter % 2) << " - captured still " << frameInfo.m_captured.load() << std::endl;
#endif
        assert(!frameInfo.m_captured.load());
        frameInfo.m_captured = true;
        frameInfo.m_cond.notify_one();

		++processCounter;
    }
    stopCapture = true;
}

///
/// \brief VideoExample::Detection
/// \param frame
/// \param regions
///
void VideoExample::Detection(FrameInfo& frame)
{
	if (m_trackerSettings.m_useAbandonedDetection)
	{
		for (const auto& track : m_tracks)
		{
			if (track.m_isStatic)
				m_detector->ResetModel(frame.m_frames[0].GetUMatBGR(), track.m_rrect.boundingRect());
		}
	}

    std::vector<cv::UMat> frames;
	for (size_t i = 0; i < frame.m_frames.size(); ++i)
	{
        if (m_detector->CanGrayProcessing())
            frames.emplace_back(frame.m_frames[i].GetUMatGray());
        else
            frames.emplace_back(frame.m_frames[i].GetUMatBGR());
	}
	frame.CleanRegions();
    m_detector->Detect(frames, frame.m_regions);
}

///
/// \brief VideoExample::Tracking
/// \param frame
/// \param regions
///
void VideoExample::Tracking(FrameInfo& frame)
{
	assert(frame.m_regions.size() == frame.m_frames.size());

	frame.CleanTracks();
	for (size_t i = 0; i < frame.m_frames.size(); ++i)
	{
		if (m_tracker->CanColorFrameToTrack())
			m_tracker->Update(frame.m_regions[i], frame.m_frames[i].GetUMatBGR(), m_fps);
		else
			m_tracker->Update(frame.m_regions[i], frame.m_frames[i].GetUMatGray(), m_fps);
		m_tracker->GetTracks(frame.m_tracks[i]);
	}
	if (m_trackerSettings.m_useAbandonedDetection)
		m_tracker->GetTracks(m_tracks);
}

///
/// \brief VideoExample::DrawTrack
/// \param frame
/// \param track
/// \param drawTrajectory
///
void VideoExample::DrawTrack(cv::Mat frame,
                             const TrackingObject& track,
                             bool drawTrajectory,
                             int framesCounter)
{
    cv::Scalar color = track.m_isStatic ? cv::Scalar(255, 0, 255) : cv::Scalar(0, 255, 0);
    cv::Point2f rectPoints[4];
    track.m_rrect.points(rectPoints);
    for (int i = 0; i < 4; ++i)
    {
        cv::line(frame, rectPoints[i], rectPoints[(i+1) % 4], color);
    }
#if 0
#if 0
	track_t minAreaRadiusPix = frame.rows / 20.f;
#else
	track_t minAreaRadiusPix = -1.f;
#endif
	track_t minAreaRadiusK = 0.5f;
	cv::Size_<track_t> minRadius(minAreaRadiusPix, minAreaRadiusPix);
	if (minAreaRadiusPix < 0)
	{
		minRadius.width = minAreaRadiusK * track.m_rrect.size.width;
		minRadius.height = minAreaRadiusK * track.m_rrect.size.height;
	}

	Point_t d(3.f * track.m_velocity[0], 3.f * track.m_velocity[1]);
	cv::Size2f els(std::max(minRadius.width, fabs(d.x)), std::max(minRadius.height, fabs(d.y)));
	Point_t p1 = track.m_rrect.center;
	Point_t p2(p1.x + d.x, p1.y + d.y);
	float angle = 0;
	Point_t nc = p1;
	Point_t p2_(p2.x - p1.x, p2.y - p1.y);
	if (fabs(p2_.x - p2_.y) > 5) // pix
	{
		if (fabs(p2_.x) > 0.0001f)
		{
			track_t l = std::min(els.width, els.height) / 3;

			track_t p2_l = sqrt(sqr(p2_.x) + sqr(p2_.y));
			nc.x = l * p2_.x / p2_l + p1.x;
			nc.y = l * p2_.y / p2_l + p1.y;

			angle = atan(p2_.y / p2_.x);
		}
		else
		{
			nc.y += d.y / 3;
			angle = CV_PI / 2.f;
		}
	}

	cv::RotatedRect rr(nc, els, 180.f * angle / CV_PI);
    cv::ellipse(frame, rr, cv::Scalar(100, 0, 100), 1);
#endif
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

	cv::Rect brect = track.m_rrect.boundingRect();
	m_resultsLog.AddTrack(framesCounter, track.m_ID, brect, track.m_type, track.m_confidence);
	m_resultsLog.AddRobustTrack(track.m_ID);
}

///
/// \brief VideoExample::OpenCapture
/// \param capture
/// \return
///
bool VideoExample::OpenCapture(cv::VideoCapture& capture)
{
	if (m_inFile.size() == 1)
	{
#ifdef _WIN32
		capture.open(atoi(m_inFile.c_str()), cv::CAP_DSHOW);
#else
		capture.open(atoi(m_inFile.c_str()));
#endif
		//if (capture.isOpened())
		//	capture.set(cv::CAP_PROP_SETTINGS, 1);
	}
    else
        capture.open(m_inFile);

    if (capture.isOpened())
    {
        capture.set(cv::CAP_PROP_POS_FRAMES, m_startFrame);

        m_fps = std::max(25.f, (float)capture.get(cv::CAP_PROP_FPS));

		std::cout << "Video " << m_inFile << " was started from " << m_startFrame << " frame with " << m_fps << " fps" << std::endl;

        return true;
    }
    return false;
}

///
/// \brief VideoExample::WriteFrame
/// \param writer
/// \param frame
/// \return
///
bool VideoExample::WriteFrame(cv::VideoWriter& writer, const cv::Mat& frame)
{
    if (!m_outFile.empty())
    {
        if (!writer.isOpened())
            writer.open(m_outFile, m_fourcc, m_fps, frame.size(), true);

        if (writer.isOpened())
        {
            writer << frame;
            return true;
        }
    }
    return false;
}
