include($$PWD/common.pri)
INCLUDEPATH += $$PWD/src/SARibbonBar
DEPENDPATH += $$PWD/src/SARibbonBar


contains( SA_RIBBON_CONFIG, use_frameless ) {
    !contains(CONFIG,C++17){
        CONFIG += c++17
    }
    # framelessHelper的引用路径
    INCLUDEPATH += $$PWD/src/SARibbonBar/3rdparty/framelesshelper/include
    DEPENDPATH += $$PWD/src/SARibbonBar/3rdparty/framelesshelper/include
    INCLUDEPATH += $$PWD/src/SARibbonBar/3rdparty/framelesshelper/qmake/inc/core
    DEPENDPATH += $$PWD/src/SARibbonBar/3rdparty/framelesshelper/qmake/inc/core
    # 定义FRAMELESSHELPER_FEATURE_static_build为-1让frameless也作为库的一部分
    DEFINES += FRAMELESSHELPER_FEATURE_static_build=-1
    # 定义SARIBBON_USE_3RDPARTY_FRAMELESSHELPER为1
    DEFINES += SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1
}else{
    DEFINES += SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=0
}
LIBS += -L$${SARIBBON_BIN_DIR} -l$${SARIBBON_LIB_NAME}

