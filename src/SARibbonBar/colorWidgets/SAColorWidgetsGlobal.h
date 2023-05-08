#ifndef SACOLORWIDGETSGLOBAL_H
#define SACOLORWIDGETSGLOBAL_H
#include <QScopedPointer>

/**
 * @def color-widgets的数字版本 MAJ.MIN.PAT
 */
#ifndef SA_COLOR_WIDGETS_VERSION_MAJ
#define SA_COLOR_WIDGETS_VERSION_MAJ 0
#endif
/**
 * @def color-widgets的数字版本 MAJ.MIN.PAT
 */
#ifndef SA_COLOR_WIDGETS_VERSION_MIN
#define SA_COLOR_WIDGETS_VERSION_MIN 1
#endif
/**
 * @def color-widgets的数字版本 MAJ.MIN.PAT
 */
#ifndef SA_COLOR_WIDGETS_VERSION_PAT
#define SA_COLOR_WIDGETS_VERSION_PAT 0
#endif

/**
 * @def   模仿Q_DECLARE_PRIVATE，但不用前置声明而是作为一个内部类
 */
#ifndef SA_COLOR_WIDGETS_DECLARE_PRIVATE
#define SA_COLOR_WIDGETS_DECLARE_PRIVATE(classname)                                                                    \
    class PrivateData;                                                                                                 \
    friend class classname::PrivateData;                                                                               \
    QScopedPointer< PrivateData > d_ptr;
#endif
/**
 * @def   模仿Q_DECLARE_PUBLIC
 */
#ifndef SA_COLOR_WIDGETS_DECLARE_PUBLIC
#define SA_COLOR_WIDGETS_DECLARE_PUBLIC(classname)                                                                     \
    friend class classname;                                                                                            \
    classname* q_ptr { nullptr };
#endif

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

#endif  // SACOLORWIDGETSGLOBAL_H
