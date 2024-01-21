

# 这里要做如下改动：
# 1.framelesshelper/qmake/inc/core/framelesshelper.config中的FRAMELESSHELPER_FEATURE_static_build设置为-1
# 2.要预定义FRAMELESSHELPER_CORE_LIBRARY宏，这样才能导出符号
# 3.要预定义FRAMELESSHELPER_WIDGETS_LIBRARY宏，这样才能导出符号
contains( SA_RIBBON_CONFIG, use_frameless ) {
    message("use frameless lib,compile with c+=17")
    # 引入第三方库frameless后，必须要求c++17
    !contains(CONFIG,C++17){
        CONFIG += c++17
    }
    !exists($$SA_RIBBON_QWindowKit_Install_DIR){
        message("SA_RIBBON_QWindowKit_Install_DIR not define,set to $$SARIBBON_BIN_DIR")
        SA_RIBBON_QWindowKit_Install_DIR = $$SARIBBON_BIN_DIR
    }
    SA_RIBBON_QWindowKit_QMake_DIR = $$SA_RIBBON_QWindowKit_Install_DIR/share/QWindowKit/qmake
    include($$SA_RIBBON_QWindowKit_QMake_DIR/QWKCore.pri)
    include($$SA_RIBBON_QWindowKit_QMake_DIR/QWKWidgets.pri)
    # 定义SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1是的使用framelss库
    DEFINES += SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1
}else{
    message("do not use_frameless lib,compile with c+=14")
    !contains(CONFIG,C++14){
        CONFIG += c++14
    }
    DEFINES += SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=0
}
message("SA_RIBBON_CONFIG="$${SA_RIBBON_CONFIG})
#-------------------------------------------------
#
# SARibbon的文件
#
#-------------------------------------------------

SOURCES += \
    $$PWD/SAFramelessHelper.cpp \
    $$PWD/SARibbonActionsManager.cpp \
    $$PWD/SARibbonBar.cpp \
    $$PWD/SARibbonColorToolButton.cpp \
    $$PWD/SARibbonCustomizeData.cpp \
    $$PWD/SARibbonCustomizeDialog.cpp \
    $$PWD/SARibbonCustomizeWidget.cpp \
    $$PWD/SARibbonElementFactory.cpp \
    $$PWD/SARibbonMainWindow.cpp \
    $$PWD/SARibbonSystemButtonBar.cpp \
    $$PWD/SARibbonApplicationButton.cpp \
    $$PWD/SARibbonTabBar.cpp \
    $$PWD/SARibbonCategory.cpp \
    $$PWD/SARibbonContextCategory.cpp \
    $$PWD/SARibbonPannel.cpp \
    $$PWD/SARibbonToolButton.cpp \
    $$PWD/SARibbonMenu.cpp \
    $$PWD/SARibbonPannelOptionButton.cpp \
    $$PWD/SARibbonSeparatorWidget.cpp \
    $$PWD/SARibbonCategoryLayout.cpp \
    $$PWD/SARibbonGallery.cpp \
    $$PWD/SARibbonControlButton.cpp \
    $$PWD/SARibbonGalleryGroup.cpp \
    $$PWD/SARibbonGalleryItem.cpp \
    $$PWD/SARibbonComboBox.cpp \
    $$PWD/SARibbonElementManager.cpp \
    $$PWD/SARibbonLineEdit.cpp \
    $$PWD/SARibbonCheckBox.cpp \
    $$PWD/SARibbonButtonGroupWidget.cpp \
    $$PWD/SARibbonStackedWidget.cpp \
    $$PWD/SARibbonQuickAccessBar.cpp \
    $$PWD/SARibbonCtrlContainer.cpp \
    $$PWD/SARibbonPannelLayout.cpp \
    $$PWD/SARibbonPannelItem.cpp \
    $$PWD/SARibbonLineWidgetContainer.cpp

HEADERS  += \
    $$PWD/SAFramelessHelper.h \
    $$PWD/SARibbonActionsManager.h \
    $$PWD/SARibbonBar.h \
    $$PWD/SARibbonColorToolButton.h \
    $$PWD/SARibbonCustomizeData.h \
    $$PWD/SARibbonCustomizeDialog.h \
    $$PWD/SARibbonCustomizeWidget.h \
    $$PWD/SARibbonElementFactory.h \
    $$PWD/SARibbonMainWindow.h \
    $$PWD/SARibbonSystemButtonBar.h \
    $$PWD/SARibbonApplicationButton.h \
    $$PWD/SARibbonTabBar.h \
    $$PWD/SARibbonCategory.h \
    $$PWD/SARibbonContextCategory.h \
    $$PWD/SARibbonPannel.h \
    $$PWD/SARibbonToolButton.h \
    $$PWD/SARibbonMenu.h \
    $$PWD/SARibbonGlobal.h \
    $$PWD/SARibbonPannelOptionButton.h \
    $$PWD/SARibbonSeparatorWidget.h \
    $$PWD/SARibbonCategoryLayout.h \
    $$PWD/SARibbonGallery.h \
    $$PWD/SARibbonControlButton.h \
    $$PWD/SARibbonGalleryGroup.h \
    $$PWD/SARibbonGalleryItem.h \
    $$PWD/SARibbonComboBox.h \
    $$PWD/SARibbonElementManager.h \
    $$PWD/SARibbonLineEdit.h \
    $$PWD/SARibbonCheckBox.h \
    $$PWD/SARibbonButtonGroupWidget.h \
    $$PWD/SARibbonStackedWidget.h \
    $$PWD/SARibbonQuickAccessBar.h \
    $$PWD/SARibbonCtrlContainer.h \
    $$PWD/SARibbonPannelLayout.h \
    $$PWD/SARibbonPannelItem.h \
    $$PWD/SARibbonLineWidgetContainer.h

RESOURCES += \
    $$PWD/resource.qrc

# 加入SAColorWidgets组件
include($$PWD/colorWidgets/SAColorWidgets.pri)

INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
