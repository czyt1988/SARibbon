#-------------------------------------------------
#
# 此文件给SARibbon静态编译使用
# 适用于SARibbon.h/SARibbon.cpp这两个文件
# 如果使用动态库，不要使用此文件
#
#
# 使用方法见例子：src\example\SimpleExample\
#
#-------------------------------------------------


# 这里判断qt的版本看看是否适合使用frameless，如果适合使用SA_RIBBON_CONFIG将加入use_frameless
equals(QT_MAJOR_VERSION, 5){
    greaterThan(QT_MINOR_VERSION, 13){
        SA_RIBBON_CONFIG += use_frameless
    }
}else{
# qt6.0不支持
    greaterThan(QT_MINOR_VERSION, 0){
        SA_RIBBON_CONFIG += use_frameless
    }
}

# 这里判断SA_RIBBON_CONFIG是否包含use_frameless，如果包含将引入frameless库，并定义SARIBBON_USE_3RDPARTY_FRAMELESSHELPER为1
contains( SA_RIBBON_CONFIG, use_frameless ) {
    !contains(CONFIG,C++17){
        CONFIG += c++17
    }
    # 引入第三方库framelesshelper
    # 1.framelesshelper/qmake/inc/core/framelesshelper.config中的FRAMELESSHELPER_FEATURE_static_build设置为-1
    include($$PWD/SARibbonBar/3rdparty/framelesshelper/qmake/core.pri)
    include($$PWD/SARibbonBar/3rdparty/framelesshelper/qmake/widgets.pri)
    DEFINES += FRAMELESSHELPER_CORE_LIBRARY
    DEFINES += FRAMELESSHELPER_WIDGETS_LIBRARY
    DEFINES += FRAMELESSHELPER_FEATURE_static_build=1
    # 定义SARIBBON_USE_3RDPARTY_FRAMELESSHELPER为1
    DEFINES += SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1
}else{
    DEFINES += SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=0
}


SOURCES += \
    $$PWD/SARibbon.cpp
    
HEADERS  += \
    $$PWD/SARibbon.h
    
RESOURCES += \
    $$PWD/SARibbonBar/resource.qrc
    
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
