#ifndef SARIBBONQMLACTIONCOMPAT_H
#define SARIBBONQMLACTIONCOMPAT_H
#include <QtGlobal>

/**
 * \if ENGLISH
 * @brief The single QAction/QActionGroup include face of the QML module
 * @details Contract D5 (R1): Qt6 keeps QAction/QActionGroup in QtGui, while
 *          Qt5 carries them in QtWidgets, so the Qt5 lane privately links
 *          Qt5::Widgets (CMake side, plan-05 S1). Every include of the action
 *          family in src/qml goes through this header — it is one of the three
 *          pinned conditional-compilation faces; a version check appearing
 *          anywhere else in the module is a debt signal.
 * \endif
 *
 * \if CHINESE
 * @brief QML 模块对 QAction/QActionGroup 的唯一 include 面
 * @details 契约 D5（R1）：Qt6 的 QAction/QActionGroup 属 QtGui，Qt5 中属
 *          QtWidgets，因此 Qt5 车道在 CMake 侧私有链接 Qt5::Widgets（计划 05
 *          S1）。src/qml 内对 action 家族的一切 include 一律经此头——它是钉死
 *          的三处条件编译面之一；版本判断出现在模块其他任何位置都是债务信号。
 * \endif
 */
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QtGui/QAction>
#include <QtGui/QActionGroup>
#else
#include <QtWidgets/QAction>
#include <QtWidgets/QActionGroup>
#endif

#endif  // SARIBBONQMLACTIONCOMPAT_H
