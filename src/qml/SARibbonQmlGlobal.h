#ifndef SARIBBONQMLGLOBAL_H
#define SARIBBONQMLGLOBAL_H
#include <QtGlobal>
#include <QMargins>
#include <QColor>
#include <QSize>

class QQmlEngine;

// Three-way export macro (plan-01 S5.2 template)
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

namespace SARibbonQml {
class RibbonTheme;
class RibbonMetrics;
class RibbonBar;
class RibbonCategory;
class RibbonTab;
class RibbonPanel;
class RibbonToolButton;
class RibbonControlContainer;
class RibbonQuickHost;
class RibbonLayoutItemHost;
class RibbonContextCategory;
class RibbonGallery;
class RibbonGalleryGroup;
class RibbonGalleryItem;
class RibbonSeparator;
class RibbonColorGrid;
class RibbonColorMenu;
class RibbonQuickAccessBar;
class RibbonButtonGroup;
class RibbonApplicationWindow;
}

/**
 * \if ENGLISH
 * @brief Register the SARibbon QML types into the global meta type registry
 * @details Imperative single-track registration (Qt5/Qt6 same code, plan-04 S1).
 *          Call once before engine.load(); multiple calls are harmless (static-once).
 *          Singletons use the CALLBACK form of qmlRegisterSingletonType with
 *          setObjectOwnership(CppOwnership) inside the callback;
 *          qmlRegisterSingletonInstance is FORBIDDEN here — it hard-binds the
 *          instance to the first engine (second engine gets nullptr).
 * \endif
 *
 * \if CHINESE
 * @brief 注册 SARibbon 的 QML 类型
 * @details 命令式单轨注册（Qt5/Qt6 同码，计划 04 S1）。在 engine.load() 前调用一次；
 *          多次调用无害（static-once 守卫）。单例用回调式 qmlRegisterSingletonType
 *          并在回调内设 CppOwnership；禁止 qmlRegisterSingletonInstance——它把实例
 *          硬绑到首个引擎（第二个引擎取到 nullptr）。
 * \endif
 */
SA_RIBBON_QML_EXPORT void saRibbonRegisterQmlTypes(QQmlEngine* engine = nullptr);

#endif  // SARIBBONQMLGLOBAL_H
