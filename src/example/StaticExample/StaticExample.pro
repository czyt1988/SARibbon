#-------------------------------------------------
#
# Project created by QtCreator 2017-09-21T14:04:12
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++17
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = StaticExample
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp 

HEADERS  += \
    mainwindow.h 
    
include($$PWD/../../SARibbon.pri)

RESOURCES += \
    icon.qrc
