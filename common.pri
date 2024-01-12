CONFIG(debug, debug|release){
    contains(QT_ARCH, i386) {
        SARIBBON_BIN_DIR = $$PWD/bin_qt$$[QT_VERSION]_Debug_x86
    }else {
        SARIBBON_BIN_DIR = $$PWD/bin_qt$$[QT_VERSION]_Debug_x64
    }
}else{
    contains(QT_ARCH, i386) {
        SARIBBON_BIN_DIR = $$PWD/bin_qt$$[QT_VERSION]_Release_x86
    }else {
        SARIBBON_BIN_DIR = $$PWD/bin_qt$$[QT_VERSION]_Release_x64
    }
}

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

#生成一个区别debug和release模式的lib名,输入一个lib名字
defineReplace(saRibbonLibNameMake) {
    LibName = $$1
    CONFIG(debug, debug|release){
        LibName = $${LibName}d
    }else{
        LibName = $${LibName}
    }
    return ($${LibName})
}


SARIBBON_SRC_DIR = $$PWD/src # 源代码路径
SARIBBONBAR_PRI_FILE_PATH = $$PWD/SARibbonBar.pri # 源代码路径
SARIBBON_LIB_NAME=$$saRibbonLibNameMake(SARibbonBar)

######################################
# Config  |  配置
######################################
#SA_RIBBON_CONFIG 用于定义一些编译选项：
# SA_RIBBON_CONFIG+=use_frameless
#     此选项将使用frameless第三方库，这个选项在SARibbonBar.pri中会自动判断，如果，达到frameless的使用要求将会自动定义
#     frameless第三方库必须C++17且只有几个版本的qt可用，目前支持（qt5.14,qt5.15,qt6.4以上）
#     除了上诉版本SA_RIBBON_CONFIG中不会加入use_frameless
#     frameless库能实现Ubuntu下和mac下的显示，同时多屏幕的支持也较好
# 使用frameless库，需要定义QWindowKit的安装目录，默认在SARIBBON_BIN_DIR
# SA_RIBBON_QWindowKit_Install_DIR = $$SARIBBON_BIN_DIR
######################################
# SA_RIBBON_CONFIG += use_frameless
