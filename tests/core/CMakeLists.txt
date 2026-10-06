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
    set_tests_properties(core_PanelLayoutGolden PROPERTIES LABELS core TIMEOUT 120
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
    set_target_properties(core_PanelLayoutGolden PROPERTIES
        AUTOMOC ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
endif()

# Engine-level golden tests with FakeItem: pure core, no widgets dependency
add_executable(core_PanelLayoutEngine tst_panelLayoutEngine.cpp)
target_link_libraries(core_PanelLayoutEngine PRIVATE
    SARibbonCore
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
)
if(NOT SARIBBON_BUILD_STATIC_LIBS)
    add_custom_command(TARGET core_PanelLayoutEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SARibbonCore> $<TARGET_FILE_DIR:core_PanelLayoutEngine>
        COMMENT "Copy SARibbonCore next to core_PanelLayoutEngine"
    )
endif()
add_test(NAME core_PanelLayoutEngine COMMAND core_PanelLayoutEngine)
set_tests_properties(core_PanelLayoutEngine PROPERTIES LABELS core TIMEOUT 120
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
set_target_properties(core_PanelLayoutEngine PROPERTIES
    AUTOMOC ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

# Customize record contract test (plan-04 WS-C1): pure core, no widgets dependency.
# Locks the enum values, the make* factory field mapping and the XML byte format
# that both front ends persist to.
add_executable(core_CustomizeRecord tst_customizeRecord.cpp)
target_link_libraries(core_CustomizeRecord PRIVATE
    SARibbonCore
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
)
if(NOT SARIBBON_BUILD_STATIC_LIBS)
    add_custom_command(TARGET core_CustomizeRecord POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SARibbonCore> $<TARGET_FILE_DIR:core_CustomizeRecord>
        COMMENT "Copy SARibbonCore next to core_CustomizeRecord"
    )
endif()
add_test(NAME core_CustomizeRecord COMMAND core_CustomizeRecord)
set_tests_properties(core_CustomizeRecord PROPERTIES LABELS core TIMEOUT 120
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
set_target_properties(core_CustomizeRecord PROPERTIES
    AUTOMOC ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)

# Bar geometry engine minimum-width rules (plan-04): pure core, no widgets
# dependency. Locks the per-style row composition and the three screen rules
# (never above screen, title sacrificed first, overlap capped at 2/3 screen)
add_executable(core_BarGeometryEngine tst_barGeometryEngine.cpp)
target_link_libraries(core_BarGeometryEngine PRIVATE
    SARibbonCore
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
)
if(NOT SARIBBON_BUILD_STATIC_LIBS)
    add_custom_command(TARGET core_BarGeometryEngine POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:SARibbonCore> $<TARGET_FILE_DIR:core_BarGeometryEngine>
        COMMENT "Copy SARibbonCore next to core_BarGeometryEngine"
    )
endif()
add_test(NAME core_BarGeometryEngine COMMAND core_BarGeometryEngine)
set_tests_properties(core_BarGeometryEngine PROPERTIES LABELS core TIMEOUT 120
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
set_target_properties(core_BarGeometryEngine PROPERTIES
    AUTOMOC ON
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
)
