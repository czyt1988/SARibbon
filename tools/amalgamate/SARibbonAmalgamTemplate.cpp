//定义此宏，将SA_RIBBON_EXPORT定义为空
#ifndef SA_RIBBON_BAR_NO_EXPORT
#define SA_RIBBON_BAR_NO_EXPORT
#endif
//定义此宏，将SA_COLOR_WIDGETS_API定义为空
#ifndef SA_COLOR_WIDGETS_NO_DLL
#define SA_COLOR_WIDGETS_NO_DLL
#endif
//3.0: core macros are defined as static within the single file
#ifndef SA_RIBBON_CORE_STATIC
#define SA_RIBBON_CORE_STATIC
#endif

/*@remap "SARibbonAmalgamTemplatePublicHeaders.h" "SARibbon.h" */
/*@remap "SARibbonCore/SARibbonCoreGlobal.h" "SARibbon.h" */
/*@remap "SARibbonCore/SARibbonQt5Compat.hpp" "SARibbon.h" */
/*@remap "SARibbonCore/SARibbonEnums.h" "SARibbon.h" */
/*@remap "SARibbonCore/SARibbonCoreUtil.h" "SARibbon.h" */
/*@remap "SARibbonCore/SARibbonThemePalette.h" "SARibbon.h" */
/*@remap "SARibbonCore/SARibbonThemeData.h" "SARibbon.h" */
#include "SARibbonAmalgamTemplateHeaderGlue.h"
// 3.0 core sources (plan 02 S1)
#include "../../src/core/global/SARibbonCoreUtil.cpp"
#include "../../src/core/theme/SARibbonThemePalette.cpp"
#include "../../src/core/theme/SARibbonThemeData.cpp"
#include "../../src/core/metrics/SARibbonMetrics.cpp"
#include "../../src/core/data/SARibbonCustomizeRecord.cpp"
#include "../../src/core/contract/SARibbonContract.cpp"
#include "../../src/core/layout/SARibbonPanelLayoutEngine.cpp"




// disable warnings about unsafe standard library calls
#ifdef _MSC_VER
#pragma push_macro ("_CRT_SECURE_NO_WARNINGS")
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma warning (push)
#pragma warning (disable: 4996) // deprecated POSIX names
#endif

// clang-format off
#include "../qrc_SARibbonResource_Datas.cpp"
#if QT_VERSION >= QT_VERSION_CHECK(5,14,0)
#include "../qrc_SARibbonResource_version3.cpp"
#else
#include "../qrc_SARibbonResource_version2.cpp"
#endif
// clang-format on

#include "../../src/widgets/colorWidgets/SAColorMenu.cpp"
#include "../../src/widgets/colorWidgets/SAColorGridWidget.cpp"
#include "../../src/widgets/colorWidgets/SAColorPaletteGridWidget.cpp"
#include "../../src/widgets/colorWidgets/SAColorToolButton.cpp"
//sa ribbon
#include "../../src/widgets/SARibbonUtil.cpp"
#include "../../src/widgets/SARibbonThemeManager.cpp"
#include "../../src/widgets/SAFramelessHelper.cpp"
#include "../../src/widgets/SARibbonApplicationButton.cpp"
#include "../../src/widgets/SARibbonSystemButtonBar.cpp"
#include "../../src/widgets/SARibbonToolButton.cpp"
#include "../../src/widgets/SARibbonColorToolButton.cpp"
#include "../../src/widgets/SARibbonLineWidgetContainer.cpp"
#include "../../src/widgets/SARibbonActionsManager.cpp"
#include "../../src/widgets/SARibbonButtonGroupWidget.cpp"
#include "../../src/widgets/SARibbonStackedWidget.cpp"
#include "../../src/widgets/SARibbonSeparatorWidget.cpp"
#include "../../src/widgets/SARibbonCtrlContainer.cpp"
#include "../../src/widgets/SARibbonQuickAccessBar.cpp"
#include "../../src/widgets/SARibbonTabBar.cpp"
#include "../../src/widgets/SARibbonMenu.cpp"
#include "../../src/widgets/SARibbonTitleIconWidget.cpp"

#include "../../src/widgets/SARibbonPanelOptionButton.cpp"
#include "../../src/widgets/SARibbonPanelItem.cpp"
#include "../../src/widgets/SARibbonPanelLayout.cpp"
#include "../../src/widgets/SARibbonPanel.cpp"
#include "../../src/widgets/SARibbonCategory.cpp"
#include "../../src/widgets/SARibbonCategoryLayout.cpp"
#include "../../src/widgets/SARibbonContextCategory.cpp"
#include "../../src/widgets/SARibbonGalleryItem.cpp"
#include "../../src/widgets/SARibbonGalleryGroup.cpp"
#include "../../src/widgets/SARibbonGallery.cpp"
#include "../../src/widgets/SARibbonMdiControlsStyle.cpp"
#include "../../src/widgets/SARibbonBar.cpp"
#include "../../src/widgets/SARibbonBarLayout.cpp"
#include "../../src/widgets/SARibbonElementFactory.cpp"
#include "../../src/widgets/SARibbonElementManager.cpp"
#include "../../src/widgets/SARibbonCustomizeData.cpp"
#include "../../src/widgets/SARibbonCustomizeWidget.cpp"
#include "../../src/widgets/SARibbonCustomizeDialog.cpp"
#include "../../src/widgets/SARibbonMainWindow.cpp"
#include "../../src/widgets/SARibbonWidget.cpp"
#include "../../src/widgets/SARibbonApplicationWidget.cpp"
#ifdef _MSC_VER
#pragma warning (pop)
#pragma pop_macro ("_CRT_SECURE_NO_WARNINGS")
#endif

