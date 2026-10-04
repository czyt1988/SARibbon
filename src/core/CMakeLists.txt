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
        SARibbonEnums.h
        SARibbonCoreUtil.h
        SARibbonCoreUtil.cpp
        SARibbonThemePalette.h
        SARibbonThemePalette.cpp
        SARibbonThemeData.h
        SARibbonThemeData.cpp
        SARibbonMetrics.h
        SARibbonMetrics.cpp
        SARibbonAbstractLayoutItem.h
        SARibbonAbstractLayoutHost.h
        SARibbonContract.cpp
        SARibbonCustomizeRecord.h
        SARibbonCustomizeRecord.cpp
        SARibbonCustomizeXml.h
        SARibbonElementFactoryInterface.h
        SARibbonPanelLayoutEngine.h
        SARibbonPanelLayoutEngine.cpp
        SARibbonCategoryLayoutEngine.h
        SARibbonCategoryLayoutEngine.cpp
        SARibbonBarGeometryEngine.h
        SARibbonBarGeometryEngine.cpp
        SARibbonToolButtonLayout.h
        SARibbonToolButtonLayout.cpp
        "${CMAKE_BINARY_DIR}/include/SARibbonCore/SARibbonCoreConfig.h"
    PREFIX SA_RIBBON_CORE
    QT_LINKS Core Gui
)
sa_sync_include(SARibbonCore SARibbonCore FLATTEN)
