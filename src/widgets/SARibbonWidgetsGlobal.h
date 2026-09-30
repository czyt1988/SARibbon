#ifndef SARIBBONWIDGETSGLOBAL_H
#define SARIBBONWIDGETSGLOBAL_H
#include <SARibbonCore/SARibbonCoreGlobal.h>

// 2.x compat: old build scripts / amalgamate products define SA_RIBBON_BAR_MAKE_LIB
// or SA_RIBBON_BAR_NO_EXPORT. The mapping must precede the three-way macro below:
// #ifdef directives are evaluated at their definition point (not lazily), otherwise
// SA_RIBBON_WIDGETS_EXPORT would be pinned to Q_DECL_IMPORT in the legacy-macro case.
#if defined(SA_RIBBON_BAR_NO_EXPORT) && !defined(SA_RIBBON_WIDGETS_STATIC)
#  define SA_RIBBON_WIDGETS_STATIC
#endif
#if defined(SA_RIBBON_BAR_MAKE_LIB) && !defined(SA_RIBBON_WIDGETS_LIBRARY)
#  define SA_RIBBON_WIDGETS_LIBRARY
#endif

// Three-way export macro (plan-01 S5.2; STATIC wins over LIBRARY, matching the
// 2.9.5 NO_EXPORT outer-priority order).
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

// 2.x public symbol: SA_RIBBON_EXPORT == SA_RIBBON_WIDGETS_EXPORT (v2 4.4 compat;
// object-like macros expand lazily so this position is unaffected by the order above).
#ifndef SA_RIBBON_EXPORT
#  define SA_RIBBON_EXPORT SA_RIBBON_WIDGETS_EXPORT
#endif

#endif  // SARIBBONWIDGETSGLOBAL_H
