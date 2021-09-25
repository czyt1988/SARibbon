#-------------------------------------------------
#
# Project created by QtCreator 2017-09-21T14:04:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SARibbonMainWindowExample
TEMPLATE = app
include($$PWD/../../../common.pri)
DESTDIR = $${SARIBBON_BIN_DIR}

SOURCES += main.cpp\
    mainwindow.cpp 

HEADERS  += \
    mainwindow.h 

RESOURCES += \
    saribbonresouce.qrc \

include($$PWD/../../../SARibbonBar.pri)

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

