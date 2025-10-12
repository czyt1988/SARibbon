QT       += core gui
# SARibbon 1.x 版本后引入frameless库，必须要cpp17及以上
CONFIG += c++17
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UseNativeFrameExample
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp

HEADERS  += \
    mainwindow.h

RESOURCES += \
    resouce.qrc \

# 下面演示了如何把SARibbon引入
# 只需要下面2句话，只要把common.pri引入工程，就可以实现SARibbon的引入
include($$PWD/../../common.pri)
include($${SARIBBONBAR_PRI_FILE_PATH})

DESTDIR = $${SARIBBON_BIN_DIR}/bin

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

