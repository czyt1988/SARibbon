// core macros are defined as static within the single file
#ifndef SA_RIBBON_CORE_STATIC
#define SA_RIBBON_CORE_STATIC
#endif

/*@remap "SARibbonCoreAmalgamTemplatePublicHeaders.h" "SARibbonCore.h" */
#include "SARibbonCoreAmalgamTemplateHeaderGlue.h"

// disable warnings about unsafe standard library calls
#ifdef _MSC_VER
#pragma push_macro ("_CRT_SECURE_NO_WARNINGS")
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma warning (push)
#pragma warning (disable: 4996) // deprecated POSIX names
#endif

#include "../../src/core/SARibbonCoreGlobal.cpp"
#include "../../src/core/global/SARibbonCoreUtil.cpp"
#include "../../src/core/theme/SARibbonThemePalette.cpp"
#include "../../src/core/theme/SARibbonThemeData.cpp"
#include "../../src/core/metrics/SARibbonMetrics.cpp"
#include "../../src/core/data/SARibbonCustomizeRecord.cpp"
#include "../../src/core/contract/SARibbonContract.cpp"
#include "../../src/core/layout/SARibbonPanelLayoutEngine.cpp"
#include "../../src/core/layout/SARibbonCategoryLayoutEngine.cpp"
#include "../../src/core/layout/SARibbonBarGeometryEngine.cpp"

#ifdef _MSC_VER
#pragma warning (pop)
#pragma pop_macro ("_CRT_SECURE_NO_WARNINGS")
#endif

// Q_OBJECT classes (SARibbonThemeData) inside the single file: AUTOMOC needs
// the explicit .moc include (same pattern the widgets product avoided by keeping
// all Q_OBJECT headers out of its .cpp).
#include "SARibbonCore.moc"
