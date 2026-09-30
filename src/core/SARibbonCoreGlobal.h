#ifndef SARIBBONCOREGLOBAL_H
#define SARIBBONCOREGLOBAL_H
#include <memory>
#include <QtGlobal>
#include <QObject>
// 注意：不要在此 include SARibbonCoreConfig.h —— 它由 configure_file 生成到 build 树
// （${CMAKE_BINARY_DIR}/include/SARibbonCore/），源码树与 amalgamate 单文件场景都看不到该文件。
// 需要 feature 开关的翻译单元显式 include <SARibbonCore/SARibbonCoreConfig.h>（计划 02 起使用）。

// 三段式导出宏（模板见计划 01 S5.2，QWK qwkglobal.h:12-22 同款）
#ifndef SA_RIBBON_CORE_EXPORT
#  ifdef SA_RIBBON_CORE_STATIC
#    define SA_RIBBON_CORE_EXPORT
#  else
#    ifdef SA_RIBBON_CORE_LIBRARY
#      define SA_RIBBON_CORE_EXPORT Q_DECL_EXPORT
#    else
#      define SA_RIBBON_CORE_EXPORT Q_DECL_IMPORT
#    endif
#  endif
#endif

// 占位导出符号：纯头模块成 DLL 时若没有任何导出符号，MSVC 不会生成导入库（.lib），
// 下游模块无法链接（计划 01 S6.2 的落地修正，见 NOTES）；计划 02 下沉真实源后保留作 ABI 探针。
SA_RIBBON_CORE_EXPORT int saRibbonCoreAbiVersion();

// ==== 以下 PIMPL 宏区 = 原 SARibbonGlobal.h L20-184 整段原样 move ====
// SA_RIBBON_DECLARE_PRIVATE / SA_RIBBON_DECLARE_PUBLIC / SA_RIBBON_IMPL_CONSTRUCT
// SA_D / SA_DC / SA_Q / SA_QC（含全部双语 Doxygen 注释，宏名与定义体一字不改）
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

// sa_as_const：原 SARibbonGlobal.h L274-283 整段 move（C++17 std::as_const / C++14 qAsConst 分支）
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

#endif  // SARIBBONCOREGLOBAL_H
