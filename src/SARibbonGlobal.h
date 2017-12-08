#ifndef SARIBBONGLOBAL_H
#define SARIBBONGLOBAL_H
#include <qglobal.h>
#define SA_RIBBON_BAR_VERSION 1
#define SA_RIBBON_BAR_VERSION_STR "0.1"


#ifndef SA_RIBBON_BAR_NO_EXPORT
    #if defined(SA_RIBBON_BAR_MAKE_LIB)     // 定义此宏将构建library
    #define SA_RIBBON_EXPORT Q_DECL_EXPORT
    #else
    #define SA_RIBBON_EXPORT Q_DECL_IMPORT
    #endif
#endif


#ifndef SA_RIBBON_EXPORT
#define SA_RIBBON_EXPORT
#endif



#endif // SARIBBONGLOBAL_H
