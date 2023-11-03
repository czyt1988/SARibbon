include($$PWD/common.pri)
INCLUDEPATH += $$PWD/src/SARibbonBar
DEPENDPATH += $$PWD/src/SARibbonBar
LIBS += -L$${SARIBBON_BIN_DIR} -l$${SARIBBON_LIB_NAME}

