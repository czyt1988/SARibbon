#ifndef SARIBBONGLOBAL_H
#define SARIBBONGLOBAL_H
#include <qglobal.h>
#include <memory>

//! 版本记录：
//!
//! 2023-05-28 -> 0.5.0
//! 调整了大按钮模式下的显示方案，去除了原来SARibbonToolButton的Lite和Normal模式，以WordWrap来表征
//! 支持文字自定义换行
//! 调整了RibbonPannel的标题栏的高度计算方案

/**
 * @def ribbon的数字版本 MAJ.MIN.PAT
 */
#ifndef SA_RIBBON_BAR_VERSION_MAJ
#define SA_RIBBON_BAR_VERSION_MAJ 0
#endif
/**
 * @def ribbon的数字版本 MAJ.MIN.PAT
 */
#ifndef SA_RIBBON_BAR_VERSION_MIN
#define SA_RIBBON_BAR_VERSION_MIN 5
#endif
/**
 * @def ribbon的数字版本 MAJ.MIN.PAT
 */
#ifndef SA_RIBBON_BAR_VERSION_PAT
#define SA_RIBBON_BAR_VERSION_PAT 0
#endif

/**
 * @def 属性，用于标记是否可以进行自定义，用于动态设置到@ref SARibbonCategory 和@ref SARibbonPannel
 * 值为bool，在为true时，可以通过@ref SARibbonCustomizeWidget 改变这个SARibbonCategory和SARibbonPannel的布局，
 * 默认不会有此属性，仅在有此属性且为true时才会在SARibbonCustomizeWidget中能显示为可设置
 */
#ifndef SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE
#define SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE "_sa_isCanCustomize"
#endif

#ifndef SA_RIBBON_BAR_NO_EXPORT
#if defined(SA_RIBBON_BAR_MAKE_LIB)  // 定义此宏将构建library
#define SA_RIBBON_EXPORT Q_DECL_EXPORT
#else
#define SA_RIBBON_EXPORT Q_DECL_IMPORT
#endif
#endif

#ifndef SA_RIBBON_EXPORT
#define SA_RIBBON_EXPORT
#endif

/**
 * @def   模仿Q_DECLARE_PRIVATE，但不用前置声明而是作为一个内部类
 */
#ifndef SA_RIBBON_DECLARE_PRIVATE
#define SA_RIBBON_DECLARE_PRIVATE(classname)                                                                           \
    class PrivateData;                                                                                                 \
    friend class classname::PrivateData;                                                                               \
    std::unique_ptr< PrivateData > d_ptr;
#endif
/**
 * @def   模仿Q_DECLARE_PUBLIC
 */
#ifndef SA_RIBBON_DECLARE_PUBLIC
#define SA_RIBBON_DECLARE_PUBLIC(classname)                                                                            \
    friend class classname;                                                                                            \
    classname* q_ptr { nullptr };
#endif

#endif  // SARIBBONGLOBAL_H
