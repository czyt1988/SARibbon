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

#include "framelesshelpercore_global.h"
#include "framelesshelpercore_global_p.h"
#include "versionnumber_p.h"
#include "utils.h"
#include <QtCore/qiodevice.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qloggingcategory.h>

#ifndef QT_NO_DEBUG_STREAM
QT_BEGIN_NAMESPACE
QDebug operator<<(QDebug d, const FRAMELESSHELPER_PREPEND_NAMESPACE(VersionNumber) &ver)
{
    const QDebugStateSaver saver(d);
    d.nospace().noquote() << "VersionNumber("
                          << ver.Major << ", "
                          << ver.Minor << ", "
                          << ver.Patch << ", "
                          << ver.Tweak << ')';
    return d;
}

QDebug operator<<(QDebug d, const FRAMELESSHELPER_PREPEND_NAMESPACE(Global)::VersionInfo &ver)
{
    const QDebugStateSaver saver(d);
    unsigned long major = 0, minor = 0, patch = 0;
    FRAMELESSHELPER_EXTRACT_VERSION(ver.version.num, major, minor, patch)
    const auto ver_num = FRAMELESSHELPER_PREPEND_NAMESPACE(VersionNumber){ major, minor, patch };
    d.nospace().noquote() << "VersionInfo("
                          << "version number: " << ver_num << ", "
                          << "version string: " << ver.version.str << ", "
                          << "commit hash: " << ver.commit.hash << ", "
                          << "commit subject: " << ver.commit.subject << ", "
                          << "commit author: " << ver.commit.author << ", "
                          << "commit date time: " << ver.commit.datetime << ", "
                          << "commit branch: " << ver.commit.branch << ", "
                          << "compiler name: " << ver.compiler.name << ", "
                          << "compiler version: " << ver.compiler.version << ", "
                          << "compiler vendor: " << ver.compiler.vendor << ", "
                          << "cmake version: " << ver.build.cmake_version << ", "
                          << "cmake configure date time: " << ver.build.configure_datetime << ", "
                          << "cmake generator: " << ver.build.generator << ", "
                          << "architecture: " << ver.build.architecture << ", "
                          << "debug build: " << ver.build.is_debug << ", "
                          << "static build: " << ver.build.is_static << ')';
    return d;
}

QDebug operator<<(QDebug d, const FRAMELESSHELPER_PREPEND_NAMESPACE(Global)::Dpi &dpi)
{
    const QDebugStateSaver saver(d);
    const qreal scaleFactor = (qreal(dpi.x) / qreal(FRAMELESSHELPER_PREPEND_NAMESPACE(Utils)::defaultScreenDpi()));
    d.nospace().noquote() << "Dpi("
                          << "x: " << dpi.x << ", "
                          << "y: " << dpi.y << ", "
                          << "scale factor: " << scaleFactor << ')';
    return d;
}
QT_END_NAMESPACE
#endif // QT_NO_DEBUG_STREAM

#if FRAMELESSHELPER_CONFIG(bundle_resource)
// The "Q_INIT_RESOURCE()" macro can't be used within a namespace,
// so we wrap it into a separate function outside of the namespace and
// then call it instead inside the namespace, that's also the recommended
// workaround provided by Qt's official documentation.
void FramelessHelperCoreInitResource()
{
    Q_INIT_RESOURCE(framelesshelpercore);
}
#endif // FRAMELESSHELPER_CONFIG(bundle_resource)

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcCoreGlobal, "wangwenx190.framelesshelper.core.global")
#  define INFO qCInfo(lcCoreGlobal)
#  define DEBUG qCDebug(lcCoreGlobal)
#  define WARNING qCWarning(lcCoreGlobal)
#  define CRITICAL qCCritical(lcCoreGlobal)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

#ifdef Q_OS_WINDOWS
static_assert(std::size(WindowsVersions) == (static_cast<int>(WindowsVersion::Latest) + 1));
#endif

FramelessCallbacks::FramelessCallbacks() = default;

FramelessCallbacks::~FramelessCallbacks() = default;

FramelessCallbacksPtr FramelessCallbacks::create()
{
    return std::make_shared<FramelessCallbacks>();
}

FramelessExtraData::FramelessExtraData() = default;

FramelessExtraData::~FramelessExtraData() = default;

FramelessData::FramelessData() = default;

FramelessData::~FramelessData() = default;

void FramelessHelperCoreInitialize()
{
    static bool inited = false;
    if (inited) {
        return;
    }
    inited = true;

    FramelessHelperPrintLogo();

#if (defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID))
    // Qt's Wayland experience is not good, so we force the XCB backend here.
    // TODO: Remove this hack once Qt's Wayland implementation is good enough.
    // We are setting the preferred QPA backend, so we have to set it early
    // enough, that is, before the construction of any Q(Gui)Application
    // instances. QCoreApplication won't instantiate the platform plugin.
    qputenv("QT_QPA_PLATFORM", "xcb");
    // Fedora and Arch users report segfault when calling XInitThreads() and gtk_init().
    //XInitThreads(); // Users report that GTK is crashing without this.
    //gtk_init(nullptr, nullptr); // Users report that GTK functionalities won't work without this.
#endif

#if (defined(Q_OS_MACOS) && (QT_VERSION < QT_VERSION_CHECK(6, 0, 0)))
    qputenv("QT_MAC_WANTS_LAYER", "1");
#endif

#ifdef Q_OS_WINDOWS
    // This is equivalent to set the "dpiAware" and "dpiAwareness" field in
    // your manifest file. It works through out Windows Vista to Windows 11.
    // It's highly recommended to enable the highest DPI awareness mode
    // (currently it's PerMonitor Version 2, or PMv2 for short) for any GUI
    // applications, to allow your user interface scale to an appropriate
    // size and still stay sharp, though you will have to do the calculation
    // and resize by yourself.
    std::ignore = Utils::tryToEnableHighestDpiAwarenessLevel();
    // This function need to be called before any dialogs are created, so
    // to be safe we call it here.
    // Without this hack, our native dialogs won't be able to respond to
    // DPI change messages correctly, especially the non-client area.
    std::ignore =  Utils::fixupDialogsDpiScaling();
#endif

    // We need this flag to enable nested frameless windows, however,
    // this flag is known to be __NOT__ compatible with QGLWidget.
    QCoreApplication::setAttribute(Qt::AA_DontCreateNativeWidgetSiblings);

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    // Enable high DPI scaling by default, but only for Qt5 applications,
    // because this has become the default setting since Qt6 and it can't
    // be changed from outside anymore (except for internal testing purposes).
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif
}

void FramelessHelperCoreUninitialize()
{
    static bool uninited = false;
    if (uninited) {
        return;
    }
    uninited = true;
}

VersionInfo FramelessHelperVersion()
{
    static const auto result = []() -> VersionInfo {
        VersionInfo vi = {};
        vi.version.num = FRAMELESSHELPER_VERSION;
        vi.version.str = FRAMELESSHELPER_VERSION_STR;
        vi.commit.hash = FRAMELESSHELPER_COMMIT_HASH_STR;
        vi.commit.subject = FRAMELESSHELPER_COMMIT_SUBJECT_STR;
        vi.commit.author = FRAMELESSHELPER_COMMIT_AUTHOR_STR;
        vi.commit.datetime = FRAMELESSHELPER_COMMIT_DATETIME_STR;
        vi.commit.branch = FRAMELESSHELPER_COMMIT_BRANCH_STR;
        vi.compiler.name = FRAMELESSHELPER_COMPILER_NAME_STR;
        vi.compiler.version = FRAMELESSHELPER_COMPILER_VERSION_STR;
        vi.compiler.vendor = FRAMELESSHELPER_COMPILER_VENDOR_STR;
        vi.build.cmake_version = FRAMELESSHELPER_CMAKE_VERSION_STR;
        vi.build.configure_datetime = FRAMELESSHELPER_BUILD_DATETIME_STR;
        vi.build.generator = FRAMELESSHELPER_CMAKE_GENERATOR_STR;
        vi.build.architecture = FRAMELESSHELPER_ARCHITECTURE_STR;
        vi.build.is_debug = []() -> bool {
#ifdef _DEBUG
            return true;
#else
            return false;
#endif
        }();
        vi.build.is_static = []() -> bool {
#if FRAMELESSHELPER_CONFIG(static_build)
            return true;
#else
            return false;
#endif
        }();
        return vi;
    }();
    return result;
}

void FramelessHelperEnableThemeAware()
{
    static bool set = false;
    if (set) {
        return;
    }
    set = true;

#ifdef Q_OS_WINDOWS
    // This hack is needed to let AllowDarkModeForWindow() work.
    std::ignore = Utils::setDarkModeAllowedForApp(true);
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    // Qt gained the ability to detect system theme change since 5.15 but
    // it's not quite useful until Qt6.
    std::ignore = Utils::setQtDarkModeAwareEnabled(true);
#  endif
#endif

#if ((defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID) && (QT_VERSION < QT_VERSION_CHECK(6, 4, 0))) || \
    (defined(Q_OS_MACOS) && (QT_VERSION < QT_VERSION_CHECK(5, 12, 0))))
    // Linux: Qt 6.4 gained the ability to detect system theme change.
    // macOS: Qt 5.12.
    std::ignore = Utils::registerThemeChangeNotification();
#endif
}

void FramelessHelperPrintLogo()
{
    static const bool noLogo = (qEnvironmentVariableIntValue("FRAMELESSHELPER_NO_LOGO") != 0);
    if (noLogo) {
        return;
    }
    const VersionInfo ver = FramelessHelperVersion();
    QString message = {};
    QTextStream stream(&message, QIODevice::WriteOnly);
    stream << "FramelessHelper (" << (ver.build.is_static ? "static" : "shared")
           << ", " << (ver.build.is_debug ? "debug" : "release") << ", "
           << ver.build.architecture << ") version " << ver.version.str
           << ", author wangwenx190 (Yuhang Zhao, 2546789017@qq.com)."
           << " Built by " << ver.compiler.name << ver.compiler.version
           << " from " << ver.commit.hash << " on "
           << ver.build.configure_datetime << " (UTC).";
    INFO.nospace().noquote() << message;
}

FRAMELESSHELPER_END_NAMESPACE
