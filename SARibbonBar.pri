include($$PWD/common.pri)
INCLUDEPATH += $$PWD/src/SARibbonBar
DEPENDPATH += $$PWD/src/SARibbonBar


######################################
#SA_RIBBON_CONFIG 用于定义一些编译选项：
# SA_RIBBON_CONFIG+=use_frameless 此选项在common.pri种定义，注意必须保证编译和使用统一，否则会发生不可预估的异常
######################################

contains( SA_RIBBON_CONFIG, use_frameless ) {
    !contains(CONFIG,C++17){
        CONFIG += c++17
    }
    # 定义SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1是的使用framelss库
    DEFINES += SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=1
}else{
    !contains(CONFIG,C++14){
        CONFIG += c++14
    }
    DEFINES += SARIBBON_USE_3RDPARTY_FRAMELESSHELPER=0
}
LIBS += -L$${SARIBBON_BIN_DIR}/bin -l$${SARIBBON_LIB_NAME}

