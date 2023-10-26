include($$PWD/common.pri)
INCLUDEPATH += $$PWD/src/SARibbonBar
DEPENDPATH += $$PWD/src/SARibbonBar
# framelessHelper的引用路径
INCLUDEPATH += $$PWD/src/SARibbonBar/3rdparty/framelesshelper/include
DEPENDPATH += $$PWD/src/SARibbonBar/3rdparty/framelesshelper/include
INCLUDEPATH += $$PWD/src/SARibbonBar/3rdparty/framelesshelper/qmake/inc/core
DEPENDPATH += $$PWD/src/SARibbonBar/3rdparty/framelesshelper/qmake/inc/core
# 定义FRAMELESSHELPER_FEATURE_static_build为-1让frameless也作为库的一部分
DEFINES += FRAMELESSHELPER_FEATURE_static_build=-1
LIBS += -L$${SARIBBON_BIN_DIR} -l$${SARIBBON_LIB_NAME}

