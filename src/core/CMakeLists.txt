find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED COMPONENTS Core Gui)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/SARibbonCoreConfig.h.in"
    "${CMAKE_BINARY_DIR}/include/SARibbonCore/SARibbonCoreConfig.h"
)

sa_add_library(SARibbonCore
    SOURCES
        SARibbonCoreGlobal.h
        SARibbonCoreGlobal.cpp   # 占位源（含 ABI 探针导出符号），计划 02 起被真实源逐步取代
        SARibbonQt5Compat.hpp
        global/SARibbonEnums.h
        global/SARibbonCoreUtil.h
        global/SARibbonCoreUtil.cpp
        theme/SARibbonThemePalette.h
        theme/SARibbonThemePalette.cpp
        theme/SARibbonThemeData.h
        theme/SARibbonThemeData.cpp
        "${CMAKE_BINARY_DIR}/include/SARibbonCore/SARibbonCoreConfig.h"
    PREFIX SA_RIBBON_CORE
    QT_LINKS Core Gui
)
sa_sync_include(SARibbonCore SARibbonCore FLATTEN)
