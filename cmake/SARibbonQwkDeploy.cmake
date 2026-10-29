########################################################
# QWindowKit 运行时库部署
#
# QWindowKit 有两种来源：仓库内源码构建，或 find_package 找到的已安装包。
# 源码构建时输出目录已经被统一到构建树的 bin 目录，但使用已安装包时，
# QWK 的 DLL 留在安装目录（例如 bin_qt6.7.3_MSVC_x64/bin），Windows 没有 RPATH，
# 从 Qt Creator 直接运行示例就会弹出缺少 QWKWidgets.dll 的错误框。
#
# 这里把 QWindowKit 的运行时库复制到使用它的可执行文件旁边，
# 使两种来源的行为一致，无需手工拷贝 DLL。
########################################################

# 收集 QWindowKit 运行时库的文件路径（导入目标和普通目标都适用，取当前配置的输出）
function(_saribbon_qwk_runtime_files _out_var)
    set(_files)
    foreach(_target IN LISTS SARIBBON_QWK_RUNTIME_TARGETS)
        if(NOT TARGET ${_target})
            continue()
        endif()
        get_target_property(_type ${_target} TYPE)
        # 静态库和接口库没有运行时文件，跳过
        if(_type STREQUAL "SHARED_LIBRARY" OR _type STREQUAL "MODULE_LIBRARY")
            list(APPEND _files "$<TARGET_FILE:${_target}>")
        endif()
    endforeach()
    set(${_out_var} ${_files} PARENT_SCOPE)
endfunction()

#[[
    saribbon_deploy_qwindowkit_runtime(<target>)

    在 <target> 链接完成后，把 QWindowKit 的运行时库复制到它的输出目录。
    未启用 frameless 或 QWindowKit 全为静态库时为空操作。
]]
function(saribbon_deploy_qwindowkit_runtime _target)
    if(NOT WIN32)
        return()
    endif()
    if(NOT TARGET ${_target} OR NOT SARIBBON_QWK_RUNTIME_TARGETS)
        return()
    endif()

    _saribbon_qwk_runtime_files(_files)
    if(NOT _files)
        return()
    endif()

    add_custom_command(TARGET ${_target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${_files}
            "$<TARGET_FILE_DIR:${_target}>"
        COMMAND_EXPAND_LISTS
        COMMENT "Deploying QWindowKit runtime libraries next to ${_target}"
    )
endfunction()
