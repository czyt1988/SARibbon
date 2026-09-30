cmake_minimum_required(VERSION 3.21)
project(TestFindPackage LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(SARibbon 3.0 REQUIRED COMPONENTS Widgets)

add_executable(test_find_package
    main.cpp
    main_new_include.cpp
)
target_link_libraries(test_find_package PRIVATE SARibbon::Widgets)
