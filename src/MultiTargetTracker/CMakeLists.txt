cmake_minimum_required (VERSION 3.5)

project(MultitargetTracker)

# ----------------------------------------------------------------------------
# добавляем include директории
# ----------------------------------------------------------------------------
INCLUDE_DIRECTORIES(
                    ${PROJECT_SOURCE_DIR}/../MultiTargetTrackerLib/
                    ${PROJECT_SOURCE_DIR}/../MultiTargetTrackerLib/common
                    ${PROJECT_SOURCE_DIR}/../MultiTargetTrackerLib/Detector
                    ${PROJECT_SOURCE_DIR}/../MultiTargetTrackerLib/Detector/vibe_src
                    ${PROJECT_SOURCE_DIR}/../MultiTargetTrackerLib/Detector/Subsense
                    ${PROJECT_SOURCE_DIR}/../MultiTargetTrackerLib/Tracker
                    ${PROJECT_SOURCE_DIR}/../MultiTargetTrackerLib/Tracker/HungarianAlg
					${PROJECT_SOURCE_DIR}/../MultiTargetTrackerLib/examples
                    ${PROJECT_SOURCE_DIR}/../../thirdparty
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


ADD_EXECUTABLE(${PROJECT_NAME} main.cpp)

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${PROJECT_NAME}Lib)