
# Cmake的命令不区分大小写，例如message，set等命令；但Cmake的变量区分大小写
# 为统一风格，本项目的Cmake命令全部采用小写，变量全部采用大写加下划线组合。

cmake_minimum_required(VERSION 3.21)

project(SARibbon VERSION 3.0.0 LANGUAGES CXX)

set(SARIBBON_VERSION       ${PROJECT_VERSION})
set(SARIBBON_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(SARIBBON_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(SARIBBON_VERSION_PATCH ${PROJECT_VERSION_PATCH})
message(STATUS "SARibbon v${SARIBBON_VERSION}")

option(SARIBBON_BUILD_WIDGETS "build the SARibbonWidgets module" ON)
option(SARIBBON_BUILD_QML "build the SARibbonQml module (plan 04)" OFF)
option(SARIBBON_BUILD_STATIC_LIBS "build the SARibbon modules in static lib mode" OFF)
option(SARIBBON_BUILD_EXAMPLES "build the examples" ON)
option(SARIBBON_BUILD_TESTS "build unit tests" OFF)
# frameless能提供windows的窗口特效，如边缘吸附，且对高分屏多屏幕的支持更好
option(SARIBBON_USE_FRAMELESS_LIB "Using the QWindowKit library as a frameless solution" OFF)
# windows11的snap layout效果，仅在SARIBBON_USE_FRAMELESS_LIB=ON时生效
option(SARIBBON_ENABLE_SNAPLAYOUT "Whether to enable the Snap Layout effect in Windows 11, this option only takes effect when SARIBBON_USE_FRAMELESS_LIB=ON" OFF)
# 是否产生安装/导出规则；add_subdirectory嵌入场景可设OFF避免污染宿主工程的install/export
option(SARIBBON_INSTALL "Whether to generate install and export rules" ON)
# 在当前目录安装，此操作会让库安装在当前目录下，新建类似bin_qt5.14.2_msvc_x64这样的文件夹，否则就安装到默认位置，windows的默认位置一般位于C:\Program Files
if(WIN32)
    option(SARIBBON_INSTALL_IN_CURRENT_DIR "Whether to install in the current directory" ON)
else()
    option(SARIBBON_INSTALL_IN_CURRENT_DIR "Whether to install in the current directory" OFF)
endif()

# 旧选项名兼容（CI/脚本过渡期，一个版本周期后移除）
if(DEFINED BUILD_TESTS)
    set(SARIBBON_BUILD_TESTS "${BUILD_TESTS}" CACHE BOOL "compat shim of BUILD_TESTS" FORCE)
    message(DEPRECATION "BUILD_TESTS is deprecated, use SARIBBON_BUILD_TESTS instead")
endif()

# vcpkg detection
# When using vcpkg toolchain, CMAKE_TOOLCHAIN_FILE points to vcpkg.cmake
# and VCPKG_TOOLCHAIN is defined by the toolchain file.
if(VCPKG_TOOLCHAIN)
    message(STATUS "vcpkg toolchain detected")
    message(STATUS "  VCPKG_TARGET_TRIPLET: ${VCPKG_TARGET_TRIPLET}")
    if(VCPKG_INSTALLED_DIR)
        message(STATUS "  VCPKG_INSTALLED_DIR: ${VCPKG_INSTALLED_DIR}")
    endif()
    # When vcpkg manifest feature "frameless" is active, auto-enable the option
    if("frameless" IN_LIST VCPKG_MANIFEST_FEATURES)
        set(SARIBBON_USE_FRAMELESS_LIB ON CACHE BOOL "Auto-enabled by vcpkg frameless feature" FORCE)
        message(STATUS "  vcpkg feature 'frameless' is active, SARIBBON_USE_FRAMELESS_LIB=ON")
    endif()
    # When vcpkg manifest feature "qml" is active, auto-enable the QML module
    if("qml" IN_LIST VCPKG_MANIFEST_FEATURES)
        set(SARIBBON_BUILD_QML ON CACHE BOOL "Auto-enabled by vcpkg qml feature" FORCE)
        message(STATUS "  vcpkg feature 'qml' is active, SARIBBON_BUILD_QML=ON")
    endif()
endif()

# QML 模块的无边框窗口方案固定使用 QWindowKit（QuickWindowAgent 路线）：
# SARIBBON_BUILD_QML=ON 时强制开启 SARIBBON_USE_FRAMELESS_LIB（用户显式设 OFF
# 也会被覆盖，因为 QML 模块没有本地化无边框回退路径）；Qt 版本门槛与 QWindowKit
# 查找失败时在下方直接 FATAL_ERROR，不做静默降级
if(SARIBBON_BUILD_QML AND NOT SARIBBON_USE_FRAMELESS_LIB)
    set(SARIBBON_USE_FRAMELESS_LIB ON CACHE BOOL
        "auto-enabled by SARIBBON_BUILD_QML: the QML module requires QWindowKit" FORCE)
    message(STATUS "SARIBBON_BUILD_QML=ON: SARIBBON_USE_FRAMELESS_LIB auto-enabled (QWindowKit required)")
endif()

# load Qt library, minimum version required is 5.15
# cn:Qt库加载,最低版本要求为5.12
set(SARIBBON_MIN_QT_VERSION 5.12)
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Widgets Svg REQUIRED)
if(SARIBBON_BUILD_QML)
    # QML 模式：根层就把 Quick/Qml 找齐——已安装 QWK 的导出 target 引用
    # Qt::Quick，若根层未 find，QWK Targets 文件里的 Qt6::Quick 会缺定义
    find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} COMPONENTS
        Core
        Gui
        Widgets
        Svg
        Qml
        Quick
        QuickControls2
        REQUIRED
    )
else()
    find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} COMPONENTS
        Core
        Gui
        Widgets
        Svg
        REQUIRED
    )
endif()
message(STATUS "current Qt version is Qt${QT_VERSION_MAJOR}.${QT_VERSION_MINOR}.${QT_VERSION_PATCH}")

# 根据qt版本确认能否使用frameless库，目前frameless库支持qt5.14,qt5.15,qt6.1+,除了上诉版本，都使用不了
# 上述版本的qt如果设置SARIBBON_USE_FRAMELESS_LIB=ON也会改为OFF
set(_SARIBBON_USE_FRAMELESS_LIB ${SARIBBON_USE_FRAMELESS_LIB})
if(SARIBBON_USE_FRAMELESS_LIB)
    if(${QT_VERSION_MAJOR} EQUAL 5)
        # qt版本为5，判断是否小版本小于14，小于14无法使用
        if(${QT_VERSION_MINOR} LESS 14)
            #5.14和5.15可以使用frameless
            set(_SARIBBON_USE_FRAMELESS_LIB OFF)
            if(SARIBBON_BUILD_QML)
                # QML 模块强制使用 QWindowKit，Qt 版本不满足时直接失败而非静默降级
                message(FATAL_ERROR "SARIBBON_BUILD_QML=ON requires QWindowKit, which needs Qt >= 5.14 "
                    "(current Qt${QT_VERSION}). Use a newer Qt or set SARIBBON_BUILD_QML=OFF.")
            endif()
            message(WARNING "QT_VERSION = Qt${QT_VERSION} QT version minor is ${QT_VERSION_MINOR},less 14,will set SARIBBON_USE_FRAMELESS_LIB OFF")
        endif()
    elseif(${QT_VERSION_MAJOR} EQUAL 6)
        # qt版本为6，判断是否小版本大于6.1
        if(${QT_VERSION_MINOR} LESS_EQUAL 1)
            #6.0及bug多多无法使用frameless
            set(_SARIBBON_USE_FRAMELESS_LIB OFF)
            if(SARIBBON_BUILD_QML)
                # QML 模块强制使用 QWindowKit，Qt 版本不满足时直接失败而非静默降级
                message(FATAL_ERROR "SARIBBON_BUILD_QML=ON requires QWindowKit, which needs Qt >= 6.2 "
                    "(current Qt${QT_VERSION}). Use a newer Qt or set SARIBBON_BUILD_QML=OFF.")
            endif()
            message(WARNING "QT_VERSION = Qt${QT_VERSION} QT version minor is ${QT_VERSION_MINOR},less 1,will set SARIBBON_USE_FRAMELESS_LIB OFF")
        endif()
    endif()
endif()

# 3.0 全模块统一 C++17（各模块经 sa_add_library 的 target_compile_features PUBLIC 传播）
message(STATUS "SARibbon 3.0 requires C++17")

set(CMAKE_DEBUG_POSTFIX "d" CACHE STRING "add a postfix, usually d on windows")
set(CMAKE_RELEASE_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_RELWITHDEBINFO_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
set(CMAKE_MINSIZEREL_POSTFIX "" CACHE STRING "add a postfix, usually empty on windows")
# Set the build postfix extension according to what configuration is being built.

if(CMAKE_BUILD_TYPE MATCHES "Release")
    set(CMAKE_BUILD_POSTFIX "${CMAKE_RELEASE_POSTFIX}")
elseif (CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
    set(CMAKE_BUILD_POSTFIX "${CMAKE_MINSIZEREL_POSTFIX}")
elseif(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    set(CMAKE_BUILD_POSTFIX "${CMAKE_RELWITHDEBINFO_POSTFIX}")
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_BUILD_POSTFIX "${CMAKE_DEBUG_POSTFIX}")
else()
    set(CMAKE_BUILD_POSTFIX "")
endif()

if(MSVC)
    # /utf-8：源文件按 UTF-8（无 BOM）保存，中文注释在本地代码页（GBK）下会被
    # 误解析——注释尾字节 0x5C 还会发生行拼接吞掉下一行，连带吞掉命名空间右括号，
    # 让后续 Qt 头文件声明落入项目命名空间（表现为 SARibbon::QSet 之类的连锁
    # C2039）。Qt6 的 CMake 包自带 /utf-8，Qt5 没有，必须在工程里显式补上
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4819 /utf-8")
    # Qt 6.5.3 的 qvarlengtharray.h 使用了 stdext::unchecked_array_iterator，
    # 新版 MSVC STL 在 C++17 模式下不再声明 stdext 命名空间（_HAS_AUTO_PTR_ETC=0）
    add_compile_definitions(_HAS_AUTO_PTR_ETC=1)
endif()


# 平台判断
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(SARIBBON_PLATFORM "x86")
else()
    set(SARIBBON_PLATFORM "x64")
endif()

# The bin file directory is one level above the current directory
# cn:bin文件目录在当前目录的上上一级
# 安装目录默认名字为bin_qt5.14.2_msvc_x64
set(SARIBBON_LOCAL_INSTALL_BIN_NAME bin_qt${QT_VERSION}_${CMAKE_CXX_COMPILER_ID}_${SARIBBON_PLATFORM})
set(SARIBBON_LOCAL_INSTALL_BIN_DIR ${CMAKE_CURRENT_LIST_DIR}/${SARIBBON_LOCAL_INSTALL_BIN_NAME})

# windows系统下，默认直接安装到当前文件夹下
if(SARIBBON_INSTALL_IN_CURRENT_DIR)
    set(CMAKE_INSTALL_PREFIX "${SARIBBON_LOCAL_INSTALL_BIN_DIR}")
    message(STATUS "SARIBBON_INSTALL_IN_CURRENT_DIR=ON,will install in:${CMAKE_INSTALL_PREFIX}")
endif()


# QWindowKit的依赖
# QWindowKit的查找顺序：
# 1. 用户通过-DQWindowKit_DIR指定的路径
# 2. 项目本地安装目录 (bin_qtX.X.X_compiler_platform/lib/cmake/QWindowKit)
# 3. 系统默认安装路径
# 4. 3rdparty/qwindowkit submodule 树内构建（上述查找均未命中且 submodule 已初始化时
#    自动触发：QWK 与 SARibbon 同一次构建一起编译链接，无需分开安装）
# 对于开发者，可以在配置时指定路径：
# cmake -DQWindowKit_DIR={path-to-qwindowkit/lib/cmake/QWindowKit}
if(_SARIBBON_USE_FRAMELESS_LIB)
    # 明确记录尝试查找的路径
    set(_qwk_search_paths)

    # 如果用户已指定QWindowKit_DIR，优先使用
    if(QWindowKit_DIR)
        list(APPEND _qwk_search_paths ${QWindowKit_DIR})
        message(STATUS "Using user-specified QWindowKit_DIR: ${QWindowKit_DIR}")
    endif()

    # 添加项目本地安装目录作为备选路径
    set(_local_qwk_dir ${SARIBBON_LOCAL_INSTALL_BIN_DIR}/lib/cmake/QWindowKit)
    if(EXISTS "${_local_qwk_dir}")
        list(APPEND _qwk_search_paths ${_local_qwk_dir})
        message(STATUS "Adding local install path for QWindowKit: ${_local_qwk_dir}")
    endif()

    # 配置覆盖预检：已安装包必须覆盖本构建会产出的配置，否则直接不调用
    # find_package（IMPORTED target 不创建，树内构建的 namespace 别名才能建立）。
    # 实测踩坑：Release-only 安装包（只装过 Release）被 Debug 构建链接后，
    # 进程内同时出现 Debug Qt（Qt6Cored）与 Release Qt（QWKCore.dll 拉入）两个
    # 运行时——QWK 的跨运行时线程亲和性检查全部假性失败（"Cannot create
    # children / installEventFilter: different thread"），窗口钩子装不上，
    # 无边框静默失效（原生边框保留）。树内构建与宿主同编译器/Qt/配置，天然 ABI 一致
    set(_qwk_candidate_dir)
    if(QWindowKit_DIR)
        set(_qwk_candidate_dir "${QWindowKit_DIR}")
    elseif(EXISTS "${_local_qwk_dir}")
        set(_qwk_candidate_dir "${_local_qwk_dir}")
    endif()
    set(_qwk_config_covered TRUE)
    if(_qwk_candidate_dir)
        set(_qwk_configs)
        get_property(_qwk_is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
        if(_qwk_is_multi_config)
            set(_qwk_configs ${CMAKE_CONFIGURATION_TYPES})
        else()
            set(_qwk_configs ${CMAKE_BUILD_TYPE})
        endif()
        set(_qwk_missing_configs)
        foreach(_cfg ${_qwk_configs})
            string(TOLOWER "${_cfg}" _cfg_lower)
            if(_cfg_lower STREQUAL "debug")
                if(NOT EXISTS "${_qwk_candidate_dir}/QWindowKitTargets-debug.cmake")
                    list(APPEND _qwk_missing_configs "${_cfg}")
                endif()
            elseif(NOT _cfg_lower STREQUAL "")
                if(NOT EXISTS "${_qwk_candidate_dir}/QWindowKitTargets-release.cmake")
                    list(APPEND _qwk_missing_configs "${_cfg}")
                endif()
            endif()
        endforeach()
        if(_qwk_missing_configs)
            message(STATUS "Installed QWindowKit at ${_qwk_candidate_dir} does not cover "
                "configuration(s): ${_qwk_missing_configs} (Debug/Release QWK ABI mismatch) "
                "- skipping it and building bundled instead")
            set(_qwk_config_covered FALSE)
        endif()
    endif()

    # 尝试查找QWindowKit包
    if(_qwk_config_covered)
        find_package(QWindowKit QUIET
            PATHS ${_qwk_search_paths}
            NO_DEFAULT_PATH  # 禁止在系统默认路径查找
        )

        # 如果未找到，尝试系统路径作为最后手段
        if(NOT QWindowKit_FOUND)
            message(STATUS "QWindowKit not found in specified paths, trying system default...")
            find_package(QWindowKit QUIET)
        endif()
    endif()

    if(NOT QWindowKit_FOUND AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/qwindowkit/CMakeLists.txt")
        # ---- 树内兜底：把 3rdparty/qwindowkit submodule 直接编进本次构建 ----
        # 用户无需先到 3rdparty/ 单独编译安装：打开顶层 CMakeLists.txt 一次构建即可
        message(STATUS "QWindowKit not installed - building bundled from 3rdparty/qwindowkit")
        # ABI 一致性：QWK 跟随 SARibbon 的静态/动态选择
        set(QWINDOWKIT_BUILD_STATIC ${SARIBBON_BUILD_STATIC_LIBS} CACHE BOOL
            "bundled build: follow SARIBBON_BUILD_STATIC_LIBS" FORCE)
        set(QWINDOWKIT_BUILD_EXAMPLES OFF CACHE BOOL "bundled build: no examples" FORCE)
        # Quick 组件是 SARibbonQml 的硬依赖，树内构建始终开启
        set(QWINDOWKIT_BUILD_QUICK ON CACHE BOOL "bundled build: Quick required by SARibbonQml" FORCE)
        # 安装跟随宿主：SARIBBON_INSTALL=ON 时 QWK 一并装到同一前缀
        # （生成 lib/cmake/QWindowKit，之后无需 submodule 也能 find_package 命中）；
        # add_subdirectory 嵌入场景（SARIBBON_INSTALL=OFF）时关闭，
        # 避免污染宿主工程的 install/export 规则
        set(QWINDOWKIT_INSTALL ${SARIBBON_INSTALL} CACHE BOOL
            "bundled build: follow SARIBBON_INSTALL" FORCE)
        add_subdirectory(3rdparty/qwindowkit)
        foreach(_qwk_tgt QWKCore QWKWidgets QWKQuick)
            if(TARGET ${_qwk_tgt})
                if(WIN32)
                    # QWK 的 Windows 头需要 NOMINMAX（3rdparty 独立构建同款处理）
                    target_compile_definitions(${_qwk_tgt} PRIVATE NOMINMAX)
                endif()
                # QWK 默认输出到 out-<arch>-<config>/bin（qm_init_directories 行为），
                # 与 SARibbon 的 ${BINARY}/bin/ 不一致：统一到同一 bin 目录，
                # 示例/测试 exe 才能在运行时找到 QWK dll
                set_target_properties(${_qwk_tgt} PROPERTIES
                    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}"
                    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
                    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
                )
                # 多配置生成器（VS）下还要覆盖 per-config 变体，否则 VS 仍写
                # out-<arch>-<config>/bin/Release
                foreach(_cfg Debug Release RelWithDebInfo MinSizeRel)
                    set_target_properties(${_qwk_tgt} PROPERTIES
                        RUNTIME_OUTPUT_DIRECTORY_${_cfg} "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}"
                        LIBRARY_OUTPUT_DIRECTORY_${_cfg} "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
                        ARCHIVE_OUTPUT_DIRECTORY_${_cfg} "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
                    )
                endforeach()
            endif()
        endforeach()
        set(_SARIBBON_USE_BUNDLED_QWK ON)
    endif()

    # 最终确认是否可用（已安装包或树内构建）
    if(_SARIBBON_USE_BUNDLED_QWK)
        message(STATUS "QWindowKit: bundled in-tree build (targets QWindowKit::Core/Widgets/Quick)")
    elseif(QWindowKit_FOUND)
        message(STATUS "QWindowKit (installed) found at: ${QWindowKit_DIR}")
        # QML 模块需要 Quick 组件（QWindowKit::Quick）；无组件式 find_package
        #（旧版本）导出全部 target，此时一并检查 Quick target 是否存在
        if(SARIBBON_BUILD_QML AND NOT TARGET QWindowKit::Quick)
            message(FATAL_ERROR "SARIBBON_BUILD_QML=ON requires QWindowKit's Quick component "
                "(QWindowKit::Quick target). Please rebuild/install QWindowKit with "
                "QWINDOWKIT_BUILD_QUICK=ON (see 3rdparty/CMakeLists.txt, option QWK_BUILD_QUICK).")
        endif()
    else()
        set(_SARIBBON_USE_FRAMELESS_LIB OFF)
        if(SARIBBON_BUILD_QML)
            # QML 模块强制使用 QWindowKit，找不到时直接失败而非静默降级
            message(FATAL_ERROR "SARIBBON_BUILD_QML=ON requires the QWindowKit library, which was "
                "not found. Either build and install it first (docs: build-3rdparty.md, optionally "
                "via -DQWindowKit_DIR=<path-to-qwindowkit>/lib/cmake/QWindowKit), or initialize the "
                "submodule once with 'git submodule update --init --recursive' and reconfigure: "
                "the top-level build will then compile QWindowKit automatically. "
                "Searched paths:\n  ${_qwk_search_paths}")
        endif()
        message(WARNING "QWindowKit not found - disabling frameless support. Searched paths:\n  ${_qwk_search_paths}")
    endif()
endif()


include(cmake/WinResource.cmake)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(SARibbonUtils)

########################################################
# 自动化生成配置头文件
########################################################
configure_file (
  "${CMAKE_CURRENT_LIST_DIR}/src/widgets/SARibbonBarVersionInfo.h.in"
  "${CMAKE_CURRENT_LIST_DIR}/src/widgets/SARibbonBarVersionInfo.h"
)

##################################
# 代码目录
##################################

# 根据 SARIBBON_BUILD_STATIC_LIBS 决定库类型（在 add_library 中显式指定）
# 不再使用 FORCE 覆盖 BUILD_SHARED_LIBS，避免影响子项目嵌入场景
if(SARIBBON_BUILD_STATIC_LIBS)
    message(STATUS "SARIBBON_BUILD_STATIC_LIBS is ON. Building Static Library.")
else()
    message(STATUS "SARIBBON_BUILD_STATIC_LIBS is OFF. Building Shared Library/DLL.")
endif()

add_subdirectory(src)

# 顶层工程守卫：嵌入场景（add_subdirectory）只建库，不进 examples/tests
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    if(SARIBBON_BUILD_EXAMPLES)
        message(STATUS "build example")
        add_subdirectory(examples)
    endif()

    if(SARIBBON_BUILD_TESTS)
        message(STATUS "build tests")
        enable_testing()
        add_subdirectory(tests)
    endif()
endif()
