#ifdef WIN32

#include "FramelessWindowConverterWindows.h"
#include "FramelessWindowConverter.h"
#include <windowsx.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

#include <qdebug.h>

using namespace FWC;

// Utility functions without warnings
constexpr WORD getLOWORD(unsigned long long l)
{
    return static_cast<WORD>(static_cast<DWORD_PTR>(l) & 0xffff);
}

constexpr WORD getLOWORD(long long l)
{
    return static_cast<WORD>(static_cast<DWORD_PTR>(l) & 0xffff);
}

constexpr WORD getHIWORD(unsigned long long l)
{
    return static_cast<WORD>((static_cast<DWORD_PTR>(l) >> 16) & 0xffff);
}

constexpr WORD getHIWORD(long long l)
{
    return static_cast<WORD>((static_cast<DWORD_PTR>(l) >> 16) & 0xffff);
}

enum class CursorNames
{
    WEST_EAST,
    NORTH_SOUTH,
    NORTH_EAST_SOUTH_WEST,
    NORTH_WEST_SOUTH_EAST,
    ARROW
};

constexpr LPWSTR getCursorResource(CursorNames inCursorName)
{
    // See WinUser.h for Standard Cursor IDs (IDC_ARROW etc.)
    switch(inCursorName)
    {
    case CursorNames::WEST_EAST:
        return reinterpret_cast<LPWSTR>(static_cast<ULONG_PTR>(static_cast<WORD>(32644)));
    case CursorNames::NORTH_SOUTH:
        return reinterpret_cast<LPWSTR>(static_cast<ULONG_PTR>(static_cast<WORD>(32645)));
    case CursorNames::NORTH_EAST_SOUTH_WEST:
        return reinterpret_cast<LPWSTR>(static_cast<ULONG_PTR>(static_cast<WORD>(32643)));
    case CursorNames::NORTH_WEST_SOUTH_EAST:
        return reinterpret_cast<LPWSTR>(static_cast<ULONG_PTR>(static_cast<WORD>(32642)));
    case CursorNames::ARROW:
        return reinterpret_cast<LPWSTR>(static_cast<ULONG_PTR>(static_cast<WORD>(32512)));
    }

    return nullptr;
}
// END Utility functions without warnings


FramelessWindowConverterWindows::FramelessWindowConverterWindows(FramelessWindowConverter* q) : FramelessWindowConverterPrivate(q)
{
}

void FramelessWindowConverterWindows::convertToFrameless()
{
    handle = reinterpret_cast<HWND>(q_ptr->getWindowHandle());
    setFrameless(true);
}

void FramelessWindowConverterWindows::convertToWindowWithFrame()
{
    setFrameless(false);
}

void FramelessWindowConverterWindows::minimizeWindow()
{
    // Disable minimize in fullscreen mode -> can not be restored (no idea why)
    if(!bFullscreen)
    {
        SetWindowLongPtr(handle, GWL_STYLE, GetWindowLongPtrW(handle, GWL_STYLE) | WS_CAPTION);
        ShowWindow(handle, SW_MINIMIZE);
    }
}

void FramelessWindowConverterWindows::maximizeWindow()
{
    ShowWindow(handle, SW_MAXIMIZE);
}

void FramelessWindowConverterWindows::restoreWindow()
{
    ShowWindow(handle, SW_RESTORE);
}

void FramelessWindowConverterWindows::closeWindow()
{
    SendMessage(handle, WM_CLOSE, 0, 0);
}

void FramelessWindowConverterWindows::toggleFullscreen()
{
    bFullscreen = !bFullscreen;
    if(bFullscreen)
    {
        // When the window is already maximized restore it and go fullscreen immediatly after
        // Otherwise it will not go fullscreen
        ShowWindow(handle, SW_RESTORE);
        ShowWindow(handle, SW_MAXIMIZE);
    }
    else
    {
        ShowWindow(handle, SW_RESTORE);
    }
}

void FramelessWindowConverterWindows::setEnableShadow()
{
    if(q_ptr->getHasShadow())
    {
        const MARGINS shadow = { 1, 1, 1, 1 };
        DwmExtendFrameIntoClientArea(handle, &shadow);
        // redraw frame
        SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
        ShowWindow(handle, SW_SHOW);
    }
    else
    {
        const MARGINS shadow = { 0, 0, 0, 0 };
        DwmExtendFrameIntoClientArea(handle, &shadow);
        // redraw frame
        SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
        ShowWindow(handle, SW_SHOW);
    }
}

FWCRect FramelessWindowConverterWindows::getCurrentClientRect()
{
    RECT WINRect;
    if(!GetClientRect(handle, &WINRect))
    {
        return FWCRect(-1,-1,-1,-1);
    }
    return FWCRect(WINRect.left, WINRect.top, WINRect.right, WINRect.bottom);
}

FWCRect FramelessWindowConverterWindows::getCurrentWindowRect()
{
    RECT WINRect;
    if(!GetWindowRect(handle, &WINRect))
    {
        return FWCRect(-1,-1,-1,-1);
    }
    return FWCRect(WINRect.left, WINRect.top, WINRect.right, WINRect.bottom);
}

FWCPoint FramelessWindowConverterWindows::getCurrentMousePos(LPARAM lParam)
{
    return  FWCPoint(getLOWORD(lParam), getHIWORD(lParam));
}

bool FramelessWindowConverterWindows::filterNativeEvent(void *message, long *result)
{
#if (QT_VERSION == QT_VERSION_CHECK(5, 11, 1))
    MSG* msg = *reinterpret_cast<MSG**>(message); // Nice Bug Qt...
#else
    MSG* msg = reinterpret_cast<MSG*>(message);
#endif

    switch (msg->message)
    {
    case WM_NCCALCSIZE:
    {
        if (msg->wParam == TRUE)
        {
            // Frameless window -> kill title bar and border
            *result = 0;
            return true;
        }
        break;
    }
    case WM_WINDOWPOSCHANGING:
        // Give maximized windows special window style to stop Windows 10 from repositioning and resizing the window
        WINDOWPLACEMENT wpm;
        GetWindowPlacement(handle, &wpm);
        if(wpm.showCmd != SW_SHOWMAXIMIZED && !(GetWindowLongPtrW(handle, GWL_STYLE) & WS_SYSMENU))
        {
            SetWindowLongPtrW(handle, GWL_STYLE, static_cast<LONG>(Style::aero_borderless));
            SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
            ShowWindow(handle, SW_SHOW);
        }
        else if(wpm.showCmd == SW_SHOWMAXIMIZED && (GetWindowLongPtrW(handle, GWL_STYLE) & WS_SYSMENU))
        {
            SetWindowLongPtrW(handle, GWL_STYLE, static_cast<LONG>(Style::maximized_borderless));
            SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
            ShowWindow(handle, SW_SHOWMAXIMIZED);
        }
        break;
    case WM_NCACTIVATE:
    {
        // Prevents window frame reappearing on window activation in "basic" theme,
        // where no aero shadow is present.
        *result = 1;
        break;
    }
    case WM_ACTIVATEAPP:
    {
        if(msg->wParam == TRUE)
        {
            // Remove WS_CAPTION when switching from another window
            WINDOWPLACEMENT wpm;

            GetWindowPlacement(handle, &wpm);
            if(wpm.showCmd != SW_SHOWMINIMIZED)
            {
                SetWindowLongPtr(handle, GWL_STYLE, GetWindowLongPtrW(handle, GWL_STYLE) & ~WS_CAPTION);
            }
        }
        break;
    }
    case WM_ACTIVATE :
    {
        // Enable Minimize Animation by adding/removing WS_CAPTION window style
        if(getLOWORD(msg->wParam) == WA_ACTIVE && getHIWORD(msg->wParam) == 0) // not minimized
        {
            SetWindowLongPtr(handle, GWL_STYLE, GetWindowLongPtrW(handle, GWL_STYLE) & ~WS_CAPTION);
        }
        else if(getLOWORD(msg->wParam) == WA_INACTIVE && getHIWORD(msg->wParam) == 0) // not minimized
        {
            // This actually also gets called when switching to another window (see WM_ACTIVATEAPP for the fix)
            SetWindowLongPtr(handle, GWL_STYLE, GetWindowLongPtrW(handle, GWL_STYLE) | WS_CAPTION);
        }
        break;
    }
    case WM_NCHITTEST:
    {
        // Not using WM_NCHITTEST.
        // QAbstractNativeEventFilter does not send this message (bug?).
        // Instead WM_LBUTTONDBLCLK and WM_LBUTTONDOWN is used to send WM_NCLBUTTON*** events manually
        // WM_MOUSEMOVE is used to set appropriate cursor.
        // This is also more consistent with the implementation on other platforms (linux, mac).
        break;
        //                switch(doBorderHitTest(getCurrentWindowRect(), getCurrentMousePos(msg->lParam), q_ptr->getBorderWidth()))
        //                {
        //                case FWCBorderHitTestResult::TOP_LEFT:
        //                    *result = HTTOPLEFT;
        //                    return true;
        //                case FWCBorderHitTestResult::TOP:
        //                    *result = HTTOP;
        //                    return true;
        //                case FWCBorderHitTestResult::TOP_RIGHT:
        //                    *result = HTTOPRIGHT;
        //                    return true;
        //                case FWCBorderHitTestResult::RIGHT:
        //                    *result = HTRIGHT;
        //                    return true;
        //                case FWCBorderHitTestResult::BOTTOM_RIGHT:
        //                    *result = HTBOTTOMRIGHT;
        //                    return true;
        //                case FWCBorderHitTestResult::BOTTOM:
        //                    *result = HTBOTTOM;
        //                    return true;
        //                case FWCBorderHitTestResult::BOTTOM_LEFT:
        //                    *result = HTBOTTOMLEFT;
        //                    return true;
        //                case FWCBorderHitTestResult::LEFT:
        //                    *result = HTLEFT;
        //                    return true;
        //                case FWCBorderHitTestResult::CLIENT:
        //                    *result = HTCAPTION;
        //                    return true;
        //                case FWCBorderHitTestResult::NONE:
        //                    *result = 0;
        //                    return false;
        //                }
        //                break;
    }
    case WM_LBUTTONDBLCLK:
    {
        // Exclude Child Widgets
        FWCPoint mousePos(getCurrentMousePos(msg->lParam));

        // Only this widget is used for dragging.
        if (!q_ptr->getShouldPerformWindowDrag()(mousePos.x, mousePos.y))
        {
            return false;
        }

        if(doBorderHitTest(getCurrentClientRect(), mousePos, q_ptr->getBorderWidth()) == FWCBorderHitTestResult::CLIENT)
        {
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDBLCLK, HTCAPTION, msg->lParam);
        }
        break;
    }
    case WM_LBUTTONDOWN:
    {
        FWCPoint mousePos(getCurrentMousePos(msg->lParam));
        FWCBorderHitTestResult hitResult;
        if(q_ptr->getEnableResizing()) hitResult = doBorderHitTest(getCurrentClientRect(), mousePos, q_ptr->getBorderWidth());
        else hitResult = doBorderHitTest(getCurrentClientRect(), mousePos, 0);

        switch (hitResult)
        {
        case FWCBorderHitTestResult::LEFT:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTLEFT, msg->lParam);
            break;
        case FWCBorderHitTestResult::RIGHT:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTRIGHT, msg->lParam);
            break;
        case FWCBorderHitTestResult::TOP:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTTOP, msg->lParam);
            break;
        case FWCBorderHitTestResult::BOTTOM:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTBOTTOM, msg->lParam);
            break;
        case FWCBorderHitTestResult::BOTTOM_LEFT:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTBOTTOMLEFT, msg->lParam);
            break;
        case FWCBorderHitTestResult::BOTTOM_RIGHT:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTBOTTOMRIGHT, msg->lParam);
            break;
        case FWCBorderHitTestResult::TOP_LEFT:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTTOPLEFT, msg->lParam);
            break;
        case FWCBorderHitTestResult::TOP_RIGHT:
            ReleaseCapture();
            SendMessage(handle, WM_NCLBUTTONDOWN, HTTOPRIGHT, msg->lParam);
            break;
        default:
            // Only this widget is used for dragging.
            if (q_ptr->getShouldPerformWindowDrag()(mousePos.x, mousePos.y))
            {
                ReleaseCapture();
                SendMessage(handle, WM_NCLBUTTONDOWN, HTCAPTION, msg->lParam);
            }
            break;
        }
        break;
    }
    case WM_MOUSEMOVE:
    {
        if(!q_ptr->getEnableResizing()) break; // No Resizing

        switch (doBorderHitTest(getCurrentClientRect(), getCurrentMousePos(msg->lParam), q_ptr->getBorderWidth()))
        {
        case FWCBorderHitTestResult::LEFT:
            SetCursor(LoadCursor(nullptr, getCursorResource(CursorNames::WEST_EAST)));
            break;
        case FWCBorderHitTestResult::RIGHT:
            SetCursor(LoadCursor(nullptr, getCursorResource(CursorNames::WEST_EAST)));
            break;
        case FWCBorderHitTestResult::TOP:
            SetCursor(LoadCursor(nullptr, getCursorResource(CursorNames::NORTH_SOUTH)));
            break;
        case FWCBorderHitTestResult::BOTTOM:
            SetCursor(LoadCursor(nullptr, getCursorResource(CursorNames::NORTH_SOUTH)));
            break;
        case FWCBorderHitTestResult::BOTTOM_LEFT:
            SetCursor(LoadCursor(nullptr, getCursorResource(CursorNames::NORTH_EAST_SOUTH_WEST)));
            break;
        case FWCBorderHitTestResult::BOTTOM_RIGHT:
            SetCursor(LoadCursor(nullptr, getCursorResource(CursorNames::NORTH_WEST_SOUTH_EAST)));
            break;
        case FWCBorderHitTestResult::TOP_LEFT:
            SetCursor(LoadCursor(nullptr, getCursorResource(CursorNames::NORTH_WEST_SOUTH_EAST)));
            break;
        case FWCBorderHitTestResult::TOP_RIGHT:
            SetCursor(LoadCursor(nullptr, getCursorResource(CursorNames::NORTH_EAST_SOUTH_WEST)));
            break;
        default:
            SetCursor(LoadCursor(nullptr, getCursorResource(CursorNames::ARROW)));
            break;
        }
        break;
    }
    case WM_GETMINMAXINFO:
    {
        MINMAXINFO* minMaxInfo = reinterpret_cast<MINMAXINFO*>(msg->lParam);

        // Get Monitor Info
        auto monitor = ::MonitorFromWindow(handle, MONITOR_DEFAULTTONEAREST);
        MONITORINFO monitor_info{};
        monitor_info.cbSize = sizeof(monitor_info);
        GetMonitorInfoW(monitor, &monitor_info);

        // Set position and size of maximized window
        minMaxInfo->ptMaxPosition.x = 0;
        minMaxInfo->ptMaxPosition.y = 0;

        if(bFullscreen)
        {
            minMaxInfo->ptMaxSize.x = monitor_info.rcMonitor.right - monitor_info.rcMonitor.left;
            minMaxInfo->ptMaxSize.y = monitor_info.rcMonitor.bottom - monitor_info.rcMonitor.top;
        }
        else
        {
            minMaxInfo->ptMaxSize.x = monitor_info.rcWork.right - monitor_info.rcWork.left;
            minMaxInfo->ptMaxSize.y = monitor_info.rcWork.bottom - monitor_info.rcWork.top;
        }

        // Set limits of the size of the window
        bool bMinMaxInfo = false;
        if(q_ptr->getMinimumWindowWidth() >= 0)
        {
            minMaxInfo->ptMinTrackSize.x = q_ptr->getMinimumWindowWidth();
            bMinMaxInfo = true;
        }

        if(q_ptr->getMinimumWindowHeight() >= 0)
        {
            minMaxInfo->ptMinTrackSize.y = q_ptr->getMinimumWindowHeight();
            bMinMaxInfo = true;
        }

        if(q_ptr->getMaximumWindowWidth() >= 0)
        {
            minMaxInfo->ptMaxTrackSize.x = q_ptr->getMaximumWindowWidth();
            bMinMaxInfo = true;
        }

        if(q_ptr->getMaximumWindowHeight() >= 0)
        {
            minMaxInfo->ptMaxTrackSize.y =  q_ptr->getMaximumWindowHeight();
            bMinMaxInfo = true;
        }

        if(bMinMaxInfo)
        {
            *result = 0;
            return true;
        }
        else break;
    }
    } // end switch
    return false;
}

void FramelessWindowConverterWindows::setFrameless(bool enabled)
{
    Style new_style = (enabled) ? Style::aero_borderless : Style::windowed;
    Style old_style = static_cast<Style>(::GetWindowLongPtrW(handle, GWL_STYLE));

    if (new_style != old_style)
    {
        SetWindowLongPtrW(handle, GWL_STYLE, static_cast<LONG>(new_style));

        // Support for shadow
        setEnableShadow();

        // redraw frame
        SetWindowPos(handle, nullptr, 0, 0, 0, 0, SWP_FRAMECHANGED | SWP_NOMOVE | SWP_NOSIZE);
        ShowWindow(handle, SW_SHOW);
    }
}

#endif
