/****************************************************************************
** 这是针对Qt5.14及以上的资源生成的资源初始化相关的函数
** Qt5.14及以上QT_RCC_PREPEND_NAMESPACE使用v3版本的初始化方法
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#ifdef QT_NAMESPACE
#  define QT_RCC_PREPEND_NAMESPACE(name) ::QT_NAMESPACE::name
#  define QT_RCC_MANGLE_NAMESPACE0(x) x
#  define QT_RCC_MANGLE_NAMESPACE1(a, b) a##_##b
#  define QT_RCC_MANGLE_NAMESPACE2(a, b) QT_RCC_MANGLE_NAMESPACE1(a,b)
#  define QT_RCC_MANGLE_NAMESPACE(name) QT_RCC_MANGLE_NAMESPACE2( \
        QT_RCC_MANGLE_NAMESPACE0(name), QT_RCC_MANGLE_NAMESPACE0(QT_NAMESPACE))
#else
#   define QT_RCC_PREPEND_NAMESPACE(name) name
#   define QT_RCC_MANGLE_NAMESPACE(name) name
#endif

#ifdef QT_NAMESPACE
namespace QT_NAMESPACE {
#endif

bool qRegisterResourceData(int, const unsigned char *, const unsigned char *, const unsigned char *);
bool qUnregisterResourceData(int, const unsigned char *, const unsigned char *, const unsigned char *);

#if defined(__ELF__) || defined(__APPLE__)
static inline unsigned char qResourceFeatureZlib()
{
    extern const unsigned char qt_resourceFeatureZlib;
    return qt_resourceFeatureZlib;
}
#else
unsigned char qResourceFeatureZlib();
#endif

#ifdef QT_NAMESPACE
}
#endif

int QT_RCC_MANGLE_NAMESPACE(qInitResources_SARibbonResource)();
int QT_RCC_MANGLE_NAMESPACE(qInitResources_SARibbonResource)()
{
    int version = 3;
    QT_RCC_PREPEND_NAMESPACE(qRegisterResourceData)
        (version, qt_resource_struct, qt_resource_name, qt_resource_data);
    return 1;
}

int QT_RCC_MANGLE_NAMESPACE(qCleanupResources_SARibbonResource)();
int QT_RCC_MANGLE_NAMESPACE(qCleanupResources_SARibbonResource)()
{
    int version = 3;
    version += QT_RCC_PREPEND_NAMESPACE(qResourceFeatureZlib());
    QT_RCC_PREPEND_NAMESPACE(qUnregisterResourceData)
       (version, qt_resource_struct, qt_resource_name, qt_resource_data);
    return 1;
}

namespace {
   struct initializer {
       initializer() { QT_RCC_MANGLE_NAMESPACE(qInitResources_SARibbonResource)(); }
       ~initializer() { QT_RCC_MANGLE_NAMESPACE(qCleanupResources_SARibbonResource)(); }
   } dummy;
}
