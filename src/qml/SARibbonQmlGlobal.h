#ifndef SARIBBONQMLGLOBAL_H
#define SARIBBONQMLGLOBAL_H
#include <QtGlobal>

// 三段式导出宏（计划 01 S5.2 模板；SARibbonQml 实体归计划 04）
#ifndef SA_RIBBON_QML_EXPORT
#  ifdef SA_RIBBON_QML_STATIC
#    define SA_RIBBON_QML_EXPORT
#  else
#    ifdef SA_RIBBON_QML_LIBRARY
#      define SA_RIBBON_QML_EXPORT Q_DECL_EXPORT
#    else
#      define SA_RIBBON_QML_EXPORT Q_DECL_IMPORT
#    endif
#  endif
#endif

#endif  // SARIBBONQMLGLOBAL_H
