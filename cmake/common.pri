# this file for cmake install

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
SARIBBON_LIB_DIR = $$PWD/../../
SARIBBON_BIN_DIR = $$PWD/../../../bin
SARIBBON_INCLUDE_DIR = $$PWD/../../../include/SARibbonBar
SARIBBONBAR_PRI_FILE_PATH = $$PWD/SARibbonBar.pri # 源代码路径
SARIBBON_LIB_NAME=$$saRibbonLibNameMake(SARibbonBar)
