cmake_minimum_required(VERSION 3.5)

# Find Qt Test
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test Core Gui Widgets REQUIRED)

# Utility function to create a test
function(add_saribbon_test TEST_NAME TEST_SOURCE)
    add_executable(${TEST_NAME} ${TEST_SOURCE})
    target_link_libraries(${TEST_NAME} PRIVATE
        SARibbonBar
        Qt${QT_VERSION_MAJOR}::Test
        Qt${QT_VERSION_MAJOR}::Core
        Qt${QT_VERSION_MAJOR}::Gui
        Qt${QT_VERSION_MAJOR}::Widgets
    )
    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../src/SARibbonBar
    )
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    set_target_properties(${TEST_NAME} PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        AUTOUIC ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests
    )
endfunction()

# Add all tests
add_saribbon_test(SARibbonUtilTest SARibbonUtilTest.cpp)
add_saribbon_test(SARibbonAlignmentTest SARibbonAlignmentTest.cpp)
add_saribbon_test(SARibbonBarLayoutRTLTest SARibbonBarLayoutRTLTest.cpp)
add_saribbon_test(SARibbonCategoryLayoutRTLTest SARibbonCategoryLayoutRTLTest.cpp)
add_saribbon_test(SARibbonPanelLayoutRTLTest SARibbonPanelLayoutRTLTest.cpp)
add_saribbon_test(SARibbonLayoutDirectionChangeTest SARibbonLayoutDirectionChangeTest.cpp)
add_saribbon_test(SARibbonToolButtonRTLTest SARibbonToolButtonRTLTest.cpp)
add_saribbon_test(SARibbonButtonLayoutStrategyRTLTest SARibbonButtonLayoutStrategyRTLTest.cpp)
add_saribbon_test(SARibbonGalleryRTLTest SARibbonGalleryRTLTest.cpp)
add_saribbon_test(SARibbonSystemButtonBarRTLTest SARibbonSystemButtonBarRTLTest.cpp)
add_saribbon_test(SARibbonContextCategoryRTLTest SARibbonContextCategoryRTLTest.cpp)
add_saribbon_test(SARibbonCategoryVisibilityTest SARibbonCategoryVisibilityTest.cpp)
