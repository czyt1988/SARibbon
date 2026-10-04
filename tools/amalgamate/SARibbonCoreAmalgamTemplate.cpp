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
#include "../../src/core/SARibbonCoreUtil.cpp"
#include "../../src/core/SARibbonThemePalette.cpp"
#include "../../src/core/SARibbonThemeData.cpp"
#include "../../src/core/SARibbonMetrics.cpp"
#include "../../src/core/SARibbonCustomizeRecord.cpp"
#include "../../src/core/SARibbonContract.cpp"
#include "../../src/core/SARibbonPanelLayoutEngine.cpp"
#include "../../src/core/SARibbonCategoryLayoutEngine.cpp"
#include "../../src/core/SARibbonBarGeometryEngine.cpp"
#include "../../src/core/SARibbonToolButtonLayout.cpp"

#ifdef _MSC_VER
#pragma warning (pop)
#pragma pop_macro ("_CRT_SECURE_NO_WARNINGS")
#endif

// Q_OBJECT classes (SARibbonThemeData) inside the single file: AUTOMOC needs
// the explicit .moc include (same pattern the widgets product avoided by keeping
// all Q_OBJECT headers out of its .cpp).
#include "SARibbonCore.moc"
