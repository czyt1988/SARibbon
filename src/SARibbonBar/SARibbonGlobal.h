#ifndef SARIBBONGLOBAL_H
#define SARIBBONGLOBAL_H
#include <memory>
#include <QtGlobal>
#include <QObject>
#include "SARibbonBarVersionInfo.h"
class QWidget;

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
 * \if ENGLISH
 * @def SA_RIBBON_DECLARE_PRIVATE
 * @brief Similar to Q_DECLARE_PRIVATE, but uses an internal class instead of forward declaration
 * @code
 * //header
 * class A
 * {
 *  SA_RIBBON_DECLARE_PRIVATE(A)
 * };
 * @endcode
 * @code
 * // Expanded result:
 * class A{
 *  class PrivateData;
 *  friend class A::PrivateData;
 *  std::unique_ptr< PrivateData > d_ptr;
 * }
 * @endcode
 * @code
 * //cpp
 * class A::PrivateData{
 *  DA_DECLARE_PUBLIC(A)
 *  PrivateData(A* p):q_ptr(p){
 *  }
 * };
 *
 * A::A():d_ptr(new PrivateData(this)){
 * }
 * @endcode
 * \endif
 *
 * \if CHINESE
 * @def SA_RIBBON_DECLARE_PRIVATE
 * @brief 模仿Q_DECLARE_PRIVATE，但不用前置声明而是作为一个内部类
 * @code
 * //header
 * class A
 * {
 *  SA_RIBBON_DECLARE_PRIVATE(A)
 * };
 * @endcode
 * @code
 * // 其展开效果为：
 * class A{
 *  class PrivateData;
 *  friend class A::PrivateData;
 *  std::unique_ptr< PrivateData > d_ptr;
 * }
 * @endcode
 * @code
 * //cpp
 * class A::PrivateData{
 *  DA_DECLARE_PUBLIC(A)
 *  PrivateData(A* p):q_ptr(p){
 *  }
 * };
 *
 * A::A():d_ptr(new PrivateData(this)){
 * }
 * @endcode
 * \endif
 */
#ifndef SA_RIBBON_DECLARE_PRIVATE
#define SA_RIBBON_DECLARE_PRIVATE(classname)                                                                           \
    class PrivateData;                                                                                                 \
    friend class classname::PrivateData;                                                                               \
    std::unique_ptr< PrivateData > d_ptr;
#endif

/**
 * \if ENGLISH
 * @def SA_RIBBON_DECLARE_PUBLIC
 * @brief Similar to Q_DECLARE_PUBLIC
 * @details Used with SA_RIBBON_DECLARE_PRIVATE
 * \endif
 *
 * \if CHINESE
 * @def SA_RIBBON_DECLARE_PUBLIC
 * @brief 模仿Q_DECLARE_PUBLIC
 * @details 配套SA_RIBBON_DECLARE_PRIVATE使用
 * \endif
 */
#ifndef SA_RIBBON_DECLARE_PUBLIC
#define SA_RIBBON_DECLARE_PUBLIC(classname)                                                                            \
    friend class classname;                                                                                            \
    classname* q_ptr { nullptr };                                                                                      \
    PrivateData(const PrivateData&)            = delete;                                                               \
    PrivateData& operator=(const PrivateData&) = delete;
#endif

/**
 * \if ENGLISH
 * @def SA_RIBBON_IMPL_CONSTRUCT
 * @brief Used with SA_RIBBON_DECLARE_PRIVATE to construct PrivateData in constructor
 * \endif
 *
 * \if CHINESE
 * @def SA_RIBBON_IMPL_CONSTRUCT
 * @brief 配套SA_RIBBON_DECLARE_PRIVATE使用,在构造函数中构建PrivateData
 * \endif
 */
#ifndef SA_RIBBON_IMPL_CONSTRUCT
#define SA_RIBBON_IMPL_CONSTRUCT d_ptr(std::make_unique< PrivateData >(this))
#endif

/**
 * \if ENGLISH
 * @def SA_D
 * @brief Get impl pointer, similar to Q_D
 * \endif
 *
 * \if CHINESE
 * @def SA_D
 * @brief impl获取指针，参考Q_D
 * \endif
 */
#ifndef SA_D
#define SA_D(pointerName) PrivateData* pointerName = d_ptr.get()
#endif

/**
 * \if ENGLISH
 * @def SA_DC
 * @brief Get const impl pointer, similar to Q_DC
 * \endif
 *
 * \if CHINESE
 * @def SA_DC
 * @brief impl获取指针，参考Q_DC
 * \endif
 */
#ifndef SA_DC
#define SA_DC(pointerName) const PrivateData* pointerName = d_ptr.get()
#endif

/**
 * \if ENGLISH
 * @def SA_Q
 * @brief Get pointer in impl, similar to Q_Q
 * \endif
 *
 * \if CHINESE
 * @def SA_Q
 * @brief impl获取指针，参考Q_Q
 * \endif
 */
#ifndef SA_Q
#define SA_Q(pointerName) auto* pointerName = q_ptr
#endif

/**
 * \if ENGLISH
 * @def SA_QC
 * @brief Get const pointer in impl, similar to Q_QC
 * \endif
 *
 * \if CHINESE
 * @def SA_QC
 * @brief impl获取指针，参考Q_QC
 * \endif
 */
#ifndef SA_QC
#define SA_QC(pointerName) const auto* pointerName = q_ptr
#endif

/**
 * \if ENGLISH
 * @brief Define Ribbon alignment, currently supports left alignment and center alignment
 * @note If your compiler reports: illegal qualified name in member declaration, check if the file line endings are LF, if so, change them to CRLF
 * \endif
 *
 * \if CHINESE
 * @brief 定义Ribbon的对其方式，目前支持左对齐和居中对其
 * @note 如果你编译器提示：成员声明的限定名称非法，那么留意一下文件换行是否为LF，如果是把文件换行改为CRLF
 * \endif
 */
enum class SARibbonAlignment
{
    AlignLeft,   ///< Left alignment, tab bar left aligned, category also left aligned
    AlignCenter  ///< Center alignment, tab bar center aligned, category also center aligned
};

/**
 * \if ENGLISH
 * @brief Ribbon theme
 * @note Some QSS sizes cannot be obtained in C++ code, so for user-defined QSS themes, some sizes need to be set manually
 * @note For example, ribbon tab margin information cannot be obtained from QTabBar, which affects the drawing of SARibbonContextCategory
 * @note Therefore, after setting QSS, you need to reset the margin information into SARibbonTabBar
 * \endif
 *
 * \if CHINESE
 * @brief ribbon主题
 * @note 由于有些qss的尺寸，在C++代码中无法获取到，因此针对用户自定义的qss主题，有些尺寸是需要手动设置进去的
 * @note 例如ribbon tab的margin信息，在QTabBar是无法获取到，而这个影响了SARibbonContextCategory的绘制，
 * @note 因此，在设置qss后需要针对margin信息重新设置进SARibbonTabBar中
 * \endif
 */
enum class SARibbonTheme
{
    RibbonThemeOffice2013,      ///< Office 2013 theme
    RibbonThemeOffice2016Blue,  ///< Office 2016 - Blue theme
    RibbonThemeOffice2021Blue,  ///< Office 2021 - Blue theme
    RibbonThemeWindows7,        ///< Windows 7 theme
    RibbonThemeDark,            ///< Dark theme
    RibbonThemeDark2            ///< Dark theme 2
};

/**
 * \if ENGLISH
 * @brief RibbonMainWindow style
 * \endif
 *
 * \if CHINESE
 * @brief RibbonMainWindow的样式
 * \endif
 */
enum class SARibbonMainWindowStyleFlag : int
{
    UseRibbonFrame   = 1,  ///< Use ribbon frame, which is more compact
    UseNativeFrame   = 2,  ///< Use operating system frame
    UseRibbonMenuBar = 4,  ///< Use ribbon menu bar
    UseNativeMenuBar = 8   ///< Use native menu bar
};
Q_DECLARE_FLAGS(SARibbonMainWindowStyles, SARibbonMainWindowStyleFlag)
Q_DECLARE_OPERATORS_FOR_FLAGS(SARibbonMainWindowStyles)

/**
 * \if ENGLISH
 * @def SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE
 * @brief Property used to mark if customization is allowed, dynamically set to @ref SARibbonCategory and @ref SARibbonPanel
 * @details Value is bool, when true, the layout of SARibbonCategory and SARibbonPanel can be changed through @ref SARibbonCustomizeWidget
 * @details By default, this property does not exist, only when this property exists and is true will it be displayed as configurable in SARibbonCustomizeWidget
 * \endif
 *
 * \if CHINESE
 * @def SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE
 * @brief 属性，用于标记是否可以进行自定义，用于动态设置到@ref SARibbonCategory 和@ref SARibbonPanel
 * @details 值为bool，在为true时，可以通过@ref SARibbonCustomizeWidget 改变这个SARibbonCategory和SARibbonPanel的布局，
 * @details 默认不会有此属性，仅在有此属性且为true时才会在SARibbonCustomizeWidget中能显示为可设置
 * \endif
 */
#ifndef SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE
#define SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE "_sa_isCanCustomize"
#endif

#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L)
#ifndef sa_as_const
#define sa_as_const std::as_const
#endif
#else
// C++14 及以下版本使用 Qt 的 qwt_as_const
#ifndef sa_as_const
#define sa_as_const qAsConst
#endif
#endif

#endif  // SARIBBONGLOBAL_H
