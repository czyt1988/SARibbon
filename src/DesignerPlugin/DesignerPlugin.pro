################################################################
# SARibbon qt creator 插件
################################################################

# 引入SA_RIBBON_DIR变量
include($${PWD}/../../common.pri)
message(Qt path is:$$[QT_INSTALL_PREFIX])
message(Qt libs path is:$$[QT_INSTALL_LIBS])
message(Qt plugin path is:$$[QT_INSTALL_PLUGINS])
#CONFIG( debug_and_release ) {
#    # 插件建议只用release模式编译
#    message("debug_and_release: 插件建议只用release模式编译")
#    CONFIG -= debug_and_release
#    CONFIG += release
#}

# 插件默认的配置
QT          += widgets uiplugin designer core gui
CONFIG      += plugin
TEMPLATE        = lib
TARGET          = $$saRibbonLibNameMake(SARibbonBarDesignerPlugin)

target.path = $$[QT_INSTALL_PLUGINS]/designer
#生成的dll路径
DESTDIR         = $${SARIBBON_BIN_DIR}

#所有参与编译，这样导入designer里才可以用
DEFINES += SA_RIBBON_BAR_MAKE_LIB #定义此宏将构建库
include($$PWD/../SARibbonBar/SARibbonBar.pri)

HEADERS += \
    SARibbonBarCommands.h \
    SARibbonBarContainerExtension.h \
    SARibbonBarContainerFactory.h \
    SARibbonBarDesignerPlugin.h \
    SARibbonBarTaskMenuExtension.h \
    SARibbonBarTaskMenuFactory.h \
    SARibbonCategoryContainerExtension.h \
    SARibbonCategoryContainerFactory.h \
    SARibbonCategoryDesignerPlugin.h \
    SARibbonCategoryTaskMenuExtension.h \
    SARibbonMainWindowContainerExtension.h \
    SARibbonMainWindowContainerFactory.h \
    SARibbonMainWindowDesignerPlugin.h \
    SARibbonMainWindowTaskMenuExtension.h \
    SARibbonMainWindowTaskMenuFactory.h \
    SARibbonPannelDesignerPlugin.h \
    SARibbonPannelTaskMenuExtension.h \
    SARibbonPluginCollection.h \
    SARibbonPluginDebugHelper.h

SOURCES += \
    SARibbonBarCommands.cpp \
    SARibbonBarContainerExtension.cpp \
    SARibbonBarContainerFactory.cpp \
    SARibbonBarDesignerPlugin.cpp \
    SARibbonBarTaskMenuExtension.cpp \
    SARibbonBarTaskMenuFactory.cpp \
    SARibbonCategoryContainerExtension.cpp \
    SARibbonCategoryContainerFactory.cpp \
    SARibbonCategoryDesignerPlugin.cpp \
    SARibbonCategoryTaskMenuExtension.cpp \
    SARibbonMainWindowContainerExtension.cpp \
    SARibbonMainWindowContainerFactory.cpp \
    SARibbonMainWindowDesignerPlugin.cpp \
    SARibbonMainWindowTaskMenuExtension.cpp \
    SARibbonMainWindowTaskMenuFactory.cpp \
    SARibbonPannelDesignerPlugin.cpp \
    SARibbonPannelTaskMenuExtension.cpp \
    SARibbonPluginCollection.cpp \
    SARibbonPluginDebugHelper.cpp

RESOURCES += \
    icon.qrc

# 编译完成后自动把dll移动到$$[QT_INSTALL_PLUGINS]/designer下面
win32 {
    PLUGINDLL_FULL_PATH = $${SARIBBON_BIN_DIR}/$${TARGET}.dll
    PLUGINDLL_FULL_PATH = $$replace(PLUGINDLL_FULL_PATH, /, \\)
    PLUGINDLL_DESIGNER_PATH = $$[QT_INSTALL_PLUGINS]/designer/$${TARGET}.dll
    PLUGINDLL_DESIGNER_PATH = $$replace(PLUGINDLL_DESIGNER_PATH, /, \\)
    CMD_CPY = $${QMAKE_COPY} $${PLUGINDLL_FULL_PATH} $${PLUGINDLL_DESIGNER_PATH}
    message(cmd:$${CMD_CPY})
    QMAKE_POST_LINK += $${CMD_CPY}
}

