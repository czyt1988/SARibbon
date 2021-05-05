#-------------------------------------------------
#
# Project created by QtCreator 2017-09-21T14:04:12
#
#-------------------------------------------------

QT       += core gui
CONFIG += c++11

DEFINES += SA_RIBBON_BAR_MAKE_LIB #定义此宏将构建库
#DEFINES += SA_RIBBON_DEBUG_HELP_DRAW # 此宏将绘制辅助线用于调试
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = SARibbonBar
TEMPLATE = lib
include($$PWD/../../common.pri)
DESTDIR = $$SA_RIBBON_DIR

include($$PWD/SARibbonBar.pri)

