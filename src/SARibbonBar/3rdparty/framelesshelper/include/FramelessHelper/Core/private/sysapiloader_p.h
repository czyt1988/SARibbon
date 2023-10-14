/*
 * MIT License
 *
 * Copyright (C) 2021-2023 by wangwenx190 (Yuhang Zhao)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once

#include <FramelessHelper/Core/framelesshelpercore_global.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

class FRAMELESSHELPER_CORE_API SysApiLoader : public QObject
{
    Q_OBJECT
    FRAMELESSHELPER_QT_CLASS(SysApiLoader)

public:
    Q_NODISCARD static SysApiLoader *instance();

    Q_NODISCARD static QString platformSharedLibrarySuffixName();
    Q_NODISCARD static QString platformSystemLibraryDirectory();
    Q_NODISCARD static QString generateUniqueKey(const QString &library, const QString &function);

    Q_NODISCARD static QFunctionPointer resolve(const QString &library, const char *function);
    Q_NODISCARD static QFunctionPointer resolve(const QString &library, const QString &function);

    Q_NODISCARD bool isAvailable(const QString &library, const QString &function);

    Q_NODISCARD QFunctionPointer get(const QString &library, const QString &function);

    template<typename T>
    Q_NODISCARD T get(const QString &library, const QString &function)
    {
        return reinterpret_cast<T>(get(library, function));
    }

private:
    explicit SysApiLoader(QObject *parent = nullptr);
    ~SysApiLoader() override;
};

FRAMELESSHELPER_END_NAMESPACE

#define API_AVAILABLE(lib, func) \
  (FRAMELESSHELPER_PREPEND_NAMESPACE(SysApiLoader)::instance()->isAvailable(k##lib, k##func))

#define API_CALL_FUNCTION(lib, func, ...) \
  ((FRAMELESSHELPER_PREPEND_NAMESPACE(SysApiLoader)::instance()->get<decltype(&func)>(k##lib, k##func))(__VA_ARGS__))

#define API_CALL_FUNCTION2(lib, func, type, ...) \
  ((FRAMELESSHELPER_PREPEND_NAMESPACE(SysApiLoader)::instance()->get<type>(k##lib, k##func))(__VA_ARGS__))

#define API_CALL_FUNCTION3(lib, func, name, ...) \
  ((FRAMELESSHELPER_PREPEND_NAMESPACE(SysApiLoader)::instance()->get<decltype(&func)>(k##lib, k##name))(__VA_ARGS__))

#define API_CALL_FUNCTION4(lib, func, ...) API_CALL_FUNCTION3(lib, _##func, func, __VA_ARGS__)

#define API_CALL_FUNCTION5(lib, func, ...) API_CALL_FUNCTION3(lib, func##2, func, __VA_ARGS__)

#ifdef Q_OS_WINDOWS
#  define API_USER_AVAILABLE(func) API_AVAILABLE(user32, func)
#  define API_THEME_AVAILABLE(func) API_AVAILABLE(uxtheme, func)
#  define API_DWM_AVAILABLE(func) API_AVAILABLE(dwmapi, func)
#  define API_SHCORE_AVAILABLE(func) API_AVAILABLE(shcore, func)
#  define API_WINMM_AVAILABLE(func) API_AVAILABLE(winmm, func)
#  define API_D2D_AVAILABLE(func) API_AVAILABLE(d2d1, func)
#  define API_NT_AVAILABLE(func) API_AVAILABLE(ntdll, func)
#endif // Q_OS_WINDOWS

#if (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
#  define API_XLIB_AVAILABLE(func) API_AVAILABLE(libX11, func)
#  define API_XCB_AVAILABLE(func) API_AVAILABLE(libxcb, func)
#  define API_GTK_AVAILABLE(func) API_AVAILABLE(libgtk, func)
#endif // Q_OS_LINUX
