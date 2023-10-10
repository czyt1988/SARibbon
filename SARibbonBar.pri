include($$PWD/common.pri)
INCLUDEPATH += $$PWD/src/SARibbonBar
DEPENDPATH += $$PWD/src/SARibbonBar
# framelessHelper的引用路径
INCLUDEPATH += $$PWD/src/SARibbonBar/3rdparty/framelesshelper/include
DEPENDPATH += $$PWD/src/SARibbonBar/3rdparty/framelesshelper/include
INCLUDEPATH += $$PWD/src/SARibbonBar/3rdparty/framelesshelper/qmake/inc/core
DEPENDPATH += $$PWD/src/SARibbonBar/3rdparty/framelesshelper/qmake/inc/core
LIBS += -L$${SARIBBON_BIN_DIR} -l$${SARIBBON_LIB_NAME}

