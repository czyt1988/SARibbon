################################################################
# SARibbon qt creator 插件
################################################################

# 引入SA_RIBBON_DIR变量
include($${PWD}/../../common.pri)

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
TARGET          = SARibbonBarDesignerPlugin

target.path = $$[QT_INSTALL_PLUGINS]/designer
#生成的dll路径
DESTDIR         = $${SA_RIBBON_DIR}

#所有参与编译，这样导入designer里才可以用
DEFINES += SA_RIBBON_BAR_MAKE_LIB #定义此宏将构建库
include($$PWD/../SARibbonBar/SARibbonBar.pri)

HEADERS += \
    SARibbonBarCommands.h \
    SARibbonBarDesignerPlugin.h \
    SARibbonMainWindowContainerExtension.h \
    SARibbonMainWindowContainerFactory.h \
    SARibbonMainWindowDesignerPlugin.h \
    SARibbonMainWindowTaskMenuExtension.h \
    SARibbonMainWindowTaskMenuExtensionFactory.h \
    SARibbonPluginCollection.h \
    SARibbonPluginDebugHelper.h

SOURCES += \
    SARibbonBarCommands.cpp \
    SARibbonBarDesignerPlugin.cpp \
    SARibbonMainWindowContainerExtension.cpp \
    SARibbonMainWindowContainerFactory.cpp \
    SARibbonMainWindowDesignerPlugin.cpp \
    SARibbonMainWindowTaskMenuExtension.cpp \
    SARibbonMainWindowTaskMenuExtensionFactory.cpp \
    SARibbonPluginCollection.cpp

RESOURCES += \
    icon.qrc

