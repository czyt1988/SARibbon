#-------------------------------------------------
#
# 此例子用于验证通过ui plugin加载ribbon的ui功能，程序运行后需要选择一个ui文件，已经设置好的ui文件位于
# SARibbon\src\example\UiPluginLoaderTest\ribbonMainWindow.ui 下
#
# 使用本例子之前，需要先编译DesignerPlugin
#
#-------------------------------------------------

QT       += core gui uitools

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = UiPluginLoaderTest
TEMPLATE = app
DEFINES += QT_DEPRECATED_WARNINGS

CONFIG += c++11

SOURCES += \
        main.cpp \
        UiLoad.cpp

HEADERS += \
        UiLoad.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

include($$PWD/../../../common.pri)
DESTDIR = $${SARIBBON_BIN_DIR}
include($${SARIBBONBAR_PRI_FILE_PATH})
