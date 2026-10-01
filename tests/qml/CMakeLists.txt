# QML-side conformance tests (plan-04 S7). Every test sets LABELS qml.
find_package(Qt${QT_VERSION_MAJOR} ${SARIBBON_MIN_QT_VERSION} REQUIRED COMPONENTS Test Core Gui Qml Quick)

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
