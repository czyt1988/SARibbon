# QML-side conformance tests (plan-04 S7). Every test sets LABELS qml.
find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED COMPONENTS Test Core Gui Qml Quick Widgets)

if(TARGET SARibbonQml)
    add_executable(qml_Conformance tst_conformance_qml.cpp)
    target_link_libraries(qml_Conformance PRIVATE
        SARibbonQml
        SARibbonCore
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Qml
        Qt${QT_VERSION_MAJOR}::Quick
    )
    set_target_properties(qml_Conformance PROPERTIES AUTOMOC ON)
    if(NOT SARIBBON_BUILD_STATIC_LIBS)
        # Co-locate the test executable with the SARibbon runtime libraries
        # (both output to ${CMAKE_BINARY_DIR}/bin): no DLL copy step can go
        # stale when only the library re-links (a qrc-only change keeps the
        # import lib untouched, so a copy hook on the exe would not re-run).
        # 树内构建 QWK 时 QWKQuick/QWKCore 同在 bin 下，天然可见。
        set_target_properties(qml_Conformance PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
        )
    else()
        set_target_properties(qml_Conformance PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
        )
    endif()
    add_test(NAME qml_Conformance COMMAND qml_Conformance)
    set_tests_properties(qml_Conformance PROPERTIES LABELS qml TIMEOUT 120
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QUICK_CONTROLS_STYLE=Basic")
endif()

# QML ↔ widgets 布局一致性测试：需要同时链接两个前端（core 布局算法的唯一消费者），
# 因此仅在 widgets 目标存在时构建。
if(TARGET SARibbonQml AND TARGET SARibbonWidgets)
    add_executable(qml_LayoutParity tst_layout_parity_qml.cpp)
    target_link_libraries(qml_LayoutParity PRIVATE
        SARibbonQml
        SARibbon::Widgets
        SARibbonCore
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Qml
        Qt${QT_VERSION_MAJOR}::Quick
    )
    set_target_properties(qml_LayoutParity PROPERTIES
        AUTOMOC ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    add_test(NAME qml_LayoutParity COMMAND qml_LayoutParity)
    set_tests_properties(qml_LayoutParity PROPERTIES LABELS qml TIMEOUT 120
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QUICK_CONTROLS_STYLE=Basic")
endif()

# 颜色控件族一致性测试（WS-D）：同样需要两个前端并存，宿主几何直接对照
# SAColorGridWidget 的 sizeHint 与单元 geometry。
if(TARGET SARibbonQml AND TARGET SARibbonWidgets)
    add_executable(qml_Color tst_color_qml.cpp)
    target_link_libraries(qml_Color PRIVATE
        SARibbonQml
        SARibbon::Widgets
        SARibbonCore
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Qml
        Qt${QT_VERSION_MAJOR}::Quick
    )
    set_target_properties(qml_Color PROPERTIES
        AUTOMOC ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    add_test(NAME qml_Color COMMAND qml_Color)
    set_tests_properties(qml_Color PROPERTIES LABELS qml TIMEOUT 120
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QUICK_CONTROLS_STYLE=Basic")
endif()

# 定制系统测试（WS-C2）：记录层是 core 共享的，因此跨前端 XML 兼容性用例需要
# 同时链接两个前端（widgets 的 sa_customize_datas_to_xml / from_xml 是对照方）。
if(TARGET SARibbonQml AND TARGET SARibbonWidgets)
    add_executable(qml_Customize tst_customize_qml.cpp)
    target_link_libraries(qml_Customize PRIVATE
        SARibbonQml
        SARibbon::Widgets
        SARibbonCore
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
        Qt${QT_VERSION_MAJOR}::Qml
        Qt${QT_VERSION_MAJOR}::Quick
    )
    set_target_properties(qml_Customize PROPERTIES
        AUTOMOC ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    )
    add_test(NAME qml_Customize COMMAND qml_Customize)
    set_tests_properties(qml_Customize PROPERTIES LABELS qml TIMEOUT 120
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen;QT_QUICK_CONTROLS_STYLE=Basic")
endif()
