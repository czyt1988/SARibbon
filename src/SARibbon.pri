#-------------------------------------------------
#
# 此文件给SARibbon静态编译使用
# 适用于SARibbon.h/SARibbon.cpp这两个文件
# 如果使用动态库，不要使用此文件
#
# 使用方法见例子：src\example\SimpleExample\
#
#-------------------------------------------------
SOURCES += \
    $$PWD/SARibbon.cpp
    
HEADERS  += \
    $$PWD/SARibbon.h
    
RESOURCES += \
    $$PWD/SARibbonBar/resource.qrc
    
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD
