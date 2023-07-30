//定义此宏，将SA_RIBBON_EXPORT定义为空
#ifndef SA_RIBBON_BAR_NO_EXPORT
#define SA_RIBBON_BAR_NO_EXPORT
#endif
//定义此宏，将SA_COLOR_WIDGETS_API定义为空
#ifndef SA_COLOR_WIDGETS_NO_DLL
#define SA_COLOR_WIDGETS_NO_DLL
#endif

/*@remap "SARibbonAmalgamTemplatePublicHeaders.h" "SARibbon.h" */
#include "SARibbonAmalgamTemplateHeaderGlue.h"

// disable warnings about unsafe standard library calls
#ifdef _MSC_VER
#pragma push_macro ("_CRT_SECURE_NO_WARNINGS")
#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#endif
#pragma warning (push)
#pragma warning (disable: 4996) // deprecated POSIX names
#endif

#include "../../src/SARibbonBar/colorWidgets/SAColorMenu.cpp"
#include "../../src/SARibbonBar/colorWidgets/SAColorGridWidget.cpp"
#include "../../src/SARibbonBar/colorWidgets/SAColorPaletteGridWidget.cpp"
#include "../../src/SARibbonBar/colorWidgets/SAColorToolButton.cpp"
//sa ribbon
#include "../../src/SARibbonBar/SAFramelessHelper.cpp"
#include "../../src/SARibbonBar/SARibbonApplicationButton.cpp"
#include "../../src/SARibbonBar/SAWindowButtonGroup.cpp"
#include "../../src/SARibbonBar/SARibbonToolButton.cpp"
#include "../../src/SARibbonBar/SARibbonColorToolButton.cpp"
#include "../../src/SARibbonBar/SARibbonLineWidgetContainer.cpp"
#include "../../src/SARibbonBar/SARibbonActionsManager.cpp"
#include "../../src/SARibbonBar/SARibbonDrawHelper.cpp"
#include "../../src/SARibbonBar/SARibbonLineEdit.cpp"
#include "../../src/SARibbonBar/SARibbonCheckBox.cpp"
#include "../../src/SARibbonBar/SARibbonComboBox.cpp"
#include "../../src/SARibbonBar/SARibbonButtonGroupWidget.cpp"
#include "../../src/SARibbonBar/SARibbonStackedWidget.cpp"
#include "../../src/SARibbonBar/SARibbonSeparatorWidget.cpp"
#include "../../src/SARibbonBar/SARibbonCtrlContainer.cpp"
#include "../../src/SARibbonBar/SARibbonQuickAccessBar.cpp"
#include "../../src/SARibbonBar/SARibbonTabBar.cpp"
#include "../../src/SARibbonBar/SARibbonControlButton.cpp"
#include "../../src/SARibbonBar/SARibbonMenu.cpp"

#include "../../src/SARibbonBar/SARibbonPannelOptionButton.cpp"
#include "../../src/SARibbonBar/SARibbonPannelItem.cpp"
#include "../../src/SARibbonBar/SARibbonPannelLayout.cpp"
#include "../../src/SARibbonBar/SARibbonPannel.cpp"
#include "../../src/SARibbonBar/SARibbonCategory.cpp"
#include "../../src/SARibbonBar/SARibbonCategoryLayout.cpp"
#include "../../src/SARibbonBar/SARibbonContextCategory.cpp"
#include "../../src/SARibbonBar/SARibbonGalleryItem.cpp"
#include "../../src/SARibbonBar/SARibbonGalleryGroup.cpp"
#include "../../src/SARibbonBar/SARibbonGallery.cpp"
#include "../../src/SARibbonBar/SARibbonBar.cpp"
#include "../../src/SARibbonBar/SARibbonStyleOption.cpp"
#include "../../src/SARibbonBar/SARibbonElementFactory.cpp"
#include "../../src/SARibbonBar/SARibbonElementManager.cpp"
#include "../../src/SARibbonBar/SARibbonCustomizeData.cpp"
#include "../../src/SARibbonBar/SARibbonCustomizeWidget.cpp"
#include "../../src/SARibbonBar/SARibbonCustomizeDialog.cpp"
#include "../../src/SARibbonBar/SARibbonMainWindow.cpp"

#ifdef _MSC_VER
#pragma warning (pop)
#pragma pop_macro ("_CRT_SECURE_NO_WARNINGS")
#endif

