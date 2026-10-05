# Utility function to create a test
function(add_saribbon_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE
        SARibbon::Widgets
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
    )
    target_compile_definitions(${TEST_NAME} PRIVATE
        QT_TESTCASE_SOURCEDIR="${CMAKE_CURRENT_SOURCE_DIR}"
    )
    # The shared libraries are generated in bin/<config> while tests live in tests/<config>.
    # Windows does not search the build tree for DLLs, so without this copy the test
    # executable cannot start (STATUS_DLL_NOT_FOUND) outside of an IDE.
    # 计划 02 起 widgets 依赖 core：两个运行库都要拷贝。
    # frameless 开启时（树内构建）SARibbonWidgets 还传递依赖 QWKWidgets/QWKCore，一并拷贝。
    if(NOT SARIBBON_BUILD_STATIC_LIBS)
        set(_sa_test_runtime_libs
            $<TARGET_FILE:SARibbonWidgets>
            $<TARGET_FILE:SARibbonCore>
        )
        if(TARGET QWKWidgets)
            list(APPEND _sa_test_runtime_libs
                $<TARGET_FILE:QWKWidgets>
                $<TARGET_FILE:QWKCore>
            )
        endif()
        add_custom_command(
            TARGET ${TEST_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${_sa_test_runtime_libs} $<TARGET_FILE_DIR:${TEST_NAME}>
            COMMENT "Copy SARibbon runtime libraries next to ${TEST_NAME}"
        )
    endif()
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    set_target_properties(${TEST_NAME} PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
    # Every test constructs a QApplication. If the Qt platform plugin cannot be
    # initialized Qt shows a modal error box, which blocks the process forever on a
    # headless CI runner. Keep the timeout short so such a hang fails fast instead of
    # consuming the whole job budget and hiding the real error.
    set_tests_properties(${TEST_NAME} PROPERTIES TIMEOUT 120)
endfunction()

# Add all tests
add_saribbon_test(SARibbonUtilTest SARibbonUtilTest.cpp)
add_saribbon_test(SARibbonAlignmentTest SARibbonAlignmentTest.cpp)
add_saribbon_test(SARibbonBarLayoutRTLTest SARibbonBarLayoutRTLTest.cpp)
add_saribbon_test(SARibbonCategoryLayoutRTLTest SARibbonCategoryLayoutRTLTest.cpp)
add_saribbon_test(SARibbonPanelLayoutRTLTest SARibbonPanelLayoutRTLTest.cpp)
add_saribbon_test(SARibbonLayoutDirectionChangeTest SARibbonLayoutDirectionChangeTest.cpp)
add_saribbon_test(SARibbonToolButtonRTLTest SARibbonToolButtonRTLTest.cpp)
add_saribbon_test(SARibbonGalleryRTLTest SARibbonGalleryRTLTest.cpp)
add_saribbon_test(SARibbonSystemButtonBarRTLTest SARibbonSystemButtonBarRTLTest.cpp)
add_saribbon_test(SARibbonContextCategoryRTLTest SARibbonContextCategoryRTLTest.cpp)
add_saribbon_test(SARibbonCategoryVisibilityTest SARibbonCategoryVisibilityTest.cpp)
add_saribbon_test(SARibbonBarEventFilterTest SARibbonBarEventFilterTest.cpp)
add_saribbon_test(ThemeCoverageTest ThemeCoverageTest.cpp)
add_saribbon_test(SARibbonThemeAutoSwitchTest SARibbonThemeAutoSwitchTest.cpp)
add_saribbon_test(SARibbonThemePaletteTest auto/SARibbonThemePalette/tst_themepalette.cpp)
add_saribbon_test(SARibbonButtonGroupWidgetTest SARibbonButtonGroupWidgetTest.cpp)
add_saribbon_test(SARibbonToolButtonColorTest SARibbonToolButtonColorTest.cpp)
add_saribbon_test(SARibbonSystemButtonBarGeometryTest SARibbonSystemButtonBarGeometryTest.cpp)
add_saribbon_test(SARibbonAutomationIdTest SARibbonAutomationIdTest.cpp)
add_saribbon_test(SARibbonGalleryStretchFactorTest SARibbonGalleryStretchFactorTest.cpp)
add_saribbon_test(SARibbonTitleIconMenuTest SARibbonTitleIconMenuTest.cpp)
add_saribbon_test(SARibbonMainWindowFrameBorderTest SARibbonMainWindowFrameBorderTest.cpp)
add_saribbon_test(SARibbonTitleBarHitTestTest SARibbonTitleBarHitTestTest.cpp)
add_saribbon_test(SARibbonFrameShadowTest SARibbonFrameShadowTest.cpp)
add_saribbon_test(SARibbonQuickAccessCustomizeTest SARibbonQuickAccessCustomizeTest.cpp)
add_saribbon_test(SARibbonLargeButtonAspectRatioTest SARibbonLargeButtonAspectRatioTest.cpp)
