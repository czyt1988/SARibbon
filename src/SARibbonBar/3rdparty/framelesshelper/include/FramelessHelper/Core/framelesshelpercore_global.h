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

#include "framelesshelper.config"
#include <QtCore/qglobal.h>
#include <QtCore/qmath.h>
#include <QtCore/qpoint.h>
#include <QtCore/qsize.h>
#include <QtCore/qrect.h>
#include <QtCore/qobject.h>
#include <QtCore/qpointer.h>
#include <QtGui/qcolor.h>
#include <QtGui/qwindowdefs.h>

QT_BEGIN_NAMESPACE
class QEvent;
class QEnterEvent;
QT_END_NAMESPACE

/*
    The FRAMELESSHELPER_CONFIG macro implements a safe compile time check for features of FramelessHelper.
    Features can be in three states:
        0 or undefined: This will lead to a compile error when testing for it
        -1: The feature is not available
        1: The feature is available
*/
#ifndef FRAMELESSHELPER_CONFIG
#  define FRAMELESSHELPER_CONFIG(feature) ((1 / FRAMELESSHELPER_FEATURE_##feature) == 1)
#endif

#ifndef FRAMELESSHELPER_CORE_API
#  if FRAMELESSHELPER_CONFIG(static_build)
#    define FRAMELESSHELPER_CORE_API
#  else // !FRAMELESSHELPER_CORE_STATIC
#    ifdef FRAMELESSHELPER_CORE_LIBRARY
#      define FRAMELESSHELPER_CORE_API Q_DECL_EXPORT
#    else // !FRAMELESSHELPER_CORE_LIBRARY
#      define FRAMELESSHELPER_CORE_API Q_DECL_IMPORT
#    endif // FRAMELESSHELPER_CORE_LIBRARY
#  endif // FRAMELESSHELPER_CORE_STATIC
#endif // FRAMELESSHELPER_CORE_API

#if (defined(Q_OS_WIN) && !defined(Q_OS_WINDOWS))
#  define Q_OS_WINDOWS // Since 5.14
#endif

#ifndef Q_DISABLE_MOVE
#  define Q_DISABLE_MOVE(Class) \
      Class(Class &&) = delete; \
      Class &operator=(Class &&) = delete;
#endif

#ifndef Q_DISABLE_COPY_MOVE // Since 5.13
#  define Q_DISABLE_COPY_MOVE(Class) \
      Q_DISABLE_COPY(Class) \
      Q_DISABLE_MOVE(Class)
#endif

#if (QT_VERSION < QT_VERSION_CHECK(5, 10, 0))
   using QStringView = const QString &;
#endif

#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
#  define qExchange(a, b) std::exchange(a, b)
#endif

#ifndef Q_NAMESPACE_EXPORT // Since 5.14
#  define Q_NAMESPACE_EXPORT(...) Q_NAMESPACE
#endif

// QColor can't be constexpr before 5.14
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
#  define Q_COLOR_CONSTEXPR constexpr
#else
#  define Q_COLOR_CONSTEXPR
#endif

// MOC can't handle C++ attributes before 5.15.
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
#  define Q_NODISCARD [[nodiscard]]
#  define Q_MAYBE_UNUSED [[maybe_unused]]
#else
#  define Q_NODISCARD
#  define Q_MAYBE_UNUSED
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
   using QT_NATIVE_EVENT_RESULT_TYPE = qintptr;
   using QT_ENTER_EVENT_TYPE = QEnterEvent;
#else
   using QT_NATIVE_EVENT_RESULT_TYPE = long;
   using QT_ENTER_EVENT_TYPE = QEvent;
#endif

// QLatin1StringView can't be constexpr until Qt6?
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#  define Q_STRING_CONSTEXPR constexpr
#else
#  define Q_STRING_CONSTEXPR
#endif

#ifndef QUtf8String
#  define QUtf8String(str) QString::fromUtf8(str)
#endif

#ifndef Q_GADGET_EXPORT // Since 6.3
#  define Q_GADGET_EXPORT(...) Q_GADGET
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
  using namespace Qt::Literals::StringLiterals;
#endif

#ifndef FRAMELESSHELPER_BYTEARRAY_LITERAL
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
#    define FRAMELESSHELPER_BYTEARRAY_LITERAL(ba) ba##_ba
#  elif (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
#    define FRAMELESSHELPER_BYTEARRAY_LITERAL(ba) ba##_qba
#  else
#    define FRAMELESSHELPER_BYTEARRAY_LITERAL(ba) QByteArrayLiteral(ba)
#  endif
#endif

#ifndef FRAMELESSHELPER_STRING_LITERAL
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
#    define FRAMELESSHELPER_STRING_LITERAL(str) u##str##_s
#  elif (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
#    define FRAMELESSHELPER_STRING_LITERAL(str) u##str##_qs
#  else
#    define FRAMELESSHELPER_STRING_LITERAL(str) QStringLiteral(str)
#  endif
#endif

#ifndef FRAMELESSHELPER_BYTEARRAY
#  define FRAMELESSHELPER_BYTEARRAY(ba) ba
#endif

#ifndef FRAMELESSHELPER_STRING
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
#    define FRAMELESSHELPER_STRING(str) str##_L1
#  else
#    define FRAMELESSHELPER_STRING(str) QLatin1String(str)
#  endif
#endif

#ifndef FRAMELESSHELPER_STRING_TYPE
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 4, 0))
#    define FRAMELESSHELPER_STRING_TYPE QLatin1StringView
#  else
#    define FRAMELESSHELPER_STRING_TYPE QLatin1String
#  endif
#endif

#ifndef Q_UNREACHABLE_RETURN // Since 6.5
#  define Q_UNREACHABLE_RETURN(...) \
     do { \
         Q_UNREACHABLE(); \
         return __VA_ARGS__; \
     } while (false)
#endif

#ifndef FRAMELESSHELPER_QUOTE
#  define FRAMELESSHELPER_QUOTE(x) #x
#endif

#ifndef FRAMELESSHELPER_QUOTE2
#  define FRAMELESSHELPER_QUOTE2(x) FRAMELESSHELPER_QUOTE(x)
#endif

#ifndef FRAMELESSHELPER_CONCAT
#  define FRAMELESSHELPER_CONCAT(a, b) a##b
#endif

#ifndef FRAMELESSHELPER_CONCAT2
#  define FRAMELESSHELPER_CONCAT2(a, b) FRAMELESSHELPER_CONCAT(a, b)
#endif

#ifndef FRAMELESSHELPER_REQUIRE_CONFIG
#  define FRAMELESSHELPER_REQUIRE_CONFIG(feature) static_assert(FRAMELESSHELPER_FEATURE_##feature == 1, "Required feature " #feature " for file " __FILE__ " is not available!");
#endif

#ifndef FRAMELESSHELPER_CLASS_INFO
#  define FRAMELESSHELPER_CLASS_INFO Q_CLASSINFO("__FRAMELESSHELPER__", FRAMELESSHELPER_QUOTE2(__FRAMELESSHELPER__))
#endif

#ifndef FRAMELESSHELPER_BYTEARRAY_CONSTANT2
#  define FRAMELESSHELPER_BYTEARRAY_CONSTANT2(name, ba) \
     [[maybe_unused]] static constexpr const auto k##name = FRAMELESSHELPER_BYTEARRAY(ba);
#endif

#ifndef FRAMELESSHELPER_STRING_CONSTANT2
#  define FRAMELESSHELPER_STRING_CONSTANT2(name, str) \
     [[maybe_unused]] static Q_STRING_CONSTEXPR const auto k##name = FRAMELESSHELPER_STRING(str);
#endif

#ifndef FRAMELESSHELPER_BYTEARRAY_CONSTANT
#  define FRAMELESSHELPER_BYTEARRAY_CONSTANT(ba) FRAMELESSHELPER_BYTEARRAY_CONSTANT2(ba, #ba)
#endif

#ifndef FRAMELESSHELPER_STRING_CONSTANT
#  define FRAMELESSHELPER_STRING_CONSTANT(str) FRAMELESSHELPER_STRING_CONSTANT2(str, #str)
#endif

#ifndef FRAMELESSHELPER_NAMESPACE
#  define FRAMELESSHELPER_NAMESPACE wangwenx190::FramelessHelper
#endif

#ifndef FRAMELESSHELPER_BEGIN_NAMESPACE
#  define FRAMELESSHELPER_BEGIN_NAMESPACE namespace FRAMELESSHELPER_NAMESPACE {
#endif

#ifndef FRAMELESSHELPER_END_NAMESPACE
#  define FRAMELESSHELPER_END_NAMESPACE }
#endif

#ifndef FRAMELESSHELPER_USE_NAMESPACE
#  define FRAMELESSHELPER_USE_NAMESPACE using namespace FRAMELESSHELPER_NAMESPACE;
#endif

#ifndef FRAMELESSHELPER_PREPEND_NAMESPACE
#  define FRAMELESSHELPER_PREPEND_NAMESPACE(X) ::FRAMELESSHELPER_NAMESPACE::X
#endif

#ifndef FRAMELESSHELPER_MAKE_VERSION
#  define FRAMELESSHELPER_MAKE_VERSION(Major, Minor, Patch) \
     ((((Major) & 0xff) << 24) | (((Minor) & 0xff) << 16) | (((Patch) & 0xff) << 8))
#endif

#ifndef FRAMELESSHELPER_EXTRACT_VERSION
#  define FRAMELESSHELPER_EXTRACT_VERSION(Version, Major, Minor, Patch) \
     { \
         (Major) = (((Version) & 0xff) >> 24); \
         (Minor) = (((Version) & 0xff) >> 16); \
         (Patch) = (((Version) & 0xff) >> 8); \
     }
#endif

#ifndef FRAMELESSHELPER_CLASS
#  define FRAMELESSHELPER_CLASS(Class) \
  private: \
      Q_DISABLE_COPY(Class)
#endif

#ifndef FRAMELESSHELPER_CLASS_DPTR
#  define FRAMELESSHELPER_CLASS_DPTR(Class) \
  private: \
      Q_DECLARE_PRIVATE(Class) \
      const QScopedPointer<Class##Private> d_ptr;
#endif

#ifndef FRAMELESSHELPER_CLASS_QPTR
#  define FRAMELESSHELPER_CLASS_QPTR(Class) \
  private: \
      Q_DECLARE_PUBLIC(Class) \
  public: \
      Class *q_ptr = nullptr;
#endif

#ifndef FRAMELESSHELPER_PRIVATE_CLASS_GETTER
#  define FRAMELESSHELPER_PRIVATE_CLASS_GETTER(Class) \
  public: \
      Q_NODISCARD static Class##Private *get(Class *q); \
      Q_NODISCARD static const Class##Private *get(const Class *q);
#endif

#ifndef FRAMELESSHELPER_PRIVATE_CLASS_GETTER_IMPL
#  define FRAMELESSHELPER_PRIVATE_CLASS_GETTER_IMPL(Class) \
  Class##Private *Class##Private::get(Class *q) { \
      Q_ASSERT(q); \
      return (q ? q->d_func() : nullptr); \
  } \
  const Class##Private *Class##Private::get(const Class *q) { \
      Q_ASSERT(q); \
      return (q ? q->d_func() : nullptr); \
  }
#endif

#ifndef FRAMELESSHELPER_PUBLIC_CLASS
#  define FRAMELESSHELPER_PUBLIC_CLASS(Class) \
  private: \
      FRAMELESSHELPER_CLASS(Class) \
      FRAMELESSHELPER_CLASS_DPTR(Class)
#endif

#ifndef FRAMELESSHELPER_PRIVATE_CLASS
#  define FRAMELESSHELPER_PRIVATE_CLASS(Class) \
  private: \
      FRAMELESSHELPER_CLASS(Class##Private) \
      FRAMELESSHELPER_CLASS_QPTR(Class) \
      FRAMELESSHELPER_PRIVATE_CLASS_GETTER(Class)
#endif

#ifndef FRAMELESSHELPER_QT_CLASS
#  define FRAMELESSHELPER_QT_CLASS(Class) \
  private: \
      FRAMELESSHELPER_CLASS_INFO \
      FRAMELESSHELPER_CLASS(Class)
#endif

#ifndef FRAMELESSHELPER_PUBLIC_QT_CLASS
#  define FRAMELESSHELPER_PUBLIC_QT_CLASS(Class) \
  private: \
      FRAMELESSHELPER_QT_CLASS(Class) \
      FRAMELESSHELPER_CLASS_DPTR(Class)
#endif

#ifndef FRAMELESSHELPER_PRIVATE_QT_CLASS
#  define FRAMELESSHELPER_PRIVATE_QT_CLASS(Class) \
  private: \
      FRAMELESSHELPER_QT_CLASS(Class##Private) \
      FRAMELESSHELPER_CLASS_QPTR(Class) \
      FRAMELESSHELPER_PRIVATE_CLASS_GETTER(Class)
#endif

#if FRAMELESSHELPER_CONFIG(bundle_resource)
// Call this function in your main() function if you are using FramelessHelper as a static library,
// it can make sure the resources bundled in the static library are correctly initialized.
// NOTE: This function is intentionally not inside any namespaces.
FRAMELESSHELPER_CORE_API void FramelessHelperCoreInitResource();
inline void framelesshelpercore_initResource() { FramelessHelperCoreInitResource(); }
#endif // FRAMELESSHELPER_CORE_NO_BUNDLE_RESOURCE

FRAMELESSHELPER_BEGIN_NAMESPACE

#include "framelesshelper.version"

#ifndef __FRAMELESSHELPER__
#  define __FRAMELESSHELPER__ __FRAMELESSHELPER_VERSION__
#endif // __FRAMELESSHELPER__

namespace Global
{

Q_NAMESPACE_EXPORT(FRAMELESSHELPER_CORE_API)

[[maybe_unused]] inline constexpr const int kDefaultResizeBorderThickness = 8;
[[maybe_unused]] inline constexpr const int kDefaultCaptionHeight = 23;
[[maybe_unused]] inline constexpr const int kDefaultTitleBarHeight = 32;
[[maybe_unused]] inline constexpr const int kDefaultExtendedTitleBarHeight = 48;
[[maybe_unused]] inline constexpr const int kDefaultWindowFrameBorderThickness = 1;
[[maybe_unused]] inline constexpr const int kDefaultTitleBarFontPointSize = 11;
[[maybe_unused]] inline constexpr const int kDefaultTitleBarContentsMargin = 10;
[[maybe_unused]] inline constexpr const int kMacOSChromeButtonAreaWidth = 60;
[[maybe_unused]] inline constexpr const QSize kDefaultWindowIconSize = {16, 16};
// We have to use "qRound()" here because "std::round()" is not constexpr, yet.
[[maybe_unused]] inline constexpr const QSize kDefaultSystemButtonSize = {qRound(qreal(kDefaultTitleBarHeight) * 1.5), kDefaultTitleBarHeight};
[[maybe_unused]] inline constexpr const QSize kDefaultSystemButtonIconSize = kDefaultWindowIconSize;
[[maybe_unused]] inline constexpr const QSize kDefaultWindowSize = {160, 160}; // Value taken from Windows QPA.

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
#  define kDefaultWhiteColor QColorConstants::White
#  define kDefaultBlackColor QColorConstants::Black
#  define kDefaultTransparentColor QColorConstants::Transparent
#  define kDefaultLightGrayColor QColorConstants::LightGray
#  define kDefaultDarkGrayColor QColorConstants::DarkGray
#else // (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
   [[maybe_unused]] inline Q_COLOR_CONSTEXPR const QColor kDefaultWhiteColor = {255, 255, 255}; // #FFFFFF
   [[maybe_unused]] inline Q_COLOR_CONSTEXPR const QColor kDefaultBlackColor = {0, 0, 0}; // #000000
   [[maybe_unused]] inline Q_COLOR_CONSTEXPR const QColor kDefaultTransparentColor = {0, 0, 0, 0};
   [[maybe_unused]] inline Q_COLOR_CONSTEXPR const QColor kDefaultLightGrayColor = {211, 211, 211}; // #D3D3D3
   [[maybe_unused]] inline Q_COLOR_CONSTEXPR const QColor kDefaultDarkGrayColor = {169, 169, 169}; // #A9A9A9
#endif // (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))

[[maybe_unused]] inline Q_COLOR_CONSTEXPR const QColor kDefaultSystemLightColor = {240, 240, 240}; // #F0F0F0
[[maybe_unused]] inline Q_COLOR_CONSTEXPR const QColor kDefaultSystemDarkColor = {32, 32, 32}; // #202020
[[maybe_unused]] inline Q_COLOR_CONSTEXPR const QColor kDefaultFrameBorderActiveColorLight = {110, 110, 110}; // #6E6E6E
[[maybe_unused]] inline Q_COLOR_CONSTEXPR const QColor kDefaultFrameBorderActiveColorDark = {51, 51, 51}; // #333333
[[maybe_unused]] inline Q_COLOR_CONSTEXPR const QColor kDefaultFrameBorderInactiveColorLight = {167, 167, 167}; // #A7A7A7
[[maybe_unused]] inline Q_COLOR_CONSTEXPR const QColor kDefaultFrameBorderInactiveColorDark = {61, 61, 62}; // #3D3D3E
[[maybe_unused]] inline Q_COLOR_CONSTEXPR const QColor kDefaultSystemButtonBackgroundColor = {204, 204, 204}; // #CCCCCC
[[maybe_unused]] inline Q_COLOR_CONSTEXPR const QColor kDefaultSystemCloseButtonBackgroundColor = {232, 17, 35}; // #E81123

[[maybe_unused]] inline constexpr const char kDontOverrideCursorVar[] = "FRAMELESSHELPER_DONT_OVERRIDE_CURSOR";
[[maybe_unused]] inline constexpr const char kDontToggleMaximizeVar[] = "FRAMELESSHELPER_DONT_TOGGLE_MAXIMIZE";
[[maybe_unused]] inline constexpr const char kSysMenuDisableMinimizeVar[] = "FRAMELESSHELPER_SYSTEM_MENU_DISABLE_MINIMIZE";
[[maybe_unused]] inline constexpr const char kSysMenuDisableMaximizeVar[] = "FRAMELESSHELPER_SYSTEM_MENU_DISABLE_MAXIMIZE";
[[maybe_unused]] inline constexpr const char kSysMenuDisableRestoreVar[] = "FRAMELESSHELPER_SYSTEM_MENU_DISABLE_RESTORE";

enum class Option : quint8
{
    UseCrossPlatformQtImplementation,
    ForceHideWindowFrameBorder,
    ForceShowWindowFrameBorder,
    DisableWindowsSnapLayout,
    WindowUseRoundCorners,
    CenterWindowBeforeShow,
    EnableBlurBehindWindow,
    ForceNonNativeBackgroundBlur,
    DisableLazyInitializationForMicaMaterial,
    ForceNativeBackgroundBlur,
    WindowUseSquareCorners,
    Last = WindowUseSquareCorners
};
Q_ENUM_NS(Option)

enum class SystemTheme : quint8
{
    Unknown,
    Light,
    Dark,
    HighContrast
};
Q_ENUM_NS(SystemTheme)

enum class SystemButtonType : quint8
{
    Unknown,
    WindowIcon,
    Help,
    Minimize,
    Maximize,
    Restore,
    Close,
    Last = Close
};
Q_ENUM_NS(SystemButtonType)

#ifdef Q_OS_WINDOWS
enum class DwmColorizationArea : quint8
{
    None,
    StartMenu_TaskBar_ActionCenter,
    TitleBar_WindowBorder,
    All
};
Q_ENUM_NS(DwmColorizationArea)
#endif // Q_OS_WINDOWS

enum class ButtonState : quint8
{
    Normal,
    Hovered,
    Pressed,
    Released
};
Q_ENUM_NS(ButtonState)

#ifdef Q_OS_WINDOWS
enum class WindowsVersion : quint8
{
    _2000,
    _XP,
    _XP_64,
    _Vista,
    _Vista_SP1,
    _Vista_SP2,
    _7,
    _7_SP1,
    _8,
    _8_1,
    _8_1_Update1,
    _10_1507,
    _10_1511,
    _10_1607,
    _10_1703,
    _10_1709,
    _10_1803,
    _10_1809,
    _10_1903,
    _10_1909,
    _10_2004,
    _10_20H2,
    _10_21H1,
    _10_21H2,
    _10_22H2,
    _11_21H2,
    _11_22H2,

    _WS_03 = _XP_64, // Windows Server 2003
    _10 = _10_1507,
    _11 = _11_21H2,

    Latest = _11_22H2
};
Q_ENUM_NS(WindowsVersion)
#endif // Q_OS_WINDOWS

enum class BlurMode : quint8
{
    Disable, // Do not enable blur behind window
    Default, // Use platform default blur mode
    Windows_Aero, // Windows only, use the traditional DWM blur
    Windows_Acrylic, // Windows only, use the Acrylic blur
    Windows_Mica, // Windows only, use the Mica material
    Windows_MicaAlt // Windows only, use the Mica Alt material
};
Q_ENUM_NS(BlurMode)

enum class WallpaperAspectStyle : quint8
{
    Fill, // Keep aspect ratio to fill, expand/crop if necessary.
    Fit, // Keep aspect ratio to fill, but don't expand/crop.
    Stretch, // Ignore aspect ratio to fill.
    Tile,
    Center,
    Span // ???
};
Q_ENUM_NS(WallpaperAspectStyle)

#ifdef Q_OS_WINDOWS
enum class RegistryRootKey : quint8
{
    ClassesRoot,
    CurrentUser,
    LocalMachine,
    Users,
    PerformanceData,
    CurrentConfig,
    DynData,
    CurrentUserLocalSettings,
    PerformanceText,
    PerformanceNlsText
};
Q_ENUM_NS(RegistryRootKey)
#endif // Q_OS_WINDOWS

enum class WindowEdge : quint8
{
    Left   = 1 << 0,
    Top    = 1 << 1,
    Right  = 1 << 2,
    Bottom = 1 << 3
};
Q_ENUM_NS(WindowEdge)
Q_DECLARE_FLAGS(WindowEdges, WindowEdge)
Q_FLAG_NS(WindowEdges)
Q_DECLARE_OPERATORS_FOR_FLAGS(WindowEdges)

#ifdef Q_OS_WINDOWS
enum class DpiAwareness : quint8
{
    Unknown,
    Unaware,
    System,
    PerMonitor,
    PerMonitorVersion2,
    Unaware_GdiScaled
};
Q_ENUM_NS(DpiAwareness)
#endif // Q_OS_WINDOWS

enum class WindowCornerStyle : quint8
{
    Default,
    Square,
    Round
};
Q_ENUM_NS(WindowCornerStyle)

struct VersionInfo
{
    struct {
        unsigned long num = 0;
        const char *str = nullptr;
    } version = {};
    struct {
        const char *hash = nullptr;
        const char *subject = nullptr;
        const char *author = nullptr;
        const char *datetime = nullptr;
        const char *branch = nullptr;
    } commit = {};
    struct {
        const char *name = nullptr;
        const char *version = nullptr;
        const char *vendor = nullptr;
    } compiler = {};
    struct {
        const char *cmake_version = nullptr;
        const char *configure_datetime = nullptr;
        const char *generator = nullptr;
        const char *architecture = nullptr;
        bool is_debug = false;
        bool is_static = false;
    } build = {};
};

struct Dpi
{
    quint32 x = 0;
    quint32 y = 0;

    [[nodiscard]] friend constexpr bool operator==(const Dpi &lhs, const Dpi &rhs) noexcept
    {
        return ((lhs.x == rhs.x) && (lhs.y == rhs.y));
    }

    [[nodiscard]] friend constexpr bool operator!=(const Dpi &lhs, const Dpi &rhs) noexcept
    {
        return !operator==(lhs, rhs);
    }

    [[nodiscard]] friend constexpr bool operator>(const Dpi &lhs, const Dpi &rhs) noexcept
    {
        return ((lhs.x * lhs.y) > (rhs.x * rhs.y));
    }

    [[nodiscard]] friend constexpr bool operator>=(const Dpi &lhs, const Dpi &rhs) noexcept
    {
        return (operator>(lhs, rhs) || operator==(lhs, rhs));
    }

    [[nodiscard]] friend constexpr bool operator<(const Dpi &lhs, const Dpi &rhs) noexcept
    {
        return (operator!=(lhs, rhs) && !operator>(lhs, rhs));
    }

    [[nodiscard]] friend constexpr bool operator<=(const Dpi &lhs, const Dpi &rhs) noexcept
    {
        return (operator<(lhs, rhs) || operator==(lhs, rhs));
    }
};

} // namespace Global

FRAMELESSHELPER_CORE_API void FramelessHelperCoreInitialize();
FRAMELESSHELPER_CORE_API void FramelessHelperCoreUninitialize();
[[nodiscard]] FRAMELESSHELPER_CORE_API Global::VersionInfo FramelessHelperVersion();
FRAMELESSHELPER_CORE_API void FramelessHelperEnableThemeAware();
FRAMELESSHELPER_CORE_API void FramelessHelperPrintLogo();

namespace FramelessHelper::Core
{
inline void initialize() { FramelessHelperCoreInitialize(); }
inline void uninitialize() { FramelessHelperCoreUninitialize(); }
[[nodiscard]] inline Global::VersionInfo version() { return FramelessHelperVersion(); }
inline void setApplicationOSThemeAware() { FramelessHelperEnableThemeAware(); }
inline void outputLogo() { FramelessHelperPrintLogo(); }
} // namespace FramelessHelper::Core

FRAMELESSHELPER_END_NAMESPACE

#ifndef QT_NO_DEBUG_STREAM
QT_BEGIN_NAMESPACE
FRAMELESSHELPER_CORE_API QDebug operator<<(QDebug, const FRAMELESSHELPER_PREPEND_NAMESPACE(Global)::VersionInfo &);
FRAMELESSHELPER_CORE_API QDebug operator<<(QDebug, const FRAMELESSHELPER_PREPEND_NAMESPACE(Global)::Dpi &);
QT_END_NAMESPACE
#endif // QT_NO_DEBUG_STREAM
