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
    if(NOT SARIBBON_BUILD_STATIC_LIBS)
        add_custom_command(TARGET qml_Conformance POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SARibbonQml> $<TARGET_FILE_DIR:qml_Conformance>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SARibbonCore> $<TARGET_FILE_DIR:qml_Conformance>
            COMMENT "Copy SARibbon runtime libraries next to qml_Conformance"
        )
    endif()
    add_test(NAME qml_Conformance COMMAND qml_Conformance)
    set_tests_properties(qml_Conformance PROPERTIES LABELS qml TIMEOUT 120
        ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
    set_target_properties(qml_Conformance PROPERTIES
        AUTOMOC ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
endif()
