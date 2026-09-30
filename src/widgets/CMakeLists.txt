# SARibbonWidgets module (migrated file lists kept verbatim from 2.9.5)
find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED
    COMPONENTS Core Gui Widgets Svg)

#################################################
# SAColorWidgets相关
# https://github.com/czyt1988/SAColorWidgets
#################################################
SET(SACOLOR_DIR ${CMAKE_CURRENT_SOURCE_DIR}/colorWidgets)
SET(SACOLOR_HEADER_FILES
    ${SACOLOR_DIR}/SAColorWidgetsGlobal.h
    ${SACOLOR_DIR}/SAColorToolButton.h
    ${SACOLOR_DIR}/SAColorGridWidget.h
    ${SACOLOR_DIR}/SAColorPaletteGridWidget.h
    ${SACOLOR_DIR}/SAColorMenu.h
)
SET(SACOLOR_SOURCE_FILES
    ${SACOLOR_DIR}/SAColorToolButton.cpp
    ${SACOLOR_DIR}/SAColorGridWidget.cpp
    ${SACOLOR_DIR}/SAColorPaletteGridWidget.cpp
    ${SACOLOR_DIR}/SAColorMenu.cpp
)

#################################################
# SARibbonBar相关
#################################################

# header files
# cn:头文件
SET(SARIBBON_HEADER_FILES
    SARibbonBarVersionInfo.h
    SARibbonWidgetsGlobal.h
    SARibbonGlobal.h
    SARibbonUtil.h
    SAFramelessHelper.h
    SARibbonActionsManager.h
    SARibbonBarLayout.h
    SARibbonBar.h
    SARibbonCustomizeData.h
    SARibbonCustomizeDialog.h
    SARibbonCustomizeWidget.h
    SARibbonMainWindow.h
    SARibbonWidget.h
    SARibbonSystemButtonBar.h
    SARibbonApplicationButton.h
    SARibbonTabBar.h
    SARibbonThemeManager.h
    SARibbonThemePalette.h
    SARibbonCategory.h
    SARibbonContextCategory.h
    SARibbonPanel.h
    SARibbonPanelLayout.h
    SARibbonPanelItem.h
    SARibbonPanelOptionButton.h
    SARibbonToolButton.h
    SARibbonMenu.h
    SARibbonSeparatorWidget.h
    SARibbonCategoryLayout.h
    SARibbonGallery.h
    SARibbonGalleryGroup.h
    SARibbonGalleryItem.h
    SARibbonElementFactory.h
    SARibbonElementManager.h
    SARibbonButtonGroupWidget.h
    SARibbonStackedWidget.h
    SARibbonQuickAccessBar.h
    SARibbonCtrlContainer.h
    SARibbonLineWidgetContainer.h
    SARibbonColorToolButton.h
    SARibbonApplicationWidget.h
    SARibbonTitleIconWidget.h
)

# source files
# cn:cpp文件
SET(SARIBBON_SOURCE_FILES
    SARibbonUtil.cpp
    SAFramelessHelper.cpp
    SARibbonActionsManager.cpp
    SARibbonMdiControlsStyle.cpp
    SARibbonBarLayout.cpp
    SARibbonBar.cpp
    SARibbonCustomizeData.cpp
    SARibbonCustomizeDialog.cpp
    SARibbonCustomizeWidget.cpp
    SARibbonMainWindow.cpp
    SARibbonWidget.cpp
    SARibbonSystemButtonBar.cpp
    SARibbonApplicationButton.cpp
    SARibbonTabBar.cpp
    SARibbonThemeManager.cpp
    SARibbonThemePalette.cpp
    SARibbonCategory.cpp
    SARibbonContextCategory.cpp
    SARibbonPanel.cpp
    SARibbonPanelLayout.cpp
    SARibbonPanelItem.cpp
    SARibbonPanelOptionButton.cpp
    SARibbonToolButton.cpp
    SARibbonMenu.cpp
    SARibbonSeparatorWidget.cpp
    SARibbonCategoryLayout.cpp
    SARibbonGallery.cpp
    SARibbonGalleryGroup.cpp
    SARibbonGalleryItem.cpp
    SARibbonElementFactory.cpp
    SARibbonElementManager.cpp
    SARibbonButtonGroupWidget.cpp
    SARibbonStackedWidget.cpp
    SARibbonQuickAccessBar.cpp
    SARibbonCtrlContainer.cpp
    SARibbonLineWidgetContainer.cpp
    SARibbonColorToolButton.cpp
    SARibbonApplicationWidget.cpp
    SARibbonTitleIconWidget.cpp
)

# resource files
# cn:资源文件
SET(SARIBBON_RESOURCE_FILES
    SARibbonResource.qrc
)

message(STATUS "SARibbon resource file:${SARIBBON_RESOURCE_FILES}")

# ---- frameless（qwindowkit库 https://github.com/stdware/qwindowkit）----
# Qt X11Extras was first introduced in 5.1 and got removed in 6.0
# But it was again brought back as a private feature of QtGui in 6.2
set(_SA_WIDGETS_PRIVATE_LINKS)
if(_SARIBBON_USE_FRAMELESS_LIB)
    find_package(QWindowKit REQUIRED)
    list(APPEND _SA_WIDGETS_PRIVATE_LINKS QWindowKit::Widgets)
endif()

sa_add_library(SARibbonWidgets
    SOURCES
        ${SARIBBON_HEADER_FILES}
        ${SARIBBON_SOURCE_FILES}
        ${SACOLOR_HEADER_FILES}
        ${SACOLOR_SOURCE_FILES}
        ${SARIBBON_RESOURCE_FILES}
        SARibbonQt5Compat.hpp    # 3.0 转发头（实体在 src/core），行保留
    PREFIX SA_RIBBON_WIDGETS
    QT_LINKS Core Gui Widgets Svg
    LINKS SARibbonCore
    LINKS_PRIVATE ${_SA_WIDGETS_PRIVATE_LINKS}
)
sa_sync_include(SARibbonWidgets SARibbonWidgets)

# ---- frameless PUBLIC 宏定义块 ----
if(_SARIBBON_USE_FRAMELESS_LIB)
    #使用frameless必须设置SARIBBON_USE_3RDPARTY_FRAMELESSHELPER宏为1,此宏必须为PUBLIC
    target_compile_definitions(SARibbonWidgets PUBLIC SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1)
    message(STATUS "macros predefined:#define SARIBBON_USE_3RDPARTY_FRAMELESSHELPER 1")
    if(SARIBBON_ENABLE_SNAPLAYOUT)
        # 是否开启windows11的snap layout效果，目前这个在不同qt版本下有bug
        target_compile_definitions(SARibbonWidgets PUBLIC SARIBBON_ENABLE_SNAP_LAYOUT=1)
        message(STATUS "macros predefined:#define SARIBBON_ENABLE_SNAP_LAYOUT 1")
    else()
        target_compile_definitions(SARibbonWidgets PUBLIC SARIBBON_ENABLE_SNAP_LAYOUT=0)
        message(STATUS "macros predefined:#define SARIBBON_ENABLE_SNAP_LAYOUT 0")
    endif()
else()
    #不使用frameless必须设置SARIBBON_USE_3RDPARTY_FRAMELESSHELPER宏为0,此宏必须为PUBLIC
    target_compile_definitions(SARibbonWidgets PUBLIC SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=0)
    message(STATUS "macros predefined:#define SARIBBON_USE_3RDPARTY_FRAMELESSHELPER 0")
endif()

if(MSVC)
    # To unbreak std::min/max
    target_compile_definitions(SARibbonWidgets PRIVATE NOMINMAX)
    message(STATUS "macros predefined:#define NOMINMAX")
endif()

##################################
# 翻译
##################################
option(SARIBBON_UPDATE_TRANSLATIONS "Run lupdate to update .ts source files (modifies .ts files)" OFF)
find_package(Qt${QT_VERSION_MAJOR}LinguistTools QUIET)
    if(Qt${QT_VERSION_MAJOR}LinguistTools_FOUND)
    # 2. 准备 ts 文件列表（可以写多个语言）
    set(TS_FILES
        i18n/SARibbon_zh_CN.ts
        i18n/SARibbon_en_US.ts
    )

    set(TS_SOURCES
        ${SARIBBON_HEADER_FILES}
        ${SARIBBON_SOURCE_FILES}
        ${SACOLOR_HEADER_FILES}
        ${SACOLOR_SOURCE_FILES}
    )
    # 3. 生成/更新 ts 文件 + 编译出 qm 文件
    #    这一步会在 build 目录里生成 myapp_zh_CN.qm 等
    # 4. 根据 Qt 版本调对应宏
    if(SARIBBON_UPDATE_TRANSLATIONS)
        # lupdate + lrelease: updates .ts files with new strings from source
        if(QT_VERSION_MAJOR EQUAL 5)
            qt5_create_translation(QM_FILES ${TS_SOURCES} ${TS_FILES})
        elseif(QT_VERSION_MAJOR EQUAL 6)
            qt6_create_translation(QM_FILES ${TS_SOURCES} ${TS_FILES})
        endif()
    else()
        # lrelease only: compiles existing .ts -> .qm without modifying .ts
        if(QT_VERSION_MAJOR EQUAL 5)
            qt5_add_translation(QM_FILES ${TS_FILES})
        elseif(QT_VERSION_MAJOR EQUAL 6)
            qt6_add_translation(QM_FILES ${TS_FILES})
        endif()
    endif()
    # 5. 把生成的 qm 挂到目标上
    target_sources(SARibbonWidgets PRIVATE ${QM_FILES})

    message(STATUS "Translation support enabled (LinguistTools found).")

    # 安装 SARibbon 翻译到 bin/translations/
    if(SARIBBON_INSTALL)   # v2 §6.4 (round2 D1)
        install(FILES ${QM_FILES}
            DESTINATION ${CMAKE_INSTALL_BINDIR}/translations
            COMPONENT translations
        )
    endif()

    # 复制到构建输出目录，供开发调试时使用
    set(_saribbon_qm_output_dir "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}/translations")
    file(MAKE_DIRECTORY "${_saribbon_qm_output_dir}")
    foreach(_qm_file ${QM_FILES})
        add_custom_command(TARGET SARibbonWidgets POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                "${_qm_file}" "${_saribbon_qm_output_dir}/"
            COMMENT "Copying SARibbon translation ${_qm_file}"
        )
    endforeach()
endif()

# ---- 旧 target 名兼容（D2），过渡一个版本周期，计划 03 统一改 SARibbon::Widgets ----
add_library(SARibbonBar ALIAS SARibbonWidgets)
add_library(SARibbonBar::SARibbonBar ALIAS SARibbonWidgets)

# 3.0 兼容转发头：include/SARibbonBar/<name>.h -> #include <SARibbonWidgets/<name>.h>
set(_compat_dir "${CMAKE_BINARY_DIR}/compat-include/SARibbonBar")
file(MAKE_DIRECTORY "${_compat_dir}")
foreach(_h IN LISTS SARIBBON_HEADER_FILES)
    get_filename_component(_name "${_h}" NAME)
    file(WRITE "${_compat_dir}/${_name}"
        "#pragma once\n#include <SARibbonWidgets/${_name}>\n")
endforeach()
# colorWidgets 头的旧路径转发
file(MAKE_DIRECTORY "${_compat_dir}/colorWidgets")
foreach(_h IN LISTS SACOLOR_HEADER_FILES)
    get_filename_component(_name "${_h}" NAME)
    file(WRITE "${_compat_dir}/colorWidgets/${_name}"
        "#pragma once\n#include <SARibbonWidgets/colorWidgets/${_name}>\n")
endforeach()
if(SARIBBON_INSTALL)   # v2 §6.4 (round2 D1)
    install(DIRECTORY "${_compat_dir}/" DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/SARibbonBar")
endif()
