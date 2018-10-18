INCLUDEPATH += $$PWD/src/SARibbonBar
DEPENDPATH += $$PWD/src/SARibbonBar

#LIBS += -L$$PWD/bin/ -lSARibbonBar
CONFIG(debug, debug|release){
    LIBS += -L$$PWD/bin_qt$$[QT_VERSION]_debug/ -lSARibbonBar
}else {
    LIBS += -L$$PWD/bin_qt$$[QT_VERSION]_release/ -lSARibbonBar
}
