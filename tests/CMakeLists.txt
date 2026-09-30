cmake_minimum_required(VERSION 3.15)

# Find Qt Test
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Test Core Gui Widgets REQUIRED)

add_subdirectory(widgets)
