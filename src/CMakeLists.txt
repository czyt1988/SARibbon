add_subdirectory(core)
if(SARIBBON_BUILD_WIDGETS)
    add_subdirectory(widgets)
endif()
if(SARIBBON_BUILD_QML)
    add_subdirectory(qml)
endif()
# 统一导出（三模块共用一个 export set，只在这里 install 一次；
# SARIBBON_INSTALL 守卫内——v2 §6.4/round2 D1，与 S5.3/S11 的安装规则同守卫）
if(SARIBBON_INSTALL)
    install(EXPORT SARibbonTargets
        FILE SARibbonTargets.cmake
        NAMESPACE SARibbon::
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SARibbon)
    # 2.9.5 行为保持：单文件产物安装规则——原模块 CMakeLists 被 S6.1 删除后由这里承担
    install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/SARibbon.h
                  ${CMAKE_CURRENT_SOURCE_DIR}/SARibbon.cpp
        DESTINATION share/SARibbonBar_amalgamate)
endif()
# 组件化 SARibbonConfig 生成/安装（S11，SARibbon_INSTALL 守卫内）
if(SARIBBON_INSTALL)
    configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/SARibbonConfig.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/SARibbonConfig.cmake"
        INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SARibbon
    )
    write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/SARibbonConfigVersion.cmake"
        VERSION ${SARIBBON_VERSION}
        COMPATIBILITY SameMajorVersion
    )
    install(FILES
        "${CMAKE_CURRENT_BINARY_DIR}/SARibbonConfig.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/SARibbonConfigVersion.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SARibbon
    )
    # 旧包名 SARibbonBar 兼容薄壳（S11.2，保留一个版本周期）
    if(SARIBBON_BUILD_WIDGETS)
        configure_package_config_file(
            "${CMAKE_CURRENT_SOURCE_DIR}/widgets/SARibbonBarConfig.cmake.in"
            "${CMAKE_CURRENT_BINARY_DIR}/SARibbonBarConfig.cmake"
            INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SARibbonBar
        )
        write_basic_package_version_file(
            "${CMAKE_CURRENT_BINARY_DIR}/SARibbonBarConfigVersion.cmake"
            VERSION ${SARIBBON_VERSION}
            COMPATIBILITY SameMajorVersion
        )
        install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/SARibbonBarConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/SARibbonBarConfigVersion.cmake"
            DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SARibbonBar
        )
    endif()
endif()

