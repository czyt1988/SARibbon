TEMPLATE = subdirs
DEFINES += SA_RIBBON_DEBUG_HELP_DRAW # 此宏将绘制辅助线用于调试
SUBDIRS += \
           src/SARibbonBar \
           src/example/MainWindowExample

CONFIG += ordered

OTHER_FILES += \
           readme.md
