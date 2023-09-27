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

#include "sysapiloader_p.h"

#ifndef SYSAPILOADER_FORCE_QLIBRARY
#  define SYSAPILOADER_FORCE_QLIBRARY (0)
#endif // SYSAPILOADER_FORCE_QLIBRARY

#ifndef SYSAPILOADER_IMPL
#  if (defined(Q_OS_WINDOWS) && !defined(FRAMELESSHELPER_CORE_NO_PRIVATE))
#    define SYSAPILOADER_IMPL (1)
#  else // (!Q_OS_WINDOWS || FRAMELESSHELPER_CORE_NO_PRIVATE)
#    define SYSAPILOADER_IMPL (2)
#  endif // (defined(Q_OS_WINDOWS) && !defined(FRAMELESSHELPER_CORE_NO_PRIVATE))
#endif // SYSAPILOADER_IMPL

#ifndef SYSAPILOADER_QSYSTEMLIBRARY
#  define SYSAPILOADER_QSYSTEMLIBRARY ((SYSAPILOADER_IMPL) == 1)
#endif // SYSAPILOADER_QSYSTEMLIBRARY

#ifndef SYSAPILOADER_QLIBRARY
#  define SYSAPILOADER_QLIBRARY ((SYSAPILOADER_IMPL) == 2)
#endif // SYSAPILOADER_QLIBRARY

#include <QtCore/qhash.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qdir.h>
#include <QtCore/qvarlengtharray.h>
#if SYSAPILOADER_QSYSTEMLIBRARY
#  include <QtCore/private/qsystemlibrary_p.h>
#endif // SYSAPILOADER_QSYSTEMLIBRARY
#if SYSAPILOADER_QLIBRARY
#  include <QtCore/qlibrary.h>
#endif // SYSAPILOADER_QLIBRARY

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcSysApiLoader, "wangwenx190.framelesshelper.core.sysapiloader")
#  define INFO qCInfo(lcSysApiLoader)
#  define DEBUG qCDebug(lcSysApiLoader)
#  define WARNING qCWarning(lcSysApiLoader)
#  define CRITICAL qCCritical(lcSysApiLoader)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using SysApiLoaderData = QHash<QString, QFunctionPointer>;

Q_GLOBAL_STATIC(SysApiLoaderData, g_sysApiLoaderData)

#if FRAMELESSHELPER_CONFIG(debug_output)
[[nodiscard]] static inline bool isDebug()
{
    static const bool flag = (qEnvironmentVariableIntValue("FRAMELESSHELPER_SYSAPILOADER_DEBUG") != 0);
    return flag;
}
#endif

SysApiLoader::SysApiLoader(QObject *parent) : QObject(parent)
{
}

SysApiLoader::~SysApiLoader() = default;

SysApiLoader *SysApiLoader::instance()
{
    static SysApiLoader loader;
    return &loader;
}

QString SysApiLoader::platformSharedLibrarySuffixName()
{
    static const auto result = []() -> QString {
#ifdef Q_OS_WINDOWS
        return FRAMELESSHELPER_STRING_LITERAL(".dll");
#elif (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
        return FRAMELESSHELPER_STRING_LITERAL(".so");
#elif defined(Q_OS_MACOS)
        return FRAMELESSHELPER_STRING_LITERAL(".dylib");
#else
#  error "Unsupported platform!"
#endif
    }();
    return result;
}

QString SysApiLoader::platformSystemLibraryDirectory()
{
    static const auto result = []() -> QString {
#ifdef Q_OS_WINDOWS
        QVarLengthArray<wchar_t, MAX_PATH> buf = {};
        const UINT len = ::GetSystemDirectoryW(buf.data(), MAX_PATH);
        if (len > MAX_PATH) {
            // No need to +1 here, GetSystemDirectoryW() will always give us a null terminator.
            buf.resize(len);
            ::GetSystemDirectoryW(buf.data(), len);
        }
        return QString::fromWCharArray(buf.constData(), len);
#else
        return FRAMELESSHELPER_STRING_LITERAL("/usr/lib");
#endif
    }();
    return result;
}

QString SysApiLoader::generateUniqueKey(const QString &library, const QString &function)
{
    Q_ASSERT(!library.isEmpty());
    Q_ASSERT(!function.isEmpty());
    if (library.isEmpty() || function.isEmpty()) {
        return {};
    }
    QString key = QDir::toNativeSeparators(library);
    // Remove path, only keep the file name.
    if (const qsizetype lastSeparatorPos = key.lastIndexOf(QDir::separator()); lastSeparatorPos >= 0) {
        key.remove(0, lastSeparatorPos);
    }
#ifdef Q_OS_WINDOWS
    key = key.toLower();
#endif // Q_OS_WINDOWS
    static const QString suffix = platformSharedLibrarySuffixName();
    if (!key.endsWith(suffix)) {
        key.append(suffix);
    }
    key.append(u'@');
    key.append(function);
    return key;
}

QFunctionPointer SysApiLoader::resolve(const QString &library, const char *function)
{
    Q_ASSERT(!library.isEmpty());
    Q_ASSERT(function);
    if (library.isEmpty() || !function) {
        return nullptr;
    }
#if SYSAPILOADER_QSYSTEMLIBRARY
    return QSystemLibrary::resolve(library, function);
#endif // SYSAPILOADER_QSYSTEMLIBRARY
#if SYSAPILOADER_QLIBRARY
    return QLibrary::resolve(library, function);
#endif // SYSAPILOADER_QLIBRARY
}

QFunctionPointer SysApiLoader::resolve(const QString &library, const QString &function)
{
    Q_ASSERT(!library.isEmpty());
    Q_ASSERT(!function.isEmpty());
    if (library.isEmpty() || function.isEmpty()) {
        return nullptr;
    }
    return SysApiLoader::resolve(library, function.toUtf8().constData());
}

bool SysApiLoader::isAvailable(const QString &library, const QString &function)
{
    Q_ASSERT(!library.isEmpty());
    Q_ASSERT(!function.isEmpty());
    if (library.isEmpty() || function.isEmpty()) {
        return false;
    }
    const QString key = generateUniqueKey(library, function);
    const auto it = g_sysApiLoaderData()->constFind(key);
    if (it != g_sysApiLoaderData()->constEnd()) {
#if FRAMELESSHELPER_CONFIG(debug_output)
        if (isDebug()) {
            DEBUG << Q_FUNC_INFO << "Function cache found:" << key;
        }
#endif
        return (it.value() != nullptr);
    } else {
        const QFunctionPointer symbol = SysApiLoader::resolve(library, function);
        g_sysApiLoaderData()->insert(key, symbol);
#if FRAMELESSHELPER_CONFIG(debug_output)
        if (isDebug()) {
            DEBUG << Q_FUNC_INFO << "New function cache:" << key << (symbol ? "[VALID]" : "[NULL]");
        }
#endif
        if (symbol) {
            DEBUG << "Successfully loaded" << function << "from" << library;
            return true;
        } else {
            WARNING << "Failed to load" << function << "from" << library;
            return false;
        }
    }
}

QFunctionPointer SysApiLoader::get(const QString &library, const QString &function)
{
    Q_ASSERT(!library.isEmpty());
    Q_ASSERT(!function.isEmpty());
    if (library.isEmpty() || function.isEmpty()) {
        return nullptr;
    }
    const QString key = generateUniqueKey(library, function);
    const auto it = g_sysApiLoaderData()->constFind(key);
    if (it != g_sysApiLoaderData()->constEnd()) {
#if FRAMELESSHELPER_CONFIG(debug_output)
        if (isDebug()) {
            DEBUG << Q_FUNC_INFO << "Function cache found:" << key;
        }
#endif
        return it.value();
    } else {
#if FRAMELESSHELPER_CONFIG(debug_output)
        if (isDebug()) {
            DEBUG << Q_FUNC_INFO << "Function cache not found:" << key;
        }
#endif
        return nullptr;
    }
}

FRAMELESSHELPER_END_NAMESPACE
