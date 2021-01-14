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


SOURCES += \
    SARibbonBar.cpp \
    SARibbonMainWindow.cpp \
    FramelessHelper.cpp \
    SAWindowButtonGroup.cpp \
    SARibbonApplicationButton.cpp \
    SARibbonTabBar.cpp \
    SARibbonCategory.cpp \
    SARibbonContextCategory.cpp \
    SARibbonPannel.cpp \
    SARibbonToolButton.cpp \
    SARibbonMenu.cpp \
    SARibbonPannelOptionButton.cpp \
    SARibbonSeparatorWidget.cpp \
    SARibbonCategoryLayout.cpp \
    SARibbonGallery.cpp \
    SARibbonControlButton.cpp \
    SARibbonGalleryGroup.cpp \
    SARibbonGalleryItem.cpp \
    SARibbonComboBox.cpp \
    SARibbonElementCreateDelegate.cpp \
    SARibbonElementManager.cpp \
    SARibbonDrawHelper.cpp \
    SARibbonLineEdit.cpp \
    SARibbonCheckBox.cpp \
    SARibbonButtonGroupWidget.cpp \
    SARibbonStackedWidget.cpp \
    SARibbonQuickAccessBar.cpp \
    SARibbonCtrlContainer.cpp

HEADERS  += \
    SARibbonBar.h \
    SARibbonMainWindow.h \
    FramelessHelper.h \
    SAWindowButtonGroup.h \
    SARibbonApplicationButton.h \
    SARibbonTabBar.h \
    SARibbonCategory.h \
    SARibbonContextCategory.h \
    SARibbonPannel.h \
    SARibbonToolButton.h \
    SARibbonMenu.h \
    SARibbonGlobal.h \
    SARibbonPannelOptionButton.h \
    SARibbonSeparatorWidget.h \
    SARibbonCategoryLayout.h \
    SARibbonGallery.h \
    SARibbonControlButton.h \
    SARibbonGalleryGroup.h \
    SARibbonGalleryItem.h \
    SARibbonComboBox.h \
    SARibbonElementCreateDelegate.h \
    SARibbonElementManager.h \
    SARibbonDrawHelper.h \
    SARibbonLineEdit.h \
    SARibbonCheckBox.h \
    SARibbonButtonGroupWidget.h \
    SARibbonStackedWidget.h \
    SARibbonQuickAccessBar.h \
    SARibbonCtrlContainer.h

RESOURCES += \
    resource.qrc

