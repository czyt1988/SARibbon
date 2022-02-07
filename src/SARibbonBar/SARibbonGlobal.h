#ifndef SARIBBONGLOBAL_H
#define SARIBBONGLOBAL_H
#include <qglobal.h>

#ifndef SA_RIBBON_BAR_VERSION
#define SA_RIBBON_BAR_VERSION    0.2
#endif




#ifndef SA_RIBBON_BAR_VERSION_STR
#define SA_RIBBON_BAR_VERSION_STR    "0.2"
#endif
#ifndef SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE
#define SA_RIBBON_BAR_PROP_CAN_CUSTOMIZE    "_sa_isCanCustomize"
#endif

#ifndef SA_RIBBON_BAR_NO_EXPORT
#if defined(SA_RIBBON_BAR_MAKE_LIB)         
#define SA_RIBBON_EXPORT	Q_DECL_EXPORT
#else
#define SA_RIBBON_EXPORT	Q_DECL_IMPORT
#endif
#endif

#ifndef SA_RIBBON_EXPORT
#define SA_RIBBON_EXPORT
#endif



#endif // SARIBBONGLOBAL_H
