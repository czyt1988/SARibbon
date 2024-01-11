#-------------------------------------------------
#
# Project created by QtCreator 2017-09-21T14:04:12
#
#-------------------------------------------------

QT       += core gui

TEMPLATE = lib

DEFINES += SA_RIBBON_BAR_MAKE_LIB #定义此宏将构建库
DEFINES += SA_COLOR_WIDGETS_MAKE_LIB #定义此宏将构建ColorWidgets库
#DEFINES += SA_RIBBON_DEBUG_HELP_DRAW # 此宏将绘制辅助线用于调试

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
include($$PWD/../../common.pri)

TARGET = $${SARIBBON_LIB_NAME}
DESTDIR = $${SARIBBON_BIN_DIR}/bin

######################################
#SA_RIBBON_CONFIG 用于定义一些编译选项：
# SA_RIBBON_CONFIG+=use_frameless
#     此选项将使用frameless第三方库，这个选项在SARibbonBar.pri中会自动判断，如果，达到frameless的使用要求将会自动定义
#     frameless第三方库必须C++17且只有几个版本的qt可用，目前支持（qt5.14,qt5.15,qt6.4以上）
#     除了上诉版本SA_RIBBON_CONFIG中不会加入use_frameless
#     frameless库能实现Ubuntu下和mac下的显示，同时多屏幕的支持也较好
# 使用frameless库，需要定义QWindowKit的安装目录，默认在SARIBBON_BIN_DIR
# SA_RIBBON_QWindowKit_Install_DIR = $$SARIBBON_BIN_DIR
######################################
# SA_RIBBON_CONFIG += use_frameless


include($$PWD/SARibbonBar.pri)
