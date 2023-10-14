QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
# SARibbon 1.x 版本后引入frameless库，必须要cpp17及以上
CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    Widget.cpp

HEADERS += \
    Widget.h

FORMS += \
    Widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# 下面演示了如何把SARibbon引入
# 只需要下面2句话，只要把common.pri引入工程，就可以实现SARibbon的引入
include($$PWD/../../../common.pri)
include($${SARIBBONBAR_PRI_FILE_PATH})

DESTDIR = $${SARIBBON_BIN_DIR}

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}

RESOURCES += \
    icon.qrc
