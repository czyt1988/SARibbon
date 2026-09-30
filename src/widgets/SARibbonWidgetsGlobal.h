#ifndef SARIBBONWIDGETSGLOBAL_H
#define SARIBBONWIDGETSGLOBAL_H
#include <SARibbonCore/SARibbonCoreGlobal.h>

// 2.x 兼容：旧构建脚本 / amalgamate 产物定义 SA_RIBBON_BAR_MAKE_LIB / SA_RIBBON_BAR_NO_EXPORT。
// 顺序不可调换：#ifdef 指令在定义处即时求值（非惰性），本映射必须先于三段式，
// 否则旧宏场景下 SA_RIBBON_WIDGETS_EXPORT 被固化为 Q_DECL_IMPORT（round3 修正）。
#if defined(SA_RIBBON_BAR_NO_EXPORT) && !defined(SA_RIBBON_WIDGETS_STATIC)
#  define SA_RIBBON_WIDGETS_STATIC
#endif
#if defined(SA_RIBBON_BAR_MAKE_LIB) && !defined(SA_RIBBON_WIDGETS_LIBRARY)
#  define SA_RIBBON_WIDGETS_LIBRARY
#endif

// 三段式导出宏（S5.2 模板；STATIC 优先于 LIBRARY，与 2.9.5 的 NO_EXPORT 外层优先一致）
#ifndef SA_RIBBON_WIDGETS_EXPORT
#  ifdef SA_RIBBON_WIDGETS_STATIC
#    define SA_RIBBON_WIDGETS_EXPORT
#  else
#    ifdef SA_RIBBON_WIDGETS_LIBRARY
#      define SA_RIBBON_WIDGETS_EXPORT Q_DECL_EXPORT
#    else
#      define SA_RIBBON_WIDGETS_EXPORT Q_DECL_IMPORT
#    endif
#  endif
#endif

// 2.x 公共符号：SA_RIBBON_EXPORT ≡ SA_RIBBON_WIDGETS_EXPORT（v2 §4.4 兼容层；
// 对象宏惰性展开，此定义位置不受上面顺序影响）
#ifndef SA_RIBBON_EXPORT
#  define SA_RIBBON_EXPORT SA_RIBBON_WIDGETS_EXPORT
#endif

#endif  // SARIBBONWIDGETSGLOBAL_H
