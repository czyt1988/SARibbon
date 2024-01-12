include($$PWD/common.pri)
INCLUDEPATH += $$PWD/src/SARibbonBar
DEPENDPATH += $$PWD/src/SARibbonBar

######################################
#SA_RIBBON_CONFIG 用于定义一些编译选项：
# SA_RIBBON_CONFIG+=use_frameless
#     此选项将使用frameless第三方库，这个选项在SARibbonBar.pri中会自动判断，如果，达到frameless的使用要求将会自动定义
#     frameless第三方库必须C++17且只有几个版本的qt可用，目前支持（qt5.14,qt5.15,qt6.4以上）
#     除了上诉版本SA_RIBBON_CONFIG中不会加入use_frameless
#     frameless库能实现Ubuntu下和mac下的显示，同时多屏幕的支持也较好
# 使用库必须明确编译时是否依赖QWindowKit，否则会存在链接错误
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

