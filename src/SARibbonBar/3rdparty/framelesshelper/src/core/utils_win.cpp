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

#include "utils.h"

#ifdef Q_OS_WINDOWS

#include "framelesshelper_windows.h"
#include "framelessmanager.h"
#include "framelessmanager_p.h"
#include "framelessconfig_p.h"
#include "sysapiloader_p.h"
#include "registrykey_p.h"
#include "winverhelper_p.h"
#include "framelesshelpercore_global_p.h"
#include "versionnumber_p.h"
#include "scopeguard_p.h"
#include <optional>
#include <QtCore/qhash.h>
#include <QtCore/qloggingcategory.h>
#include <QtCore/qabstracteventdispatcher.h>
#include <QtCore/qtextstream.h>
#include <QtGui/qwindow.h>
#include <QtGui/qguiapplication.h>
#if FRAMELESSHELPER_CONFIG(private_qt)
#  include <QtCore/private/qsystemerror_p.h>
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#    include <QtGui/private/qguiapplication_p.h>
#  endif // (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#  include <QtGui/qpa/qplatformwindow.h>
#  if (QT_VERSION < QT_VERSION_CHECK(6, 2, 0))
#    include <QtGui/qpa/qplatformnativeinterface.h>
#  else // (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
#    include <QtGui/qpa/qplatformwindow_p.h>
#  endif // (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#endif
#include <d2d1.h>

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
Q_DECLARE_METATYPE(QMargins)
#endif // (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcUtilsWin, "wangwenx190.framelesshelper.core.utils.win")
#  define INFO qCInfo(lcUtilsWin)
#  define DEBUG qCDebug(lcUtilsWin)
#  define WARNING qCWarning(lcUtilsWin)
#  define CRITICAL qCCritical(lcUtilsWin)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

static constexpr const char kDpiNoAccessErrorMessage[] =
    "FramelessHelper doesn't have access to change the current process's DPI awareness mode,"
    " most likely due to it has been set externally already. Eg: application manifest file.";
static constexpr const char kQtWindowCustomMarginsVar[] = "_q_windowsCustomMargins";
FRAMELESSHELPER_STRING_CONSTANT2(SuccessMessageText, "The operation completed successfully.")
FRAMELESSHELPER_STRING_CONSTANT2(ErrorMessageTemplate, "Function %1() failed with error code %2: %3.")
FRAMELESSHELPER_STRING_CONSTANT(Composition)
FRAMELESSHELPER_STRING_CONSTANT(ColorizationColor)
FRAMELESSHELPER_STRING_CONSTANT(AppsUseLightTheme)
FRAMELESSHELPER_STRING_CONSTANT(WindowsCustomMargins)
FRAMELESSHELPER_STRING_CONSTANT(user32)
FRAMELESSHELPER_STRING_CONSTANT(dwmapi)
FRAMELESSHELPER_STRING_CONSTANT(winmm)
FRAMELESSHELPER_STRING_CONSTANT(shcore)
FRAMELESSHELPER_STRING_CONSTANT(uxtheme)
FRAMELESSHELPER_STRING_CONSTANT(GetWindowRect)
FRAMELESSHELPER_STRING_CONSTANT(DwmIsCompositionEnabled)
FRAMELESSHELPER_STRING_CONSTANT(SetWindowPos)
FRAMELESSHELPER_STRING_CONSTANT(DwmExtendFrameIntoClientArea)
FRAMELESSHELPER_STRING_CONSTANT(DwmGetColorizationColor)
FRAMELESSHELPER_STRING_CONSTANT(PostMessageW)
FRAMELESSHELPER_STRING_CONSTANT(MonitorFromWindow)
FRAMELESSHELPER_STRING_CONSTANT(GetMonitorInfoW)
FRAMELESSHELPER_STRING_CONSTANT(GetWindowPlacement)
FRAMELESSHELPER_STRING_CONSTANT(QueryPerformanceFrequency)
FRAMELESSHELPER_STRING_CONSTANT(QueryPerformanceCounter)
FRAMELESSHELPER_STRING_CONSTANT(DwmGetCompositionTimingInfo)
FRAMELESSHELPER_STRING_CONSTANT(SystemParametersInfoW)
#ifdef Q_PROCESSOR_X86_64
  FRAMELESSHELPER_STRING_CONSTANT(GetClassLongPtrW)
  FRAMELESSHELPER_STRING_CONSTANT(SetClassLongPtrW)
  FRAMELESSHELPER_STRING_CONSTANT(GetWindowLongPtrW)
  FRAMELESSHELPER_STRING_CONSTANT(SetWindowLongPtrW)
#else // !Q_PROCESSOR_X86_64
  // WinUser.h defines G/SetClassLongPtr as G/SetClassLong due to the
  // "Ptr" suffixed APIs are not available on 32-bit platforms, so we
  // have to add the following workaround. Undefine the macros and then
  // redefine them is also an option but the following solution is more simple.
  FRAMELESSHELPER_STRING_CONSTANT2(GetClassLongPtrW, "GetClassLongW")
  FRAMELESSHELPER_STRING_CONSTANT2(SetClassLongPtrW, "SetClassLongW")
  FRAMELESSHELPER_STRING_CONSTANT2(GetWindowLongPtrW, "GetWindowLongW")
  FRAMELESSHELPER_STRING_CONSTANT2(SetWindowLongPtrW, "SetWindowLongW")
#endif // Q_PROCESSOR_X86_64
FRAMELESSHELPER_STRING_CONSTANT(ReleaseCapture)
FRAMELESSHELPER_STRING_CONSTANT(SetWindowTheme)
FRAMELESSHELPER_STRING_CONSTANT(SetProcessDpiAwarenessContext)
FRAMELESSHELPER_STRING_CONSTANT(SetProcessDpiAwareness)
FRAMELESSHELPER_STRING_CONSTANT(SetProcessDPIAware)
FRAMELESSHELPER_STRING_CONSTANT(GetDpiForMonitor)
FRAMELESSHELPER_STRING_CONSTANT(GetDC)
FRAMELESSHELPER_STRING_CONSTANT(ReleaseDC)
FRAMELESSHELPER_STRING_CONSTANT(GetDeviceCaps)
FRAMELESSHELPER_STRING_CONSTANT(DwmSetWindowAttribute)
FRAMELESSHELPER_STRING_CONSTANT(EnableMenuItem)
FRAMELESSHELPER_STRING_CONSTANT(SetMenuDefaultItem)
FRAMELESSHELPER_STRING_CONSTANT(HiliteMenuItem)
FRAMELESSHELPER_STRING_CONSTANT(TrackPopupMenu)
FRAMELESSHELPER_STRING_CONSTANT(DrawMenuBar)
FRAMELESSHELPER_STRING_CONSTANT(DeleteMenu)
FRAMELESSHELPER_STRING_CONSTANT(RemoveMenu)
FRAMELESSHELPER_STRING_CONSTANT(ClientToScreen)
FRAMELESSHELPER_STRING_CONSTANT(DwmEnableBlurBehindWindow)
FRAMELESSHELPER_STRING_CONSTANT(SetWindowCompositionAttribute)
FRAMELESSHELPER_STRING_CONSTANT(GetSystemMetricsForDpi)
FRAMELESSHELPER_STRING_CONSTANT(timeGetDevCaps)
FRAMELESSHELPER_STRING_CONSTANT(timeBeginPeriod)
FRAMELESSHELPER_STRING_CONSTANT(timeEndPeriod)
FRAMELESSHELPER_STRING_CONSTANT(GetDpiForWindow)
FRAMELESSHELPER_STRING_CONSTANT(GetSystemDpiForProcess)
FRAMELESSHELPER_STRING_CONSTANT(GetDpiForSystem)
FRAMELESSHELPER_STRING_CONSTANT(DwmGetWindowAttribute)
FRAMELESSHELPER_STRING_CONSTANT(ntdll)
FRAMELESSHELPER_STRING_CONSTANT(RtlGetVersion)
FRAMELESSHELPER_STRING_CONSTANT(GetModuleHandleW)
FRAMELESSHELPER_STRING_CONSTANT(RegisterClassExW)
FRAMELESSHELPER_STRING_CONSTANT(CreateWindowExW)
FRAMELESSHELPER_STRING_CONSTANT(AccentColor)
FRAMELESSHELPER_STRING_CONSTANT(GetScaleFactorForMonitor)
FRAMELESSHELPER_STRING_CONSTANT(WallpaperStyle)
FRAMELESSHELPER_STRING_CONSTANT(TileWallpaper)
FRAMELESSHELPER_STRING_CONSTANT(UnregisterClassW)
FRAMELESSHELPER_STRING_CONSTANT(DestroyWindow)
FRAMELESSHELPER_STRING_CONSTANT(SetWindowThemeAttribute)
FRAMELESSHELPER_STRING_CONSTANT(CreateDCW)
FRAMELESSHELPER_STRING_CONSTANT(DeleteDC)
FRAMELESSHELPER_STRING_CONSTANT(d2d1)
FRAMELESSHELPER_STRING_CONSTANT(D2D1CreateFactory)
FRAMELESSHELPER_STRING_CONSTANT(ReloadSystemMetrics)
FRAMELESSHELPER_STRING_CONSTANT(SetPreferredAppMode)
FRAMELESSHELPER_STRING_CONSTANT(AllowDarkModeForApp)
FRAMELESSHELPER_STRING_CONSTANT(AllowDarkModeForWindow)
FRAMELESSHELPER_STRING_CONSTANT(FlushMenuThemes)
FRAMELESSHELPER_STRING_CONSTANT(RefreshImmersiveColorPolicyState)
FRAMELESSHELPER_STRING_CONSTANT(SetPropW)
FRAMELESSHELPER_STRING_CONSTANT(GetIsImmersiveColorUsingHighContrast)
FRAMELESSHELPER_STRING_CONSTANT(EnableNonClientDpiScaling)
FRAMELESSHELPER_STRING_CONSTANT(GetWindowDpiAwarenessContext)
FRAMELESSHELPER_STRING_CONSTANT(GetAwarenessFromDpiAwarenessContext)
FRAMELESSHELPER_STRING_CONSTANT(GetThreadDpiAwarenessContext)
FRAMELESSHELPER_STRING_CONSTANT(GetDpiAwarenessContextForProcess)
FRAMELESSHELPER_STRING_CONSTANT(GetCurrentProcess)
FRAMELESSHELPER_STRING_CONSTANT(GetProcessDpiAwareness)
FRAMELESSHELPER_STRING_CONSTANT(IsProcessDPIAware)
FRAMELESSHELPER_STRING_CONSTANT(AreDpiAwarenessContextsEqual)
FRAMELESSHELPER_STRING_CONSTANT(GetWindowDPI)
FRAMELESSHELPER_STRING_CONSTANT(AdjustWindowRectExForDpi)
FRAMELESSHELPER_STRING_CONSTANT(GetDpiMetrics)
FRAMELESSHELPER_STRING_CONSTANT(EnablePerMonitorDialogScaling)
FRAMELESSHELPER_STRING_CONSTANT(EnableChildWindowDpiMessage)
FRAMELESSHELPER_STRING_CONSTANT(GetForegroundWindow)
FRAMELESSHELPER_STRING_CONSTANT(SendMessageTimeoutW)
FRAMELESSHELPER_STRING_CONSTANT(AttachThreadInput)
FRAMELESSHELPER_STRING_CONSTANT(BringWindowToTop)
FRAMELESSHELPER_STRING_CONSTANT(SetActiveWindow)
FRAMELESSHELPER_STRING_CONSTANT(RedrawWindow)
FRAMELESSHELPER_STRING_CONSTANT(ScreenToClient)
FRAMELESSHELPER_STRING_CONSTANT(DwmFlush)
FRAMELESSHELPER_STRING_CONSTANT(GetCursorPos)
FRAMELESSHELPER_STRING_CONSTANT(DeleteObject)

struct UtilsWinExtraData : public FramelessExtraData
{
    WNDPROC qtWindowProc = nullptr;
    bool windowProcHooked = false;
    bool mica = false;

    UtilsWinExtraData();
    ~UtilsWinExtraData() override;

    [[nodiscard]] static FramelessExtraDataPtr create();
};
using UtilsWinExtraDataPtr = std::shared_ptr<UtilsWinExtraData>;

UtilsWinExtraData::UtilsWinExtraData() = default;

UtilsWinExtraData::~UtilsWinExtraData() = default;

FramelessExtraDataPtr UtilsWinExtraData::create()
{
    return std::make_shared<UtilsWinExtraData>();
}

[[nodiscard]] static inline UtilsWinExtraDataPtr tryGetExtraData(const FramelessDataPtr &data, const bool create)
{
    Q_ASSERT(data);
    if (!data) {
        return nullptr;
    }
    auto it = data->extraData.find(ExtraDataType::WindowsUtilities);
    if (it == data->extraData.end()) {
        if (create) {
            it = data->extraData.insert(ExtraDataType::WindowsUtilities, UtilsWinExtraData::create());
        } else {
            return nullptr;
        }
    }
    return std::dynamic_pointer_cast<UtilsWinExtraData>(it.value());
}

struct Win32Message
{
    UINT Code = 0;
    LPCSTR Str = nullptr;

    [[nodiscard]] friend inline constexpr bool operator==(const Win32Message &lhs, const Win32Message &rhs) noexcept
    {
        return (lhs.Code == rhs.Code);
    }

    [[nodiscard]] friend inline constexpr bool operator!=(const Win32Message &lhs, const Win32Message &rhs) noexcept
    {
        return !operator==(lhs, rhs);
    }

    [[nodiscard]] friend inline constexpr bool operator>(const Win32Message &lhs, const Win32Message &rhs) noexcept
    {
        return (lhs.Code > rhs.Code);
    }

    [[nodiscard]] friend inline constexpr bool operator>=(const Win32Message &lhs, const Win32Message &rhs) noexcept
    {
        return (operator>(lhs, rhs) || operator==(lhs, rhs));
    }

    [[nodiscard]] friend inline constexpr bool operator<(const Win32Message &lhs, const Win32Message &rhs) noexcept
    {
        return (operator!=(lhs, rhs) && !operator>(lhs, rhs));
    }

    [[nodiscard]] friend inline constexpr bool operator<=(const Win32Message &lhs, const Win32Message &rhs) noexcept
    {
        return (operator<(lhs, rhs) || operator==(lhs, rhs));
    }
};

#define DEFINE_WIN32_MESSAGE(Message) Win32Message{ Message, #Message },
static constexpr const std::array<Win32Message, 333> g_win32MessageMap =
{
    DEFINE_WIN32_MESSAGE(WM_NULL)
    DEFINE_WIN32_MESSAGE(WM_CREATE)
    DEFINE_WIN32_MESSAGE(WM_DESTROY)
    DEFINE_WIN32_MESSAGE(WM_MOVE)
    DEFINE_WIN32_MESSAGE(WM_SIZE)
    DEFINE_WIN32_MESSAGE(WM_ACTIVATE)
    DEFINE_WIN32_MESSAGE(WM_SETFOCUS)
    DEFINE_WIN32_MESSAGE(WM_KILLFOCUS)
    DEFINE_WIN32_MESSAGE(WM_ENABLE)
    DEFINE_WIN32_MESSAGE(WM_SETREDRAW)
    DEFINE_WIN32_MESSAGE(WM_SETTEXT)
    DEFINE_WIN32_MESSAGE(WM_GETTEXT)
    DEFINE_WIN32_MESSAGE(WM_GETTEXTLENGTH)
    DEFINE_WIN32_MESSAGE(WM_PAINT)
    DEFINE_WIN32_MESSAGE(WM_CLOSE)
    DEFINE_WIN32_MESSAGE(WM_QUERYENDSESSION)
    DEFINE_WIN32_MESSAGE(WM_QUERYOPEN)
    DEFINE_WIN32_MESSAGE(WM_ENDSESSION)
    DEFINE_WIN32_MESSAGE(WM_QUIT)
    DEFINE_WIN32_MESSAGE(WM_ERASEBKGND)
    DEFINE_WIN32_MESSAGE(WM_SYSCOLORCHANGE)
    DEFINE_WIN32_MESSAGE(WM_SHOWWINDOW)
    DEFINE_WIN32_MESSAGE(WM_SETTINGCHANGE) // WM_WININICHANGE
    DEFINE_WIN32_MESSAGE(WM_DEVMODECHANGE)
    DEFINE_WIN32_MESSAGE(WM_ACTIVATEAPP)
    DEFINE_WIN32_MESSAGE(WM_FONTCHANGE)
    DEFINE_WIN32_MESSAGE(WM_TIMECHANGE)
    DEFINE_WIN32_MESSAGE(WM_CANCELMODE)
    DEFINE_WIN32_MESSAGE(WM_SETCURSOR)
    DEFINE_WIN32_MESSAGE(WM_MOUSEACTIVATE)
    DEFINE_WIN32_MESSAGE(WM_CHILDACTIVATE)
    DEFINE_WIN32_MESSAGE(WM_QUEUESYNC)
    DEFINE_WIN32_MESSAGE(WM_GETMINMAXINFO)
    DEFINE_WIN32_MESSAGE(WM_PAINTICON)
    DEFINE_WIN32_MESSAGE(WM_ICONERASEBKGND)
    DEFINE_WIN32_MESSAGE(WM_NEXTDLGCTL)
    DEFINE_WIN32_MESSAGE(WM_SPOOLERSTATUS)
    DEFINE_WIN32_MESSAGE(WM_DRAWITEM)
    DEFINE_WIN32_MESSAGE(WM_MEASUREITEM)
    DEFINE_WIN32_MESSAGE(WM_DELETEITEM)
    DEFINE_WIN32_MESSAGE(WM_VKEYTOITEM)
    DEFINE_WIN32_MESSAGE(WM_CHARTOITEM)
    DEFINE_WIN32_MESSAGE(WM_SETFONT)
    DEFINE_WIN32_MESSAGE(WM_GETFONT)
    DEFINE_WIN32_MESSAGE(WM_SETHOTKEY)
    DEFINE_WIN32_MESSAGE(WM_GETHOTKEY)
    DEFINE_WIN32_MESSAGE(WM_QUERYDRAGICON)
    DEFINE_WIN32_MESSAGE(WM_COMPAREITEM)
    DEFINE_WIN32_MESSAGE(WM_GETOBJECT)
    DEFINE_WIN32_MESSAGE(WM_COMPACTING)
    DEFINE_WIN32_MESSAGE(WM_COMMNOTIFY)
    DEFINE_WIN32_MESSAGE(WM_WINDOWPOSCHANGING)
    DEFINE_WIN32_MESSAGE(WM_WINDOWPOSCHANGED)
    DEFINE_WIN32_MESSAGE(WM_POWER)
    DEFINE_WIN32_MESSAGE(WM_COPYDATA)
    DEFINE_WIN32_MESSAGE(WM_CANCELJOURNAL)
    DEFINE_WIN32_MESSAGE(WM_NOTIFY)
    DEFINE_WIN32_MESSAGE(WM_INPUTLANGCHANGEREQUEST)
    DEFINE_WIN32_MESSAGE(WM_INPUTLANGCHANGE)
    DEFINE_WIN32_MESSAGE(WM_TCARD)
    DEFINE_WIN32_MESSAGE(WM_HELP)
    DEFINE_WIN32_MESSAGE(WM_USERCHANGED)
    DEFINE_WIN32_MESSAGE(WM_NOTIFYFORMAT)
    DEFINE_WIN32_MESSAGE(WM_CONTEXTMENU)
    DEFINE_WIN32_MESSAGE(WM_STYLECHANGING)
    DEFINE_WIN32_MESSAGE(WM_STYLECHANGED)
    DEFINE_WIN32_MESSAGE(WM_DISPLAYCHANGE)
    DEFINE_WIN32_MESSAGE(WM_GETICON)
    DEFINE_WIN32_MESSAGE(WM_SETICON)
    DEFINE_WIN32_MESSAGE(WM_NCCREATE)
    DEFINE_WIN32_MESSAGE(WM_NCDESTROY)
    DEFINE_WIN32_MESSAGE(WM_NCCALCSIZE)
    DEFINE_WIN32_MESSAGE(WM_NCHITTEST)
    DEFINE_WIN32_MESSAGE(WM_NCPAINT)
    DEFINE_WIN32_MESSAGE(WM_NCACTIVATE)
    DEFINE_WIN32_MESSAGE(WM_GETDLGCODE)
    DEFINE_WIN32_MESSAGE(WM_SYNCPAINT)
    DEFINE_WIN32_MESSAGE(WM_NCMOUSEMOVE)
    DEFINE_WIN32_MESSAGE(WM_NCLBUTTONDOWN)
    DEFINE_WIN32_MESSAGE(WM_NCLBUTTONUP)
    DEFINE_WIN32_MESSAGE(WM_NCLBUTTONDBLCLK)
    DEFINE_WIN32_MESSAGE(WM_NCRBUTTONDOWN)
    DEFINE_WIN32_MESSAGE(WM_NCRBUTTONUP)
    DEFINE_WIN32_MESSAGE(WM_NCRBUTTONDBLCLK)
    DEFINE_WIN32_MESSAGE(WM_NCMBUTTONDOWN)
    DEFINE_WIN32_MESSAGE(WM_NCMBUTTONUP)
    DEFINE_WIN32_MESSAGE(WM_NCMBUTTONDBLCLK)
    DEFINE_WIN32_MESSAGE(WM_NCXBUTTONDOWN)
    DEFINE_WIN32_MESSAGE(WM_NCXBUTTONUP)
    DEFINE_WIN32_MESSAGE(WM_NCXBUTTONDBLCLK)
    DEFINE_WIN32_MESSAGE(WM_INPUT_DEVICE_CHANGE)
    DEFINE_WIN32_MESSAGE(WM_INPUT)
    DEFINE_WIN32_MESSAGE(WM_KEYDOWN) // WM_KEYFIRST
    DEFINE_WIN32_MESSAGE(WM_KEYUP)
    DEFINE_WIN32_MESSAGE(WM_CHAR)
    DEFINE_WIN32_MESSAGE(WM_DEADCHAR)
    DEFINE_WIN32_MESSAGE(WM_SYSKEYDOWN)
    DEFINE_WIN32_MESSAGE(WM_SYSKEYUP)
    DEFINE_WIN32_MESSAGE(WM_SYSCHAR)
    DEFINE_WIN32_MESSAGE(WM_SYSDEADCHAR)
    DEFINE_WIN32_MESSAGE(WM_UNICHAR) // WM_KEYLAST
    DEFINE_WIN32_MESSAGE(WM_IME_STARTCOMPOSITION)
    DEFINE_WIN32_MESSAGE(WM_IME_ENDCOMPOSITION)
    DEFINE_WIN32_MESSAGE(WM_IME_COMPOSITION) // WM_IME_KEYLAST
    DEFINE_WIN32_MESSAGE(WM_INITDIALOG)
    DEFINE_WIN32_MESSAGE(WM_COMMAND)
    DEFINE_WIN32_MESSAGE(WM_SYSCOMMAND)
    DEFINE_WIN32_MESSAGE(WM_TIMER)
    DEFINE_WIN32_MESSAGE(WM_HSCROLL)
    DEFINE_WIN32_MESSAGE(WM_VSCROLL)
    DEFINE_WIN32_MESSAGE(WM_INITMENU)
    DEFINE_WIN32_MESSAGE(WM_INITMENUPOPUP)
#if (WINVER >= _WIN32_WINNT_WIN7)
    DEFINE_WIN32_MESSAGE(WM_GESTURE)
    DEFINE_WIN32_MESSAGE(WM_GESTURENOTIFY)
#endif
    DEFINE_WIN32_MESSAGE(WM_MENUSELECT)
    DEFINE_WIN32_MESSAGE(WM_MENUCHAR)
    DEFINE_WIN32_MESSAGE(WM_ENTERIDLE)
    DEFINE_WIN32_MESSAGE(WM_MENURBUTTONUP)
    DEFINE_WIN32_MESSAGE(WM_MENUDRAG)
    DEFINE_WIN32_MESSAGE(WM_MENUGETOBJECT)
    DEFINE_WIN32_MESSAGE(WM_UNINITMENUPOPUP)
    DEFINE_WIN32_MESSAGE(WM_MENUCOMMAND)
    DEFINE_WIN32_MESSAGE(WM_CHANGEUISTATE)
    DEFINE_WIN32_MESSAGE(WM_UPDATEUISTATE)
    DEFINE_WIN32_MESSAGE(WM_QUERYUISTATE)
    DEFINE_WIN32_MESSAGE(WM_CTLCOLORMSGBOX)
    DEFINE_WIN32_MESSAGE(WM_CTLCOLOREDIT)
    DEFINE_WIN32_MESSAGE(WM_CTLCOLORLISTBOX)
    DEFINE_WIN32_MESSAGE(WM_CTLCOLORBTN)
    DEFINE_WIN32_MESSAGE(WM_CTLCOLORDLG)
    DEFINE_WIN32_MESSAGE(WM_CTLCOLORSCROLLBAR)
    DEFINE_WIN32_MESSAGE(WM_CTLCOLORSTATIC)
    DEFINE_WIN32_MESSAGE(WM_MOUSEMOVE) // WM_MOUSEFIRST
    DEFINE_WIN32_MESSAGE(WM_LBUTTONDOWN)
    DEFINE_WIN32_MESSAGE(WM_LBUTTONUP)
    DEFINE_WIN32_MESSAGE(WM_LBUTTONDBLCLK)
    DEFINE_WIN32_MESSAGE(WM_RBUTTONDOWN)
    DEFINE_WIN32_MESSAGE(WM_RBUTTONUP)
    DEFINE_WIN32_MESSAGE(WM_RBUTTONDBLCLK)
    DEFINE_WIN32_MESSAGE(WM_MBUTTONDOWN)
    DEFINE_WIN32_MESSAGE(WM_MBUTTONUP)
    DEFINE_WIN32_MESSAGE(WM_MBUTTONDBLCLK)
    DEFINE_WIN32_MESSAGE(WM_MOUSEWHEEL)
    DEFINE_WIN32_MESSAGE(WM_XBUTTONDOWN)
    DEFINE_WIN32_MESSAGE(WM_XBUTTONUP)
    DEFINE_WIN32_MESSAGE(WM_XBUTTONDBLCLK)
    DEFINE_WIN32_MESSAGE(WM_MOUSEHWHEEL) // WM_MOUSELAST
    DEFINE_WIN32_MESSAGE(WM_PARENTNOTIFY)
    DEFINE_WIN32_MESSAGE(WM_ENTERMENULOOP)
    DEFINE_WIN32_MESSAGE(WM_EXITMENULOOP)
    DEFINE_WIN32_MESSAGE(WM_NEXTMENU)
    DEFINE_WIN32_MESSAGE(WM_SIZING)
    DEFINE_WIN32_MESSAGE(WM_CAPTURECHANGED)
    DEFINE_WIN32_MESSAGE(WM_MOVING)
    DEFINE_WIN32_MESSAGE(WM_POWERBROADCAST)
    DEFINE_WIN32_MESSAGE(WM_DEVICECHANGE)
    DEFINE_WIN32_MESSAGE(WM_MDICREATE)
    DEFINE_WIN32_MESSAGE(WM_MDIDESTROY)
    DEFINE_WIN32_MESSAGE(WM_MDIACTIVATE)
    DEFINE_WIN32_MESSAGE(WM_MDIRESTORE)
    DEFINE_WIN32_MESSAGE(WM_MDINEXT)
    DEFINE_WIN32_MESSAGE(WM_MDIMAXIMIZE)
    DEFINE_WIN32_MESSAGE(WM_MDITILE)
    DEFINE_WIN32_MESSAGE(WM_MDICASCADE)
    DEFINE_WIN32_MESSAGE(WM_MDIICONARRANGE)
    DEFINE_WIN32_MESSAGE(WM_MDIGETACTIVE)
    DEFINE_WIN32_MESSAGE(WM_MDISETMENU)
    DEFINE_WIN32_MESSAGE(WM_ENTERSIZEMOVE)
    DEFINE_WIN32_MESSAGE(WM_EXITSIZEMOVE)
    DEFINE_WIN32_MESSAGE(WM_DROPFILES)
    DEFINE_WIN32_MESSAGE(WM_MDIREFRESHMENU)
#if (WINVER >= _WIN32_WINNT_WIN8)
    DEFINE_WIN32_MESSAGE(WM_POINTERDEVICECHANGE)
    DEFINE_WIN32_MESSAGE(WM_POINTERDEVICEINRANGE)
    DEFINE_WIN32_MESSAGE(WM_POINTERDEVICEOUTOFRANGE)
#endif
#if (WINVER >= _WIN32_WINNT_WIN7)
    DEFINE_WIN32_MESSAGE(WM_TOUCH)
#endif
#if (WINVER >= _WIN32_WINNT_WIN8)
    DEFINE_WIN32_MESSAGE(WM_NCPOINTERUPDATE)
    DEFINE_WIN32_MESSAGE(WM_NCPOINTERDOWN)
    DEFINE_WIN32_MESSAGE(WM_NCPOINTERUP)
    DEFINE_WIN32_MESSAGE(WM_POINTERUPDATE)
    DEFINE_WIN32_MESSAGE(WM_POINTERDOWN)
    DEFINE_WIN32_MESSAGE(WM_POINTERUP)
    DEFINE_WIN32_MESSAGE(WM_POINTERENTER)
    DEFINE_WIN32_MESSAGE(WM_POINTERLEAVE)
    DEFINE_WIN32_MESSAGE(WM_POINTERACTIVATE)
    DEFINE_WIN32_MESSAGE(WM_POINTERCAPTURECHANGED)
    DEFINE_WIN32_MESSAGE(WM_TOUCHHITTESTING)
    DEFINE_WIN32_MESSAGE(WM_POINTERWHEEL)
    DEFINE_WIN32_MESSAGE(WM_POINTERHWHEEL)
#endif
#ifdef WM_POINTERROUTEDTO
    DEFINE_WIN32_MESSAGE(WM_POINTERROUTEDTO)
#endif
#ifdef WM_POINTERROUTEDAWAY
    DEFINE_WIN32_MESSAGE(WM_POINTERROUTEDAWAY)
#endif
#ifdef WM_POINTERROUTEDRELEASED
    DEFINE_WIN32_MESSAGE(WM_POINTERROUTEDRELEASED)
#endif
    DEFINE_WIN32_MESSAGE(WM_IME_SETCONTEXT)
    DEFINE_WIN32_MESSAGE(WM_IME_NOTIFY)
    DEFINE_WIN32_MESSAGE(WM_IME_CONTROL)
    DEFINE_WIN32_MESSAGE(WM_IME_COMPOSITIONFULL)
    DEFINE_WIN32_MESSAGE(WM_IME_SELECT)
    DEFINE_WIN32_MESSAGE(WM_IME_CHAR)
    DEFINE_WIN32_MESSAGE(WM_IME_REQUEST)
    DEFINE_WIN32_MESSAGE(WM_IME_KEYDOWN)
    DEFINE_WIN32_MESSAGE(WM_IME_KEYUP)
    DEFINE_WIN32_MESSAGE(WM_MOUSEHOVER)
    DEFINE_WIN32_MESSAGE(WM_MOUSELEAVE)
    DEFINE_WIN32_MESSAGE(WM_NCMOUSEHOVER)
    DEFINE_WIN32_MESSAGE(WM_NCMOUSELEAVE)
    DEFINE_WIN32_MESSAGE(WM_WTSSESSION_CHANGE)
    DEFINE_WIN32_MESSAGE(WM_TABLET_FIRST)
    DEFINE_WIN32_MESSAGE(WM_TABLET_LAST)
    DEFINE_WIN32_MESSAGE(WM_DPICHANGED)
#ifdef WM_DPICHANGED_BEFOREPARENT
    DEFINE_WIN32_MESSAGE(WM_DPICHANGED_BEFOREPARENT)
#endif
#ifdef WM_DPICHANGED_AFTERPARENT
    DEFINE_WIN32_MESSAGE(WM_DPICHANGED_AFTERPARENT)
#endif
    DEFINE_WIN32_MESSAGE(WM_GETDPISCALEDSIZE)
    DEFINE_WIN32_MESSAGE(WM_CUT)
    DEFINE_WIN32_MESSAGE(WM_COPY)
    DEFINE_WIN32_MESSAGE(WM_PASTE)
    DEFINE_WIN32_MESSAGE(WM_CLEAR)
    DEFINE_WIN32_MESSAGE(WM_UNDO)
    DEFINE_WIN32_MESSAGE(WM_RENDERFORMAT)
    DEFINE_WIN32_MESSAGE(WM_RENDERALLFORMATS)
    DEFINE_WIN32_MESSAGE(WM_DESTROYCLIPBOARD)
    DEFINE_WIN32_MESSAGE(WM_DRAWCLIPBOARD)
    DEFINE_WIN32_MESSAGE(WM_PAINTCLIPBOARD)
    DEFINE_WIN32_MESSAGE(WM_VSCROLLCLIPBOARD)
    DEFINE_WIN32_MESSAGE(WM_SIZECLIPBOARD)
    DEFINE_WIN32_MESSAGE(WM_ASKCBFORMATNAME)
    DEFINE_WIN32_MESSAGE(WM_CHANGECBCHAIN)
    DEFINE_WIN32_MESSAGE(WM_HSCROLLCLIPBOARD)
    DEFINE_WIN32_MESSAGE(WM_QUERYNEWPALETTE)
    DEFINE_WIN32_MESSAGE(WM_PALETTEISCHANGING)
    DEFINE_WIN32_MESSAGE(WM_PALETTECHANGED)
    DEFINE_WIN32_MESSAGE(WM_HOTKEY)
    DEFINE_WIN32_MESSAGE(WM_PRINT)
    DEFINE_WIN32_MESSAGE(WM_PRINTCLIENT)
    DEFINE_WIN32_MESSAGE(WM_APPCOMMAND)
    DEFINE_WIN32_MESSAGE(WM_THEMECHANGED)
    DEFINE_WIN32_MESSAGE(WM_CLIPBOARDUPDATE)
    DEFINE_WIN32_MESSAGE(WM_DWMCOMPOSITIONCHANGED)
#ifdef WM_DWMNCRENDERINGCHANGED
    DEFINE_WIN32_MESSAGE(WM_DWMNCRENDERINGCHANGED)
#endif
    DEFINE_WIN32_MESSAGE(WM_DWMCOLORIZATIONCOLORCHANGED)
#ifdef WM_DWMWINDOWMAXIMIZEDCHANGE
    DEFINE_WIN32_MESSAGE(WM_DWMWINDOWMAXIMIZEDCHANGE)
#endif
#ifdef WM_DWMSENDICONICTHUMBNAIL
    DEFINE_WIN32_MESSAGE(WM_DWMSENDICONICTHUMBNAIL)
#endif
#ifdef WM_DWMSENDICONICLIVEPREVIEWBITMAP
    DEFINE_WIN32_MESSAGE(WM_DWMSENDICONICLIVEPREVIEWBITMAP)
#endif
#ifdef WM_GETTITLEBARINFOEX
    DEFINE_WIN32_MESSAGE(WM_GETTITLEBARINFOEX)
#endif
    DEFINE_WIN32_MESSAGE(WM_HANDHELDFIRST)
    DEFINE_WIN32_MESSAGE(WM_HANDHELDLAST)
    DEFINE_WIN32_MESSAGE(WM_AFXFIRST)
    DEFINE_WIN32_MESSAGE(WM_AFXLAST)
    DEFINE_WIN32_MESSAGE(WM_PENWINFIRST)
    DEFINE_WIN32_MESSAGE(WM_PENWINLAST)
    DEFINE_WIN32_MESSAGE(WM_APP)
    DEFINE_WIN32_MESSAGE(WM_USER)
    // Undocumented messages:
    DEFINE_WIN32_MESSAGE(WM_SIZEWAIT)
    DEFINE_WIN32_MESSAGE(WM_SETVISIBLE)
    DEFINE_WIN32_MESSAGE(WM_SYSTEMERROR)
    DEFINE_WIN32_MESSAGE(WM_CTLCOLOR)
    DEFINE_WIN32_MESSAGE(WM_LOGOFF)
    DEFINE_WIN32_MESSAGE(WM_ALTTABACTIVE)
    DEFINE_WIN32_MESSAGE(WM_SHELLNOTIFY)
    DEFINE_WIN32_MESSAGE(WM_ISACTIVEICON)
    DEFINE_WIN32_MESSAGE(WM_QUERYPARKICON)
    DEFINE_WIN32_MESSAGE(WM_WINHELP)
    DEFINE_WIN32_MESSAGE(WM_FULLSCREEN)
    DEFINE_WIN32_MESSAGE(WM_CLIENTSHUTDOWN)
    DEFINE_WIN32_MESSAGE(WM_DDEMLEVENT)
    DEFINE_WIN32_MESSAGE(WM_TESTING)
    DEFINE_WIN32_MESSAGE(WM_OTHERWINDOWCREATED)
    DEFINE_WIN32_MESSAGE(WM_OTHERWINDOWDESTROYED)
    DEFINE_WIN32_MESSAGE(WM_COPYGLOBALDATA)
    DEFINE_WIN32_MESSAGE(WM_KEYF1)
    DEFINE_WIN32_MESSAGE(WM_ACCESS_WINDOW)
    DEFINE_WIN32_MESSAGE(WM_FINALDESTROY)
    DEFINE_WIN32_MESSAGE(WM_MEASUREITEM_CLIENTDATA)
    DEFINE_WIN32_MESSAGE(WM_SYNCTASK)
    DEFINE_WIN32_MESSAGE(WM_KLUDGEMINRECT)
    DEFINE_WIN32_MESSAGE(WM_LPKDRAWSWITCHWND)
    DEFINE_WIN32_MESSAGE(WM_UAHDESTROYWINDOW)
    DEFINE_WIN32_MESSAGE(WM_UAHDRAWMENU)
    DEFINE_WIN32_MESSAGE(WM_UAHDRAWMENUITEM)
    DEFINE_WIN32_MESSAGE(WM_UAHINITMENU)
    DEFINE_WIN32_MESSAGE(WM_UAHMEASUREMENUITEM)
    DEFINE_WIN32_MESSAGE(WM_UAHNCPAINTMENUPOPUP)
    DEFINE_WIN32_MESSAGE(WM_UAHUPDATE)
    DEFINE_WIN32_MESSAGE(WM_NCUAHDRAWCAPTION)
    DEFINE_WIN32_MESSAGE(WM_NCUAHDRAWFRAME)
    DEFINE_WIN32_MESSAGE(WM_YOMICHAR)
    DEFINE_WIN32_MESSAGE(WM_CONVERTREQUEST)
    DEFINE_WIN32_MESSAGE(WM_CONVERTRESULT)
    DEFINE_WIN32_MESSAGE(WM_INTERIM)
    DEFINE_WIN32_MESSAGE(WM_SYSTIMER)
    DEFINE_WIN32_MESSAGE(WM_GESTUREINPUT)
    DEFINE_WIN32_MESSAGE(WM_GESTURENOTIFIED)
    DEFINE_WIN32_MESSAGE(WM_LBTRACKPOINT)
    DEFINE_WIN32_MESSAGE(WM_DROPOBJECT)
    DEFINE_WIN32_MESSAGE(WM_QUERYDROPOBJECT)
    DEFINE_WIN32_MESSAGE(WM_BEGINDRAG)
    DEFINE_WIN32_MESSAGE(WM_DRAGLOOP)
    DEFINE_WIN32_MESSAGE(WM_DRAGSELECT)
    DEFINE_WIN32_MESSAGE(WM_DRAGMOVE)
    DEFINE_WIN32_MESSAGE(WM_STOPINERTIA)
    DEFINE_WIN32_MESSAGE(WM_ENDINERTIA)
    DEFINE_WIN32_MESSAGE(WM_EDGYINERTIA)
    DEFINE_WIN32_MESSAGE(WM_VISIBILITYCHANGED)
    DEFINE_WIN32_MESSAGE(WM_VIEWSTATECHANGED)
    DEFINE_WIN32_MESSAGE(WM_UNREGISTER_WINDOW_SERVICES)
    DEFINE_WIN32_MESSAGE(WM_CONSOLIDATED)
    DEFINE_WIN32_MESSAGE(WM_IME_REPORT)
    DEFINE_WIN32_MESSAGE(WM_IME_SYSTEM)
    DEFINE_WIN32_MESSAGE(WM_POINTERDEVICEADDED)
    DEFINE_WIN32_MESSAGE(WM_POINTERDEVICEDELETED)
    DEFINE_WIN32_MESSAGE(WM_FLICK)
    DEFINE_WIN32_MESSAGE(WM_FLICKINTERNAL)
    DEFINE_WIN32_MESSAGE(WM_BRIGHTNESSCHANGED)
    DEFINE_WIN32_MESSAGE(WM_SYSMENU)
    DEFINE_WIN32_MESSAGE(WM_HOOKMSG)
    DEFINE_WIN32_MESSAGE(WM_EXITPROCESS)
    DEFINE_WIN32_MESSAGE(WM_WAKETHREAD)
    DEFINE_WIN32_MESSAGE(WM_UAHINIT)
    DEFINE_WIN32_MESSAGE(WM_DESKTOPNOTIFY)
    DEFINE_WIN32_MESSAGE(WM_DWMEXILEFRAME)
    DEFINE_WIN32_MESSAGE(WM_MAGNIFICATION_STARTED)
    DEFINE_WIN32_MESSAGE(WM_MAGNIFICATION_ENDED)
    DEFINE_WIN32_MESSAGE(WM_DWMTHUMBNAILSIZECHANGED)
    DEFINE_WIN32_MESSAGE(WM_MAGNIFICATION_OUTPUT)
    DEFINE_WIN32_MESSAGE(WM_BSDRDATA)
    DEFINE_WIN32_MESSAGE(WM_DWMTRANSITIONSTATECHANGED)
    DEFINE_WIN32_MESSAGE(WM_KEYBOARDCORRECTIONCALLOUT)
    DEFINE_WIN32_MESSAGE(WM_KEYBOARDCORRECTIONACTION)
    DEFINE_WIN32_MESSAGE(WM_UIACTION)
    DEFINE_WIN32_MESSAGE(WM_ROUTED_UI_EVENT)
    DEFINE_WIN32_MESSAGE(WM_MEASURECONTROL)
    DEFINE_WIN32_MESSAGE(WM_GETACTIONTEXT)
    DEFINE_WIN32_MESSAGE(WM_FORWARDKEYDOWN)
    DEFINE_WIN32_MESSAGE(WM_FORWARDKEYUP)
    DEFINE_WIN32_MESSAGE(WM_NOTIFYWOW)
};
#undef DEFINE_WIN32_MESSAGE

[[nodiscard]] std::optional<MONITORINFOEXW> getMonitorForWindow(const HWND hwnd)
{
    Q_ASSERT(hwnd);
    if (!hwnd) {
        return std::nullopt;
    }
    // Use "MONITOR_DEFAULTTONEAREST" here so that we can still get the correct
    // monitor even if the window is minimized.
    const HMONITOR monitor = ::MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST);
    if (!monitor) {
        WARNING << Utils::getSystemErrorMessage(kMonitorFromWindow);
        return std::nullopt;
    }
    MONITORINFOEXW monitorInfo;
    SecureZeroMemory(&monitorInfo, sizeof(monitorInfo));
    monitorInfo.cbSize = sizeof(monitorInfo);
    if (::GetMonitorInfoW(monitor, &monitorInfo) == FALSE) {
        WARNING << Utils::getSystemErrorMessage(kGetMonitorInfoW);
        return std::nullopt;
    }
    return monitorInfo;
};

[[maybe_unused]] [[nodiscard]] static inline QString qtWindowCustomMarginsProp()
{
    static const QString prop = QString::fromUtf8(kQtWindowCustomMarginsVar);
    return prop;
}

[[nodiscard]] static inline QString dwmRegistryKey()
{
    static const QString key = QString::fromWCharArray(kDwmRegistryKey);
    return key;
}

[[nodiscard]] static inline QString personalizeRegistryKey()
{
    static const QString key = QString::fromWCharArray(kPersonalizeRegistryKey);
    return key;
}

[[nodiscard]] static inline QString desktopRegistryKey()
{
    static const QString key = QString::fromWCharArray(kDesktopRegistryKey);
    return key;
}

[[nodiscard]] static inline QString dwmColorKeyName()
{
    static const QString name = QString::fromWCharArray(kDwmColorKeyName);
    return name;
}

[[nodiscard]] static inline bool doCompareWindowsVersion(const VersionNumber &targetOsVer)
{
    static const auto currentOsVer = []() -> std::optional<VersionNumber> {
        if (API_NT_AVAILABLE(RtlGetVersion)) {
            using RtlGetVersionPtr = _NTSTATUS(WINAPI *)(PRTL_OSVERSIONINFOW);
            const auto pRtlGetVersion = reinterpret_cast<RtlGetVersionPtr>(SysApiLoader::instance()->get(kntdll, kRtlGetVersion));
            RTL_OSVERSIONINFOEXW osvi;
            SecureZeroMemory(&osvi, sizeof(osvi));
            osvi.dwOSVersionInfoSize = sizeof(osvi);
            if (pRtlGetVersion(reinterpret_cast<PRTL_OSVERSIONINFOW>(&osvi)) == _STATUS_SUCCESS) {
                return VersionNumber{ osvi.dwMajorVersion, osvi.dwMinorVersion, osvi.dwBuildNumber };
            }
        }
        return std::nullopt;
    }();
    if (currentOsVer.has_value()) {
        return (currentOsVer >= targetOsVer);
    }
    // We can fallback to "VerifyVersionInfoW" if we can't determine the current system
    // version, but this function will be affected by the manifest file of your application.
    // For example, if you don't claim your application supports Windows 10 explicitly
    // in the manifest file, Windows will assume your application only supports up to Windows
    // 8.1, so this function will be told the current system is at most Windows 8.1, to keep
    // good backward-compatiability. This behavior usually won't cause any issues if you
    // always use an appropriate manifest file for your application, however, it does cause
    // some issues for people who don't use the manifest file at all. There have been some
    // bug reports about it already.
    OSVERSIONINFOEXW osvi;
    SecureZeroMemory(&osvi, sizeof(osvi));
    osvi.dwOSVersionInfoSize = sizeof(osvi);
    osvi.dwMajorVersion = targetOsVer.Major;
    osvi.dwMinorVersion = targetOsVer.Minor;
    osvi.dwBuildNumber = targetOsVer.Patch;
    DWORDLONG dwlConditionMask = 0;
    static constexpr const auto op = VER_GREATER_EQUAL;
    VER_SET_CONDITION(dwlConditionMask, VER_MAJORVERSION, op);
    VER_SET_CONDITION(dwlConditionMask, VER_MINORVERSION, op);
    VER_SET_CONDITION(dwlConditionMask, VER_BUILDNUMBER, op);
    return (::VerifyVersionInfoW(&osvi, (VER_MAJORVERSION | VER_MINORVERSION | VER_BUILDNUMBER), dwlConditionMask) != FALSE);
}

[[nodiscard]] static inline QString getSystemErrorMessageImpl(const QString &function, const DWORD code)
{
    Q_ASSERT(!function.isEmpty());
    if (function.isEmpty()) {
        return {};
    }
    if (code == ERROR_SUCCESS) {
        return kSuccessMessageText;
    }
#if FRAMELESSHELPER_CONFIG(private_qt)
    const QString errorText = QSystemError::windowsString(code);
    return kErrorMessageTemplate.arg(function, QString::number(code), errorText);
#else // !FRAMELESSHELPER_CONFIG(private_qt)
    LPWSTR buf = nullptr;
    if (::FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPWSTR>(&buf), 0, nullptr) == 0) {
        return FRAMELESSHELPER_STRING_LITERAL("FormatMessageW() returned empty string.");
    }
    const QString errorText = QString::fromWCharArray(buf).trimmed();
    ::LocalFree(buf);
    buf = nullptr;
    return kErrorMessageTemplate.arg(function, QString::number(code), errorText);
#endif // FRAMELESSHELPER_CONFIG(private_qt)
}

[[nodiscard]] static inline QString getSystemErrorMessageImpl(const QString &function, const HRESULT hr)
{
    Q_ASSERT(!function.isEmpty());
    if (function.isEmpty()) {
        return {};
    }
    if (SUCCEEDED(hr)) {
        return kSuccessMessageText;
    }
    const DWORD dwError = HRESULT_CODE(hr);
    return getSystemErrorMessageImpl(function, dwError);
}

[[nodiscard]] static inline bool moveWindowToMonitor(const HWND hwnd, const MONITORINFOEXW &activeMonitor)
{
    Q_ASSERT(hwnd);
    if (!hwnd) {
        return false;
    }
    const std::optional<MONITORINFOEXW> currentMonitor = getMonitorForWindow(hwnd);
    if (!currentMonitor.has_value()) {
        WARNING << "Failed to retrieve the window's monitor.";
        return false;
    }
    const RECT currentMonitorRect = currentMonitor.value().rcMonitor;
    const RECT activeMonitorRect = activeMonitor.rcMonitor;
    // We are in the same monitor, nothing to adjust here.
    if (currentMonitorRect == activeMonitorRect) {
        return true;
    }
    RECT currentWindowRect = {};
    if (::GetWindowRect(hwnd, &currentWindowRect) == FALSE) {
        WARNING << Utils::getSystemErrorMessage(kGetWindowRect);
        return false;
    }
    const int currentWindowWidth = (currentWindowRect.right - currentWindowRect.left);
    const int currentWindowHeight = (currentWindowRect.bottom - currentWindowRect.top);
    const int currentWindowOffsetX = (currentWindowRect.left - currentMonitorRect.left);
    const int currentWindowOffsetY = (currentWindowRect.top - currentMonitorRect.top);
    const int newWindowX = (activeMonitorRect.left + currentWindowOffsetX);
    const int newWindowY = (activeMonitorRect.top + currentWindowOffsetY);
    static constexpr const UINT flags =
        (SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOOWNERZORDER);
    if (::SetWindowPos(hwnd, nullptr, newWindowX, newWindowY, currentWindowWidth, currentWindowHeight, flags) == FALSE) {
        WARNING << Utils::getSystemErrorMessage(kSetWindowPos);
        return false;
    }
    return true;
}

[[nodiscard]] static inline int getSystemMetrics2(const int index, const bool horizontal, const quint32 dpi)
{
    Q_ASSERT(dpi != 0);
    if (dpi == 0) {
        return 0;
    }
    if (const int result = _GetSystemMetricsForDpi2(index, dpi); result > 0) {
        return result;
    }
    static constexpr const auto defaultDpi = qreal(USER_DEFAULT_SCREEN_DPI);
    const qreal currentDpr = (qreal(Utils::getPrimaryScreenDpi(horizontal)) / defaultDpi);
    const qreal requestedDpr = (qreal(dpi) / defaultDpi);
    return std::round(qreal(::GetSystemMetrics(index)) / currentDpr * requestedDpr);
}

[[nodiscard]] static inline int getSystemMetrics2(const WId windowId, const int index, const bool horizontal, const bool scaled)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return 0;
    }
    const UINT realDpi = Utils::getWindowDpi(windowId, horizontal);
    {
        const UINT dpi = (scaled ? realDpi : USER_DEFAULT_SCREEN_DPI);
        if (const int result = _GetSystemMetricsForDpi2(index, dpi); result > 0) {
            return result;
        }
    }
    // GetSystemMetrics() will always return a scaled value, so if we want to get an unscaled
    // one, we have to calculate it ourself.
    const qreal dpr = (scaled ? qreal(1) : (qreal(realDpi) / qreal(USER_DEFAULT_SCREEN_DPI)));
    return std::round(qreal(::GetSystemMetrics(index)) / dpr);
}

[[maybe_unused]] [[nodiscard]] static inline constexpr
    DWORD qtEdgesToWin32Orientation(const Qt::Edges edges)
{
    if (edges == Qt::Edges{}) {
        return 0;
    } else if (edges == (Qt::LeftEdge)) {
        return SC_SIZELEFT;
    } else if (edges == (Qt::RightEdge)) {
        return SC_SIZERIGHT;
    } else if (edges == (Qt::TopEdge)) {
        return SC_SIZETOP;
    } else if (edges == (Qt::TopEdge | Qt::LeftEdge)) {
        return SC_SIZETOPLEFT;
    } else if (edges == (Qt::TopEdge | Qt::RightEdge)) {
        return SC_SIZETOPRIGHT;
    } else if (edges == (Qt::BottomEdge)) {
        return SC_SIZEBOTTOM;
    } else if (edges == (Qt::BottomEdge | Qt::LeftEdge)) {
        return SC_SIZEBOTTOMLEFT;
    } else if (edges == (Qt::BottomEdge | Qt::RightEdge)) {
        return SC_SIZEBOTTOMRIGHT;
    } else {
        return SC_SIZE;
    }
}

#if FRAMELESSHELPER_CONFIG(debug_output)
[[nodiscard]] static inline bool isWin32MessageDebuggingEnabled()
{
    static const bool result = (qEnvironmentVariableIntValue("FRAMELESSHELPER_ENABLE_WIN32_MESSAGE_DEBUGGING") != 0);
    return result;
}
#endif

[[nodiscard]] static inline QByteArray qtNativeEventType()
{
    static const auto result = FRAMELESSHELPER_BYTEARRAY_LITERAL("windows_generic_MSG");
    return result;
}

[[nodiscard]] static inline constexpr bool isNonClientMessage(const UINT message)
{
    if (((message >= WM_NCCREATE) && (message <= WM_NCACTIVATE))
            || ((message >= WM_NCMOUSEMOVE) && (message <= WM_NCMBUTTONDBLCLK))
            || ((message >= WM_NCXBUTTONDOWN) && (message <= WM_NCXBUTTONDBLCLK))
#if (WINVER >= _WIN32_WINNT_WIN8)
            || ((message >= WM_NCPOINTERUPDATE) && (message <= WM_NCPOINTERUP))
#endif
            || ((message == WM_NCMOUSEHOVER) || (message == WM_NCMOUSELEAVE))) {
        return true;
    } else {
        return false;
    }
}

[[nodiscard]] static inline constexpr bool isMouseMessage(const UINT message, bool *isNonClient = nullptr)
{
    if (((message >= WM_MOUSEFIRST) && (message <= WM_MOUSELAST))
            || ((message == WM_MOUSEHOVER) || (message == WM_MOUSELEAVE))) {
        if (isNonClient) {
            *isNonClient = false;
        }
        return true;
    } else if (((message >= WM_NCMOUSEMOVE) && (message <= WM_NCMBUTTONDBLCLK))
            || ((message >= WM_NCXBUTTONDOWN) && (message <= WM_NCXBUTTONDBLCLK))
            || ((message == WM_NCMOUSEHOVER) || (message == WM_NCMOUSELEAVE))) {
        if (isNonClient) {
            *isNonClient = true;
        }
        return true;
    } else {
        return false;
    }
}

[[nodiscard]] static inline LRESULT CALLBACK FramelessHelperHookWindowProc
    (const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam)
{
    Q_ASSERT(hWnd);
    if (!hWnd) {
        return FALSE;
    }
#if FRAMELESSHELPER_CONFIG(debug_output)
    if (isWin32MessageDebuggingEnabled()) {
        MSG message;
        SecureZeroMemory(&message, sizeof(message));
        message.hwnd = hWnd;
        message.message = uMsg;
        message.wParam = wParam;
        message.lParam = lParam;
        // The time and pt members are not used.
        Utils::printWin32Message(&message);
    }
#endif
    const auto defaultWindowProcessing = [hWnd, uMsg, wParam, lParam]() -> LRESULT { return ::DefWindowProcW(hWnd, uMsg, wParam, lParam); };
    const auto windowId = reinterpret_cast<WId>(hWnd);
    const QObject *window = FramelessManagerPrivate::getWindow(windowId);
    if (!window) {
        return defaultWindowProcessing();
    }
    const FramelessDataPtr data = FramelessManagerPrivate::getData(window);
    if (!data || !data->frameless || !data->callbacks) {
        return defaultWindowProcessing();
    }
    const UtilsWinExtraDataPtr extraData = tryGetExtraData(data, false);
    Q_ASSERT(extraData);
    if (!extraData) {
        return defaultWindowProcessing();
    }
    const QWindow *qWindow = data->callbacks->getWindowHandle();
    Q_ASSERT(qWindow);
    if (!qWindow) {
        return defaultWindowProcessing();
    }
#if FRAMELESSHELPER_CONFIG(native_impl)
    // https://github.com/qt/qtbase/blob/e26a87f1ecc40bc8c6aa5b889fce67410a57a702/src/plugins/platforms/windows/qwindowscontext.cpp#L1025
    // We can see from the source code that Qt will filter out some messages first and then send the unfiltered
    // messages to the event dispatcher. To activate the Snap Layout feature on Windows 11, we must process
    // some non-client area messages ourself, but unfortunately these messages have been filtered out already
    // in that line and thus we'll never have the chance to process them ourself. This is Qt's low level platform
    // specific code so we don't have any official ways to change this behavior. But luckily we can replace the
    // window procedure function of Qt's windows, and in this hooked window procedure function, we finally have
    // the chance to process window messages before Qt touches them. So we reconstruct the MSG structure and
    // send it to our own custom native event filter to do all the magic works. But since the system menu feature
    // doesn't necessarily belong to the native implementation, we seperate the handling code and always process
    // the system menu part in this function for both implementations.
    //
    // https://github.com/qt/qtbase/blob/946f15efb76fffda37b77f7d194d679b904305b1/src/plugins/platforms/windows/qwindowscontext.cpp#L1541
    // However, we can't just do this when the message is non-client area size re-calculating, because Qt QPA will
    // do some extra work after native event filter's handling to ensure Qt windows always have correct frame margins
    // (and correct client area size, especially when the window is maximized/fullscreen). So we still go through
    // the normal code path of the original qWindowsWndProc() function, but only for this specific message. It should
    // be OK because Qt won't prevent us from handling WM_NCCALCSIZE.
    if (uMsg != WM_NCCALCSIZE) {
        MSG message;
        SecureZeroMemory(&message, sizeof(message));
        message.hwnd = hWnd;
        message.message = uMsg;
        message.wParam = wParam;
        message.lParam = lParam;
        message.time = ::GetMessageTime();
#if 1
        const DWORD dwScreenPos = ::GetMessagePos();
        message.pt.x = GET_X_LPARAM(dwScreenPos);
        message.pt.y = GET_Y_LPARAM(dwScreenPos);
#else
        if (::GetCursorPos(&message.pt) == FALSE) {
            WARNING << Utils::getSystemErrorMessage(kGetCursorPos);
            message.pt = {};
        }
#endif
        if (!isNonClientMessage(uMsg)) {
            if (::ScreenToClient(hWnd, &message.pt) == FALSE) {
                WARNING << Utils::getSystemErrorMessage(kScreenToClient);
                message.pt = {};
            }
        }
        QT_NATIVE_EVENT_RESULT_TYPE filterResult = 0;
        const auto dispatcher = QAbstractEventDispatcher::instance();
        if (dispatcher && dispatcher->filterNativeEvent(qtNativeEventType(), &message, &filterResult)) {
            return LRESULT(filterResult);
        }
    }
#endif // native_impl
    const auto getNativePosFromMouse = [lParam]() -> QPoint {
        return {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};
    };
    const auto getNativeGlobalPosFromKeyboard = [hWnd, windowId]() -> QPoint {
        RECT windowPos = {};
        if (::GetWindowRect(hWnd, &windowPos) == FALSE) {
            WARNING << Utils::getSystemErrorMessage(kGetWindowRect);
            return {};
        }
        const bool maxOrFull = (IsMaximized(hWnd) || Utils::isFullScreen(windowId));
        const int frameSizeX = Utils::getResizeBorderThickness(windowId, true, true);
        const bool frameBorderVisible = Utils::isWindowFrameBorderVisible();
        const int horizontalOffset = ((maxOrFull || !frameBorderVisible) ? 0 : frameSizeX);
        const auto verticalOffset = [windowId, frameBorderVisible, maxOrFull]() -> int {
            const int titleBarHeight = Utils::getTitleBarHeight(windowId, true);
            if (!frameBorderVisible) {
                return titleBarHeight;
            }
            const int frameSizeY = Utils::getResizeBorderThickness(windowId, false, true);
            if (WindowsVersionHelper::isWin11OrGreater()) {
                if (maxOrFull) {
                    return (titleBarHeight + frameSizeY);
                }
                return titleBarHeight;
            }
            if (maxOrFull) {
                return titleBarHeight;
            }
            return (titleBarHeight - frameSizeY);
        }();
        return {windowPos.left + horizontalOffset, windowPos.top + verticalOffset};
    };
    bool shouldShowSystemMenu = false;
    bool broughtByKeyboard = false;
    QPoint nativeGlobalPos = {};
    switch (uMsg) {
    case WM_RBUTTONUP: {
        const QPoint nativeLocalPos = getNativePosFromMouse();
        const QPoint qtScenePos = Utils::fromNativeLocalPosition(qWindow, nativeLocalPos);
        if (data->callbacks->isInsideTitleBarDraggableArea(qtScenePos)) {
            POINT pos = {nativeLocalPos.x(), nativeLocalPos.y()};
            if (::ClientToScreen(hWnd, &pos) == FALSE) {
                WARNING << Utils::getSystemErrorMessage(kClientToScreen);
                break;
            }
            shouldShowSystemMenu = true;
            nativeGlobalPos = {pos.x, pos.y};
        }
    } break;
    case WM_NCRBUTTONUP:
        if (wParam == HTCAPTION) {
            shouldShowSystemMenu = true;
            nativeGlobalPos = getNativePosFromMouse();
        }
        break;
    case WM_SYSCOMMAND: {
        const WPARAM filteredWParam = (wParam & 0xFFF0);
        if ((filteredWParam == SC_KEYMENU) && (lParam == VK_SPACE)) {
            shouldShowSystemMenu = true;
            broughtByKeyboard = true;
            nativeGlobalPos = getNativeGlobalPosFromKeyboard();
        }
    } break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN: {
        const bool altPressed = ((wParam == VK_MENU) || (::GetKeyState(VK_MENU) < 0));
        const bool spacePressed = ((wParam == VK_SPACE) || (::GetKeyState(VK_SPACE) < 0));
        if (altPressed && spacePressed) {
            shouldShowSystemMenu = true;
            broughtByKeyboard = true;
            nativeGlobalPos = getNativeGlobalPosFromKeyboard();
        }
    } break;
    default:
        break;
    }
    if (shouldShowSystemMenu) {
        std::ignore = Utils::showSystemMenu(windowId, nativeGlobalPos, broughtByKeyboard);
        // QPA's internal code will handle system menu events separately, and its
        // behavior is not what we would want to see because it doesn't know our
        // window doesn't have any window frame now, so return early here to avoid
        // entering Qt's own handling logic.
        return FALSE; // Return 0 means we have handled this event.
    }
    Q_ASSERT(extraData->qtWindowProc);
    if (extraData->qtWindowProc) {
        // Hand over to Qt's original window proc function for events we are not
        // interested in.
        return ::CallWindowProcW(extraData->qtWindowProc, hWnd, uMsg, wParam, lParam);
    } else {
        return defaultWindowProcessing();
    }
}

bool Utils::isWindowsVersionOrGreater(const WindowsVersion version)
{
    return doCompareWindowsVersion(WindowsVersions.at(static_cast<int>(version)));
}

bool Utils::isDwmCompositionEnabled()
{
    // DWM composition is always enabled and can't be disabled since Windows 8.
    if (WindowsVersionHelper::isWin8OrGreater()) {
        return true;
    }
    const auto resultFromRegistry = []() -> bool {
        const RegistryKey registry(RegistryRootKey::CurrentUser, dwmRegistryKey());
        if (!registry.isValid()) {
            return false;
        }
        const DWORD value = registry.value<DWORD>(kComposition).value_or(0);
        return (value != 0);
    };
    if (!API_DWM_AVAILABLE(DwmIsCompositionEnabled)) {
        return resultFromRegistry();
    }
    BOOL enabled = FALSE;
    const HRESULT hr = API_CALL_FUNCTION(dwmapi, DwmIsCompositionEnabled, &enabled);
    if (FAILED(hr)) {
        WARNING << getSystemErrorMessageImpl(kDwmIsCompositionEnabled, hr);
        return resultFromRegistry();
    }
    return (enabled != FALSE);
}

bool Utils::triggerFrameChange(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    static constexpr const UINT swpFlags = (SWP_FRAMECHANGED | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE | SWP_NOZORDER | SWP_NOOWNERZORDER);
    if (::SetWindowPos(hwnd, nullptr, 0, 0, 0, 0, swpFlags) == FALSE) {
        WARNING << getSystemErrorMessage(kSetWindowPos);
        return false;
    }
    static constexpr const UINT rdwFlags = (RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_ALLCHILDREN);
    if (::RedrawWindow(hwnd, nullptr, nullptr, rdwFlags) == FALSE) {
        WARNING << getSystemErrorMessage(kRedrawWindow);
        return false;
    }
    return true;
}

bool Utils::updateWindowFrameMargins(const WId windowId, const bool reset)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    if (!API_DWM_AVAILABLE(DwmExtendFrameIntoClientArea)) {
        return false;
    }
    // We can't extend the window frame when DWM composition is disabled anyway.
    // No need to try further in this case.
    if (!isDwmCompositionEnabled()) {
        return false;
    }
    const QObject *window = FramelessManagerPrivate::getWindow(windowId);
    if (!window) {
        return false;
    }
    const FramelessDataPtr data = FramelessManagerPrivate::getData(window);
    if (!data) {
        return false;
    }
    const UtilsWinExtraDataPtr extraData = tryGetExtraData(data, false);
    Q_ASSERT(extraData);
    if (!extraData) {
        return false;
    }
    const auto margins = [&extraData, reset]() -> MARGINS {
        // To make Mica/Mica Alt work for normal Win32 windows, we have to
        // let the window frame extend to the whole window (or disable the
        // redirection surface, but this will break GDI's rendering, so we
        // can't do this, unfortunately), so we can't change the window frame
        // margins in this case, otherwise Mica/Mica Alt will be broken.
        if (extraData->mica) {
            return {-1, -1, -1, -1};
        }
        if (reset || isWindowFrameBorderVisible()) {
            return {0, 0, 0, 0};
        }
        return {1, 1, 1, 1};
    }();
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    const HRESULT hr = API_CALL_FUNCTION(dwmapi, DwmExtendFrameIntoClientArea, hwnd, &margins);
    if (FAILED(hr)) {
        WARNING << getSystemErrorMessageImpl(kDwmExtendFrameIntoClientArea, hr);
        return false;
    }
    return triggerFrameChange(windowId);
}

bool Utils::updateInternalWindowFrameMargins(QWindow *window, const bool enable)
{
    Q_ASSERT(window);
    if (!window) {
        return false;
    }
    const WId windowId = window->winId();
    const auto margins = [enable, windowId]() -> QMargins {
        if (!enable) {
            return {};
        }
        const int titleBarHeight = getTitleBarHeight(windowId, true);
        if (isWindowFrameBorderVisible()) {
            return {0, -titleBarHeight, 0, 0};
        } else {
            const int frameSizeX = getResizeBorderThickness(windowId, true, true);
            const int frameSizeY = getResizeBorderThickness(windowId, false, true);
            return {-frameSizeX, -titleBarHeight, -frameSizeX, -frameSizeY};
        }
    }();
    const QVariant marginsVar = QVariant::fromValue(margins);
    window->setProperty(kQtWindowCustomMarginsVar, marginsVar);
#if FRAMELESSHELPER_CONFIG(private_qt)
#  if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    if (QPlatformWindow *platformWindow = window->handle()) {
        if (const auto ni = QGuiApplication::platformNativeInterface()) {
            ni->setWindowProperty(platformWindow, qtWindowCustomMarginsProp(), marginsVar);
        } else {
            WARNING << "Failed to retrieve the platform native interface.";
            return false;
        }
    } else {
        WARNING << "Failed to retrieve the platform window.";
        return false;
    }
#  else // (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    if (const auto platformWindow = dynamic_cast<QNativeInterface::Private::QWindowsWindow *>(window->handle())) {
        platformWindow->setCustomMargins(margins);
    } else {
        WARNING << "Failed to retrieve the platform window.";
        return false;
    }
#  endif // (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#endif // FRAMELESSHELPER_CONFIG(private_qt)
    return triggerFrameChange(windowId);
}

QString Utils::getSystemErrorMessage(const QString &function)
{
    Q_ASSERT(!function.isEmpty());
    if (function.isEmpty()) {
        return {};
    }
    const DWORD code = ::GetLastError();
    if (code == ERROR_SUCCESS) {
        return kSuccessMessageText;
    }
    return getSystemErrorMessageImpl(function, code);
}

QColor Utils::getDwmColorizationColor(bool *opaque, bool *ok)
{
    const auto resultFromRegistry = []() -> QColor {
        const RegistryKey registry(RegistryRootKey::CurrentUser, dwmRegistryKey());
        if (!registry.isValid()) {
            return kDefaultDarkGrayColor;
        }
        const QVariant value = registry.value(kColorizationColor);
        if (!value.isValid()) {
            return kDefaultDarkGrayColor;
        }
        return QColor::fromRgba(qvariant_cast<DWORD>(value));
    };
    if (!API_DWM_AVAILABLE(DwmGetColorizationColor)) {
        if (ok) {
            *ok = false;
        }
        return resultFromRegistry();
    }
    DWORD color = 0;
    BOOL bOpaque = FALSE;
    const HRESULT hr = API_CALL_FUNCTION(dwmapi, DwmGetColorizationColor, &color, &bOpaque);
    if (FAILED(hr)) {
        WARNING << getSystemErrorMessageImpl(kDwmGetColorizationColor, hr);
        if (ok) {
            *ok = false;
        }
        return resultFromRegistry();
    }
    if (opaque) {
        *opaque = (bOpaque != FALSE);
    }
    if (ok) {
        *ok = true;
    }
    return QColor::fromRgba(color);
}

DwmColorizationArea Utils::getDwmColorizationArea()
{
    // It's a Win10 only feature. (TO BE VERIFIED)
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        return DwmColorizationArea::None;
    }
    const RegistryKey themeRegistry(RegistryRootKey::CurrentUser, personalizeRegistryKey());
    const DWORD themeValue = themeRegistry.isValid() ? themeRegistry.value<DWORD>(dwmColorKeyName()).value_or(0) : 0;
    const RegistryKey dwmRegistry(RegistryRootKey::CurrentUser, dwmRegistryKey());
    const DWORD dwmValue = dwmRegistry.isValid() ? dwmRegistry.value<DWORD>(dwmColorKeyName()).value_or(0) : 0;
    const bool theme = (themeValue != 0);
    const bool dwm = (dwmValue != 0);
    if (theme && dwm) {
        return DwmColorizationArea::All;
    } else if (theme) {
        return DwmColorizationArea::StartMenu_TaskBar_ActionCenter;
    } else if (dwm) {
        return DwmColorizationArea::TitleBar_WindowBorder;
    }
    return DwmColorizationArea::None;
}

bool Utils::showSystemMenu(const WId windowId, const QPoint &pos, const bool selectFirstEntry)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }

    const QObject *window = FramelessManagerPrivate::getWindow(windowId);
    if (!window) {
        return false;
    }
    const FramelessDataPtr data = FramelessManagerPrivate::getData(window);
    if (!data || !data->frameless || !data->callbacks) {
        return false;
    }

    const auto hWnd = reinterpret_cast<HWND>(windowId);
    const HMENU hMenu = ::GetSystemMenu(hWnd, FALSE);
    if (!hMenu) {
        // The corresponding window doesn't have a system menu, most likely due to the
        // lack of the "WS_SYSMENU" window style. This situation should not be treated
        // as an error so just ignore it and return early.
        return true;
    }

    // Tweak the menu items according to the current window status and user settings.
    const bool disableClose = data->callbacks->getProperty(kSysMenuDisableCloseVar, false).toBool();
    const bool disableRestore = data->callbacks->getProperty(kSysMenuDisableRestoreVar, false).toBool();
    const bool disableMinimize = data->callbacks->getProperty(kSysMenuDisableMinimizeVar, false).toBool();
    const bool disableMaximize = data->callbacks->getProperty(kSysMenuDisableMaximizeVar, false).toBool();
    const bool disableSize = data->callbacks->getProperty(kSysMenuDisableSizeVar, false).toBool();
    const bool disableMove = data->callbacks->getProperty(kSysMenuDisableMoveVar, false).toBool();
    const bool removeClose = data->callbacks->getProperty(kSysMenuRemoveCloseVar, false).toBool();
    const bool removeSeparator = data->callbacks->getProperty(kSysMenuRemoveSeparatorVar, false).toBool();
    const bool removeRestore = data->callbacks->getProperty(kSysMenuRemoveRestoreVar, false).toBool();
    const bool removeMinimize = data->callbacks->getProperty(kSysMenuRemoveMinimizeVar, false).toBool();
    const bool removeMaximize = data->callbacks->getProperty(kSysMenuRemoveMaximizeVar, false).toBool();
    const bool removeSize = data->callbacks->getProperty(kSysMenuRemoveSizeVar, false).toBool();
    const bool removeMove = data->callbacks->getProperty(kSysMenuRemoveMoveVar, false).toBool();
    const bool maxOrFull = (IsMaximized(hWnd) || isFullScreen(windowId));
    const bool fixedSize = data->callbacks->isWindowFixedSize();
    if (removeClose) {
        if (::DeleteMenu(hMenu, SC_CLOSE, MF_BYCOMMAND) == FALSE) {
            //WARNING << getSystemErrorMessage(kDeleteMenu);
        }
    } else {
        ::EnableMenuItem(hMenu, SC_CLOSE, (MF_BYCOMMAND | (disableClose ? MFS_DISABLED : MFS_ENABLED)));
    }
    if (removeSeparator) {
        // Looks like we must use 0 for the second parameter here, otherwise we can't remove the separator.
        if (::DeleteMenu(hMenu, 0, MFT_SEPARATOR) == FALSE) {
            //WARNING << getSystemErrorMessage(kDeleteMenu);
        }
    }
    if (removeMaximize) {
        if (::DeleteMenu(hMenu, SC_MAXIMIZE, MF_BYCOMMAND) == FALSE) {
            //WARNING << getSystemErrorMessage(kDeleteMenu);
        }
    } else {
        ::EnableMenuItem(hMenu, SC_MAXIMIZE, (MF_BYCOMMAND | ((!maxOrFull && !fixedSize && !disableMaximize) ? MFS_ENABLED : MFS_DISABLED)));
    }
    if (removeRestore) {
        if (::DeleteMenu(hMenu, SC_RESTORE, MF_BYCOMMAND) == FALSE) {
            //WARNING << getSystemErrorMessage(kDeleteMenu);
        }
    } else {
        ::EnableMenuItem(hMenu, SC_RESTORE, (MF_BYCOMMAND | ((maxOrFull && !fixedSize && !disableRestore) ? MFS_ENABLED : MFS_DISABLED)));
        // The first menu item should be selected by default if the menu is brought
        // up by keyboard. I don't know how to pre-select a menu item but it seems
        // highlight can do the job. However, there's an annoying issue if we do
        // this manually: the highlighted menu item is really only highlighted,
        // not selected, so even if the mouse cursor hovers on other menu items
        // or the user navigates to other menu items through keyboard, the original
        // highlight bar will not move accordingly, the OS will generate another
        // highlight bar to indicate the current selected menu item, which will make
        // the menu look kind of weird. Currently I don't know how to fix this issue.
        ::HiliteMenuItem(hWnd, hMenu, SC_RESTORE, (MF_BYCOMMAND | (selectFirstEntry ? MFS_HILITE : MFS_UNHILITE)));
    }
    if (removeMinimize) {
        if (::DeleteMenu(hMenu, SC_MINIMIZE, MF_BYCOMMAND) == FALSE) {
            //WARNING << getSystemErrorMessage(kDeleteMenu);
        }
    } else {
        ::EnableMenuItem(hMenu, SC_MINIMIZE, (MF_BYCOMMAND | (disableMinimize ? MFS_DISABLED : MFS_ENABLED)));
    }
    if (removeSize) {
        if (::DeleteMenu(hMenu, SC_SIZE, MF_BYCOMMAND) == FALSE) {
            //WARNING << getSystemErrorMessage(kDeleteMenu);
        }
    } else {
        ::EnableMenuItem(hMenu, SC_SIZE, (MF_BYCOMMAND | ((!maxOrFull && !fixedSize && !(disableSize || disableMinimize || disableMaximize)) ? MFS_ENABLED : MFS_DISABLED)));
    }
    if (removeMove) {
        if (::DeleteMenu(hMenu, SC_MOVE, MF_BYCOMMAND) == FALSE) {
            //WARNING << getSystemErrorMessage(kDeleteMenu);
        }
    } else {
        ::EnableMenuItem(hMenu, SC_MOVE, (MF_BYCOMMAND | ((disableMove || maxOrFull) ? MFS_DISABLED : MFS_ENABLED)));
    }

    // The default menu item will appear in bold font. There can only be one default
    // menu item per menu at most. Set the item ID to "UINT_MAX" (or simply "-1")
    // can clear the default item for the given menu.
    std::optional<UINT> defaultItemId = std::nullopt;
    if (WindowsVersionHelper::isWin11OrGreater()) {
        if (maxOrFull) {
            if (!removeRestore) {
                defaultItemId = SC_RESTORE;
            }
        } else {
            if (!removeMaximize) {
                defaultItemId = SC_MAXIMIZE;
            }
        }
    }
    if (!(defaultItemId.has_value() || removeClose)) {
        defaultItemId = SC_CLOSE;
    }
    if (::SetMenuDefaultItem(hMenu, defaultItemId.value_or(UINT_MAX), FALSE) == FALSE) {
        WARNING << getSystemErrorMessage(kSetMenuDefaultItem);
    }

    if (::DrawMenuBar(hWnd) == FALSE) {
        WARNING << getSystemErrorMessage(kDrawMenuBar);
    }

    // Popup the system menu at the required position.
    const int result = ::TrackPopupMenu(hMenu, (TPM_RETURNCMD | (QGuiApplication::isRightToLeft() ? TPM_RIGHTALIGN : TPM_LEFTALIGN)), pos.x(), pos.y(), 0, hWnd, nullptr);

    if (!removeRestore) {
        // Unhighlight the first menu item after the popup menu is closed, otherwise it will keep
        // highlighting until we unhighlight it manually.
        ::HiliteMenuItem(hWnd, hMenu, SC_RESTORE, (MF_BYCOMMAND | MFS_UNHILITE));
    }

    if (result == FALSE) {
        // The user canceled the menu, no need to continue.
        return true;
    }

    // Send the command that the user choses to the corresponding window.
    if (::PostMessageW(hWnd, WM_SYSCOMMAND, result, 0) == FALSE) {
        WARNING << getSystemErrorMessage(kPostMessageW);
        return false;
    }

    return true;
}

bool Utils::isFullScreen(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    RECT windowRect = {};
    if (::GetWindowRect(hwnd, &windowRect) == FALSE) {
        WARNING << getSystemErrorMessage(kGetWindowRect);
        return false;
    }
    const std::optional<MONITORINFOEXW> mi = getMonitorForWindow(hwnd);
    if (!mi.has_value()) {
        WARNING << "Failed to retrieve the window's monitor.";
        return false;
    }
    // Compare to the full area of the screen, not the work area.
    return (windowRect == mi.value().rcMonitor);
}

bool Utils::isWindowNoState(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
#if 0
    WINDOWPLACEMENT wp;
    SecureZeroMemory(&wp, sizeof(wp));
    wp.length = sizeof(wp); // This line is important! Don't miss it!
    if (::GetWindowPlacement(hwnd, &wp) == FALSE) {
        WARNING << getSystemErrorMessage(kGetWindowPlacement);
        return false;
    }
    return ((wp.showCmd == SW_NORMAL) || (wp.showCmd == SW_RESTORE));
#else
    ::SetLastError(ERROR_SUCCESS);
    const auto style = static_cast<DWORD>(::GetWindowLongPtrW(hwnd, GWL_STYLE));
    if (style == 0) {
        WARNING << getSystemErrorMessage(kGetWindowLongPtrW);
        return false;
    }
    return (!(style & (WS_MINIMIZE | WS_MAXIMIZE)));
#endif
}

bool Utils::syncWmPaintWithDwm()
{
    if (!(API_WINMM_AVAILABLE(timeGetDevCaps)
        && API_WINMM_AVAILABLE(timeBeginPeriod)
        && API_WINMM_AVAILABLE(timeEndPeriod)
        && API_DWM_AVAILABLE(DwmGetCompositionTimingInfo))) {
        return false;
    }
    // No need to sync with DWM if DWM composition is disabled.
    if (!isDwmCompositionEnabled()) {
        return false;
    }
    // Dirty hack to workaround the resize flicker caused by DWM.
    LARGE_INTEGER freq = {};
    if (::QueryPerformanceFrequency(&freq) == FALSE) {
        WARNING << getSystemErrorMessage(kQueryPerformanceFrequency);
        return false;
    }
    _TIMECAPS tc = {};
    if (API_CALL_FUNCTION4(winmm, timeGetDevCaps, &tc, sizeof(tc)) != MMSYSERR_NOERROR) {
        WARNING << "timeGetDevCaps() failed.";
        return false;
    }
    const UINT ms_granularity = tc.wPeriodMin;
    if (API_CALL_FUNCTION4(winmm, timeBeginPeriod, ms_granularity) != TIMERR_NOERROR) {
        WARNING << "timeBeginPeriod() failed.";
        return false;
    }
    LARGE_INTEGER now0 = {};
    if (::QueryPerformanceCounter(&now0) == FALSE) {
        WARNING << getSystemErrorMessage(kQueryPerformanceCounter);
        return false;
    }
    // ask DWM where the vertical blank falls
    DWM_TIMING_INFO dti;
    SecureZeroMemory(&dti, sizeof(dti));
    dti.cbSize = sizeof(dti);
    const HRESULT hr = API_CALL_FUNCTION(dwmapi, DwmGetCompositionTimingInfo, nullptr, &dti);
    if (FAILED(hr)) {
        WARNING << getSystemErrorMessage(kDwmGetCompositionTimingInfo);
        return false;
    }
    LARGE_INTEGER now1 = {};
    if (::QueryPerformanceCounter(&now1) == FALSE) {
        WARNING << getSystemErrorMessage(kQueryPerformanceCounter);
        return false;
    }
    // - DWM told us about SOME vertical blank
    //   - past or future, possibly many frames away
    // - convert that into the NEXT vertical blank
    const auto period = qreal(dti.qpcRefreshPeriod);
    const auto dt = qreal(dti.qpcVBlank - now1.QuadPart);
    const qreal ratio = (dt / period);
    auto w = qreal(0);
    auto m = qreal(0);
    if ((dt > qreal(0)) || qFuzzyIsNull(dt)) {
        w = ratio;
    } else {
        // reach back to previous period
        // - so m represents consistent position within phase
        w = (ratio - qreal(1));
    }
    m = (dt - (period * w));
    //Q_ASSERT((m > qreal(0)) || qFuzzyIsNull(m));
    //Q_ASSERT(m < period);
    if ((m < qreal(0)) || qFuzzyCompare(m, period) || (m > period)) {
        return false;
    }
    const qreal m_ms = (qreal(1000) * m / qreal(freq.QuadPart));
    ::Sleep(static_cast<DWORD>(std::round(m_ms)));
    if (API_CALL_FUNCTION4(winmm, timeEndPeriod, ms_granularity) != TIMERR_NOERROR) {
        WARNING << "timeEndPeriod() failed.";
        return false;
    }
    return true;
}

bool Utils::isHighContrastModeEnabled()
{
    HIGHCONTRASTW hc;
    SecureZeroMemory(&hc, sizeof(hc));
    hc.cbSize = sizeof(hc);
    if (::SystemParametersInfoW(SPI_GETHIGHCONTRAST, sizeof(hc), &hc, FALSE) == FALSE) {
        WARNING << getSystemErrorMessage(kSystemParametersInfoW);
        return false;
    }
    return (hc.dwFlags & HCF_HIGHCONTRASTON);
}

quint32 Utils::getPrimaryScreenDpi(const bool horizontal)
{
    // GetDesktopWindow(): The desktop window will always be in the primary monitor.
    if (const HMONITOR hMonitor = MonitorFromWindow(GetDesktopWindow(), MONITOR_DEFAULTTOPRIMARY)) {
        // GetDpiForMonitor() is only available on Windows 8 and onwards.
        if (API_SHCORE_AVAILABLE(GetDpiForMonitor)) {
            UINT dpiX = 0, dpiY = 0;
            const HRESULT hr = API_CALL_FUNCTION4(shcore, GetDpiForMonitor, hMonitor, _MDT_EFFECTIVE_DPI, &dpiX, &dpiY);
            if (SUCCEEDED(hr) && (dpiX > 0) && (dpiY > 0)) {
                return (horizontal ? dpiX : dpiY);
            } else {
                WARNING << getSystemErrorMessageImpl(kGetDpiForMonitor, hr);
            }
        }
        // GetScaleFactorForMonitor() is only available on Windows 8 and onwards.
        if (API_SHCORE_AVAILABLE(GetScaleFactorForMonitor)) {
            _DEVICE_SCALE_FACTOR factor = _DEVICE_SCALE_FACTOR_INVALID;
            const HRESULT hr = API_CALL_FUNCTION4(shcore, GetScaleFactorForMonitor, hMonitor, &factor);
            if (SUCCEEDED(hr) && (factor != _DEVICE_SCALE_FACTOR_INVALID)) {
                return quint32(std::round(qreal(USER_DEFAULT_SCREEN_DPI) * qreal(factor) / qreal(100)));
            } else {
                WARNING << getSystemErrorMessageImpl(kGetScaleFactorForMonitor, hr);
            }
        }
        // This solution is supported on Windows 2000 and onwards.
        MONITORINFOEXW monitorInfo;
        SecureZeroMemory(&monitorInfo, sizeof(monitorInfo));
        monitorInfo.cbSize = sizeof(monitorInfo);
        if (::GetMonitorInfoW(hMonitor, &monitorInfo) != FALSE) {
            if (const HDC hdc = ::CreateDCW(monitorInfo.szDevice, monitorInfo.szDevice, nullptr, nullptr)) {
                bool valid = false;
                const int dpiX = ::GetDeviceCaps(hdc, LOGPIXELSX);
                const int dpiY = ::GetDeviceCaps(hdc, LOGPIXELSY);
                if ((dpiX > 0) && (dpiY > 0)) {
                    valid = true;
                } else {
                    WARNING << getSystemErrorMessage(kGetDeviceCaps);
                }
                if (::DeleteDC(hdc) == FALSE) {
                    WARNING << getSystemErrorMessage(kDeleteDC);
                }
                if (valid) {
                    return (horizontal ? dpiX : dpiY);
                }
            } else {
                WARNING << getSystemErrorMessage(kCreateDCW);
            }
        } else {
            WARNING << getSystemErrorMessage(kGetMonitorInfoW);
        }
    } else {
        WARNING << getSystemErrorMessage(kMonitorFromWindow);
    }

    // Using Direct2D to get the primary monitor's DPI is only available on Windows 7
    // and onwards, but it has been marked as deprecated by Microsoft.
    if (API_D2D_AVAILABLE(D2D1CreateFactory)) {
        using D2D1CreateFactoryPtr = HRESULT(WINAPI *)(D2D1_FACTORY_TYPE, REFIID, CONST D2D1_FACTORY_OPTIONS *, void **);
        const auto pD2D1CreateFactory = reinterpret_cast<D2D1CreateFactoryPtr>(SysApiLoader::instance()->get(kd2d1, kD2D1CreateFactory));
        ID2D1Factory *d2dFactory = nullptr;
        HRESULT hr = pD2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), nullptr, reinterpret_cast<void **>(&d2dFactory));
        if (SUCCEEDED(hr)) {
            // We want to get the newest system DPI, so refresh the system metrics
            // manually to ensure that.
            hr = d2dFactory->ReloadSystemMetrics();
            if (SUCCEEDED(hr)) {
                FLOAT dpiX = FLOAT(0), dpiY = FLOAT(0);
                QT_WARNING_PUSH
                QT_WARNING_DISABLE_DEPRECATED
                d2dFactory->GetDesktopDpi(&dpiX, &dpiY);
                QT_WARNING_POP
                if ((dpiX > FLOAT(0)) && (dpiY > FLOAT(0))) {
                    return (horizontal ? quint32(std::round(dpiX)) : quint32(std::round(dpiY)));
                } else {
                    WARNING << "GetDesktopDpi() failed.";
                }
            } else {
                WARNING << getSystemErrorMessageImpl(kReloadSystemMetrics, hr);
            }
        } else {
            WARNING << getSystemErrorMessageImpl(kD2D1CreateFactory, hr);
        }
        if (d2dFactory) {
            d2dFactory->Release();
            d2dFactory = nullptr;
        }
    }

    // Our last hope to get the DPI of the primary monitor, if all the above
    // solutions failed, however, it won't happen in most cases.
    if (const HDC hdc = ::GetDC(nullptr)) {
        bool valid = false;
        const int dpiX = ::GetDeviceCaps(hdc, LOGPIXELSX);
        const int dpiY = ::GetDeviceCaps(hdc, LOGPIXELSY);
        if ((dpiX > 0) && (dpiY > 0)) {
            valid = true;
        } else {
            WARNING << getSystemErrorMessage(kGetDeviceCaps);
        }
        if (::ReleaseDC(nullptr, hdc) == 0) {
            WARNING << getSystemErrorMessage(kReleaseDC);
        }
        if (valid) {
            return (horizontal ? dpiX : dpiY);
        }
    } else {
        WARNING << getSystemErrorMessage(kGetDC);
    }

    // We should never go here, but let's make it extra safe. Just assume we
    // are not scaled (96 DPI) if we really can't get the real DPI.
    return USER_DEFAULT_SCREEN_DPI;
}

quint32 Utils::getWindowDpi(const WId windowId, const bool horizontal)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return USER_DEFAULT_SCREEN_DPI;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    {
        if (const UINT dpi = _GetDpiForWindow2(hwnd)) {
            return dpi;
        }
        // ERROR_CALL_NOT_IMPLEMENTED: the function is not available on
        // current platform, not an error.
        if (::GetLastError() != ERROR_CALL_NOT_IMPLEMENTED) {
            WARNING << getSystemErrorMessage(kGetDpiForWindow);
        }
    }
    if (API_USER_AVAILABLE(GetSystemDpiForProcess)) {
        const HANDLE process = ::GetCurrentProcess();
        if (process) {
            const UINT dpi = API_CALL_FUNCTION4(user32, GetSystemDpiForProcess, process);
            if (dpi > 0) {
                return dpi;
            } else {
                WARNING << getSystemErrorMessage(kGetSystemDpiForProcess);
            }
        } else {
            WARNING << getSystemErrorMessage(kGetCurrentProcess);
        }
    }
    if (API_USER_AVAILABLE(GetDpiForSystem)) {
        const UINT dpi = API_CALL_FUNCTION4(user32, GetDpiForSystem);
        if (dpi > 0) {
            return dpi;
        } else {
            WARNING << getSystemErrorMessage(kGetDpiForSystem);
        }
    }
    if (const HDC hdc = ::GetDC(hwnd)) {
        bool valid = false;
        const int dpiX = ::GetDeviceCaps(hdc, LOGPIXELSX);
        const int dpiY = ::GetDeviceCaps(hdc, LOGPIXELSY);
        if ((dpiX > 0) && (dpiY > 0)) {
            valid = true;
        } else {
            WARNING << getSystemErrorMessage(kGetDeviceCaps);
        }
        if (::ReleaseDC(hwnd, hdc) == 0) {
            WARNING << getSystemErrorMessage(kReleaseDC);
        }
        if (valid) {
            return (horizontal ? dpiX : dpiY);
        }
    } else {
        WARNING << getSystemErrorMessage(kGetDC);
    }
    return getPrimaryScreenDpi(horizontal);
}

quint32 Utils::getResizeBorderThicknessForDpi(const bool horizontal, const quint32 dpi)
{
    Q_ASSERT(dpi != 0);
    if (dpi == 0) {
        return 0;
    }
    if (horizontal) {
        return (getSystemMetrics2(SM_CXSIZEFRAME, true, dpi) + getSystemMetrics2(SM_CXPADDEDBORDER, true, dpi));
    } else {
        return (getSystemMetrics2(SM_CYSIZEFRAME, false, dpi) + getSystemMetrics2(SM_CYPADDEDBORDER, false, dpi));
    }
}

quint32 Utils::getResizeBorderThickness(const WId windowId, const bool horizontal, const bool scaled)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return 0;
    }
    if (horizontal) {
        return (getSystemMetrics2(windowId, SM_CXSIZEFRAME, true, scaled) + getSystemMetrics2(windowId, SM_CXPADDEDBORDER, true, scaled));
    } else {
        return (getSystemMetrics2(windowId, SM_CYSIZEFRAME, false, scaled) + getSystemMetrics2(windowId, SM_CYPADDEDBORDER, false, scaled));
    }
}

quint32 Utils::getCaptionBarHeightForDpi(const quint32 dpi)
{
    Q_ASSERT(dpi != 0);
    if (dpi == 0) {
        return 0;
    }
    return getSystemMetrics2(SM_CYCAPTION, false, dpi);
}

quint32 Utils::getCaptionBarHeight(const WId windowId, const bool scaled)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return 0;
    }
    return getSystemMetrics2(windowId, SM_CYCAPTION, false, scaled);
}

quint32 Utils::getTitleBarHeightForDpi(const quint32 dpi)
{
    Q_ASSERT(dpi != 0);
    if (dpi == 0) {
        return 0;
    }
    return (getCaptionBarHeightForDpi(dpi) + getResizeBorderThicknessForDpi(false, dpi));
}

quint32 Utils::getTitleBarHeight(const WId windowId, const bool scaled)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return 0;
    }
    return (getCaptionBarHeight(windowId, scaled) + getResizeBorderThickness(windowId, false, scaled));
}

quint32 Utils::getFrameBorderThicknessForDpi(const quint32 dpi)
{
    Q_ASSERT(dpi != 0);
    if (dpi == 0) {
        return 0;
    }
    // There's no window frame border before Windows 10.
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        return 0;
    }
    const qreal dpr = (qreal(dpi) / qreal(USER_DEFAULT_SCREEN_DPI));
    return std::round(qreal(kDefaultWindowFrameBorderThickness) * dpr);
}

quint32 Utils::getFrameBorderThickness(const WId windowId, const bool scaled)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return 0;
    }
    // There's no window frame border before Windows 10.
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        return 0;
    }
    if (!API_DWM_AVAILABLE(DwmGetWindowAttribute)) {
        return 0;
    }
    const UINT dpi = getWindowDpi(windowId, true);
    const qreal scaleFactor = (qreal(dpi) / qreal(USER_DEFAULT_SCREEN_DPI));
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    UINT value = 0;
    const HRESULT hr = API_CALL_FUNCTION(dwmapi, DwmGetWindowAttribute, hwnd, _DWMWA_VISIBLE_FRAME_BORDER_THICKNESS, &value, sizeof(value));
    if (SUCCEEDED(hr)) {
        const qreal dpr = (scaled ? qreal(1) : scaleFactor);
        return std::round(qreal(value) / dpr);
    } else {
        const qreal dpr = (scaled ? scaleFactor : qreal(1));
        return std::round(qreal(kDefaultWindowFrameBorderThickness) * dpr);
    }
}

QColor Utils::getFrameBorderColor(const bool active)
{
    // There's no window frame border before Windows 10.
    // So we just return a default value which is based on most window managers.
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        return (active ? kDefaultBlackColor : kDefaultDarkGrayColor);
    }
    const bool dark = (FramelessManager::instance()->systemTheme() == SystemTheme::Dark);
    if (active) {
        if (isFrameBorderColorized()) {
            return getAccentColor();
        }
        return (dark ? kDefaultFrameBorderActiveColorDark : kDefaultFrameBorderActiveColorLight);
    } else {
        return (dark ? kDefaultFrameBorderInactiveColorDark : kDefaultFrameBorderInactiveColorLight);
    }
}

bool Utils::maybeFixupQtInternals(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    static const bool dont = (qEnvironmentVariableIntValue("FRAMELESSHELPER_WINDOWS_DONT_FIX_QT") != 0);
    if (dont) {
        return true;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
#if 0
    ::SetLastError(ERROR_SUCCESS);
    const auto classStyle = static_cast<DWORD>(::GetClassLongPtrW(hwnd, GCL_STYLE));
    if (classStyle != 0) {
        // CS_HREDRAW/CS_VREDRAW will trigger a repaint event when the window size changes
        // horizontally/vertically, which will cause flicker and jitter during window resizing,
        // mostly for the applications which do all the painting by themselves (eg: Qt).
        // So we remove these flags from the window class here. Qt by default won't add them
        // but let's make it extra safe in case the user may add them by accident.
        static constexpr const DWORD badClassStyle = (CS_HREDRAW | CS_VREDRAW);
        if (classStyle & badClassStyle) {
            ::SetLastError(ERROR_SUCCESS);
            if (::SetClassLongPtrW(hwnd, GCL_STYLE, (classStyle & ~badClassStyle)) == 0) {
                WARNING << getSystemErrorMessage(kSetClassLongPtrW);
                return false;
            }
        }
    } else {
        WARNING << getSystemErrorMessage(kGetClassLongPtrW);
        return false;
    }
#endif
    ::SetLastError(ERROR_SUCCESS);
    const auto windowStyle = static_cast<DWORD>(::GetWindowLongPtrW(hwnd, GWL_STYLE));
    if (windowStyle == 0) {
        WARNING << getSystemErrorMessage(kGetWindowLongPtrW);
        return false;
    } else {
        // Qt by default adds the "WS_POPUP" flag to all Win32 windows it created and maintained,
        // which is not a good thing (although it won't cause any obvious issues in most cases
        // either), because popup windows have some different behavior with normal overlapped
        // windows, for example, it will affect DWM's default policy. And Qt will also lack some
        // necessary window styles in some cases (caused by misconfigured setWindowFlag(s) calls)
        // and this will also break the normal functionalities for our windows, so we do the
        // correction here unconditionally.
        static constexpr const DWORD badWindowStyle = WS_POPUP;
        static constexpr const DWORD goodWindowStyle = (WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
        if ((windowStyle & badWindowStyle) || !(windowStyle & goodWindowStyle)) {
            ::SetLastError(ERROR_SUCCESS);
            if (::SetWindowLongPtrW(hwnd, GWL_STYLE, ((windowStyle & ~badWindowStyle) | goodWindowStyle)) == 0) {
                WARNING << getSystemErrorMessage(kSetWindowLongPtrW);
                return false;
            }
        }
    }
    ::SetLastError(ERROR_SUCCESS);
    const auto extendedWindowStyle = static_cast<DWORD>(::GetWindowLongPtrW(hwnd, GWL_EXSTYLE));
    if (extendedWindowStyle == 0) {
        WARNING << getSystemErrorMessage(kGetWindowLongPtrW);
        return false;
    } else {
        static constexpr const DWORD badWindowStyle = (WS_EX_OVERLAPPEDWINDOW | WS_EX_STATICEDGE | WS_EX_DLGMODALFRAME | WS_EX_CONTEXTHELP);
        static constexpr const DWORD goodWindowStyle = WS_EX_APPWINDOW;
        if ((extendedWindowStyle & badWindowStyle) || !(extendedWindowStyle & goodWindowStyle)) {
            ::SetLastError(ERROR_SUCCESS);
            if (::SetWindowLongPtrW(hwnd, GWL_EXSTYLE, ((extendedWindowStyle & ~badWindowStyle) | goodWindowStyle)) == 0) {
                WARNING << getSystemErrorMessage(kSetWindowLongPtrW);
                return false;
            }
        }
    }
    return triggerFrameChange(windowId);
}

bool Utils::startSystemMove(QWindow *window, const QPoint &globalPos)
{
    Q_UNUSED(globalPos);
    Q_ASSERT(window);
    if (!window) {
        return false;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    return window->startSystemMove();
#else
    if (::ReleaseCapture() == FALSE) {
        WARNING << getSystemErrorMessage(kReleaseCapture);
        return false;
    }
    const auto hwnd = reinterpret_cast<HWND>(window->winId());
    if (::PostMessageW(hwnd, WM_SYSCOMMAND, SC_DRAGMOVE, 0) == FALSE) {
        WARNING << getSystemErrorMessage(kPostMessageW);
        return false;
    }
    return true;
#endif
}

bool Utils::startSystemResize(QWindow *window, const Qt::Edges edges, const QPoint &globalPos)
{
    Q_UNUSED(globalPos);
    Q_ASSERT(window);
    if (!window) {
        return false;
    }
    if (edges == Qt::Edges{}) {
        return false;
    }
#if (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    return window->startSystemResize(edges);
#else
    if (::ReleaseCapture() == FALSE) {
        WARNING << getSystemErrorMessage(kReleaseCapture);
        return false;
    }
    const auto hwnd = reinterpret_cast<HWND>(window->winId());
    if (::PostMessageW(hwnd, WM_SYSCOMMAND, qtEdgesToWin32Orientation(edges), 0) == FALSE) {
        WARNING << getSystemErrorMessage(kPostMessageW);
        return false;
    }
    return true;
#endif
}

bool Utils::isWindowFrameBorderVisible()
{
    static const auto result = []() -> bool {
#if FRAMELESSHELPER_CONFIG(native_impl)
        const FramelessConfig * const config = FramelessConfig::instance();
        if (config->isSet(Option::ForceShowWindowFrameBorder)) {
            return true;
        }
        if (config->isSet(Option::ForceHideWindowFrameBorder)) {
            return false;
        }
        return WindowsVersionHelper::isWin10OrGreater();
#else
        return false;
#endif
    }();
    return result;
}

bool Utils::isTitleBarColorized()
{
    // CHECK: is it supported on win7?
    if (!WindowsVersionHelper::isWin10OrGreater()) {
        return false;
    }
    const DwmColorizationArea area = getDwmColorizationArea();
    return ((area == DwmColorizationArea::TitleBar_WindowBorder) || (area == DwmColorizationArea::All));
}

bool Utils::isFrameBorderColorized()
{
    return isTitleBarColorized();
}

bool Utils::installWindowProcHook(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    const QObject *window = FramelessManagerPrivate::getWindow(windowId);
    Q_ASSERT(window);
    if (!window) {
        return false;
    }
    const FramelessDataPtr data = FramelessManagerPrivate::getData(window);
    Q_ASSERT(data);
    if (!data || data->frameless) {
        return false;
    }
    const UtilsWinExtraDataPtr extraData = tryGetExtraData(data, true);
    Q_ASSERT(extraData);
    if (!extraData) {
        return false;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    if (!extraData->qtWindowProc) {
        ::SetLastError(ERROR_SUCCESS);
        const auto qtWindowProc = reinterpret_cast<WNDPROC>(::GetWindowLongPtrW(hwnd, GWLP_WNDPROC));
        Q_ASSERT(qtWindowProc);
        if (!qtWindowProc) {
            WARNING << getSystemErrorMessage(kGetWindowLongPtrW);
            return false;
        }
        extraData->qtWindowProc = qtWindowProc;
    }
    if (!extraData->windowProcHooked) {
        ::SetLastError(ERROR_SUCCESS);
        if (::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(FramelessHelperHookWindowProc)) == 0) {
            WARNING << getSystemErrorMessage(kSetWindowLongPtrW);
            return false;
        }
        extraData->windowProcHooked = true;
    }
    return true;
}

bool Utils::uninstallWindowProcHook(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    const QObject *window = FramelessManagerPrivate::getWindow(windowId);
    if (!window) {
        return false;
    }
    const FramelessDataPtr data = FramelessManagerPrivate::getData(window);
    if (!data || !data->frameless) {
        return false;
    }
    const UtilsWinExtraDataPtr extraData = tryGetExtraData(data, false);
    if (!extraData || !extraData->windowProcHooked) {
        return false;
    }
    Q_ASSERT(extraData->qtWindowProc);
    if (!extraData->qtWindowProc) {
        return false;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    ::SetLastError(ERROR_SUCCESS);
    if (::SetWindowLongPtrW(hwnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(extraData->qtWindowProc)) == 0) {
        WARNING << getSystemErrorMessage(kSetWindowLongPtrW);
        return false;
    }
    extraData->qtWindowProc = nullptr;
    extraData->windowProcHooked = false;
    return true;
}

bool Utils::setAeroSnappingEnabled(const WId windowId, const bool enable)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    ::SetLastError(ERROR_SUCCESS);
    const auto oldWindowStyle = static_cast<DWORD>(::GetWindowLongPtrW(hwnd, GWL_STYLE));
    if (oldWindowStyle == 0) {
        WARNING << getSystemErrorMessage(kGetWindowLongPtrW);
        return false;
    }
    // The key is the existence of the "WS_THICKFRAME" flag.
    // But we should also disallow window maximize if Aero Snapping is disabled.
    static constexpr const DWORD resizableFlags = (WS_THICKFRAME | WS_MAXIMIZEBOX);
    const auto newWindowStyle = [enable, oldWindowStyle]() -> DWORD {
        if (enable) {
            return ((oldWindowStyle & ~WS_POPUP) | resizableFlags);
        } else {
            return ((oldWindowStyle & ~resizableFlags) | WS_POPUP);
        }
    }();
    ::SetLastError(ERROR_SUCCESS);
    if (::SetWindowLongPtrW(hwnd, GWL_STYLE, static_cast<LONG_PTR>(newWindowStyle)) == 0) {
        WARNING << getSystemErrorMessage(kSetWindowLongPtrW);
        return false;
    }
    return triggerFrameChange(windowId);
}

bool Utils::tryToEnableHighestDpiAwarenessLevel()
{
    bool isHighestAlready = false;
    const DpiAwareness currentAwareness = getDpiAwarenessForCurrentProcess(&isHighestAlready);
    DEBUG << "Current DPI awareness mode:" << currentAwareness;
    if (isHighestAlready) {
        return true;
    }
    if (API_USER_AVAILABLE(SetProcessDpiAwarenessContext)) {
        const auto SetProcessDpiAwarenessContext2 = [](const _DPI_AWARENESS_CONTEXT context) -> bool {
            Q_ASSERT(context);
            if (!context) {
                return false;
            }
            if (API_CALL_FUNCTION4(user32, SetProcessDpiAwarenessContext, context) != FALSE) {
                return true;
            }
            const DWORD dwError = ::GetLastError();
            // "ERROR_ACCESS_DENIED" means set externally (mostly due to manifest file).
            // Any attempt to change the DPI awareness mode through API will always fail,
            // so we treat this situation as succeeded.
            if (dwError == ERROR_ACCESS_DENIED) {
                DEBUG << kDpiNoAccessErrorMessage;
                return true;
            }
            WARNING << getSystemErrorMessageImpl(kSetProcessDpiAwarenessContext, dwError);
            return false;
        };
        if (currentAwareness == DpiAwareness::PerMonitorVersion2) {
            return true;
        }
        if (SetProcessDpiAwarenessContext2(_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)) {
            return true;
        }
        if (currentAwareness == DpiAwareness::PerMonitor) {
            return true;
        }
        if (SetProcessDpiAwarenessContext2(_DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE)) {
            return true;
        }
        if (currentAwareness == DpiAwareness::System) {
            return true;
        }
        if (SetProcessDpiAwarenessContext2(_DPI_AWARENESS_CONTEXT_SYSTEM_AWARE)) {
            return true;
        }
        if (currentAwareness == DpiAwareness::Unaware_GdiScaled) {
            return true;
        }
        if (SetProcessDpiAwarenessContext2(_DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED)) {
            return true;
        }
    }
    if (API_SHCORE_AVAILABLE(SetProcessDpiAwareness)) {
        const auto SetProcessDpiAwareness2 = [](const _PROCESS_DPI_AWARENESS pda) -> bool {
            const HRESULT hr = API_CALL_FUNCTION4(shcore, SetProcessDpiAwareness, pda);
            if (SUCCEEDED(hr)) {
                return true;
            }
            // "E_ACCESSDENIED" means set externally (mostly due to manifest file).
            // Any attempt to change the DPI awareness mode through API will always fail,
            // so we treat this situation as succeeded.
            if (hr == E_ACCESSDENIED) {
                DEBUG << kDpiNoAccessErrorMessage;
                return true;
            }
            WARNING << getSystemErrorMessageImpl(kSetProcessDpiAwareness, hr);
            return false;
        };
        if (currentAwareness == DpiAwareness::PerMonitorVersion2) {
            return true;
        }
        if (SetProcessDpiAwareness2(_PROCESS_PER_MONITOR_V2_DPI_AWARE)) {
            return true;
        }
        if (currentAwareness == DpiAwareness::PerMonitor) {
            return true;
        }
        if (SetProcessDpiAwareness2(_PROCESS_PER_MONITOR_DPI_AWARE)) {
            return true;
        }
        if (currentAwareness == DpiAwareness::System) {
            return true;
        }
        if (SetProcessDpiAwareness2(_PROCESS_SYSTEM_DPI_AWARE)) {
            return true;
        }
        if (currentAwareness == DpiAwareness::Unaware_GdiScaled) {
            return true;
        }
        if (SetProcessDpiAwareness2(_PROCESS_DPI_UNAWARE_GDISCALED)) {
            return true;
        }
    }
    // Some really old MinGW SDK may lack this function, we workaround this
    // issue by always load it dynamically at runtime.
    if (API_USER_AVAILABLE(SetProcessDPIAware)) {
        if (currentAwareness == DpiAwareness::System) {
            return true;
        }
        if (API_CALL_FUNCTION4(user32, SetProcessDPIAware) != FALSE) {
            return true;
        }
        WARNING << getSystemErrorMessage(kSetProcessDPIAware);
    }
    return false;
}

bool Utils::updateGlobalWin32ControlsTheme(const WId windowId, const bool dark)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    // There's no global dark theme for common Win32 controls before Win10 1809.
    if (!WindowsVersionHelper::isWin10RS5OrGreater()) {
        return false;
    }
    if (!API_THEME_AVAILABLE(SetWindowTheme)) {
        return false;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    const HRESULT hr = API_CALL_FUNCTION(uxtheme, SetWindowTheme, hwnd,
        (dark ? kSystemDarkThemeResourceName : kSystemLightThemeResourceName), nullptr);
    if (FAILED(hr)) {
        WARNING << getSystemErrorMessageImpl(kSetWindowTheme, hr);
        return false;
    }
    return true;
}

bool Utils::shouldAppsUseDarkMode_windows()
{
    // The global dark mode was first introduced in Windows 10 1607.
    if (!WindowsVersionHelper::isWin10RS1OrGreater() || isHighContrastModeEnabled()) {
        return false;
    }
#if FRAMELESSHELPER_CONFIG(private_qt)
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
    if (const auto app = qApp->nativeInterface<QNativeInterface::Private::QWindowsApplication>()) {
        return app->isDarkMode();
    } else {
        WARNING << "QWindowsApplication is not available.";
    }
#  elif (QT_VERSION >= QT_VERSION_CHECK(5, 15, 0))
    if (const auto ni = QGuiApplication::platformNativeInterface()) {
        return ni->property("darkMode").toBool();
    } else {
        WARNING << "Failed to retrieve the platform native interface.";
    }
#  else // (QT_VERSION < QT_VERSION_CHECK(5, 15, 0))
    // Qt gained the ability to detect the system dark mode setting only since 5.15.
    // We should detect it ourself on versions below that.
#  endif // (QT_VERSION >= QT_VERSION_CHECK(6, 2, 0))
#endif // FRAMELESSHELPER_CONFIG(private_qt)
    // Starting from Windows 10 1903, "ShouldAppsUseDarkMode()" (exported by UXTHEME.DLL,
    // ordinal number 132) always return "TRUE" (actually, a random non-zero number at
    // runtime), so we can't use it due to this unreliability. In this case, we just simply
    // read the user's setting from the registry instead, it's not elegant but at least
    // it works well.
    // However, reverse engineering of Win11's Task Manager reveals that Microsoft still
    // uses this function internally to determine the system theme, and the Task Manager
    // can correctly respond to the theme change event indeed. But strangely, I've checked
    // that it's still broken on Win11 22H2. What's going on here?
    if (WindowsVersionHelper::isWin10RS5OrGreater()
        && !WindowsVersionHelper::isWin1019H1OrGreater()) {
        return (_ShouldAppsUseDarkMode() != FALSE);
    }
    const auto resultFromRegistry = []() -> bool {
        const RegistryKey registry(RegistryRootKey::CurrentUser, personalizeRegistryKey());
        if (!registry.isValid()) {
            return false;
        }
        const DWORD value = registry.value<DWORD>(kAppsUseLightTheme).value_or(0);
        return (value == 0);
    };
    return resultFromRegistry();
}

bool Utils::setCornerStyleForWindow(const WId windowId, const WindowCornerStyle style)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    // We cannot change the window corner style until Windows 11.
    if (!WindowsVersionHelper::isWin11OrGreater()) {
        return false;
    }
    if (!API_DWM_AVAILABLE(DwmSetWindowAttribute)) {
        return false;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    const auto wcp = [style]() -> _DWM_WINDOW_CORNER_PREFERENCE {
        switch (style) {
        case WindowCornerStyle::Default:
            return _DWMWCP_DEFAULT;
        case WindowCornerStyle::Square:
            return _DWMWCP_DONOTROUND;
        case WindowCornerStyle::Round:
            return _DWMWCP_ROUND;
        }
        QT_WARNING_PUSH
        QT_WARNING_DISABLE_MSVC(4702)
        Q_UNREACHABLE_RETURN(_DWMWCP_DEFAULT);
        QT_WARNING_POP
    }();
    const HRESULT hr = API_CALL_FUNCTION(dwmapi, DwmSetWindowAttribute,
        hwnd, _DWMWA_WINDOW_CORNER_PREFERENCE, &wcp, sizeof(wcp));
    if (FAILED(hr)) {
        WARNING << getSystemErrorMessageImpl(kDwmSetWindowAttribute, hr);
        return false;
    }
    return true;
}

bool Utils::setBlurBehindWindowEnabled(const WId windowId, const BlurMode mode, const QColor &color)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    const QObject *window = FramelessManagerPrivate::getWindow(windowId);
    if (!window) {
        return false;
    }
    const FramelessDataPtr data = FramelessManagerPrivate::getData(window);
    if (!data) {
        return false;
    }
    const UtilsWinExtraDataPtr extraData = tryGetExtraData(data, false);
    // Don't assert here, the user may be using pure Qt solution.
    if (!extraData) {
        return false;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    if (WindowsVersionHelper::isWin8OrGreater()) {
        if (!(API_DWM_AVAILABLE(DwmSetWindowAttribute)
            && API_DWM_AVAILABLE(DwmExtendFrameIntoClientArea))) {
            WARNING << "Blur behind window is not available on current platform.";
            return false;
        }
        const auto restoreWindowFrameMargins = [windowId, &extraData]() -> void {
            extraData->mica = false;
            std::ignore = updateWindowFrameMargins(windowId, false);
        };
        static const auto userPreferredBlurMode = []() -> std::optional<BlurMode> {
            const QString option = qEnvironmentVariable("FRAMELESSHELPER_BLUR_MODE");
            if (option.isEmpty()) {
                return std::nullopt;
            }
            if (option.contains(FRAMELESSHELPER_STRING_LITERAL("MICAALT"), Qt::CaseInsensitive)) {
                return BlurMode::Windows_MicaAlt;
            }
            if (option.contains(FRAMELESSHELPER_STRING_LITERAL("MICA"), Qt::CaseInsensitive)) {
                return BlurMode::Windows_Mica;
            }
            if (option.contains(FRAMELESSHELPER_STRING_LITERAL("ACRYLIC"), Qt::CaseInsensitive)) {
                return BlurMode::Windows_Acrylic;
            }
            if (option.contains(FRAMELESSHELPER_STRING_LITERAL("AERO"), Qt::CaseInsensitive)) {
                return BlurMode::Windows_Aero;
            }
            return std::nullopt;
        }();
        static constexpr const auto kDefaultAcrylicOpacity = 0.8f;
        static const auto acrylicOpacity = []() -> float {
            const QString option = qEnvironmentVariable("FRAMELESSHELPER_ACRYLIC_OPACITY");
            if (option.isEmpty()) {
                return kDefaultAcrylicOpacity;
            }
            bool ok = false;
            const float num = option.toFloat(&ok);
            if (ok && !qIsNaN(num) && (num > float(0)) && (num < float(1))) {
                return num;
            }
            return kDefaultAcrylicOpacity;
        }();
        static const bool preferMicaAlt = (qEnvironmentVariableIntValue("FRAMELESSHELPER_PREFER_MICA_ALT") != 0);
        const auto recommendedBlurMode = [mode]() -> BlurMode {
            if ((mode == BlurMode::Disable) || (mode == BlurMode::Windows_Aero)) {
                return mode;
            }
            if (((mode == BlurMode::Windows_Mica) || (mode == BlurMode::Windows_MicaAlt))
                    && !WindowsVersionHelper::isWin11OrGreater()) {
                WARNING << "The Mica material is not supported on your system, fallback to the Acrylic blur instead...";
                if (WindowsVersionHelper::isWin10OrGreater()) {
                    return BlurMode::Windows_Acrylic;
                }
                WARNING << "The Acrylic blur is not supported on your system, fallback to the traditional DWM blur instead...";
                return BlurMode::Windows_Aero;
            }
            if ((mode == BlurMode::Windows_Acrylic) && !WindowsVersionHelper::isWin10OrGreater()) {
                WARNING << "The Acrylic blur is not supported on your system, fallback to the traditional DWM blur instead...";
                return BlurMode::Windows_Aero;
            }
            if (mode == BlurMode::Default) {
                if (WindowsVersionHelper::isWin11OrGreater()) {
                    return (preferMicaAlt ? BlurMode::Windows_MicaAlt : BlurMode::Windows_Mica);
                }
                if (WindowsVersionHelper::isWin10OrGreater()) {
                    return BlurMode::Windows_Acrylic;
                }
                return BlurMode::Windows_Aero;
            }
            QT_WARNING_PUSH
            QT_WARNING_DISABLE_MSVC(4702)
            Q_UNREACHABLE_RETURN(BlurMode::Default);
            QT_WARNING_POP
        }();
        const BlurMode blurMode = ((recommendedBlurMode == BlurMode::Disable) ? BlurMode::Disable : userPreferredBlurMode.value_or(recommendedBlurMode));
        if (blurMode == BlurMode::Disable) {
            bool result = true;
            if (WindowsVersionHelper::isWin1122H2OrGreater()) {
                const _DWM_SYSTEMBACKDROP_TYPE dwmsbt = _DWMSBT_NONE;
                const HRESULT hr = API_CALL_FUNCTION(dwmapi, DwmSetWindowAttribute,
                    hwnd, _DWMWA_SYSTEMBACKDROP_TYPE, &dwmsbt, sizeof(dwmsbt));
                if (FAILED(hr)) {
                    result = false;
                    WARNING << getSystemErrorMessageImpl(kDwmSetWindowAttribute, hr);
                }
            } else if (WindowsVersionHelper::isWin11OrGreater()) {
                const BOOL enable = FALSE;
                HRESULT hr = API_CALL_FUNCTION(dwmapi, DwmSetWindowAttribute,
                    hwnd, _DWMWA_MICA_EFFECT, &enable, sizeof(enable));
                if (FAILED(hr)) {
                    result = false;
                    WARNING << getSystemErrorMessageImpl(kDwmSetWindowAttribute, hr);
                }
            } else {
                ACCENT_POLICY policy;
                SecureZeroMemory(&policy, sizeof(policy));
                policy.dwAccentState = ACCENT_DISABLED;
                policy.dwAccentFlags = ACCENT_NONE;
                WINDOWCOMPOSITIONATTRIBDATA wcad;
                SecureZeroMemory(&wcad, sizeof(wcad));
                wcad.Attrib = WCA_ACCENT_POLICY;
                wcad.pvData = &policy;
                wcad.cbData = sizeof(policy);
                if (_SetWindowCompositionAttribute(hwnd, &wcad) == FALSE) {
                    result = false;
                    WARNING << getSystemErrorMessage(kSetWindowCompositionAttribute);
                }
            }
            if (WindowsVersionHelper::isWin11OrGreater()) {
                restoreWindowFrameMargins();
            }
            return result;
        } else {
            if ((blurMode == BlurMode::Windows_Mica) || (blurMode == BlurMode::Windows_MicaAlt)) {
                extraData->mica = true;
                // By giving a negative value, DWM will extend the window frame into the whole
                // client area. We need this step because the Mica material can only be applied
                // to the non-client area of a window. Without this step, you'll get a window
                // with a pure black background.
                // Actually disabling the redirection surface (by enabling WS_EX_NOREDIRECTIONBITMAP
                // when you call CreateWindow(), it won't have any effect if you set it after the
                // window has been created) can achieve the same effect with extending the window
                // frame, however, it will completely break GDI's rendering, so sadly we can't choose
                // this solution. But this can be used if you can make sure your application don't
                // use GDI at all, for example, you only use Direct3D to draw your window (like
                // UWP/WPF applications). And one additional note, it will also break OpenGL and Vulkan
                // due to they also use the legacy swap chain model. In theory you can try this flag
                // for Qt Quick applications when the rhi backend is Direct3D, however, some elements
                // will still be broken because Qt Quick still use GDI to render some native controls
                // such as the window menu.
                const MARGINS margins = {-1, -1, -1, -1};
                HRESULT hr = API_CALL_FUNCTION(dwmapi, DwmExtendFrameIntoClientArea, hwnd, &margins);
                if (SUCCEEDED(hr)) {
                    if (WindowsVersionHelper::isWin1122H2OrGreater()) {
                        const auto dwmsbt = (
                            ((blurMode == BlurMode::Windows_MicaAlt) || preferMicaAlt)
                                ? _DWMSBT_TABBEDWINDOW : _DWMSBT_MAINWINDOW);
                        hr = API_CALL_FUNCTION(dwmapi, DwmSetWindowAttribute, hwnd,
                            _DWMWA_SYSTEMBACKDROP_TYPE, &dwmsbt, sizeof(dwmsbt));
                        if (SUCCEEDED(hr)) {
                            return true;
                        } else {
                            WARNING << getSystemErrorMessageImpl(kDwmSetWindowAttribute, hr);
                        }
                    } else {
                        const BOOL enable = TRUE;
                        hr = API_CALL_FUNCTION(dwmapi, DwmSetWindowAttribute,
                            hwnd, _DWMWA_MICA_EFFECT, &enable, sizeof(enable));
                        if (SUCCEEDED(hr)) {
                            return true;
                        } else {
                            WARNING << getSystemErrorMessageImpl(kDwmSetWindowAttribute, hr);
                        }
                    }
                } else {
                    WARNING << getSystemErrorMessageImpl(kDwmExtendFrameIntoClientArea, hr);
                }
                restoreWindowFrameMargins();
            } else {
                ACCENT_POLICY policy;
                SecureZeroMemory(&policy, sizeof(policy));
                if (blurMode == BlurMode::Windows_Acrylic) {
                    policy.dwAccentState = ACCENT_ENABLE_ACRYLICBLURBEHIND;
                    policy.dwAccentFlags = ACCENT_ENABLE_ACRYLIC_WITH_LUMINOSITY;
                    const auto gradientColor = [&color]() -> QColor {
                        if (color.isValid()) {
                            return color;
                        }
                        QColor clr = ((FramelessManager::instance()->systemTheme() == SystemTheme::Dark) ? kDefaultSystemDarkColor : kDefaultSystemLightColor);
                        clr.setAlphaF(acrylicOpacity);
                        return clr;
                    }();
                    // This API expects the #AABBGGRR format.
                    policy.dwGradientColor = DWORD(qRgba(gradientColor.blue(),
                        gradientColor.green(), gradientColor.red(), gradientColor.alpha()));
                } else if (blurMode == BlurMode::Windows_Aero) {
                    policy.dwAccentState = ACCENT_ENABLE_BLURBEHIND;
                    policy.dwAccentFlags = ACCENT_NONE;
                } else {
                    QT_WARNING_PUSH
                    QT_WARNING_DISABLE_MSVC(4702)
                    Q_UNREACHABLE_RETURN(false);
                    QT_WARNING_POP
                }
                WINDOWCOMPOSITIONATTRIBDATA wcad;
                SecureZeroMemory(&wcad, sizeof(wcad));
                wcad.Attrib = WCA_ACCENT_POLICY;
                wcad.pvData = &policy;
                wcad.cbData = sizeof(policy);
                if (_SetWindowCompositionAttribute(hwnd, &wcad) != FALSE) {
                    if ((blurMode == BlurMode::Windows_Acrylic)
                        && !WindowsVersionHelper::isWin11OrGreater()) {
                        DEBUG << "Enabling the Acrylic blur for Win32 windows on Windows 10 "
                                 "is very buggy. The only recommended way by Microsoft is to "
                                 "use the XAML Island technology or use pure UWP instead. If "
                                 "you find your window becomes very laggy during moving and "
                                 "resizing, please disable the Acrylic blur immediately (or "
                                 "disable the transparent effect in your personalize settings).";
                    }
                    return true;
                }
                WARNING << getSystemErrorMessage(kSetWindowCompositionAttribute);
            }
        }
    } else {
        // We prefer to use "DwmEnableBlurBehindWindow" on Windows 7 because it behaves
        // better than the undocumented API.
        if (!API_DWM_AVAILABLE(DwmEnableBlurBehindWindow)) {
            WARNING << "Blur behind window is not available on current platform.";
            return false;
        }
        DWM_BLURBEHIND dwmbb;
        SecureZeroMemory(&dwmbb, sizeof(dwmbb));
        dwmbb.dwFlags = DWM_BB_ENABLE;
        dwmbb.fEnable = [mode]() -> BOOL {
            if (mode == BlurMode::Disable) {
                return FALSE;
            }
            if ((mode != BlurMode::Default) && (mode != BlurMode::Windows_Aero)) {
                WARNING << "The only supported blur mode on Windows 7 is the traditional DWM blur.";
            }
            return TRUE;
        }();
        const HRESULT hr = API_CALL_FUNCTION(dwmapi, DwmEnableBlurBehindWindow, hwnd, &dwmbb);
        if (SUCCEEDED(hr)) {
            return true;
        }
        WARNING << getSystemErrorMessageImpl(kDwmEnableBlurBehindWindow, hr);
    }
    return false;
}

QColor Utils::getAccentColor_windows()
{
    // According to my experiments, this AccentColor will be exactly the same with
    // ColorizationColor, what's the meaning of it? But Microsoft products
    // usually read this setting instead of using DwmGetColorizationColor(),
    // so we'd better also do the same thing.
    // There's no Windows API to get this value, so we can only read it
    // directly from the registry.
    const QColor alternative = getDwmColorizationColor();
    const RegistryKey registry(RegistryRootKey::CurrentUser, dwmRegistryKey());
    if (!registry.isValid()) {
        return alternative;
    }
    const QVariant value = registry.value(kAccentColor);
    if (!value.isValid()) {
        return alternative;
    }
    // The retrieved value is in the #AABBGGRR format, we need to
    // convert it to the #AARRGGBB format which Qt expects.
    const QColor abgr = QColor::fromRgba(qvariant_cast<DWORD>(value));
    if (!abgr.isValid()) {
        return alternative;
    }
    return QColor(abgr.blue(), abgr.green(), abgr.red(), abgr.alpha());
}

QString Utils::getWallpaperFilePath()
{
    wchar_t path[MAX_PATH] = {};
    if (::SystemParametersInfoW(SPI_GETDESKWALLPAPER, MAX_PATH, path, FALSE) == FALSE) {
        WARNING << getSystemErrorMessage(kSystemParametersInfoW);
        return {};
    }
    return QString::fromWCharArray(path);
}

WallpaperAspectStyle Utils::getWallpaperAspectStyle()
{
    static constexpr const auto defaultStyle = WallpaperAspectStyle::Fill;
    const RegistryKey registry(RegistryRootKey::CurrentUser, desktopRegistryKey());
    if (!registry.isValid()) {
        return defaultStyle;
    }
    const DWORD wallpaperStyle = registry.value<DWORD>(kWallpaperStyle).value_or(0);
    switch (wallpaperStyle) {
    case 0: {
        const DWORD tileWallpaper = registry.value<DWORD>(kTileWallpaper).value_or(0);
        if (tileWallpaper != 0) {
            return WallpaperAspectStyle::Tile;
        }
        return WallpaperAspectStyle::Center;
    }
    case 2:
        return WallpaperAspectStyle::Stretch; // Ignore aspect ratio to fill.
    case 6:
        return WallpaperAspectStyle::Fit; // Keep aspect ratio to fill, but don't expand/crop.
    case 10:
        return WallpaperAspectStyle::Fill; // Keep aspect ratio to fill, expand/crop if necessary.
    case 22:
        return WallpaperAspectStyle::Span; // ???
    default:
        return defaultStyle;
    }
}

bool Utils::isBlurBehindWindowSupported()
{
    static const auto result = []() -> bool {
        if (FramelessConfig::instance()->isSet(Option::ForceNativeBackgroundBlur)) {
            return true;
        }
        if (FramelessConfig::instance()->isSet(Option::ForceNonNativeBackgroundBlur)) {
            return false;
        }
        // Enabling Mica on Win11 make it very hard to hide the original three caption buttons,
        // and enabling Acrylic on Win10 makes the window very laggy during moving and resizing.
        //return WindowsVersionHelper::isWin10OrGreater();
        return false;
    }();
    return result;
}

bool Utils::hideOriginalTitleBarElements(const WId windowId, const bool disable)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    static constexpr const DWORD validBits = (WTNCA_NODRAWCAPTION | WTNCA_NODRAWICON | WTNCA_NOSYSMENU);
    const DWORD mask = (disable ? validBits : 0);
    const HRESULT hr = _SetWindowThemeNonClientAttributes(hwnd, mask, mask);
    if (FAILED(hr)) {
        WARNING << getSystemErrorMessageImpl(kSetWindowThemeAttribute, hr);
        return false;
    }
    return true;
}

bool Utils::setQtDarkModeAwareEnabled(const bool enable)
{
#if FRAMELESSHELPER_CONFIG(private_qt)
#  if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    // We'll call QPA functions, so we have to ensure that the QGuiApplication
    // instance has already been created and initialized, because the platform
    // integration infrastructure is created and maintained by QGuiApplication.
    if (!qGuiApp) {
        return false;
    }
    using App = QNativeInterface::Private::QWindowsApplication;
    if (const auto app = qApp->nativeInterface<App>()) {
        app->setDarkModeHandling([enable]() -> App::DarkModeHandling {
            if (!enable) {
                return {}; // Clear the flags.
            }
#    if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
            // Enabling the DarkModeWindowFrames flag will save us the call of the
            // DwmSetWindowAttribute function. Qt will adjust the non-client area
            // (title bar & frame border) automatically.
            // Enabling the DarkModeStyle flag will make Qt Widgets apply dark theme
            // automatically when the system is in dark mode, but before Qt6.5 it's
            // own dark theme is really broken, so don't use it before 6.5.
            // There's no global dark theme for Qt Quick applications, so setting this
            // flag has no effect for pure Qt Quick applications.
            return {App::DarkModeWindowFrames | App::DarkModeStyle};
#    else // (QT_VERSION < QT_VERSION_CHECK(6, 5, 0)) \
            // Don't try to use the broken dark theme for Qt Widgets applications. \
            // For Qt Quick applications this is also enough. There's no global dark \
            // theme for them anyway.
            return {App::DarkModeWindowFrames};
#    endif // (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
        }());
        return true;
    } else {
        WARNING << "QWindowsApplication is not available.";
        return false;
    }
#  else // (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    Q_UNUSED(enable);
    return true;
#  endif // (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#else // !FRAMELESSHELPER_CONFIG(private_qt)
    Q_UNUSED(enable);
    return true;
#endif // FRAMELESSHELPER_CONFIG(private_qt)
}

bool Utils::registerThemeChangeNotification()
{
    // On Windows we don't need to subscribe to the theme change event
    // manually. Windows will send the theme change notification to all
    // top level windows by default.
    return true;
}

bool Utils::refreshWin32ThemeResources(const WId windowId, const bool dark)
{
    // Code learned from the following repositories. Thank very much for their great effort!
    // https://github.com/ysc3839/win32-darkmode/blob/master/win32-darkmode/DarkMode.h
    // https://github.com/TortoiseGit/TortoiseGit/blob/master/src/TortoiseGitBlame/MainFrm.cpp
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    // We have no way to adjust such things until Win10 1809.
    if (!WindowsVersionHelper::isWin10RS5OrGreater()) {
        return false;
    }
    if (!API_DWM_AVAILABLE(DwmSetWindowAttribute)) {
        return false;
    }
    const auto hWnd = reinterpret_cast<HWND>(windowId);
    const DWORD borderFlag = (WindowsVersionHelper::isWin1020H1OrGreater()
        ? _DWMWA_USE_IMMERSIVE_DARK_MODE : _DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1);
    const BOOL darkFlag = (dark ? TRUE : FALSE);
    WINDOWCOMPOSITIONATTRIBDATA wcad;
    SecureZeroMemory(&wcad, sizeof(wcad));
    wcad.Attrib = WCA_USEDARKMODECOLORS;
    wcad.pvData = const_cast<BOOL *>(&darkFlag);
    wcad.cbData = sizeof(darkFlag);
    if (dark) {
        if (_AllowDarkModeForWindow(hWnd, darkFlag) == FALSE) {
            WARNING << getSystemErrorMessage(kAllowDarkModeForWindow);
        }
        if (WindowsVersionHelper::isWin1019H1OrGreater()) {
            if (_SetWindowCompositionAttribute(hWnd, &wcad) == FALSE) {
                WARNING << getSystemErrorMessage(kSetWindowCompositionAttribute);
            }
        } else {
            if (::SetPropW(hWnd, kDarkModePropertyName,
                reinterpret_cast<HANDLE>(static_cast<INT_PTR>(darkFlag))) == FALSE) {
                WARNING << getSystemErrorMessage(kSetPropW);
            }
        }
        const HRESULT hr = API_CALL_FUNCTION(dwmapi, DwmSetWindowAttribute, hWnd, borderFlag, &darkFlag, sizeof(darkFlag));
        if (FAILED(hr)) {
            WARNING << getSystemErrorMessageImpl(kDwmSetWindowAttribute, hr);
        }
        ::SetLastError(ERROR_SUCCESS);
        _FlushMenuThemes();
        if (GetLastError() != ERROR_SUCCESS) {
            WARNING << getSystemErrorMessage(kFlushMenuThemes);
        }
        ::SetLastError(ERROR_SUCCESS);
        _RefreshImmersiveColorPolicyState();
        if (::GetLastError() != ERROR_SUCCESS) {
            WARNING << getSystemErrorMessage(kRefreshImmersiveColorPolicyState);
        }
    } else {
        if (_AllowDarkModeForWindow(hWnd, darkFlag) == FALSE) {
            WARNING << getSystemErrorMessage(kAllowDarkModeForWindow);
        }
        if (WindowsVersionHelper::isWin1019H1OrGreater()) {
            if (_SetWindowCompositionAttribute(hWnd, &wcad) == FALSE) {
                WARNING << getSystemErrorMessage(kSetWindowCompositionAttribute);
            }
        } else {
            if (::SetPropW(hWnd, kDarkModePropertyName,
                reinterpret_cast<HANDLE>(static_cast<INT_PTR>(darkFlag))) == FALSE) {
                WARNING << getSystemErrorMessage(kSetPropW);
            }
        }
        const HRESULT hr = API_CALL_FUNCTION(dwmapi, DwmSetWindowAttribute, hWnd, borderFlag, &darkFlag, sizeof(darkFlag));
        if (FAILED(hr)) {
            WARNING << getSystemErrorMessageImpl(kDwmSetWindowAttribute, hr);
        }
        ::SetLastError(ERROR_SUCCESS);
        _FlushMenuThemes();
        if (::GetLastError() != ERROR_SUCCESS) {
            WARNING << getSystemErrorMessage(kFlushMenuThemes);
        }
        ::SetLastError(ERROR_SUCCESS);
        _RefreshImmersiveColorPolicyState();
        if (::GetLastError() != ERROR_SUCCESS) {
            WARNING << getSystemErrorMessage(kRefreshImmersiveColorPolicyState);
        }
    }
    return true;
}

bool Utils::enableNonClientAreaDpiScalingForWindow(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    if (!API_USER_AVAILABLE(EnableNonClientDpiScaling)) {
        return false;
    }
    // The PMv2 DPI awareness mode will take care of it for us.
    if (getDpiAwarenessForCurrentProcess() == DpiAwareness::PerMonitorVersion2) {
        return true;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    if (API_CALL_FUNCTION4(user32, EnableNonClientDpiScaling, hwnd) == FALSE) {
        WARNING << getSystemErrorMessage(kEnableNonClientDpiScaling);
        return false;
    }
    return true;
}

DpiAwareness Utils::getDpiAwarenessForCurrentProcess(bool *highest)
{
    if ((API_USER_AVAILABLE(GetDpiAwarenessContextForProcess)
         || API_USER_AVAILABLE(GetThreadDpiAwarenessContext))
        && API_USER_AVAILABLE(AreDpiAwarenessContextsEqual)
        && API_USER_AVAILABLE(GetAwarenessFromDpiAwarenessContext)) {
        const auto context = []() -> _DPI_AWARENESS_CONTEXT {
            if (API_USER_AVAILABLE(GetDpiAwarenessContextForProcess)) {
                const HANDLE process = ::GetCurrentProcess();
                if (process) {
                    const _DPI_AWARENESS_CONTEXT result = API_CALL_FUNCTION4(user32, GetDpiAwarenessContextForProcess, process);
                    if (result) {
                        return result;
                    }
                    WARNING << getSystemErrorMessage(kGetDpiAwarenessContextForProcess);
                } else {
                    WARNING << getSystemErrorMessage(kGetCurrentProcess);
                }
            }
            const _DPI_AWARENESS_CONTEXT result = API_CALL_FUNCTION4(user32, GetThreadDpiAwarenessContext);
            if (result) {
                return result;
            }
            WARNING << getSystemErrorMessage(kGetThreadDpiAwarenessContext);
            return nullptr;
        }();
        if (!context) {
            return DpiAwareness::Unknown;
        }
        auto result = DpiAwareness::Unknown;
        // We have to use another API to compare PMv2 and GdiScaled because it seems the
        // GetAwarenessFromDpiAwarenessContext() function won't give us these two values.
        if (API_CALL_FUNCTION4(user32, AreDpiAwarenessContextsEqual, context,
                _DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2) != FALSE) {
            result = DpiAwareness::PerMonitorVersion2;
        } else if (API_CALL_FUNCTION4(user32, AreDpiAwarenessContextsEqual, context,
                          _DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED) != FALSE) {
            result = DpiAwareness::Unaware_GdiScaled;
        } else {
            const _DPI_AWARENESS awareness = API_CALL_FUNCTION4(user32, GetAwarenessFromDpiAwarenessContext, context);
            switch (awareness) {
            case _DPI_AWARENESS_INVALID:
                break;
            case _DPI_AWARENESS_UNAWARE:
                result = DpiAwareness::Unaware;
                break;
            case _DPI_AWARENESS_SYSTEM_AWARE:
                result = DpiAwareness::System;
                break;
            case _DPI_AWARENESS_PER_MONITOR_AWARE:
                result = DpiAwareness::PerMonitor;
                break;
            case _DPI_AWARENESS_PER_MONITOR_V2_AWARE:
                result = DpiAwareness::PerMonitorVersion2;
                break;
            case _DPI_AWARENESS_UNAWARE_GDISCALED:
                result = DpiAwareness::Unaware_GdiScaled;
                break;
            }
        }
        if (highest) {
            *highest = (result == DpiAwareness::PerMonitorVersion2);
        }
        return result;
    }
    if (API_SHCORE_AVAILABLE(GetProcessDpiAwareness)) {
        _PROCESS_DPI_AWARENESS pda = _PROCESS_DPI_UNAWARE;
        const HRESULT hr = API_CALL_FUNCTION4(shcore, GetProcessDpiAwareness, nullptr, &pda);
        if (FAILED(hr)) {
            WARNING << getSystemErrorMessageImpl(kGetProcessDpiAwareness, hr);
            return DpiAwareness::Unknown;
        }
        auto result = DpiAwareness::Unknown;
        switch (pda) {
        case _PROCESS_DPI_UNAWARE:
            result = DpiAwareness::Unaware;
            break;
        case _PROCESS_SYSTEM_DPI_AWARE:
            result = DpiAwareness::System;
            break;
        case _PROCESS_PER_MONITOR_DPI_AWARE:
            result = DpiAwareness::PerMonitor;
            break;
        case _PROCESS_PER_MONITOR_V2_DPI_AWARE:
            result = DpiAwareness::PerMonitorVersion2;
            break;
        case _PROCESS_DPI_UNAWARE_GDISCALED:
            result = DpiAwareness::Unaware_GdiScaled;
            break;
        }
        if (highest) {
            *highest = (result == DpiAwareness::PerMonitor);
        }
        return result;
    }
    if (API_USER_AVAILABLE(IsProcessDPIAware)) {
        const BOOL isAware = API_CALL_FUNCTION(user32, IsProcessDPIAware);
        const auto result = ((isAware == FALSE) ? DpiAwareness::Unaware : DpiAwareness::System);
        if (highest) {
            *highest = (result == DpiAwareness::System);
        }
        return result;
    }
    return DpiAwareness::Unknown;
}

bool Utils::fixupChildWindowsDpiMessage(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    // This hack is only available on Windows 10 and newer, and starting from
    // Win10 build 14986 it become useless due to the PMv2 DPI awareness mode
    // already takes care of it for us.
    if (!WindowsVersionHelper::isWin10OrGreater()
        || (WindowsVersionHelper::isWin10RS2OrGreater()
            && (getDpiAwarenessForCurrentProcess() == DpiAwareness::PerMonitorVersion2))) {
        return true;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    if (_EnableChildWindowDpiMessage2(hwnd, TRUE) != FALSE) {
        return true;
    }
    // This API is not available on current platform, it's fine.
    if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED) {
        return true;
    }
    WARNING << getSystemErrorMessage(kEnableChildWindowDpiMessage);
    return false;
}

bool Utils::fixupDialogsDpiScaling()
{
    // This hack is only available on Windows 10 and newer, and starting from
    // Win10 build 14986 it become useless due to the PMv2 DPI awareness mode
    // already takes care of it for us.
    if (!WindowsVersionHelper::isWin10OrGreater()
        || (WindowsVersionHelper::isWin10RS2OrGreater()
            && (getDpiAwarenessForCurrentProcess() == DpiAwareness::PerMonitorVersion2))) {
        return true;
    }
    if (_EnablePerMonitorDialogScaling2() != FALSE) {
        return true;
    }
    // This API is not available on current platform, it's fine.
    if (::GetLastError() == ERROR_CALL_NOT_IMPLEMENTED) {
        return true;
    }
    WARNING << getSystemErrorMessage(kEnablePerMonitorDialogScaling);
    return false;
}

bool Utils::setDarkModeAllowedForApp(const bool allow)
{
    // This hack is only available since Win10 1809.
    if (!WindowsVersionHelper::isWin10RS5OrGreater()) {
        return false;
    }
    // This hack is necessary to let AllowDarkModeForWindow() work ...
    if (WindowsVersionHelper::isWin1019H1OrGreater()) {
        if (_SetPreferredAppMode(allow ? PAM_AUTO : PAM_DEFAULT) == PAM_MAX) {
            WARNING << getSystemErrorMessage(kSetPreferredAppMode);
            return false;
        }
    } else {
        if (_AllowDarkModeForApp(allow ? TRUE : FALSE) == FALSE) {
            WARNING << getSystemErrorMessage(kAllowDarkModeForApp);
            return false;
        }
    }
    return true;
}

bool Utils::bringWindowToFront(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    const HWND oldForegroundWindow = ::GetForegroundWindow();
    if (!oldForegroundWindow) {
        // The foreground window can be NULL, it's not an API error.
        return true;
    }
    const std::optional<MONITORINFOEXW> activeMonitor = getMonitorForWindow(oldForegroundWindow);
    if (!activeMonitor.has_value()) {
        WARNING << "Failed to retrieve the window's monitor.";
        return false;
    }
    // We need to show the window first, otherwise we won't be able to bring it to front.
    if (::IsWindowVisible(hwnd) == FALSE) {
        ::ShowWindow(hwnd, SW_SHOW);
    }
    if (IsMinimized(hwnd)) {
        // Restore the window if it is minimized.
        ::ShowWindow(hwnd, SW_RESTORE);
        // Once we've been restored, throw us on the active monitor.
        // When the window is restored, it will always become the foreground window.
        // So return early here, we don't need the following code to bring it to front.
        return moveWindowToMonitor(hwnd, activeMonitor.value());
    }
    // OK, our window is not minimized, so now we will try to bring it to front manually.
    // First try to send a message to the current foreground window to check whether
    // it is currently hanging or not.
    static constexpr const UINT kTimeout = 1000;
    if (::SendMessageTimeoutW(oldForegroundWindow, WM_NULL, 0, 0,
            SMTO_BLOCK | SMTO_ABORTIFHUNG | SMTO_NOTIMEOUTIFNOTHUNG, kTimeout, nullptr) == 0) {
        if (::GetLastError() == ERROR_TIMEOUT) {
            WARNING << "The foreground window hangs, can't activate current window.";
        } else {
            WARNING << getSystemErrorMessage(kSendMessageTimeoutW);
        }
        return false;
    }
    const DWORD windowThreadProcessId = ::GetWindowThreadProcessId(oldForegroundWindow, nullptr);
    const DWORD currentThreadId = ::GetCurrentThreadId();
    // We won't be able to change a window's Z order if it's not our own window,
    // so we use this small technique to pretend the foreground window is ours.
    if (::AttachThreadInput(windowThreadProcessId, currentThreadId, TRUE) == FALSE) {
        WARNING << getSystemErrorMessage(kAttachThreadInput);
        return false;
    }
    // And also don't forget to disconnect from it.
    volatile const auto cleanup = qScopeGuard([windowThreadProcessId, currentThreadId]() -> void {
        if (::AttachThreadInput(windowThreadProcessId, currentThreadId, FALSE) == FALSE) {
            WARNING << getSystemErrorMessage(kAttachThreadInput);
        }
    });
    Q_UNUSED(cleanup);
    // Make our window be the first one in the Z order.
    if (::BringWindowToTop(hwnd) == FALSE) {
        WARNING << getSystemErrorMessage(kBringWindowToTop);
        return false;
    }
    // Activate the window too. This will force us to the virtual desktop this
    // window is on, if it's on another virtual desktop.
    if (::SetActiveWindow(hwnd) == nullptr) {
        WARNING << getSystemErrorMessage(kSetActiveWindow);
        return false;
    }
    // Throw us on the active monitor.
    return moveWindowToMonitor(hwnd, activeMonitor.value());
}

QPoint Utils::getWindowPlacementOffset(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return {};
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    ::SetLastError(ERROR_SUCCESS);
    const auto exStyle = static_cast<DWORD>(::GetWindowLongPtrW(hwnd, GWL_EXSTYLE));
    if (exStyle == 0) {
        WARNING << getSystemErrorMessage(kGetWindowLongPtrW);
        return {};
    }
    // Tool windows are special and they don't need any offset.
    if (exStyle & WS_EX_TOOLWINDOW) {
        return {};
    }
    const std::optional<MONITORINFOEXW> mi = getMonitorForWindow(hwnd);
    if (!mi.has_value()) {
        WARNING << "Failed to retrieve the window's monitor.";
        return {};
    }
    const RECT work = mi.value().rcWork;
    const RECT total = mi.value().rcMonitor;
    return {work.left - total.left, work.top - total.top};
}

QRect Utils::getWindowRestoreGeometry(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return {};
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    WINDOWPLACEMENT wp;
    SecureZeroMemory(&wp, sizeof(wp));
    wp.length = sizeof(wp);
    if (::GetWindowPlacement(hwnd, &wp) == FALSE) {
        WARNING << getSystemErrorMessage(kGetWindowPlacement);
        return {};
    }
    return rect2qrect(wp.rcNormalPosition).translated(getWindowPlacementOffset(windowId));
}

quint64 Utils::getKeyState()
{
    quint64 result = 0;
    const auto get = [](const int virtualKey) -> bool {
        return (::GetAsyncKeyState(virtualKey) < 0);
    };
    const bool buttonSwapped = (::GetSystemMetrics(SM_SWAPBUTTON) != FALSE);
    if (get(VK_LBUTTON)) {
        result |= (buttonSwapped ? MK_RBUTTON : MK_LBUTTON);
    }
    if (get(VK_RBUTTON)) {
        result |= (buttonSwapped ? MK_LBUTTON : MK_RBUTTON);
    }
    if (get(VK_SHIFT)) {
        result |= MK_SHIFT;
    }
    if (get(VK_CONTROL)) {
        result |= MK_CONTROL;
    }
    if (get(VK_MBUTTON)) {
        result |= MK_MBUTTON;
    }
    if (get(VK_XBUTTON1)) {
        result |= MK_XBUTTON1;
    }
    if (get(VK_XBUTTON2)) {
        result |= MK_XBUTTON2;
    }
    return result;
}

bool Utils::isValidWindow(const WId windowId, const bool checkVisible, const bool checkTopLevel)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    if (::IsWindow(hwnd) == FALSE) {
        return false;
    }
    const LONG_PTR styles = ::GetWindowLongPtrW(hwnd, GWL_STYLE);
    if ((styles == 0) || (styles & WS_DISABLED)) {
        return false;
    }
    const LONG_PTR exStyles = ::GetWindowLongPtrW(hwnd, GWL_EXSTYLE);
    if ((exStyles == 0) || (exStyles & WS_EX_TOOLWINDOW)) {
        return false;
    }
    RECT rect = { 0, 0, 0, 0 };
    if (::GetWindowRect(hwnd, &rect) == FALSE) {
        return false;
    }
    if ((rect.left >= rect.right) || (rect.top >= rect.bottom)) {
        return false;
    }
    if (checkVisible) {
        if (::IsWindowVisible(hwnd) == FALSE) {
            return false;
        }
    }
    if (checkTopLevel) {
        if (::GetAncestor(hwnd, GA_ROOT) != hwnd) {
            return false;
        }
    }
    return true;
}

bool Utils::updateFramebufferTransparency(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return false;
    }
    if (!API_DWM_AVAILABLE(DwmEnableBlurBehindWindow)) {
        return false;
    }
    // DwmEnableBlurBehindWindow() won't be functional if DWM composition
    // is not enabled, so we bail out early if this is the case.
    if (!isDwmCompositionEnabled()) {
        return false;
    }
    const auto hwnd = reinterpret_cast<HWND>(windowId);
    bool opaque = false;
    bool ok = false;
    std::ignore = getDwmColorizationColor(&opaque, &ok);
    if (WindowsVersionHelper::isWin8OrGreater() || (ok && !opaque)) {
        DWM_BLURBEHIND bb;
        SecureZeroMemory(&bb, sizeof(bb));
        bb.dwFlags = (DWM_BB_ENABLE | DWM_BB_BLURREGION);
        bb.hRgnBlur = ::CreateRectRgn(0, 0, -1, -1);
        bb.fEnable = TRUE;
        const HRESULT hr = API_CALL_FUNCTION(dwmapi, DwmEnableBlurBehindWindow, hwnd, &bb);
        if (bb.hRgnBlur) {
            if (::DeleteObject(bb.hRgnBlur) == FALSE) {
                WARNING << getSystemErrorMessage(kDeleteObject);
            }
        }
        if (FAILED(hr)) {
            WARNING << getSystemErrorMessageImpl(kDwmEnableBlurBehindWindow, hr);
            return false;
        }
    } else {
        // HACK: Disable framebuffer transparency on Windows 7 when the
        //       colorization color is opaque, because otherwise the window
        //       contents is blended additively with the previous frame instead
        //       of replacing it
        DWM_BLURBEHIND bb;
        SecureZeroMemory(&bb, sizeof(bb));
        bb.dwFlags = DWM_BB_ENABLE;
        bb.fEnable = FALSE;
        const HRESULT hr = API_CALL_FUNCTION(dwmapi, DwmEnableBlurBehindWindow, hwnd, &bb);
        if (FAILED(hr)) {
            WARNING << getSystemErrorMessageImpl(kDwmEnableBlurBehindWindow, hr);
            return false;
        }
    }
    return true;
}

QMargins Utils::getWindowSystemFrameMargins(const WId windowId)
{
    Q_ASSERT(windowId);
    if (!windowId) {
        return {};
    }
    const auto horizontalMargin = int(getResizeBorderThickness(windowId, true, true));
    const auto verticalMargin = int(getResizeBorderThickness(windowId, false, true));
    return QMargins{ horizontalMargin, verticalMargin, horizontalMargin, verticalMargin };
}

QMargins Utils::getWindowCustomFrameMargins(const QWindow *window)
{
    Q_ASSERT(window);
    if (!window) {
        return {};
    }
#if FRAMELESSHELPER_CONFIG(private_qt)
#  if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    if (QPlatformWindow *platformWindow = window->handle()) {
        if (const auto ni = QGuiApplication::platformNativeInterface()) {
            const QVariant marginsVar = ni->windowProperty(platformWindow, qtWindowCustomMarginsProp());
            if (marginsVar.isValid() && !marginsVar.isNull()) {
                return qvariant_cast<QMargins>(marginsVar);
            }
        } else {
            WARNING << "Failed to retrieve the platform native interface.";
        }
    } else {
        WARNING << "Failed to retrieve the platform window.";
    }
#  else // (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    if (const auto platformWindow = dynamic_cast<QNativeInterface::Private::QWindowsWindow *>(window->handle())) {
        return platformWindow->customMargins();
    } else {
        WARNING << "Failed to retrieve the platform window.";
    }
#  endif // (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#endif // FRAMELESSHELPER_CONFIG(private_qt)
    const QVariant marginsVar = window->property(kQtWindowCustomMarginsVar);
    if (marginsVar.isValid() && !marginsVar.isNull()) {
        return qvariant_cast<QMargins>(marginsVar);
    }
    return {};
}

bool Utils::updateAllDirectXSurfaces()
{
    if (!API_DWM_AVAILABLE(DwmFlush)) {
        return false;
    }
    const HRESULT hr = API_CALL_FUNCTION(dwmapi, DwmFlush);
    if (FAILED(hr)) {
        WARNING << getSystemErrorMessageImpl(kDwmFlush, hr);
        return false;
    }
    return true;
}

void Utils::printWin32Message(void *msgPtr)
{
    Q_ASSERT(msgPtr);
    if (!msgPtr) {
        return;
    }
    const auto msg = static_cast<const MSG *>(msgPtr);
    const HWND hWnd = msg->hwnd;
    const UINT uMsg = msg->message;
    const WPARAM wParam = msg->wParam;
    const LPARAM lParam = msg->lParam;
    const QString messageCodeHex = FRAMELESSHELPER_STRING_LITERAL("0x") + QString::number(uMsg, 16).toUpper().rightJustified(4, u'0');
    QString text = {};
    QTextStream stream(&text, QIODevice::WriteOnly);
    stream << "Windows message received: window handle: " << hwnd2str(hWnd) << ", message: ";
    if (uMsg >= WM_APP) {
        const UINT diff = (uMsg - WM_APP);
        stream << "WM_APP + " << diff << " (" << messageCodeHex
               << ") [private message owned by current application]";
    } else if (uMsg >= WM_USER) {
        const UINT diff = (uMsg - WM_USER);
        stream << "WM_USER + " << diff << " (" << messageCodeHex
               << ") [private message owned by all kinds of controls]";
    } else {
        const auto it = std::find(g_win32MessageMap.cbegin(), g_win32MessageMap.cend(), Win32Message{ uMsg, nullptr });
        if (it == g_win32MessageMap.cend()) {
            stream << "UNKNOWN";
        } else {
            stream << it->Str;
        }
        stream << " (" << messageCodeHex << ')';
        auto screenPos = POINT{ 0, 0 };
        auto clientPos = POINT{ 0, 0 };
        bool isNonClientMouseMessage = false;
        if (isMouseMessage(uMsg, &isNonClientMouseMessage)) {
            if (isNonClientMouseMessage) {
                screenPos = [uMsg, lParam]() -> POINT {
                    if (uMsg == WM_NCMOUSELEAVE) {
                        const DWORD dwScreenPos = ::GetMessagePos();
                        return POINT{ GET_X_LPARAM(dwScreenPos), GET_Y_LPARAM(dwScreenPos) };
                    } else {
                        return POINT{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                    }
                }();
                clientPos = screenPos;
                if (::ScreenToClient(hWnd, &clientPos) == FALSE) {
                    WARNING << Utils::getSystemErrorMessage(kScreenToClient);
                    clientPos = {};
                }
            } else {
                if (uMsg == WM_MOUSELEAVE) {
                    const DWORD dwScreenPos = ::GetMessagePos();
                    screenPos = POINT{ GET_X_LPARAM(dwScreenPos), GET_Y_LPARAM(dwScreenPos) };
                    clientPos = screenPos;
                    if (::ScreenToClient(hWnd, &clientPos) == FALSE) {
                        WARNING << Utils::getSystemErrorMessage(kScreenToClient);
                        clientPos = {};
                    }
                } else {
                    clientPos = POINT{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                    screenPos = clientPos;
                    if (::ClientToScreen(hWnd, &screenPos) == FALSE) {
                        WARNING << Utils::getSystemErrorMessage(kClientToScreen);
                        screenPos = {};
                    }
                }
            }
            stream << ", screen coordinate: POINT(x: " << screenPos.x << ", y: "
                   << screenPos.y << "), client coordinate: POINT(x: "
                   << clientPos.x << ", y: " << clientPos.y << ')';
        } else {
            stream << ", wParam: " << wParam << ", lParam: " << lParam;
        }
    }
    DEBUG.noquote() << text;
}

FRAMELESSHELPER_END_NAMESPACE

#endif // Q_OS_WINDOWS
