# sa_add_library: unified library creation for SARibbon modules (QWK-style)
function(sa_add_library _target)
    cmake_parse_arguments(FUNC "NO_WIN_RC" "PREFIX"
        "SOURCES;QT_LINKS;LINKS;LINKS_PRIVATE" ${ARGN})

    if(NOT FUNC_PREFIX)
        message(FATAL_ERROR "sa_add_library(${_target}): PREFIX is required")
    endif()

    if(SARIBBON_BUILD_STATIC_LIBS)
        set(_type STATIC)
    else()
        set(_type SHARED)
    endif()

    add_library(${_target} ${_type})
    target_sources(${_target} PRIVATE ${FUNC_SOURCES})

    # Export macros, see plan-01 S5.2 (QWK qwkglobal.h three-way pattern).
    # qm_export_defines always defines <PREFIX>_LIBRARY PRIVATE and adds
    # <PREFIX>_STATIC PUBLIC for static builds (qmsetup:QMSetupAPI.cmake:222-228);
    # the if/else below is the stricter mutually-exclusive variant, matching
    # 2.9.5 behavior. Headers must keep STATIC-first precedence either way.
    if(_type STREQUAL "STATIC")
        target_compile_definitions(${_target} PUBLIC ${FUNC_PREFIX}_STATIC)
    else()
        target_compile_definitions(${_target} PRIVATE ${FUNC_PREFIX}_LIBRARY)
    endif()

    set_target_properties(${_target} PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
        CXX_EXTENSIONS OFF
        DEBUG_POSTFIX "${CMAKE_DEBUG_POSTFIX}"
        VERSION "${SARIBBON_VERSION}"
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}"
    )
    target_compile_features(${_target} PUBLIC cxx_std_17)

    # SARibbonCore -> Core ; alias SARibbon::Core ; EXPORT_NAME Core
    string(REGEX REPLACE "^SARibbon(.+)$" "\\1" _comp "${_target}")
    set_target_properties(${_target} PROPERTIES EXPORT_NAME ${_comp})
    add_library(SARibbon::${_comp} ALIAS ${_target})

    # Qt components: QT_LINKS Core Gui -> Qt${QT_VERSION_MAJOR}::Core ...
    set(_qt_links)
    foreach(_c IN LISTS FUNC_QT_LINKS)
        list(APPEND _qt_links Qt${QT_VERSION_MAJOR}::${_c})
    endforeach()
    target_link_libraries(${_target} PUBLIC ${_qt_links})

    # Internal modules: LINKS SARibbonCore -> SARibbon::Core
    set(_sa_links)
    foreach(_l IN LISTS FUNC_LINKS)
        string(REGEX REPLACE "^SARibbon(.+)$" "SARibbon::\\1" _a "${_l}")
        list(APPEND _sa_links ${_a})
    endforeach()
    if(_sa_links)
        target_link_libraries(${_target} PUBLIC ${_sa_links})
    endif()
    if(FUNC_LINKS_PRIVATE)
        target_link_libraries(${_target} PRIVATE ${FUNC_LINKS_PRIVATE})
    endif()

    # Include propagation, see plan-01 S5.5 (source dir kept PUBLIC for the
    # 190 flat includes in tests/examples; sync dir gives <SARibbonXxx/...>)
    target_include_directories(${_target} PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )

    # OPTIONAL on every artifact kind, same as QWK (src/CMakeLists.txt:101-106);
    # whole block under SARIBBON_INSTALL guard (v2 §6.4, round2 D1; QWK's
    # install rules live inside QWINDOWKIT_INSTALL the same way)
    if(SARIBBON_INSTALL)
        install(TARGETS ${_target} EXPORT SARibbonTargets
            RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}" OPTIONAL
            LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
            ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}" OPTIONAL
        )
    endif()

    if(WIN32 AND NOT FUNC_NO_WIN_RC AND _type STREQUAL "SHARED")
        create_win32_resource_version(
            TARGET ${_target}
            FILENAME ${_target}
            EXT "dll"
            DESCRIPTION "Ribbon control library for Qt"
        )
    endif()
endfunction()

# sa_sync_include: mirror public headers into ${CMAKE_BINARY_DIR}/include/<module>/
function(sa_sync_include _target _module)
    set(_src_root "${CMAKE_CURRENT_SOURCE_DIR}")
    set(_dst_root "${CMAKE_BINARY_DIR}/include/${_module}")
    file(GLOB_RECURSE _headers RELATIVE "${_src_root}"
        "${_src_root}/*.h" "${_src_root}/*.hpp")
    # tst/ is colorWidgets' legacy qmake test project (not part of the CMake build,
    # 2.9.5 never installed its headers); keep it out of the public sync set.
    list(FILTER _headers EXCLUDE REGEX "(^|/)tst/")
    set(_synced)
    foreach(_h IN LISTS _headers)
        if(_h MATCHES "_p\\.h$")
            continue()   # private headers are neither synced nor installed
        endif()
        get_filename_component(_sub "${_h}" DIRECTORY)   # e.g. colorWidgets
        file(COPY "${_src_root}/${_h}" DESTINATION "${_dst_root}/${_sub}")
        list(APPEND _synced "${_dst_root}/${_h}")
    endforeach()
    set(${_target}_SYNC_FILES "${_synced}" PARENT_SCOPE)
    if(SARIBBON_INSTALL)   # v2 §6.4 (round2 D1)
        install(DIRECTORY "${_dst_root}/"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${_module}"
            FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
    endif()
endfunction()
