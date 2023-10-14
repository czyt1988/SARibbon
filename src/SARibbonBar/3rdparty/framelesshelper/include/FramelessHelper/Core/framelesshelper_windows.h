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

// Copyright (C) Microsoft Corporation.
// Licensed under the MIT license.

#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#  define WIN32_LEAN_AND_MEAN
#endif

#ifndef WINRT_LEAN_AND_MEAN
#  define WINRT_LEAN_AND_MEAN
#endif

#ifndef UNICODE
#  define UNICODE
#endif

#ifndef _UNICODE
#  define _UNICODE
#endif

#ifndef _CRT_NON_CONFORMING_SWPRINTFS
#  define _CRT_NON_CONFORMING_SWPRINTFS
#endif

#ifndef _CRT_SECURE_NO_WARNINGS
#  define _CRT_SECURE_NO_WARNINGS
#endif

#ifndef NOMINMAX
#  define NOMINMAX
#endif

#if __has_include(<sdkddkver.h>)
#  include <sdkddkver.h>
#endif

#ifndef _WIN32_WINNT_WIN10
#  define _WIN32_WINNT_WIN10 0x0A00
#endif

#ifndef NTDDI_WIN10_NI
#  define NTDDI_WIN10_NI 0x0A00000C
#endif

#ifndef WINVER
#  define WINVER _WIN32_WINNT_WIN10
#endif

#ifndef _WIN32_WINNT
#  define _WIN32_WINNT _WIN32_WINNT_WIN10
#endif

#ifndef NTDDI_VERSION
#  define NTDDI_VERSION NTDDI_WIN10_NI
#endif

#include <windows.h>
#include <uxtheme.h>
#include <shellapi.h>
#include <dwmapi.h>

#include <FramelessHelper/Core/framelesshelpercore_global.h>

#ifndef SC_SIZE
#  define SC_SIZE (0xF000)
#endif

#ifndef SC_SIZELEFT
#  define SC_SIZELEFT (0xF001)
#endif

#ifndef SC_SIZERIGHT
#  define SC_SIZERIGHT (0xF002)
#endif

#ifndef SC_SIZETOP
#  define SC_SIZETOP (0xF003)
#endif

#ifndef SC_SIZETOPLEFT
#  define SC_SIZETOPLEFT (0xF004)
#endif

#ifndef SC_SIZETOPRIGHT
#  define SC_SIZETOPRIGHT (0xF005)
#endif

#ifndef SC_SIZEBOTTOM
#  define SC_SIZEBOTTOM (0xF006)
#endif

#ifndef SC_SIZEBOTTOMLEFT
#  define SC_SIZEBOTTOMLEFT (0xF007)
#endif

#ifndef SC_SIZEBOTTOMRIGHT
#  define SC_SIZEBOTTOMRIGHT (0xF008)
#endif

#ifndef SC_DRAGMOVE
#  define SC_DRAGMOVE (0xF012)
#endif

#ifndef WM_SIZEWAIT
#  define WM_SIZEWAIT (0x0004)
#endif

#ifndef WM_SETVISIBLE
#  define WM_SETVISIBLE (0x0009)
#endif

#ifndef WM_SYSTEMERROR
#  define WM_SYSTEMERROR (0x0017)
#endif

#ifndef WM_CTLCOLOR
#  define WM_CTLCOLOR (0x0019)
#endif

#ifndef WM_LOGOFF
#  define WM_LOGOFF (0x0025)
#endif

#ifndef WM_ALTTABACTIVE
#  define WM_ALTTABACTIVE (0x0029)
#endif

#ifndef WM_SHELLNOTIFY
#  define WM_SHELLNOTIFY (0x0034)
#endif

#ifndef WM_ISACTIVEICON
#  define WM_ISACTIVEICON (0x0035)
#endif

#ifndef WM_QUERYPARKICON
#  define WM_QUERYPARKICON (0x0036)
#endif

#ifndef WM_WINHELP
#  define WM_WINHELP (0x0038)
#endif

#ifndef WM_FULLSCREEN
#  define WM_FULLSCREEN (0x003A)
#endif

#ifndef WM_CLIENTSHUTDOWN
#  define WM_CLIENTSHUTDOWN (0x003B)
#endif

#ifndef WM_DDEMLEVENT
#  define WM_DDEMLEVENT (0x003C)
#endif

#ifndef WM_TESTING
#  define WM_TESTING (0x0040)
#endif

#ifndef WM_OTHERWINDOWCREATED
#  define WM_OTHERWINDOWCREATED (0x0042)
#endif

#ifndef WM_OTHERWINDOWDESTROYED
#  define WM_OTHERWINDOWDESTROYED (0x0043)
#endif

#ifndef WM_COPYGLOBALDATA
#  define WM_COPYGLOBALDATA (0x0049)
#endif

#ifndef WM_KEYF1
#  define WM_KEYF1 (0x004D)
#endif

#ifndef WM_ACCESS_WINDOW
#  define WM_ACCESS_WINDOW (0x004F)
#endif

#ifndef WM_FINALDESTROY
#  define WM_FINALDESTROY (0x0070)
#endif

#ifndef WM_MEASUREITEM_CLIENTDATA
#  define WM_MEASUREITEM_CLIENTDATA (0x0071)
#endif

#ifndef WM_SYNCTASK
#  define WM_SYNCTASK (0x0089)
#endif

#ifndef WM_KLUDGEMINRECT
#  define WM_KLUDGEMINRECT (0x008B)
#endif

#ifndef WM_LPKDRAWSWITCHWND
#  define WM_LPKDRAWSWITCHWND (0x008C)
#endif

#ifndef WM_UAHDESTROYWINDOW
#  define WM_UAHDESTROYWINDOW (0x0090)
#endif

#ifndef WM_UAHDRAWMENU
#  define WM_UAHDRAWMENU (0x0091)
#endif

#ifndef WM_UAHDRAWMENUITEM
#  define WM_UAHDRAWMENUITEM (0x0092)
#endif

#ifndef WM_UAHINITMENU
#  define WM_UAHINITMENU (0x0093)
#endif

#ifndef WM_UAHMEASUREMENUITEM
#  define WM_UAHMEASUREMENUITEM (0x0094)
#endif

#ifndef WM_UAHNCPAINTMENUPOPUP
#  define WM_UAHNCPAINTMENUPOPUP (0x0095)
#endif

#ifndef WM_UAHUPDATE
#  define WM_UAHUPDATE (0x0096)
#endif

#ifndef WM_NCUAHDRAWCAPTION
#  define WM_NCUAHDRAWCAPTION (0x00AE)
#endif

#ifndef WM_NCUAHDRAWFRAME
#  define WM_NCUAHDRAWFRAME (0x00AF)
#endif

#ifndef WM_YOMICHAR
#  define WM_YOMICHAR (0x0108)
#endif

#ifndef WM_CONVERTREQUEST
#  define WM_CONVERTREQUEST (0x010A)
#endif

#ifndef WM_CONVERTRESULT
#  define WM_CONVERTRESULT (0x010B)
#endif

#ifndef WM_INTERIM
#  define WM_INTERIM (0x010C)
#endif

#ifndef WM_SYSTIMER
#  define WM_SYSTIMER (0x0118)
#endif

#ifndef WM_GESTUREINPUT
#  define WM_GESTUREINPUT (0x011B)
#endif

#ifndef WM_GESTURENOTIFIED
#  define WM_GESTURENOTIFIED (0x011C)
#endif

#ifndef WM_LBTRACKPOINT
#  define WM_LBTRACKPOINT (0x0131)
#endif

#ifndef WM_DROPOBJECT
#  define WM_DROPOBJECT (0x022A)
#endif

#ifndef WM_QUERYDROPOBJECT
#  define WM_QUERYDROPOBJECT (0x022B)
#endif

#ifndef WM_BEGINDRAG
#  define WM_BEGINDRAG (0x022C)
#endif

#ifndef WM_DRAGLOOP
#  define WM_DRAGLOOP (0x022D)
#endif

#ifndef WM_DRAGSELECT
#  define WM_DRAGSELECT (0x022E)
#endif

#ifndef WM_DRAGMOVE
#  define WM_DRAGMOVE (0x022F)
#endif

#ifndef WM_STOPINERTIA
#  define WM_STOPINERTIA (0x023B)
#endif

#ifndef WM_ENDINERTIA
#  define WM_ENDINERTIA (0x023C)
#endif

#ifndef WM_EDGYINERTIA
#  define WM_EDGYINERTIA (0x023D)
#endif

#ifndef WM_VISIBILITYCHANGED
#  define WM_VISIBILITYCHANGED (0x0270)
#endif

#ifndef WM_VIEWSTATECHANGED
#  define WM_VIEWSTATECHANGED (0x0271)
#endif

#ifndef WM_UNREGISTER_WINDOW_SERVICES
#  define WM_UNREGISTER_WINDOW_SERVICES (0x0272)
#endif

#ifndef WM_CONSOLIDATED
#  define WM_CONSOLIDATED (0x0273)
#endif

#ifndef WM_IME_REPORT
#  define WM_IME_REPORT (0x0280)
#endif

#ifndef WM_IME_SYSTEM
#  define WM_IME_SYSTEM (0x0287)
#endif

#ifndef WM_POINTERDEVICEADDED
#  define WM_POINTERDEVICEADDED (0x02C8)
#endif

#ifndef WM_POINTERDEVICEDELETED
#  define WM_POINTERDEVICEDELETED (0x02C9)
#endif

#ifndef WM_FLICK
#  define WM_FLICK (0x02CB)
#endif

#ifndef WM_FLICKINTERNAL
#  define WM_FLICKINTERNAL (0x02CD)
#endif

#ifndef WM_BRIGHTNESSCHANGED
#  define WM_BRIGHTNESSCHANGED (0x02CE)
#endif

#ifndef WM_SYSMENU
#  define WM_SYSMENU (0x0313)
#endif

#ifndef WM_HOOKMSG
#  define WM_HOOKMSG (0x0314)
#endif

#ifndef WM_EXITPROCESS
#  define WM_EXITPROCESS (0x0315)
#endif

#ifndef WM_WAKETHREAD
#  define WM_WAKETHREAD (0x0316)
#endif

#ifndef WM_UAHINIT
#  define WM_UAHINIT (0x031B)
#endif

#ifndef WM_DESKTOPNOTIFY
#  define WM_DESKTOPNOTIFY (0x031C)
#endif

#ifndef WM_DWMEXILEFRAME
#  define WM_DWMEXILEFRAME (0x0322)
#endif

#ifndef WM_MAGNIFICATION_STARTED
#  define WM_MAGNIFICATION_STARTED (0x0324)
#endif

#ifndef WM_MAGNIFICATION_ENDED
#  define WM_MAGNIFICATION_ENDED (0x0325)
#endif

#ifndef WM_DWMTHUMBNAILSIZECHANGED
#  define WM_DWMTHUMBNAILSIZECHANGED (0x0327)
#endif

#ifndef WM_MAGNIFICATION_OUTPUT
#  define WM_MAGNIFICATION_OUTPUT (0x0328)
#endif

#ifndef WM_BSDRDATA
#  define WM_BSDRDATA (0x0329)
#endif

#ifndef WM_DWMTRANSITIONSTATECHANGED
#  define WM_DWMTRANSITIONSTATECHANGED (0x032A)
#endif

#ifndef WM_KEYBOARDCORRECTIONCALLOUT
#  define WM_KEYBOARDCORRECTIONCALLOUT (0x032C)
#endif

#ifndef WM_KEYBOARDCORRECTIONACTION
#  define WM_KEYBOARDCORRECTIONACTION (0x032D)
#endif

#ifndef WM_UIACTION
#  define WM_UIACTION (0x032E)
#endif

#ifndef WM_ROUTED_UI_EVENT
#  define WM_ROUTED_UI_EVENT (0x032F)
#endif

#ifndef WM_MEASURECONTROL
#  define WM_MEASURECONTROL (0x0330)
#endif

#ifndef WM_GETACTIONTEXT
#  define WM_GETACTIONTEXT (0x0331)
#endif

#ifndef WM_FORWARDKEYDOWN
#  define WM_FORWARDKEYDOWN (0x0333)
#endif

#ifndef WM_FORWARDKEYUP
#  define WM_FORWARDKEYUP (0x0334)
#endif

#ifndef WM_NOTIFYWOW
#  define WM_NOTIFYWOW (0x0340)
#endif

#ifndef WM_DWMCOMPOSITIONCHANGED
#  define WM_DWMCOMPOSITIONCHANGED (0x031E)
#endif

#ifndef WM_DWMCOLORIZATIONCOLORCHANGED
#  define WM_DWMCOLORIZATIONCOLORCHANGED (0x0320)
#endif

#ifndef WM_DPICHANGED
#  define WM_DPICHANGED (0x02E0)
#endif

#ifndef WM_GETDPISCALEDSIZE
#  define WM_GETDPISCALEDSIZE (0x02E4)
#endif

#ifndef SM_CXPADDEDBORDER
#  define SM_CXPADDEDBORDER (92)
#endif

#ifndef SM_CYPADDEDBORDER
#  define SM_CYPADDEDBORDER SM_CXPADDEDBORDER
#endif

#ifndef ABM_GETAUTOHIDEBAREX
#  define ABM_GETAUTOHIDEBAREX (0x0000000b)
#endif

#ifndef MAKEWORD
#  define MAKEWORD(a, b) ((WORD)(((BYTE)(((DWORD_PTR)(a)) & 0xff)) | ((WORD)((BYTE)(((DWORD_PTR)(b)) & 0xff))) << 8))
#endif

#ifndef MAKELONG
#  define MAKELONG(a, b) ((LONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#endif

#ifndef LOWORD
#  define LOWORD(l) ((WORD)(((DWORD_PTR)(l)) & 0xffff))
#endif

#ifndef HIWORD
#  define HIWORD(l) ((WORD)((((DWORD_PTR)(l)) >> 16) & 0xffff))
#endif

#ifndef LOBYTE
#  define LOBYTE(w) ((BYTE)(((DWORD_PTR)(w)) & 0xff))
#endif

#ifndef HIBYTE
#  define HIBYTE(w) ((BYTE)((((DWORD_PTR)(w)) >> 8) & 0xff))
#endif

#ifndef GET_X_LPARAM
#  define GET_X_LPARAM(lp) (static_cast<int>(static_cast<short>(LOWORD(lp))))
#endif

#ifndef GET_Y_LPARAM
#  define GET_Y_LPARAM(lp) (static_cast<int>(static_cast<short>(HIWORD(lp))))
#endif

#ifndef GET_KEYSTATE_WPARAM
#  define GET_KEYSTATE_WPARAM(wParam) (LOWORD(wParam))
#endif

#ifndef GET_NCHITTEST_WPARAM
#  define GET_NCHITTEST_WPARAM(wParam) (static_cast<short>(LOWORD(wParam)))
#endif

#ifndef GET_XBUTTON_WPARAM
#  define GET_XBUTTON_WPARAM(wParam) (HIWORD(wParam))
#endif

#ifndef POINTSTOPOINT
#  define POINTSTOPOINT(pt, pts) \
   { \
       (pt).x = static_cast<LONG>(static_cast<SHORT>(LOWORD(*(LONG*)&pts))); \
       (pt).y = static_cast<LONG>(static_cast<SHORT>(HIWORD(*(LONG*)&pts))); \
   }
#endif

#ifndef POINTTOPOINTS
#  define POINTTOPOINTS(pt) (MAKELONG(static_cast<short>((pt).x), static_cast<short>((pt).y)))
#endif

#ifndef MAKEWPARAM
#  define MAKEWPARAM(l, h) (static_cast<WPARAM>(static_cast<DWORD>(MAKELONG(l, h))))
#endif

#ifndef MAKELPARAM
#  define MAKELPARAM(l, h) (static_cast<LPARAM>(static_cast<DWORD>(MAKELONG(l, h))))
#endif

#ifndef MAKELRESULT
#  define MAKELRESULT(l, h) (static_cast<LRESULT>(static_cast<DWORD>(MAKELONG(l, h))))
#endif

#ifndef IsMinimized
#  define IsMinimized(hwnd) (::IsIconic(hwnd) != FALSE)
#endif

#ifndef IsMaximized
#  define IsMaximized(hwnd) (::IsZoomed(hwnd) != FALSE)
#endif

#ifndef RECT_WIDTH
#  define RECT_WIDTH(rect) ((rect).right - (rect).left)
#endif

#ifndef RECT_HEIGHT
#  define RECT_HEIGHT(rect) ((rect).bottom - (rect).top)
#endif

#ifndef MMSYSERR_NOERROR
#  define MMSYSERR_NOERROR (0)
#endif

#ifndef TIMERR_NOERROR
#  define TIMERR_NOERROR (0)
#endif

#ifndef WS_EX_NOREDIRECTIONBITMAP
#  define WS_EX_NOREDIRECTIONBITMAP (0x00200000L)
#endif

#ifndef USER_DEFAULT_SCREEN_DPI
#  define USER_DEFAULT_SCREEN_DPI (96)
#endif

DECLARE_HANDLE(_DPI_AWARENESS_CONTEXT);

#ifndef _DPI_AWARENESS_CONTEXT_UNAWARE
#  define _DPI_AWARENESS_CONTEXT_UNAWARE (reinterpret_cast<_DPI_AWARENESS_CONTEXT>(-1))
#endif

#ifndef _DPI_AWARENESS_CONTEXT_SYSTEM_AWARE
#  define _DPI_AWARENESS_CONTEXT_SYSTEM_AWARE (reinterpret_cast<_DPI_AWARENESS_CONTEXT>(-2))
#endif

#ifndef _DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE
#  define _DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE (reinterpret_cast<_DPI_AWARENESS_CONTEXT>(-3))
#endif

#ifndef _DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2
#  define _DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 (reinterpret_cast<_DPI_AWARENESS_CONTEXT>(-4))
#endif

#ifndef _DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED
#  define _DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED (reinterpret_cast<_DPI_AWARENESS_CONTEXT>(-5))
#endif

#ifndef HKEY_CLASSES_ROOT
#  define HKEY_CLASSES_ROOT (reinterpret_cast<HKEY>(static_cast<ULONG_PTR>(static_cast<LONG>(0x80000000))))
#endif

#ifndef HKEY_CURRENT_USER
#  define HKEY_CURRENT_USER (reinterpret_cast<HKEY>(static_cast<ULONG_PTR>(static_cast<LONG>(0x80000001))))
#endif

#ifndef HKEY_LOCAL_MACHINE
#  define HKEY_LOCAL_MACHINE (reinterpret_cast<HKEY>(static_cast<ULONG_PTR>(static_cast<LONG>(0x80000002))))
#endif

#ifndef HKEY_USERS
#  define HKEY_USERS (reinterpret_cast<HKEY>(static_cast<ULONG_PTR>(static_cast<LONG>(0x80000003))))
#endif

#ifndef HKEY_PERFORMANCE_DATA
#  define HKEY_PERFORMANCE_DATA (reinterpret_cast<HKEY>(static_cast<ULONG_PTR>(static_cast<LONG>(0x80000004))))
#endif

#ifndef HKEY_CURRENT_CONFIG
#  define HKEY_CURRENT_CONFIG (reinterpret_cast<HKEY>(static_cast<ULONG_PTR>(static_cast<LONG>(0x80000005))))
#endif

#ifndef HKEY_DYN_DATA
#  define HKEY_DYN_DATA (reinterpret_cast<HKEY>(static_cast<ULONG_PTR>(static_cast<LONG>(0x80000006))))
#endif

#ifndef HKEY_CURRENT_USER_LOCAL_SETTINGS
#  define HKEY_CURRENT_USER_LOCAL_SETTINGS (reinterpret_cast<HKEY>(static_cast<ULONG_PTR>(static_cast<LONG>(0x80000007))))
#endif

#ifndef HKEY_PERFORMANCE_TEXT
#  define HKEY_PERFORMANCE_TEXT (reinterpret_cast<HKEY>(static_cast<ULONG_PTR>(static_cast<LONG>(0x80000050))))
#endif

#ifndef HKEY_PERFORMANCE_NLSTEXT
#  define HKEY_PERFORMANCE_NLSTEXT (reinterpret_cast<HKEY>(static_cast<ULONG_PTR>(static_cast<LONG>(0x80000060))))
#endif

#ifndef _STATUS_SUCCESS
#  define _STATUS_SUCCESS (static_cast<_NTSTATUS>(0x00000000L))
#endif

#ifndef WTNCA_NODRAWCAPTION
#  define WTNCA_NODRAWCAPTION (0x00000001) // don't draw the window caption
#endif

#ifndef WTNCA_NODRAWICON
#  define WTNCA_NODRAWICON (0x00000002) // don't draw the system icon
#endif

#ifndef WTNCA_NOSYSMENU
#  define WTNCA_NOSYSMENU (0x00000004) // don't expose the system menu icon functionality
#endif

#ifndef WTNCA_NOMIRRORHELP
#  define WTNCA_NOMIRRORHELP (0x00000008) // don't mirror the question mark, even in RTL layout
#endif

#ifndef WTNCA_VALIDBITS
#  define WTNCA_VALIDBITS (WTNCA_NODRAWCAPTION | WTNCA_NODRAWICON | WTNCA_NOSYSMENU | WTNCA_NOMIRRORHELP)
#endif

#ifndef EXTERN_C
#  define EXTERN_C extern "C"
#endif

#ifndef EXTERN_C_START
#  define EXTERN_C_START EXTERN_C {
#endif

#ifndef EXTERN_C_END
#  define EXTERN_C_END }
#endif

using _NTSTATUS = LONG;

using _MMRESULT = UINT;
using _TIMECAPS = struct _TIMECAPS
{
    UINT wPeriodMin; // minimum period supported
    UINT wPeriodMax; // maximum period supported
};
using _PTIMECAPS = _TIMECAPS *;

using _PROCESS_DPI_AWARENESS = enum _PROCESS_DPI_AWARENESS
{
    _PROCESS_DPI_UNAWARE = 0,
    _PROCESS_SYSTEM_DPI_AWARE = 1,
    _PROCESS_PER_MONITOR_DPI_AWARE = 2,
    _PROCESS_PER_MONITOR_V2_DPI_AWARE = 3,
    _PROCESS_DPI_UNAWARE_GDISCALED = 4
};

using _MONITOR_DPI_TYPE = enum _MONITOR_DPI_TYPE
{
    _MDT_EFFECTIVE_DPI = 0,
    _MDT_ANGULAR_DPI = 1,
    _MDT_RAW_DPI = 2,
    _MDT_DEFAULT = _MDT_EFFECTIVE_DPI
};

using _DEVICE_SCALE_FACTOR = enum _DEVICE_SCALE_FACTOR
{
    _DEVICE_SCALE_FACTOR_INVALID = 0,
    _SCALE_100_PERCENT = 100,
    _SCALE_120_PERCENT = 120,
    _SCALE_125_PERCENT = 125,
    _SCALE_140_PERCENT = 140,
    _SCALE_150_PERCENT = 150,
    _SCALE_160_PERCENT = 160,
    _SCALE_175_PERCENT = 175,
    _SCALE_180_PERCENT = 180,
    _SCALE_200_PERCENT = 200,
    _SCALE_225_PERCENT = 225,
    _SCALE_250_PERCENT = 250,
    _SCALE_300_PERCENT = 300,
    _SCALE_350_PERCENT = 350,
    _SCALE_400_PERCENT = 400,
    _SCALE_450_PERCENT = 450,
    _SCALE_500_PERCENT = 500
};

using _DPI_AWARENESS = enum _DPI_AWARENESS
{
    _DPI_AWARENESS_INVALID = -1,
    _DPI_AWARENESS_UNAWARE = 0,
    _DPI_AWARENESS_SYSTEM_AWARE = 1,
    _DPI_AWARENESS_PER_MONITOR_AWARE = 2,
    _DPI_AWARENESS_PER_MONITOR_V2_AWARE = 3,
    _DPI_AWARENESS_UNAWARE_GDISCALED = 4
};

using _DWMWINDOWATTRIBUTE = enum _DWMWINDOWATTRIBUTE
{
    _DWMWA_USE_HOSTBACKDROPBRUSH = 17, // [set] BOOL, Allows the use of host backdrop brushes for the window.
    _DWMWA_USE_IMMERSIVE_DARK_MODE_BEFORE_20H1 = 19, // Undocumented, the same with DWMWA_USE_IMMERSIVE_DARK_MODE, but available on systems before Win10 20H1.
    _DWMWA_USE_IMMERSIVE_DARK_MODE = 20, // [set] BOOL, Allows a window to either use the accent color, or dark, according to the user Color Mode preferences.
    _DWMWA_WINDOW_CORNER_PREFERENCE = 33, // [set] WINDOW_CORNER_PREFERENCE, Controls the policy that rounds top-level window corners
    _DWMWA_VISIBLE_FRAME_BORDER_THICKNESS = 37, // [get] UINT, width of the visible border around a thick frame window
    _DWMWA_SYSTEMBACKDROP_TYPE = 38, // [get, set] SYSTEMBACKDROP_TYPE, Controls the system-drawn backdrop material of a window, including behind the non-client area.
    _DWMWA_MICA_EFFECT = 1029 // Undocumented, use this value to enable Mica material on Win11 21H2. You should use DWMWA_SYSTEMBACKDROP_TYPE instead on Win11 22H2 and newer.
};

using _DWM_WINDOW_CORNER_PREFERENCE = enum _DWM_WINDOW_CORNER_PREFERENCE
{
    _DWMWCP_DEFAULT = 0, // Let the system decide whether or not to round window corners
    _DWMWCP_DONOTROUND = 1, // Never round window corners
    _DWMWCP_ROUND = 2, // Round the corners if appropriate
    _DWMWCP_ROUNDSMALL = 3 // Round the corners if appropriate, with a small radius
};

using _DWM_SYSTEMBACKDROP_TYPE = enum _DWM_SYSTEMBACKDROP_TYPE
{
    _DWMSBT_AUTO = 0, // [Default] Let DWM automatically decide the system-drawn backdrop for this window.
    _DWMSBT_NONE = 1, // Do not draw any system backdrop.
    _DWMSBT_MAINWINDOW = 2, /* Mica */ // Draw the backdrop material effect corresponding to a long-lived window.
    _DWMSBT_TRANSIENTWINDOW = 3, /* Acrylic */ // Draw the backdrop material effect corresponding to a transient window.
    _DWMSBT_TABBEDWINDOW = 4 /* Mica Alt */ // Draw the backdrop material effect corresponding to a window with a tabbed title bar.
};

using WINDOWCOMPOSITIONATTRIB = enum WINDOWCOMPOSITIONATTRIB
{
    WCA_UNDEFINED = 0,
    WCA_NCRENDERING_ENABLED = 1,
    WCA_NCRENDERING_POLICY = 2,
    WCA_TRANSITIONS_FORCEDISABLED = 3,
    WCA_ALLOW_NCPAINT = 4,
    WCA_CAPTION_BUTTON_BOUNDS = 5,
    WCA_NONCLIENT_RTL_LAYOUT = 6,
    WCA_FORCE_ICONIC_REPRESENTATION = 7,
    WCA_EXTENDED_FRAME_BOUNDS = 8,
    WCA_HAS_ICONIC_BITMAP = 9,
    WCA_THEME_ATTRIBUTES = 10,
    WCA_NCRENDERING_EXILED = 11,
    WCA_NCADORNMENTINFO = 12,
    WCA_EXCLUDED_FROM_LIVEPREVIEW = 13,
    WCA_VIDEO_OVERLAY_ACTIVE = 14,
    WCA_FORCE_ACTIVEWINDOW_APPEARANCE = 15,
    WCA_DISALLOW_PEEK = 16,
    WCA_CLOAK = 17,
    WCA_CLOAKED = 18,
    WCA_ACCENT_POLICY = 19,
    WCA_FREEZE_REPRESENTATION = 20,
    WCA_EVER_UNCLOAKED = 21,
    WCA_VISUAL_OWNER = 22,
    WCA_HOLOGRAPHIC = 23,
    WCA_EXCLUDED_FROM_DDA = 24,
    WCA_PASSIVEUPDATEMODE = 25,
    WCA_USEDARKMODECOLORS = 26,
    WCA_CORNER_STYLE = 27,
    WCA_PART_COLOR = 28,
    WCA_DISABLE_MOVESIZE_FEEDBACK = 29,
    WCA_LAST = 30
};

using ACCENT_STATE = enum ACCENT_STATE
{
    ACCENT_DISABLED = 0,
    ACCENT_ENABLE_GRADIENT = 1,
    ACCENT_ENABLE_TRANSPARENTGRADIENT = 2,
    ACCENT_ENABLE_BLURBEHIND = 3, // Traditional DWM blur
    ACCENT_ENABLE_ACRYLICBLURBEHIND = 4, // RS4 1803
    ACCENT_ENABLE_HOST_BACKDROP = 5, // RS5 1809
    ACCENT_INVALID_STATE = 6 // Using this value will remove the window background
};

using ACCENT_FLAG = enum ACCENT_FLAG
{
    ACCENT_NONE = 0,
    ACCENT_ENABLE_ACRYLIC = 1,
    ACCENT_ENABLE_ACRYLIC_WITH_LUMINOSITY = 482
};

using ACCENT_POLICY = struct ACCENT_POLICY
{
    DWORD dwAccentState;
    DWORD dwAccentFlags;
    DWORD dwGradientColor; // #AABBGGRR
    DWORD dwAnimationId;
};
using PACCENT_POLICY = ACCENT_POLICY *;

using WINDOWCOMPOSITIONATTRIBDATA = struct WINDOWCOMPOSITIONATTRIBDATA
{
    WINDOWCOMPOSITIONATTRIB Attrib;
    PVOID pvData;
    SIZE_T cbData;
};
using PWINDOWCOMPOSITIONATTRIBDATA = WINDOWCOMPOSITIONATTRIBDATA *;

using _WINDOWTHEMEATTRIBUTETYPE = enum _WINDOWTHEMEATTRIBUTETYPE
{
    _WTA_NONCLIENT = 1
};

using WTA_OPTIONS2 = struct WTA_OPTIONS2
{
    DWORD dwFlags; // Values for each style option specified in the bitmask.
    DWORD dwMask; // Bitmask for flags that are changing.
};
using PWTA_OPTIONS2 = WTA_OPTIONS2 *;

using IMMERSIVE_HC_CACHE_MODE = enum IMMERSIVE_HC_CACHE_MODE
{
    IHCM_USE_CACHED_VALUE = 0,
    IHCM_REFRESH = 1
};

using PREFERRED_APP_MODE = enum PREFERRED_APP_MODE
{
    PAM_DEFAULT = 0, // Default behavior on systems before Win10 1809. It indicates the application doesn't support dark mode at all.
    PAM_AUTO = 1, // Available since Win10 1809, let system decide whether to enable dark mode or not.
    PAM_DARK = 2, // Available since Win10 1903, force dark mode regardless of the system theme.
    PAM_LIGHT = 3, // Available since Win10 1903, force light mode regardless of the system theme.
    PAM_MAX = 4
};

using GetWindowCompositionAttributePtr = BOOL(WINAPI *)(HWND, PWINDOWCOMPOSITIONATTRIBDATA);
using SetWindowCompositionAttributePtr = BOOL(WINAPI *)(HWND, PWINDOWCOMPOSITIONATTRIBDATA);
// Win10 1809 (10.0.17763)
using ShouldAppsUseDarkModePtr = BOOL(WINAPI *)(VOID); // Ordinal 132
using AllowDarkModeForWindowPtr = BOOL(WINAPI *)(HWND, BOOL); // Ordinal 133
using AllowDarkModeForAppPtr = BOOL(WINAPI *)(BOOL); // Ordinal 135
using FlushMenuThemesPtr = VOID(WINAPI *)(VOID); // Ordinal 136
using RefreshImmersiveColorPolicyStatePtr = VOID(WINAPI *)(VOID); // Ordinal 104
using IsDarkModeAllowedForWindowPtr = BOOL(WINAPI *)(HWND); // Ordinal 137
using GetIsImmersiveColorUsingHighContrastPtr = BOOL(WINAPI *)(IMMERSIVE_HC_CACHE_MODE); // Ordinal 106
using OpenNcThemeDataPtr = HTHEME(WINAPI *)(HWND, LPCWSTR); // Ordinal 49
// Win10 1903 (10.0.18362)
using ShouldSystemUseDarkModePtr = BOOL(WINAPI *)(VOID); // Ordinal 138
using SetPreferredAppModePtr = PREFERRED_APP_MODE(WINAPI *)(PREFERRED_APP_MODE); // Ordinal 135
using IsDarkModeAllowedForAppPtr = BOOL(WINAPI *)(VOID); // Ordinal 139

EXTERN_C_START

///////////////////////////////////////////////
// Function prototypes

_MMRESULT WINAPI
_timeGetDevCaps(
    _PTIMECAPS ptc,
    UINT cbtc
);

_MMRESULT WINAPI
_timeBeginPeriod(
    UINT uPeriod
);

_MMRESULT WINAPI
_timeEndPeriod(
    UINT uPeriod
);

HRESULT WINAPI
_SetProcessDpiAwareness(
    _PROCESS_DPI_AWARENESS value
);

HRESULT WINAPI
_GetProcessDpiAwareness(
    HANDLE hProcess,
    _PROCESS_DPI_AWARENESS *value
);

HRESULT WINAPI
_GetDpiForMonitor(
    HMONITOR hMonitor,
    _MONITOR_DPI_TYPE dpiType,
    UINT *dpiX,
    UINT *dpiY
);

int WINAPI
_GetSystemMetricsForDpi(
    int nIndex,
    UINT dpi
);

UINT WINAPI
_GetWindowDPI(
    HWND hWnd
);

UINT WINAPI
_GetDpiForWindow(
    HWND hWnd
);

UINT WINAPI
_GetDpiForSystem(
    VOID
);

UINT WINAPI
_GetSystemDpiForProcess(
    HANDLE hProcess
);

BOOL WINAPI
_SetProcessDpiAwarenessContext(
    _DPI_AWARENESS_CONTEXT value
);

BOOL WINAPI
_SetProcessDPIAware(
    VOID
);

HRESULT WINAPI
_GetScaleFactorForMonitor(
    HMONITOR hMon,
    _DEVICE_SCALE_FACTOR *pScale
);

BOOL WINAPI
_EnableNonClientDpiScaling(
    HWND hWnd
);

_DPI_AWARENESS_CONTEXT WINAPI
_GetThreadDpiAwarenessContext(
    VOID
);

_DPI_AWARENESS_CONTEXT WINAPI
_GetWindowDpiAwarenessContext(
    HWND hWnd
);

_DPI_AWARENESS WINAPI
_GetAwarenessFromDpiAwarenessContext(
    _DPI_AWARENESS_CONTEXT value
);

_DPI_AWARENESS_CONTEXT WINAPI
_GetDpiAwarenessContextForProcess(
    HANDLE hProcess
);

BOOL WINAPI
_AreDpiAwarenessContextsEqual(
    _DPI_AWARENESS_CONTEXT dpiContextA,
    _DPI_AWARENESS_CONTEXT dpiContextB
);

BOOL WINAPI
_EnableChildWindowDpiMessage(
    HWND hWnd,
    BOOL fEnable
);

BOOL WINAPI
_EnablePerMonitorDialogScaling(
    VOID
);

int WINAPI
_GetDpiMetrics(
    int nIndex,
    UINT dpi
);

BOOL WINAPI
_AdjustWindowRectExForDpi(
    LPRECT lpRect,
    DWORD dwStyle,
    BOOL bMenu,
    DWORD dwExStyle,
    UINT dpi
);

///////////////////////////////////////////////
// API thunks

FRAMELESSHELPER_CORE_API BOOL WINAPI
_GetWindowCompositionAttribute(
    const HWND hWnd,
    PWINDOWCOMPOSITIONATTRIBDATA pvData
);

FRAMELESSHELPER_CORE_API BOOL WINAPI
_SetWindowCompositionAttribute(
    const HWND hWnd,
    PWINDOWCOMPOSITIONATTRIBDATA pvData
);

FRAMELESSHELPER_CORE_API HRESULT WINAPI
_SetWindowThemeAttribute(
    const HWND hWnd,
    const _WINDOWTHEMEATTRIBUTETYPE attrib,
    PVOID pvData,
    const DWORD cbData
);

FRAMELESSHELPER_CORE_API HRESULT WINAPI
_SetWindowThemeNonClientAttributes(
    const HWND hWnd,
    const DWORD dwMask,
    const DWORD dwAttributes
);

FRAMELESSHELPER_CORE_API BOOL WINAPI
_ShouldAppsUseDarkMode(
    VOID
);

FRAMELESSHELPER_CORE_API BOOL WINAPI
_AllowDarkModeForWindow(
    const HWND hWnd,
    const BOOL bAllow
);

FRAMELESSHELPER_CORE_API BOOL WINAPI
_AllowDarkModeForApp(
    const BOOL bAllow
);

FRAMELESSHELPER_CORE_API VOID WINAPI
_FlushMenuThemes(
    VOID
);

FRAMELESSHELPER_CORE_API VOID WINAPI
_RefreshImmersiveColorPolicyState(
    VOID
);

FRAMELESSHELPER_CORE_API BOOL WINAPI
_IsDarkModeAllowedForWindow(
    const HWND hWnd
);

FRAMELESSHELPER_CORE_API BOOL WINAPI
_GetIsImmersiveColorUsingHighContrast(
    const IMMERSIVE_HC_CACHE_MODE mode
);

FRAMELESSHELPER_CORE_API HTHEME WINAPI
_OpenNcThemeData(
    const HWND hWnd,
    LPCWSTR pszClassList
);

FRAMELESSHELPER_CORE_API BOOL WINAPI
_ShouldSystemUseDarkMode(
    VOID
);

FRAMELESSHELPER_CORE_API PREFERRED_APP_MODE WINAPI
_SetPreferredAppMode(
    const PREFERRED_APP_MODE mode
);

FRAMELESSHELPER_CORE_API BOOL WINAPI
_IsDarkModeAllowedForApp(
    VOID
);

FRAMELESSHELPER_CORE_API BOOL WINAPI
_EnableChildWindowDpiMessage2(
    const HWND hWnd,
    const BOOL fEnable
);

FRAMELESSHELPER_CORE_API BOOL WINAPI
_EnablePerMonitorDialogScaling2(
    VOID
);

FRAMELESSHELPER_CORE_API UINT WINAPI
_GetDpiForWindow2(
    const HWND hWnd
);

FRAMELESSHELPER_CORE_API int WINAPI
_GetSystemMetricsForDpi2(
    const int nIndex,
    const UINT dpi
);

FRAMELESSHELPER_CORE_API BOOL WINAPI
_AdjustWindowRectExForDpi2(
    LPRECT lpRect,
    const DWORD dwStyle,
    const BOOL bMenu,
    const DWORD dwExStyle,
    const UINT dpi
);

EXTERN_C_END

[[maybe_unused]] inline constexpr const unsigned char kAutoHideTaskBarThickness = 2; // The thickness of an auto-hide taskbar in pixels.
[[maybe_unused]] inline constexpr const wchar_t kDwmRegistryKey[] = LR"(Software\Microsoft\Windows\DWM)";
[[maybe_unused]] inline constexpr const wchar_t kPersonalizeRegistryKey[] = LR"(Software\Microsoft\Windows\CurrentVersion\Themes\Personalize)";
[[maybe_unused]] inline constexpr const wchar_t kThemeSettingChangeEventName[] = L"ImmersiveColorSet";
[[maybe_unused]] inline constexpr const wchar_t kDwmColorKeyName[] = L"ColorPrevalence";
[[maybe_unused]] inline constexpr const wchar_t kSystemDarkThemeResourceName[] = L"DarkMode_Explorer";
[[maybe_unused]] inline constexpr const wchar_t kSystemLightThemeResourceName[] = L"Explorer";
[[maybe_unused]] inline constexpr const wchar_t kDesktopRegistryKey[] = LR"(Control Panel\Desktop)";
[[maybe_unused]] inline constexpr const wchar_t kDarkModePropertyName[] = L"UseImmersiveDarkModeColors";

[[maybe_unused]] [[nodiscard]] inline constexpr bool operator==(const POINT &lhs, const POINT &rhs) noexcept
{
    return ((lhs.x == rhs.x) && (lhs.y == rhs.y));
}

[[maybe_unused]] [[nodiscard]] inline constexpr bool operator!=(const POINT &lhs, const POINT &rhs) noexcept
{
    return !operator==(lhs, rhs);
}

[[maybe_unused]] [[nodiscard]] inline constexpr bool operator==(const SIZE &lhs, const SIZE &rhs) noexcept
{
    return ((lhs.cx == rhs.cx) && (lhs.cy == rhs.cy));
}

[[maybe_unused]] [[nodiscard]] inline constexpr bool operator!=(const SIZE &lhs, const SIZE &rhs) noexcept
{
    return !operator==(lhs, rhs);
}

[[maybe_unused]] [[nodiscard]] inline constexpr bool operator>(const SIZE &lhs, const SIZE &rhs) noexcept
{
    return ((lhs.cx * lhs.cy) > (rhs.cx * rhs.cy));
}

[[maybe_unused]] [[nodiscard]] inline constexpr bool operator>=(const SIZE &lhs, const SIZE &rhs) noexcept
{
    return (operator>(lhs, rhs) || operator==(lhs, rhs));
}

[[maybe_unused]] [[nodiscard]] inline constexpr bool operator<(const SIZE &lhs, const SIZE &rhs) noexcept
{
    return (operator!=(lhs, rhs) && !operator>(lhs, rhs));
}

[[maybe_unused]] [[nodiscard]] inline constexpr bool operator<=(const SIZE &lhs, const SIZE &rhs) noexcept
{
    return (operator<(lhs, rhs) || operator==(lhs, rhs));
}

[[maybe_unused]] [[nodiscard]] inline constexpr bool operator==(const RECT &lhs, const RECT &rhs) noexcept
{
    return ((lhs.left == rhs.left) && (lhs.top == rhs.top) && (lhs.right == rhs.right) && (lhs.bottom == rhs.bottom));
}

[[maybe_unused]] [[nodiscard]] inline constexpr bool operator!=(const RECT &lhs, const RECT &rhs) noexcept
{
    return !operator==(lhs, rhs);
}

[[nodiscard]] inline constexpr QPoint point2qpoint(const POINT &point)
{
    return QPoint{ int(point.x), int(point.y) };
}

[[nodiscard]] inline constexpr POINT qpoint2point(const QPoint &point)
{
    return POINT{ LONG(point.x()), LONG(point.y()) };
}

[[nodiscard]] inline constexpr QSize size2qsize(const SIZE &size)
{
    return QSize{ int(size.cx), int(size.cy) };
}

[[nodiscard]] inline constexpr SIZE qsize2size(const QSize &size)
{
    return SIZE{ LONG(size.width()), LONG(size.height()) };
}

[[nodiscard]] inline constexpr QRect rect2qrect(const RECT &rect)
{
    return QRect{ QPoint{ int(rect.left), int(rect.top) }, QSize{ int(RECT_WIDTH(rect)), int(RECT_HEIGHT(rect)) } };
}

[[nodiscard]] inline constexpr RECT qrect2rect(const QRect &qrect)
{
    return RECT{ LONG(qrect.left()), LONG(qrect.top()), LONG(qrect.right()), LONG(qrect.bottom()) };
}

[[nodiscard]] inline /*constexpr*/ QString hwnd2str(const WId windowId)
{
    // NULL handle is allowed here.
    return FRAMELESSHELPER_STRING_LITERAL("0x") + QString::number(windowId, 16).toUpper().rightJustified(8, u'0');
}

[[nodiscard]] inline /*constexpr*/ QString hwnd2str(const HWND hwnd)
{
    // NULL handle is allowed here.
    return hwnd2str(reinterpret_cast<WId>(hwnd));
}
