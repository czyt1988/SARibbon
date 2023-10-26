#-------------------------------------------------
#
# 此文件给SARibbon静态编译使用
# 适用于SARibbon.h/SARibbon.cpp这两个文件
# 如果使用动态库，不要使用此文件
#
# 使用方法见例子：src\example\SimpleExample\
#
#-------------------------------------------------

# 引入第三方库framelesshelper
# 1.framelesshelper/qmake/inc/core/framelesshelper.config中的FRAMELESSHELPER_FEATURE_static_build设置为-1
include($$PWD/SARibbonBar/3rdparty/framelesshelper/qmake/core.pri)
include($$PWD/SARibbonBar/3rdparty/framelesshelper/qmake/widgets.pri)
DEFINES += FRAMELESSHELPER_CORE_LIBRARY
DEFINES += FRAMELESSHELPER_WIDGETS_LIBRARY
DEFINES += FRAMELESSHELPER_FEATURE_static_build=1

SOURCES += \
    $$PWD/SARibbon.cpp
    
HEADERS  += \
    $$PWD/SARibbon.h
    
RESOURCES += \
    $$PWD/SARibbonBar/resource.qrc
    
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
