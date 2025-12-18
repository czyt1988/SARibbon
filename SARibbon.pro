TEMPLATE = subdirs
SUBDIRS += \
           src/SARibbonBar \
           example/NormalMenuBarExample\
           example/WidgetWithRibbon \
           example/MdiAreaWindowExample \
           # example/StaticExample \
           example/MainWindowExample \
           example/UseNativeFrameExample \
           example/MatlabUI


CONFIG += ordered

OTHER_FILES += \
           readme.md \
           readme-cn.md \
           update-record.md \
