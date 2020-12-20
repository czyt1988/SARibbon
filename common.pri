CONFIG(debug, debug|release){
    contains(QT_ARCH, i386) {
        SA_RIBBON_DIR = $$PWD/bin_qt$$[QT_VERSION]_debug
    }else {
        SA_RIBBON_DIR = $$PWD/bin_qt$$[QT_VERSION]_debug_64
    }
}else{
    contains(QT_ARCH, i386) {
        SA_RIBBON_DIR = $$PWD/bin_qt$$[QT_VERSION]_release
    }else {
        SA_RIBBON_DIR = $$PWD/bin_qt$$[QT_VERSION]_release_64
    }
}
