#-------------------------------------------------
#
# Project created by QtCreator 2017-09-21T14:04:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SimpleExample
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp 

HEADERS  += \
    mainwindow.h 
    
include($$PWD/../../SARibbon.pri)