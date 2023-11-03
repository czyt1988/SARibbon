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
win32:LIBS += -luser32
macx:LIBS += -framework ApplicationServices -framework Carbon -framework AppKit
unix:!macx:LIBS += -lX11 -lxcb -lX11-xcb

SOURCES += \
    $$PWD/SARibbon.cpp
    
HEADERS  += \
    $$PWD/SARibbon.h
    
RESOURCES += \
    $$PWD/SARibbonBar/resource.qrc
macx{
    OBJECTIVE_SOURCES +=  $$PWD/SARibbonBar/3edparty/FramelessWindowConverter/FramelessWindowConverterMacos.mm
}
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
