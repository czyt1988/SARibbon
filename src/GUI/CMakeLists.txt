set(EXPAMPLE_NAME BallDTP)

set(CMAKE_AUTOMOC ON)

include_directories(${CMAKE_SOURCE_DIR}/src/SARibbonBar)
LINK_DIRECTORIES(${CMAKE_SOURCE_DIR}/src/SARibbon/lib)

include_directories(
                    ${PROJECT_SOURCE_DIR}/src/MultitargetTrackerLib
					${PROJECT_SOURCE_DIR}/src/MultitargetTrackerLib/examples
                    ${PROJECT_SOURCE_DIR}/src/MultitargetTrackerLib/common
                    ${PROJECT_SOURCE_DIR}/src/MultitargetTrackerLib/Detector
                    ${PROJECT_SOURCE_DIR}/src/MultitargetTrackerLib/Detector/vibe_src
                    ${PROJECT_SOURCE_DIR}/src/MultitargetTrackerLib/Detector/Subsense
                    ${PROJECT_SOURCE_DIR}/src/MultitargetTrackerLib/Tracker
                    ${PROJECT_SOURCE_DIR}/src/MultitargetTrackerLib/Tracker/HungarianAlg
                    ${PROJECT_SOURCE_DIR}/thirdparty
)

set(LIBS
    ${OpenCV_LIBS}
	Qt5::Widgets
	SARibbonBar
	MultitargetTrackerLib
)

qt5_add_resources(QRCS BallDTP.qrc)

add_executable(${EXPAMPLE_NAME} 
    ${QRCS}
	mainwindow.h
    mainwindow.cpp
    main.cpp
    )



target_link_libraries(${EXPAMPLE_NAME} PUBLIC ${LIBS})

