/**
 * \if ENGLISH
 * @brief Global header file for color widgets
 * 
 * This file defines version information, macros, and API declarations for the color widgets library.
 * \endif
 *
 * \if CHINESE
 * @brief 颜色组件的全局头文件
 * 
 * 此文件定义了颜色组件库的版本信息、宏和API声明。
 * \endif
 */
#ifndef SACOLORWIDGETSGLOBAL_H
#define SACOLORWIDGETSGLOBAL_H
#include <QScopedPointer>

/**
 * \if ENGLISH
 * @def color-widgets version MAJ.MIN.PAT
 * \endif
 *
 * \if CHINESE
 * @def color-widgets的数字版本 MAJ.MIN.PAT
 * \endif
 */
#ifndef SA_COLOR_WIDGETS_VERSION_MAJ
#define SA_COLOR_WIDGETS_VERSION_MAJ 0
#endif

/**
 * \if ENGLISH
 * @def color-widgets version MAJ.MIN.PAT
 * \endif
 *
 * \if CHINESE
 * @def color-widgets的数字版本 MAJ.MIN.PAT
 * \endif
 */
#ifndef SA_COLOR_WIDGETS_VERSION_MIN
#define SA_COLOR_WIDGETS_VERSION_MIN 1
#endif

/**
 * \if ENGLISH
 * @def color-widgets version MAJ.MIN.PAT
 * \endif
 *
 * \if CHINESE
 * @def color-widgets的数字版本 MAJ.MIN.PAT
 * \endif
 */
#ifndef SA_COLOR_WIDGETS_VERSION_PAT
#define SA_COLOR_WIDGETS_VERSION_PAT 0
#endif

/**
 * \if ENGLISH
 * @def Imitates Q_DECLARE_PRIVATE but uses an internal class instead of forward declaration
 * \endif
 *
 * \if CHINESE
 * @def 模仿Q_DECLARE_PRIVATE，但不用前置声明而是作为一个内部类
 * \endif
 */
#ifndef SA_COLOR_WIDGETS_DECLARE_PRIVATE
#define SA_COLOR_WIDGETS_DECLARE_PRIVATE(classname)                                                                    \
    class PrivateData;                                                                                                 \
    friend class classname::PrivateData;                                                                               \
    QScopedPointer< PrivateData > d_ptr;
#endif

/**
 * \if ENGLISH
 * @def Imitates Q_DECLARE_PUBLIC
 * \endif
 *
 * \if CHINESE
 * @def 模仿Q_DECLARE_PUBLIC
 * \endif
 */
#ifndef SA_COLOR_WIDGETS_DECLARE_PUBLIC
#define SA_COLOR_WIDGETS_DECLARE_PUBLIC(classname)                                                                     \
    friend class classname;                                                                                            \
    classname* q_ptr { nullptr };
#endif

/**
 * \if ENGLISH
 * @def API declaration for color widgets library
 * \endif
 *
 * \if CHINESE
 * @def 颜色组件库的API声明
 * \endif
 */
#ifndef SA_COLOR_WIDGETS_NO_DLL
#if defined(SA_COLOR_WIDGETS_MAKE_LIB)  // 定义此宏将构建library
#ifndef SA_COLOR_WIDGETS_API
#define SA_COLOR_WIDGETS_API Q_DECL_EXPORT
#endif
#else
#ifndef SA_COLOR_WIDGETS_API
#define SA_COLOR_WIDGETS_API Q_DECL_IMPORT
#endif
#endif
#else
#ifndef SA_COLOR_WIDGETS_API
#define SA_COLOR_WIDGETS_API
#endif
#endif

/**
 * \if ENGLISH
 * @def const helper for different C++ standards
 * \endif
 *
 * \if CHINESE
 * @def 不同C++标准的const辅助宏
 * \endif
 */
#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#ifndef sacolor_as_const
#define sacolor_as_const std::as_const
#endif
#else
// C++14 及以下版本使用 Qt 的 qwt_as_const
#ifndef sacolor_as_const
#define sacolor_as_const qAsConst
#endif
#endif

#endif  // SACOLORWIDGETSGLOBAL_H
