QT += widgets

BASE_INC_DIR = $$PWD/../include
COMMON_INC_DIR = $$BASE_INC_DIR/FramelessHelper
WIDGETS_PUB_INC_DIR = $$COMMON_INC_DIR/Widgets
WIDGETS_PRIV_INC_DIR = $$WIDGETS_PUB_INC_DIR/private
WIDGETS_SRC_DIR = $$PWD/../src/widgets

INCLUDEPATH += \
    $$BASE_INC_DIR \
    $$COMMON_INC_DIR \
    $$WIDGETS_PUB_INC_DIR \
    $$WIDGETS_PRIV_INC_DIR

DEPENDPATH += \
    $$BASE_INC_DIR \
    $$COMMON_INC_DIR \
    $$WIDGETS_PUB_INC_DIR \
    $$WIDGETS_PRIV_INC_DIR

HEADERS += \
    $$WIDGETS_PUB_INC_DIR/framelesshelperwidgets_global.h \
    $$WIDGETS_PUB_INC_DIR/framelesswidget.h \
    $$WIDGETS_PUB_INC_DIR/framelessmainwindow.h \
    $$WIDGETS_PUB_INC_DIR/standardsystembutton.h \
    $$WIDGETS_PUB_INC_DIR/framelesswidgetshelper.h \
    $$WIDGETS_PUB_INC_DIR/standardtitlebar.h \
    $$WIDGETS_PUB_INC_DIR/framelessdialog.h \
    $$WIDGETS_PRIV_INC_DIR/framelesswidgetshelper_p.h \
    $$WIDGETS_PRIV_INC_DIR/standardsystembutton_p.h \
    $$WIDGETS_PRIV_INC_DIR/standardtitlebar_p.h \
    $$WIDGETS_PRIV_INC_DIR/framelesswidget_p.h \
    $$WIDGETS_PRIV_INC_DIR/framelessmainwindow_p.h \
    $$WIDGETS_PRIV_INC_DIR/widgetssharedhelper_p.h \
    $$WIDGETS_PRIV_INC_DIR/framelessdialog_p.h

SOURCES += \
    $$WIDGETS_SRC_DIR/framelessmainwindow.cpp \
    $$WIDGETS_SRC_DIR/framelesswidgetshelper.cpp \
    $$WIDGETS_SRC_DIR/framelesswidget.cpp \
    $$WIDGETS_SRC_DIR/standardsystembutton.cpp \
    $$WIDGETS_SRC_DIR/standardtitlebar.cpp \
    $$WIDGETS_SRC_DIR/widgetssharedhelper.cpp \
    $$WIDGETS_SRC_DIR/framelesshelperwidgets_global.cpp \
    $$WIDGETS_SRC_DIR/framelessdialog.cpp
