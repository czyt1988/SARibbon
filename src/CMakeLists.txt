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
# 组件化 SARibbonConfig 的生成/安装见 S11（同样在 SARIBBON_INSTALL 守卫内）
