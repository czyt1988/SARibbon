macro(saribbon_set_bin_name _var)
    set(DA_MIN_QT_VERSION 5.14)
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
    ########################################################
    # 平台判断
    ########################################################
    if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
        set(_platform_name "x86")
    else()
        set(_platform_name "x64")
    endif()
    ########################################################
    # 安装路径设置
    ########################################################
    set(_var bin_qt${QT_VERSION}_${CMAKE_BUILD_TYPE}_${_platform_name})
endmacro(damacro_set_bin_name)