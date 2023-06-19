#-------------------------------------------------
#
# Project created by QtCreator 2017-09-21T14:04:12
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SARibbonMainWindowExample
TEMPLATE = app

SOURCES += main.cpp\
    mainwindow.cpp 

HEADERS  += \
    mainwindow.h 

RESOURCES += \
    saribbonresouce.qrc \

# ������ʾ����ΰ�SARibbon����
# ֻ��Ҫ����2�仰��ֻҪ��common.pri���빤�̣��Ϳ���ʵ��SARibbon������
include($$PWD/../../../common.pri)
include($${SARIBBONBAR_PRI_FILE_PATH})

DESTDIR = $${SARIBBON_BIN_DIR}

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

