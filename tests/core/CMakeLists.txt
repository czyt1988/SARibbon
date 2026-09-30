# Core-side tests (plan-02 S5.0): golden geometry fixtures.
# Every test here MUST set LABELS core (CI filter: ctest -L core).

find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED COMPONENTS Test Core Gui)

# Golden geometry test drives the real SARibbonPanelLayout, so it needs widgets;
# it lives here (not in tests/widgets) because it is the engine's baseline and
# will migrate to the core engine (Step B). Registered only when widgets exists
# so core-only configurations still configure.
if(TARGET SARibbonWidgets)
    find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)
    add_executable(core_PanelLayoutGolden tst_panelLayoutGolden.cpp)
    target_link_libraries(core_PanelLayoutGolden PRIVATE
        SARibbonWidgets
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
    )
    target_compile_definitions(core_PanelLayoutGolden PRIVATE
        QT_TESTCASE_SOURCEDIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )
    if(NOT SARIBBON_BUILD_STATIC_LIBS)
        add_custom_command(TARGET core_PanelLayoutGolden POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SARibbonWidgets> $<TARGET_FILE_DIR:core_PanelLayoutGolden>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SARibbonCore> $<TARGET_FILE_DIR:core_PanelLayoutGolden>
            COMMENT "Copy SARibbon runtime libraries next to core_PanelLayoutGolden"
        )
    endif()
    add_test(NAME core_PanelLayoutGolden COMMAND core_PanelLayoutGolden)
    set_tests_properties(core_PanelLayoutGolden PROPERTIES LABELS core TIMEOUT 120)
    set_target_properties(core_PanelLayoutGolden PROPERTIES
        AUTOMOC ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
endif()
