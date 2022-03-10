cmake_minimum_required (VERSION 3.5)

project(MultitargetTrackerLib)

set(SOURCES
    VideoExample.cpp
	VideoExampleTest.cpp
)

set(HEADERS
    MouseExample.h
    VideoExample.h
    examples.h
    FileLogger.h
	VideoExampleTest.h
)

qt5_wrap_cpp(HEADER_MOC ${HEADERS})

if (BUILD_CARS_COUNTING)
    set(SOURCES ${SOURCES} CarsCounting.cpp)
    set(HEADERS ${HEADERS} CarsCounting.h)
endif(BUILD_CARS_COUNTING)

# ----------------------------------------------------------------------------
# добавляем include директории
# ----------------------------------------------------------------------------
INCLUDE_DIRECTORIES(
                    ${PROJECT_SOURCE_DIR}/..
                    ${PROJECT_SOURCE_DIR}/../common
                    ${PROJECT_SOURCE_DIR}/../Detector
                    ${PROJECT_SOURCE_DIR}/../Detector/vibe_src
                    ${PROJECT_SOURCE_DIR}/../Detector/Subsense
                    ${PROJECT_SOURCE_DIR}/../Tracker
                    ${PROJECT_SOURCE_DIR}/../Tracker/HungarianAlg
                    ${PROJECT_SOURCE_DIR}/../../../thirdparty
)

set(LIBS
    ${OpenCV_LIBS}
	Qt5::Core
	Qt5::Widgets
    mtracking
    mdetection
    inih
)

if (BUILD_YOLO_LIB)
    if (MSVC)
      if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
          set(BIT_SYSTEM x32)
      else()
          set(BIT_SYSTEM x64)
      endif()

      link_directories(${PROJECT_SOURCE_DIR}/../MultitargetTracker/Detector/darknet/3rdparty/lib/${BIT_SYSTEM})
    endif(MSVC)

    add_definitions(-DBUILD_YOLO_LIB)
endif(BUILD_YOLO_LIB)

if (BUILD_YOLO_TENSORRT)
    add_definitions(-DBUILD_YOLO_TENSORRT)
endif(BUILD_YOLO_TENSORRT)

if (BUILD_CARS_COUNTING)
    add_definitions(-DBUILD_CARS_COUNTING)
endif(BUILD_CARS_COUNTING)

add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES} ${HEADER_MOC})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${LIBS})