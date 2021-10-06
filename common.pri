CONFIG(debug, debug|release){
    contains(QT_ARCH, i386) {
        SARIBBON_BIN_DIR = $$PWD/bin_qt$$[QT_VERSION]_debug
    }else {
        SARIBBON_BIN_DIR = $$PWD/bin_qt$$[QT_VERSION]_debug_64
    }
}else{
    contains(QT_ARCH, i386) {
        SARIBBON_BIN_DIR = $$PWD/bin_qt$$[QT_VERSION]_release
    }else {
        SARIBBON_BIN_DIR = $$PWD/bin_qt$$[QT_VERSION]_release_64
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
