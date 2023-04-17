#ifndef SACOLORWIDGETSGLOBAL_H
#define SACOLORWIDGETSGLOBAL_H
#include <QScopedPointer>
/**
 * @def   模仿Q_DECLARE_PRIVATE
 */
#ifndef SA_DECLARE_PRIVATE
#define SA_DECLARE_PRIVATE(classname)                                                                                  \
    class PrivateData;                                                                                                 \
    friend class classname::PrivateData;                                                                               \
    QScopedPointer< PrivateData > d_ptr;
#endif
/**
 * @def   模仿Q_DECLARE_PUBLIC
 */
#ifndef SA_DECLARE_PUBLIC
#define SA_DECLARE_PUBLIC(classname)                                                                                   \
    friend class classname;                                                                                            \
    classname* q_ptr { nullptr };
#endif

#endif  // SACOLORWIDGETSGLOBAL_H
