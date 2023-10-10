QT += \
    quick quick-private \
    quicktemplates2 quicktemplates2-private \
    quickcontrols2 quickcontrols2-private

BASE_INC_DIR = $$PWD/../include
COMMON_INC_DIR = $$BASE_INC_DIR/FramelessHelper
QUICK_PUB_INC_DIR = $$COMMON_INC_DIR/Quick
QUICK_PRIV_INC_DIR = $$QUICK_PUB_INC_DIR/private
QUICK_SRC_DIR = $$PWD/../src/quick

INCLUDEPATH += \
    $$BASE_INC_DIR \
    $$COMMON_INC_DIR \
    $$QUICK_PUB_INC_DIR \
    $$QUICK_PRIV_INC_DIR

DEPENDPATH += \
    $$BASE_INC_DIR \
    $$COMMON_INC_DIR \
    $$QUICK_PUB_INC_DIR \
    $$QUICK_PRIV_INC_DIR

HEADERS += \
    $$QUICK_PUB_INC_DIR/framelesshelperquick_global.h \
    $$QUICK_PUB_INC_DIR/framelessquickmodule.h \
    $$QUICK_PUB_INC_DIR/framelessquickhelper.h \
    $$QUICK_PUB_INC_DIR/framelessquickutils.h \
    $$QUICK_PUB_INC_DIR/quickchromepalette.h \
    $$QUICK_PUB_INC_DIR/quickmicamaterial.h \
    $$QUICK_PUB_INC_DIR/quickimageitem.h \
    $$QUICK_PUB_INC_DIR/quickwindowborder.h \
    $$QUICK_PRIV_INC_DIR/quickstandardsystembutton_p.h \
    $$QUICK_PRIV_INC_DIR/quickstandardtitlebar_p.h \
    $$QUICK_PRIV_INC_DIR/framelessquickhelper_p.h \
    $$QUICK_PRIV_INC_DIR/framelessquickwindow_p.h \
    $$QUICK_PRIV_INC_DIR/framelessquickwindow_p_p.h \
    $$QUICK_PRIV_INC_DIR/framelessquickapplicationwindow_p.h \
    $$QUICK_PRIV_INC_DIR/framelessquickapplicationwindow_p_p.h \
    $$QUICK_PRIV_INC_DIR/quickmicamaterial_p.h \
    $$QUICK_PRIV_INC_DIR/quickimageitem_p.h \
    $$QUICK_PRIV_INC_DIR/quickwindowborder_p.h

SOURCES += \
    $$QUICK_SRC_DIR/quickstandardsystembutton.cpp \
    $$QUICK_SRC_DIR/quickstandardtitlebar.cpp \
    $$QUICK_SRC_DIR/framelessquickutils.cpp \
    $$QUICK_SRC_DIR/framelessquickmodule.cpp \
    $$QUICK_SRC_DIR/framelessquickwindow.cpp \
    $$QUICK_SRC_DIR/framelessquickapplicationwindow.cpp \
    $$QUICK_SRC_DIR/framelessquickhelper.cpp \
    $$QUICK_SRC_DIR/quickchromepalette.cpp \
    $$QUICK_SRC_DIR/framelesshelperquick_global.cpp \
    $$QUICK_SRC_DIR/quickmicamaterial.cpp \
    $$QUICK_SRC_DIR/quickimageitem.cpp \
    $$QUICK_SRC_DIR/quickwindowborder.cpp
