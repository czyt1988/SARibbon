#-------------------------------------------------
#
# Project created by QtCreator 2017-09-21T14:04:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SARibbonMainWindowExample
TEMPLATE = app

CONFIG(debug, debug|release){
    DESTDIR = $$PWD/../../../bin_qt$$[QT_VERSION]_debug
}else {
    DESTDIR = $$PWD/../../../bin_qt$$[QT_VERSION]_release
}

SOURCES += main.cpp\
    mainwindow.cpp 

HEADERS  += \
    mainwindow.h 

RESOURCES += \
    saribbonresouce.qrc \

include($$PWD/../../../SARibbonBar.pri)
