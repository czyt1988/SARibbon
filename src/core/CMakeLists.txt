find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED COMPONENTS Core Gui)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/SARibbonCoreConfig.h.in"
    "${CMAKE_BINARY_DIR}/include/SARibbonCore/SARibbonCoreConfig.h"
)

sa_add_library(SARibbonCore
    SOURCES
        SARibbonCoreGlobal.h
        SARibbonCoreGlobal.cpp   # 1 行占位源（仅 include 本头），使纯头模块可成库，计划 02 被真实源取代
        SARibbonQt5Compat.hpp
        "${CMAKE_BINARY_DIR}/include/SARibbonCore/SARibbonCoreConfig.h"
    PREFIX SA_RIBBON_CORE
    QT_LINKS Core Gui
)
sa_sync_include(SARibbonCore SARibbonCore)
