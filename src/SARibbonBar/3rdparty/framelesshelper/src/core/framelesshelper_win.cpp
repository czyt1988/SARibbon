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

#include "framelesshelper_win.h"

#ifdef Q_OS_WINDOWS

#if FRAMELESSHELPER_CONFIG(native_impl)

#include "framelessmanager.h"
#include "framelessmanager_p.h"
#include "framelessconfig_p.h"
#include "utils.h"
#include "winverhelper_p.h"
#include "framelesshelper_windows.h"
#include "framelesshelpercore_global_p.h"
#include "scopeguard_p.h"
#include <optional>
#include <memory>
#include <QtCore/qhash.h>
#include <QtCore/qvariant.h>
#include <QtCore/qcoreapplication.h>
#include <QtCore/qtimer.h>
#include <QtCore/qloggingcategory.h>
#include <QtGui/qwindow.h>

FRAMELESSHELPER_BEGIN_NAMESPACE

#if FRAMELESSHELPER_CONFIG(debug_output)
[[maybe_unused]] static Q_LOGGING_CATEGORY(lcFramelessHelperWin, "wangwenx190.framelesshelper.core.impl.win")
#  define INFO qCInfo(lcFramelessHelperWin)
#  define DEBUG qCDebug(lcFramelessHelperWin)
#  define WARNING qCWarning(lcFramelessHelperWin)
#  define CRITICAL qCCritical(lcFramelessHelperWin)
#else
#  define INFO QT_NO_QDEBUG_MACRO()
#  define DEBUG QT_NO_QDEBUG_MACRO()
#  define WARNING QT_NO_QDEBUG_MACRO()
#  define CRITICAL QT_NO_QDEBUG_MACRO()
#endif

using namespace Global;

static constexpr const auto kMessageTag = WPARAM(0x97CCEA99);

FRAMELESSHELPER_STRING_CONSTANT(MonitorFromWindow)
FRAMELESSHELPER_STRING_CONSTANT(GetMonitorInfoW)
FRAMELESSHELPER_STRING_CONSTANT(ScreenToClient)
FRAMELESSHELPER_STRING_CONSTANT(ClientToScreen)
FRAMELESSHELPER_STRING_CONSTANT(GetClientRect)
#ifdef Q_PROCESSOR_X86_64
  FRAMELESSHELPER_STRING_CONSTANT(GetWindowLongPtrW)
  FRAMELESSHELPER_STRING_CONSTANT(SetWindowLongPtrW)
#else // !Q_PROCESSOR_X86_64
  // WinUser.h defines G/SetClassLongPtr as G/SetClassLong due to the
  // "Ptr" suffixed APIs are not available on 32-bit platforms, so we
  // have to add the following workaround. Undefine the macros and then
  // redefine them is also an option but the following solution is more simple.
  FRAMELESSHELPER_STRING_CONSTANT2(GetWindowLongPtrW, "GetWindowLongW")
  FRAMELESSHELPER_STRING_CONSTANT2(SetWindowLongPtrW, "SetWindowLongW")
#endif // Q_PROCESSOR_X86_64
FRAMELESSHELPER_STRING_CONSTANT(RegisterClassExW)
FRAMELESSHELPER_STRING_CONSTANT(GetModuleHandleW)
FRAMELESSHELPER_STRING_CONSTANT(CreateWindowExW)
FRAMELESSHELPER_STRING_CONSTANT(SetLayeredWindowAttributes)
FRAMELESSHELPER_STRING_CONSTANT(SetWindowPos)
FRAMELESSHELPER_STRING_CONSTANT(TrackMouseEvent)
FRAMELESSHELPER_STRING_CONSTANT(FindWindowW)
FRAMELESSHELPER_STRING_CONSTANT(UnregisterClassW)
FRAMELESSHELPER_STRING_CONSTANT(DestroyWindow)
FRAMELESSHELPER_STRING_CONSTANT(GetWindowPlacement)
FRAMELESSHELPER_STRING_CONSTANT(SetWindowPlacement)

enum class WindowPart : quint8
{
    Outside,
    ClientArea,
    ChromeButton,
    ResizeBorder,
    FixedBorder,
    TitleBar
};

struct FramelessDataWin : public FramelessData
{
    // Store the last hit test result, it's helpful to handle WM_MOUSEMOVE and WM_NCMOUSELEAVE.
    WindowPart lastHitTestResult = WindowPart::Outside;
    // True if we blocked a WM_MOUSELEAVE when mouse moves on chrome button, false when a
    // WM_MOUSELEAVE comes or we manually call TrackMouseEvent().
    bool mouseLeaveBlocked = false;
    Dpi dpi = {};
    HMONITOR monitor = nullptr;
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 1))
    QRect restoreGeometry = {};
#endif // (QT_VERSION < QT_VERSION_CHECK(6, 5, 1))

    FramelessDataWin();
    ~FramelessDataWin() override;
};
using FramelessDataWinPtr = std::shared_ptr<FramelessDataWin>;

FramelessDataWin::FramelessDataWin() = default;

FramelessDataWin::~FramelessDataWin() = default;

[[nodiscard]] FramelessDataPtr FramelessData::create()
{
    return std::make_shared<FramelessDataWin>();
}

[[nodiscard]] static inline FramelessDataWinPtr tryGetData(const QObject *window)
{
    Q_ASSERT(window);
    if (!window) {
        return nullptr;
    }
    const FramelessDataPtr data = FramelessManagerPrivate::getData(window);
    if (!data) {
        return nullptr;
    }
    return std::dynamic_pointer_cast<FramelessDataWin>(data);
}

struct FramelessHelperWinInternal
{
    std::unique_ptr<FramelessHelperWin> eventFilter = nullptr;
};
Q_GLOBAL_STATIC(FramelessHelperWinInternal, g_internalData)

[[nodiscard]] extern std::optional<MONITORINFOEXW> getMonitorForWindow(const HWND hwnd);

[[nodiscard]] static inline QByteArray qtNativeEventType()
{
    static const auto result = FRAMELESSHELPER_BYTEARRAY_LITERAL("windows_generic_MSG");
    return result;
}

[[nodiscard]] static inline WindowPart getHittedWindowPart(const int hitTestResult)
{
    switch (hitTestResult) {
    case HTCLIENT:
        return WindowPart::ClientArea;
    case HTCAPTION:
        return WindowPart::TitleBar;
    case HTSYSMENU:
    case HTHELP:
    case HTREDUCE:
    case HTZOOM:
    case HTCLOSE:
        return WindowPart::ChromeButton;
    case HTLEFT:
    case HTRIGHT:
    case HTTOP:
    case HTTOPLEFT:
    case HTTOPRIGHT:
    case HTBOTTOM:
    case HTBOTTOMLEFT:
    case HTBOTTOMRIGHT:
        return WindowPart::ResizeBorder;
    case HTBORDER:
        return WindowPart::FixedBorder;
    default:
        break;
    }
    return WindowPart::Outside;
}

[[nodiscard]] static inline constexpr bool isTaggedMessage(const WPARAM wParam)
{
    return (wParam == kMessageTag);
}

[[nodiscard]] static inline bool requestForMouseLeaveMessage(const HWND hWnd, const bool nonClient)
{
    Q_ASSERT(hWnd);
    if (!hWnd) {
        return false;
    }
    TRACKMOUSEEVENT tme;
    SecureZeroMemory(&tme, sizeof(tme));
    tme.cbSize = sizeof(tme);
    tme.dwFlags = TME_LEAVE;
    if (nonClient) {
        tme.dwFlags |= TME_NONCLIENT;
    }
    tme.hwndTrack = hWnd;
    tme.dwHoverTime = HOVER_DEFAULT;
    if (::TrackMouseEvent(&tme) == FALSE) {
        WARNING << Utils::getSystemErrorMessage(kTrackMouseEvent);
        return false;
    }
    return true;
}

FramelessHelperWin::FramelessHelperWin() : QAbstractNativeEventFilter() {}

FramelessHelperWin::~FramelessHelperWin() = default;

void FramelessHelperWin::addWindow(const QObject *window)
{
    Q_ASSERT(window);
    if (!window) {
        return;
    }
    const FramelessDataWinPtr data = tryGetData(window);
    if (!data || data->frameless || !data->callbacks) {
        return;
    }
    QWindow *qWindow = data->callbacks->getWindowHandle();
    Q_ASSERT(qWindow);
    if (!qWindow) {
        return;
    }
    data->frameless = true;
    data->dpi = Dpi{ Utils::getWindowDpi(data->windowId, true), Utils::getWindowDpi(data->windowId, false) };
    DEBUG.noquote() << "The DPI of window" << hwnd2str(data->windowId) << "is" << data->dpi;
    data->monitor = ::MonitorFromWindow(reinterpret_cast<HWND>(data->windowId), MONITOR_DEFAULTTONEAREST);
    Q_ASSERT(data->monitor);
    if (!data->monitor) {
        WARNING << Utils::getSystemErrorMessage(kMonitorFromWindow);
    }
    // Remove the bad window styles added by Qt (it's not that "bad" though).
    std::ignore = Utils::maybeFixupQtInternals(data->windowId);
#if 0
    data->callbacks->setWindowFlags(data->callbacks->getWindowFlags() | Qt::FramelessWindowHint);
#else
#  if (QT_VERSION < QT_VERSION_CHECK(6, 5, 3))
    // Qt maintains a frame margin internally, we need to update it accordingly
    // otherwise we'll get lots of warning messages when we change the window
    // geometry, it will also affect the final window geometry because QPA will
    // always take it into account when setting window size and position.
    std::ignore = Utils::updateInternalWindowFrameMargins(qWindow, true);
#  endif
#endif
    // Tell DWM our preferred frame margin.
    std::ignore = Utils::updateWindowFrameMargins(data->windowId, false);
    // Tell DWM we don't use the window icon/caption/sysmenu, don't draw them.
    std::ignore = Utils::hideOriginalTitleBarElements(data->windowId);
    // Without this hack, the child windows can't get DPI change messages from
    // Windows, which means only the top level windows can be scaled to the correct
    // size, we of course don't want such thing from happening.
    std::ignore = Utils::fixupChildWindowsDpiMessage(data->windowId);
#if 0 // Conflicts with our blur mode setting.
    // If we are using 3D APIs (D3D, Vulkan, OpenGL, etc) to draw the window content,
    // we need to setup the DWM rendering policy as well.
    if (Utils::isWindowAccelerated(qWindow) && Utils::isWindowTransparent(qWindow)) {
        std::ignore = Utils::updateFramebufferTransparency(data->windowId);
    }
#endif
    if (WindowsVersionHelper::isWin10RS1OrGreater()) {
        // Tell DWM we may need dark theme non-client area (title bar & frame border).
        FramelessHelperEnableThemeAware();
        if (WindowsVersionHelper::isWin10RS5OrGreater()) {
            const bool dark = (FramelessManager::instance()->systemTheme() == SystemTheme::Dark);
            const auto isWidget = [&data]() -> bool {
                const QObject *widget = data->callbacks->getWidgetHandle();
                return (widget && widget->isWidgetType());
            }();
            if (!isWidget) {
                // Tell UXTheme we may need dark theme controls.
                // Causes some QtWidgets paint incorrectly, so only apply to Qt Quick applications.
                std::ignore = Utils::updateGlobalWin32ControlsTheme(data->windowId, dark);
            }
            std::ignore = Utils::refreshWin32ThemeResources(data->windowId, dark);
            if (WindowsVersionHelper::isWin11OrGreater()) {
                // DWM provides official API to adjust the window corner style, but only since Windows 11.
                if (FramelessConfig::instance()->isSet(Option::WindowUseSquareCorners)) {
                    std::ignore = Utils::setCornerStyleForWindow(data->windowId, WindowCornerStyle::Square);
                }
            }
        }
    }
    if (!g_internalData()->eventFilter) {
        g_internalData()->eventFilter = std::make_unique<FramelessHelperWin>();
        qApp->installNativeEventFilter(g_internalData()->eventFilter.get());
    }
}

void FramelessHelperWin::removeWindow(const QObject *window)
{
    Q_UNUSED(window);
}

bool FramelessHelperWin::nativeEventFilter(const QByteArray &eventType, void *message, QT_NATIVE_EVENT_RESULT_TYPE *result)
{
    if ((eventType != qtNativeEventType()) || !message || !result) {
        return false;
    }
    // QPA by default stores the global mouse position in the pt field,
    // but let's not reply on such Qt-specific extensions.
#if (QT_VERSION == QT_VERSION_CHECK(5, 11, 1))
    // Work-around a bug caused by typo which only exists in Qt 5.11.1
    const auto msg = *static_cast<MSG **>(message);
#else
    const auto msg = static_cast<const MSG *>(message);
#endif
    const HWND hWnd = msg->hwnd;
    if (!hWnd) {
        // Why sometimes the window handle is null? Is it designed to be like this?
        // Anyway, we should skip the entire processing in this case.
        return false;
    }

    const UINT uMsg = msg->message;
    // We should skip these messages otherwise we will get crashes.
    // NOTE: WM_QUIT won't be posted to the WindowProc function.
    switch (uMsg) {
    case WM_CLOSE:
    case WM_DESTROY:
    case WM_NCDESTROY:
    // Undocumented messages:
    case WM_UAHDESTROYWINDOW:
    case WM_UNREGISTER_WINDOW_SERVICES:
        return false;
    default:
        break;
    }

    const auto windowId = reinterpret_cast<WId>(hWnd);
    // Let's be extra safe.
    if (!Utils::isValidWindow(windowId, false, true)) {
        return false;
    }

    const QObject *window = FramelessManagerPrivate::getWindow(windowId);
    if (!window) {
        return false;
    }
    const FramelessDataWinPtr data = tryGetData(window);
    if (!data || !data->frameless || !data->callbacks) {
        return false;
    }

    QWindow *qWindow = data->callbacks->getWindowHandle();

    const bool frameBorderVisible = Utils::isWindowFrameBorderVisible();

    const WPARAM wParam = msg->wParam;
    const LPARAM lParam = msg->lParam;

#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 1))
    const auto updateRestoreGeometry = [windowId, &data](const bool ignoreWindowState) -> void {
        if (!ignoreWindowState && !Utils::isWindowNoState(windowId)) {
            return;
        }
        const QRect rect = Utils::getWindowRestoreGeometry(windowId);
        if (!Utils::isValidGeometry(rect)) {
            WARNING << "The calculated restore geometry is invalid.";
            return;
        }
        if (Utils::isValidGeometry(data->restoreGeometry) && (data->restoreGeometry == rect)) {
            return;
        }
        data->restoreGeometry = rect;
    };
#endif // (QT_VERSION < QT_VERSION_CHECK(6, 5, 1))

    const auto emulateClientAreaMessage = [hWnd, uMsg, wParam, lParam](const std::optional<int> &overrideMessage = std::nullopt) -> void {
        const int myMsg = overrideMessage.value_or(uMsg);
        const auto wparam = [myMsg, wParam]() -> WPARAM {
            if (myMsg == WM_NCMOUSELEAVE) {
                // wParam is always ignored in mouse leave messages, but here we
                // give them a special tag to be able to distinguish which messages
                // are sent by ourselves.
                return kMessageTag;
            }
            const quint64 keyState = Utils::getKeyState();
            if ((myMsg >= WM_NCXBUTTONDOWN) && (myMsg <= WM_NCXBUTTONDBLCLK)) {
                const auto xButtonMask = GET_XBUTTON_WPARAM(wParam);
                return MAKEWPARAM(keyState, xButtonMask);
            }
            return keyState;
        }();
        const auto lparam = [myMsg, lParam, hWnd]() -> LPARAM {
            if (myMsg == WM_NCMOUSELEAVE) {
                // lParam is always ignored in mouse leave messages.
                return 0;
            }
            const auto screenPos = POINT{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
            POINT clientPos = screenPos;
            if (::ScreenToClient(hWnd, &clientPos) == FALSE) {
                WARNING << Utils::getSystemErrorMessage(kScreenToClient);
                return 0;
            }
            return MAKELPARAM(clientPos.x, clientPos.y);
        }();
#if 0
#  define SEND_MESSAGE ::SendMessageW
#else
#  define SEND_MESSAGE ::PostMessageW
#endif
        switch (myMsg) {
        case WM_NCHITTEST: // Treat hit test messages as mouse move events.
        case WM_NCMOUSEMOVE:
            SEND_MESSAGE(hWnd, WM_MOUSEMOVE, wparam, lparam);
            break;
        case WM_NCLBUTTONDOWN:
            SEND_MESSAGE(hWnd, WM_LBUTTONDOWN, wparam, lparam);
            break;
        case WM_NCLBUTTONUP:
            SEND_MESSAGE(hWnd, WM_LBUTTONUP, wparam, lparam);
            break;
        case WM_NCLBUTTONDBLCLK:
            SEND_MESSAGE(hWnd, WM_LBUTTONDBLCLK, wparam, lparam);
            break;
        case WM_NCRBUTTONDOWN:
            SEND_MESSAGE(hWnd, WM_RBUTTONDOWN, wparam, lparam);
            break;
        case WM_NCRBUTTONUP:
            SEND_MESSAGE(hWnd, WM_RBUTTONUP, wparam, lparam);
            break;
        case WM_NCRBUTTONDBLCLK:
            SEND_MESSAGE(hWnd, WM_RBUTTONDBLCLK, wparam, lparam);
            break;
        case WM_NCMBUTTONDOWN:
            SEND_MESSAGE(hWnd, WM_MBUTTONDOWN, wparam, lparam);
            break;
        case WM_NCMBUTTONUP:
            SEND_MESSAGE(hWnd, WM_MBUTTONUP, wparam, lparam);
            break;
        case WM_NCMBUTTONDBLCLK:
            SEND_MESSAGE(hWnd, WM_MBUTTONDBLCLK, wparam, lparam);
            break;
        case WM_NCXBUTTONDOWN:
            SEND_MESSAGE(hWnd, WM_XBUTTONDOWN, wparam, lparam);
            break;
        case WM_NCXBUTTONUP:
            SEND_MESSAGE(hWnd, WM_XBUTTONUP, wparam, lparam);
            break;
        case WM_NCXBUTTONDBLCLK:
            SEND_MESSAGE(hWnd, WM_XBUTTONDBLCLK, wparam, lparam);
            break;
#if 0 // ### TODO: How to handle touch events?
        case WM_NCPOINTERUPDATE:
        case WM_NCPOINTERDOWN:
        case WM_NCPOINTERUP:
            break;
#endif
        case WM_NCMOUSEHOVER:
            SEND_MESSAGE(hWnd, WM_MOUSEHOVER, wparam, lparam);
            break;
        case WM_NCMOUSELEAVE:
            SEND_MESSAGE(hWnd, WM_MOUSELEAVE, wparam, lparam);
            break;
        default:
            Q_UNREACHABLE();
        }
    };

    if (uMsg == WM_MOUSELEAVE) {
        if (!isTaggedMessage(wParam)) {
            // Qt will call TrackMouseEvent() to get the WM_MOUSELEAVE message when it receives
            // WM_MOUSEMOVE messages, and since we are converting every WM_NCMOUSEMOVE message
            // to WM_MOUSEMOVE message and send it back to the window to be able to hover our
            // controls, we also get lots of WM_MOUSELEAVE messages at the same time because of
            // the reason above, and these superfluous mouse leave events cause Qt to think the
            // mouse has left the control, and thus we actually lost the hover state.
            // So we filter out these superfluous mouse leave events here to avoid this issue.
            const QPoint qtScenePos = Utils::fromNativeLocalPosition(qWindow, QPoint{ msg->pt.x, msg->pt.y });
            SystemButtonType dummy = SystemButtonType::Unknown;
            if (data->callbacks->isInsideSystemButtons(qtScenePos, &dummy)) {
                data->mouseLeaveBlocked = true;
                *result = FALSE;
                return true;
            }
        }
        data->mouseLeaveBlocked = false;
    }

    switch (uMsg) {
#if (QT_VERSION < QT_VERSION_CHECK(5, 9, 0)) // Qt has done this for us since 5.9.0
    case WM_NCCREATE:
        // Enable automatic DPI scaling for the non-client area of the window,
        // such as the caption bar, the scrollbars, and the menu bar. We need
        // to do this explicitly and manually here (only inside WM_NCCREATE).
        // If we are using the PMv2 DPI awareness mode, the non-client area
        // of the window will be scaled by the OS automatically, so there will
        // be no need to do this in that case.
        std::ignore = Utils::enableNonClientAreaDpiScalingForWindow(windowId);
        break;
#endif
    case WM_NCCALCSIZE: {
        // Windows是根据这个消息的返回值来设置窗口的客户区（窗口中真正显示的内容）
        // 和非客户区（标题栏、窗口边框、菜单栏和状态栏等Windows系统自行提供的部分
        // ，不过对于Qt来说，除了标题栏和窗口边框，非客户区基本也都是自绘的）的范
        // 围的，lParam里存放的就是新客户区的几何区域，默认是整个窗口的大小，正常
        // 的程序需要修改这个参数，告知系统窗口的客户区和非客户区的范围（一般来说可
        // 以完全交给Windows，让其自行处理，使用默认的客户区和非客户区），因此如果
        // 我们不修改lParam，就可以使客户区充满整个窗口，从而去掉标题栏和窗口边框
        // （因为这些东西都被客户区给盖住了。但边框阴影也会因此而丢失，不过我们会使
        // 用其他方式将其带回，请参考其他消息的处理，此处不过多提及）。但有个情况要
        // 特别注意，那就是窗口最大化后，窗口的实际尺寸会比屏幕的尺寸大一点，从而使
        // 用户看不到窗口的边界，这样用户就不能在窗口最大化后调整窗口的大小了（虽然
        // 这个做法听起来特别奇怪，但Windows确实就是这样做的），因此如果我们要自行
        // 处理窗口的非客户区，就要在窗口最大化后，将窗口边框的宽度和高度（一般是相
        // 等的）从客户区裁剪掉，否则我们窗口所显示的内容就会超出屏幕边界，显示不全。
        // 如果用户开启了任务栏自动隐藏，在窗口最大化后，还要考虑任务栏的位置。因为
        // 如果窗口最大化后，其尺寸和屏幕尺寸相等（因为任务栏隐藏了，所以窗口最大化
        // 后其实是充满了整个屏幕，变相的全屏了），Windows会认为窗口已经进入全屏的
        // 状态，从而导致自动隐藏的任务栏无法弹出。要避免这个状况，就要使窗口的尺寸
        // 小于屏幕尺寸。我下面的做法参考了火狐、Chromium和Windows Terminal
        // 如果没有开启任务栏自动隐藏，是不存在这个问题的，所以要先进行判断。
        // 一般情况下，*result设置为0（相当于DefWindowProc的返回值为0）就可以了，
        // 根据MSDN的说法，返回0意为此消息已经被程序自行处理了，让Windows跳过此消
        // 息，否则Windows会添加对此消息的默认处理，对于当前这个消息而言，就意味着
        // 标题栏和窗口边框又会回来，这当然不是我们想要的结果。根据MSDN，当wParam
        // 为FALSE时，只能返回0，但当其为TRUE时，可以返回0，也可以返回一个WVR_常
        // 量。根据Chromium的注释，当存在非客户区时，如果返回WVR_REDRAW会导致子
        // 窗口/子控件出现奇怪的bug（自绘控件错位），并且Lucas在Windows 10
        // 上成功复现，说明这个bug至今都没有解决。我查阅了大量资料，发现唯一的解决
        // 方案就是返回0。但如果不存在非客户区，且wParam为TRUE，最好返回
        // WVR_REDRAW，否则窗口在调整大小可能会产生严重的闪烁现象。
        // 虽然对大多数消息来说，返回0都代表让Windows忽略此消息，但实际上不同消息
        // 能接受的返回值是不一样的，请注意自行查阅MSDN。

        // Sent when the size and position of a window's client area must be
        // calculated. By processing this message, an application can
        // control the content of the window's client area when the size or
        // position of the window changes. If wParam is TRUE, lParam points
        // to an NCCALCSIZE_PARAMS structure that contains information an
        // application can use to calculate the new size and position of the
        // client rectangle. If wParam is FALSE, lParam points to a RECT
        // structure. On entry, the structure contains the proposed window
        // rectangle for the window. On exit, the structure should contain
        // the screen coordinates of the corresponding window client area.
        // The client area is the window's content area, the non-client area
        // is the area which is provided by the system, such as the title
        // bar, the four window borders, the frame shadow, the menu bar, the
        // status bar, the scroll bar, etc. But for Qt, it draws most of the
        // window area (client + non-client) itself. We now know that the
        // title bar and the window frame is in the non-client area and we
        // can set the scope of the client area in this message, so we can
        // remove the title bar and the window frame by let the non-client
        // area be covered by the client area (because we can't really get
        // rid of the non-client area, it will always be there, all we can
        // do is to hide it) , which means we should let the client area's
        // size the same with the whole window's size. So there is no room
        // for the non-client area and then the user won't be able to see it
        // again. But how to achieve this? Very easy, just leave lParam (the
        // re-calculated client area) untouched. But of course you can
        // modify lParam, then the non-client area will be seen and the
        // window borders and the window frame will show up. However, things
        // are quite different when you try to modify the top margin of the
        // client area. DWM will always draw the whole title bar no matter
        // what margin value you set for the top, unless you don't modify it
        // and remove the whole top area (the title bar + the one pixel
        // height window border). This can be confirmed in Windows
        // Terminal's source code, you can also try yourself to verify
        // it. So things will become quite complicated if you want to
        // preserve the four window borders.

        // If `wParam` is `FALSE`, `lParam` points to a `RECT` that contains
        // the proposed window rectangle for our window. During our
        // processing of the `WM_NCCALCSIZE` message, we are expected to
        // modify the `RECT` that `lParam` points to, so that its value upon
        // our return is the new client area. We must return 0 if `wParam`
        // is `FALSE`.
        // If `wParam` is `TRUE`, `lParam` points to a `NCCALCSIZE_PARAMS`
        // struct. This struct contains an array of 3 `RECT`s, the first of
        // which has the exact same meaning as the `RECT` that is pointed to
        // by `lParam` when `wParam` is `FALSE`. The remaining `RECT`s, in
        // conjunction with our return value, can
        // be used to specify portions of the source and destination window
        // rectangles that are valid and should be preserved. We opt not to
        // implement an elaborate client-area preservation technique, and
        // simply return 0, which means "preserve the entire old client area
        // and align it with the upper-left corner of our new client area".
        const auto clientRect = ((wParam == FALSE) ? reinterpret_cast<LPRECT>(lParam) : &(reinterpret_cast<LPNCCALCSIZE_PARAMS>(lParam))->rgrc[0]);
        if (frameBorderVisible) {
            // Store the original top margin before the default window procedure applies the default frame.
            const LONG originalTop = clientRect->top;
            // Apply the default frame because we don't want to remove the whole window frame,
            // we still need the standard window frame (the resizable frame border and the frame
            // shadow) for the left, bottom and right edges.
            // If we return 0 here directly, the whole window frame will be removed (which means
            // there will be no resizable frame border and the frame shadow will also disappear),
            // and that's also how most applications customize their title bars on Windows. It's
            // totally OK but since we want to preserve as much original frame as possible, we
            // can't use that solution.
            const LRESULT hitTestResult = ::DefWindowProcW(hWnd, WM_NCCALCSIZE, wParam, lParam);
            if ((hitTestResult != HTERROR) && (hitTestResult != HTNOWHERE)) {
                *result = hitTestResult;
                return true;
            }
            // Re-apply the original top from before the size of the default frame was applied,
            // and the whole top frame (the title bar and the top border) is gone now.
            // For the top frame, we only has 2 choices: (1) remove the top frame entirely, or
            // (2) don't touch it at all. We can't preserve the top border by adjusting the top
            // margin here. If we try to modify the top margin, the original title bar will
            // always be painted by DWM regardless what margin we set, so here we can only remove
            // the top frame entirely and use some special technique to bring the top border back.
            clientRect->top = originalTop;
        }
        const bool max = IsMaximized(hWnd);
        const bool full = Utils::isFullScreen(windowId);
        // We don't need this correction when we're fullscreen. We will
        // have the WS_POPUP size, so we don't have to worry about
        // borders, and the default frame will be fine.
        if (max && !full) {
            // When a window is maximized, its size is actually a little bit more
            // than the monitor's work area. The window is positioned and sized in
            // such a way that the resize handles are outside of the monitor and
            // then the window is clipped to the monitor so that the resize handle
            // do not appear because you don't need them (because you can't resize
            // a window when it's maximized unless you restore it).
            const int frameSizeY = Utils::getResizeBorderThickness(windowId, false, true);
            clientRect->top += frameSizeY;
            if (!frameBorderVisible) {
                clientRect->bottom -= frameSizeY;
                const int frameSizeX = Utils::getResizeBorderThickness(windowId, true, true);
                clientRect->left += frameSizeX;
                clientRect->right -= frameSizeX;
            }
        }
        // Attempt to detect if there's an autohide taskbar, and if
        // there is, reduce our size a bit on the side with the taskbar,
        // so the user can still mouse-over the taskbar to reveal it.
        // Make sure to use MONITOR_DEFAULTTONEAREST, so that this will
        // still find the right monitor even when we're restoring from
        // minimized.
        if (max || full) {
            APPBARDATA abd;
            SecureZeroMemory(&abd, sizeof(abd));
            abd.cbSize = sizeof(abd);
            const UINT taskbarState = ::SHAppBarMessage(ABM_GETSTATE, &abd);
            // First, check if we have an auto-hide taskbar at all:
            if (taskbarState & ABS_AUTOHIDE) {
                bool top = false, bottom = false, left = false, right = false;
                // Due to ABM_GETAUTOHIDEBAREX was introduced in Windows 8.1,
                // we have to use another way to judge this if we are running
                // on Windows 7 or Windows 8.
                if (WindowsVersionHelper::isWin8Point1OrGreater()) {
                    const std::optional<MONITORINFOEXW> monitorInfo = getMonitorForWindow(hWnd);
                    if (!monitorInfo.has_value()) {
                        WARNING << "Failed to retrieve the window's monitor.";
                        break;
                    }
                    const RECT monitorRect = monitorInfo.value().rcMonitor;
                    // This helper can be used to determine if there's a
                    // auto-hide taskbar on the given edge of the monitor
                    // we're currently on.
                    const auto hasAutohideTaskbar = [monitorRect](const UINT edge) -> bool {
                        APPBARDATA abd2;
                        SecureZeroMemory(&abd2, sizeof(abd2));
                        abd2.cbSize = sizeof(abd2);
                        abd2.uEdge = edge;
                        abd2.rc = monitorRect;
                        const auto hTaskbar = reinterpret_cast<HWND>(::SHAppBarMessage(ABM_GETAUTOHIDEBAREX, &abd2));
                        return (hTaskbar != nullptr);
                    };
                    top = hasAutohideTaskbar(ABE_TOP);
                    bottom = hasAutohideTaskbar(ABE_BOTTOM);
                    left = hasAutohideTaskbar(ABE_LEFT);
                    right = hasAutohideTaskbar(ABE_RIGHT);
                } else {
                    int edge = -1;
                    APPBARDATA abd2;
                    SecureZeroMemory(&abd2, sizeof(abd2));
                    abd2.cbSize = sizeof(abd2);
                    abd2.hWnd = ::FindWindowW(L"Shell_TrayWnd", nullptr);
                    if (abd2.hWnd) {
                        const HMONITOR windowMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
                        if (!windowMonitor) {
                            WARNING << Utils::getSystemErrorMessage(kMonitorFromWindow);
                            break;
                        }
                        const HMONITOR taskbarMonitor = ::MonitorFromWindow(abd2.hWnd, MONITOR_DEFAULTTOPRIMARY);
                        if (!taskbarMonitor) {
                            WARNING << Utils::getSystemErrorMessage(kMonitorFromWindow);
                            break;
                        }
                        if (taskbarMonitor == windowMonitor) {
                            ::SHAppBarMessage(ABM_GETTASKBARPOS, &abd2);
                            edge = abd2.uEdge;
                        }
                    } else {
                        WARNING << Utils::getSystemErrorMessage(kFindWindowW);
                        break;
                    }
                    top = (edge == ABE_TOP);
                    bottom = (edge == ABE_BOTTOM);
                    left = (edge == ABE_LEFT);
                    right = (edge == ABE_RIGHT);
                }
                // If there's a taskbar on any side of the monitor, reduce
                // our size a little bit on that edge.
                // Note to future code archeologists:
                // This doesn't seem to work for fullscreen on the primary
                // display. However, testing a bunch of other apps with
                // fullscreen modes and an auto-hiding taskbar has
                // shown that _none_ of them reveal the taskbar from
                // fullscreen mode. This includes Edge, Firefox, Chrome,
                // Sublime Text, PowerPoint - none seemed to support this.
                // This does however work fine for maximized.
                if (top) {
                    // Peculiarly, when we're fullscreen,
                    clientRect->top += kAutoHideTaskBarThickness;
                } else if (bottom) {
                    clientRect->bottom -= kAutoHideTaskBarThickness;
                } else if (left) {
                    clientRect->left += kAutoHideTaskBarThickness;
                } else if (right) {
                    clientRect->right -= kAutoHideTaskBarThickness;
                }
            }
        }
        // This line improves the synchronization problem of DirectX surfaces greatly, especially on Win11.
        std::ignore = Utils::updateAllDirectXSurfaces();
        std::ignore = Utils::syncWmPaintWithDwm(); // This should be executed at the very last.
        // By returning WVR_REDRAW we can make the window resizing look less broken.
        // But we must return 0 if wParam is FALSE, according to Microsoft Docs.
        // **IMPORTANT NOTE**:
        // If you are drawing something manually through D3D in your window, don't
        // try to return WVR_REDRAW here, otherwise Windows exhibits bugs where
        // client pixels and child windows are mispositioned by the width/height
        // of the upper-left non-client area. It's confirmed that this issue exists
        // from Windows 7 to Windows 10. Not tested on Windows 11 yet. Don't know
        // whether it exists on Windows XP to Windows Vista or not.
        static const bool needD3DWorkaround = (qEnvironmentVariableIntValue("FRAMELESSHELPER_USE_D3D_WORKAROUND") != 0);
        *result = (((wParam == FALSE) || needD3DWorkaround) ? FALSE : WVR_REDRAW);
        return true;
    }
    case WM_NCHITTEST: {
        // 原生Win32窗口只有顶边是在窗口内部resize的，其余三边都是在窗口
        // 外部进行resize的，其原理是，WS_THICKFRAME这个窗口样式会在窗
        // 口的左、右和底边添加三个透明的resize区域，这三个区域在正常状态
        // 下是完全不可见的，它们由DWM负责绘制和控制。这些区域的宽度等于
        // (SM_CXSIZEFRAME + SM_CXPADDEDBORDER)，高度等于
        // (SM_CYSIZEFRAME + SM_CXPADDEDBORDER)，在100%缩放时，均等
        // 于8像素。它们属于窗口区域的一部分，但不属于客户区，而是属于非客
        // 户区，因此GetWindowRect获取的区域中是包含这三个resize区域的，
        // 而GetClientRect获取的区域是不包含它们的。当把
        // DWMWA_EXTENDED_FRAME_BOUNDS作为参数调用
        // DwmGetWindowAttribute时，也能获取到一个窗口大小，这个大小介
        // 于前面两者之间，暂时不知道这个数据的意义及其作用。我们在
        // WM_NCCALCSIZE消息的处理中，已经把整个窗口都设置为客户区了，也
        // 就是说，我们的窗口已经没有非客户区了，因此那三个透明的resize区
        // 域，此刻也已经成为窗口客户区的一部分了，从而变得不透明了。所以
        // 现在的resize，看起来像是在窗口内部resize，是因为原本透明的地方
        // 现在变得不透明了，实际上，单纯从范围上来看，现在我们resize的地方，
        // 就是普通窗口的边框外部，那三个透明区域的范围。
        // 因此，如果我们把边框完全去掉（就是我们正在做的事情），resize就
        // 会看起来是在内部进行，这个问题通过常规方法非常难以解决。我测试过
        // QQ和钉钉的窗口，它们的窗口就是在外部resize，但实际上它们是通过
        // 把窗口实际的内容，嵌入到一个完全透明的但尺寸要大一圈的窗口中实现
        // 的，虽然看起来效果还不错，但对于此项目而言，代码和窗口结构过于复
        // 杂，因此我没有采用此方案。然而，对于具体的软件项目而言，其做法也
        // 不失为一个优秀的解决方案，毕竟其在大多数条件下的表现都还可以。
        //
        // 和1.x的做法不同，现在的2.x选择了保留窗口三边，去除整个窗口顶部，
        // 好处是保留了系统的原生边框，外观较好，且与系统结合紧密，而且resize
        // 的表现也有很大改善，缺点是需要自行绘制顶部边框线。原本以为只能像
        // Windows Terminal那样在WM_PAINT里搞黑魔法，但后来发现，其实只
        // 要颜色相近，我们自行绘制一根实线也几乎能以假乱真，而且这样也不会
        // 破坏Qt自己的绘制系统，能做到不依赖黑魔法就能实现像Windows Terminal
        // 那样外观和功能都比较完美的自定义边框。

        // A normal Win32 window can be resized outside of it. Here is the
        // reason: the WS_THICKFRAME window style will cause a window has three
        // transparent areas beside the window's left, right and bottom
        // edge. Their width or height is eight pixels if the window is not
        // scaled. In most cases, they are totally invisible. It's DWM's
        // responsibility to draw and control them. They exist to let the
        // user resize the window, visually outside of it. They are in the
        // window area, but not the client area, so they are in the
        // non-client area actually. But we have turned the whole window
        // area into client area in WM_NCCALCSIZE, so the three transparent
        // resize areas also become a part of the client area and thus they
        // become visible. When we resize the window, it looks like we are
        // resizing inside of it, however, that's because the transparent
        // resize areas are visible now, we ARE resizing outside of the
        // window actually. But I don't know how to make them become
        // transparent again without breaking the frame shadow drawn by DWM.
        // If you really want to solve it, you can try to embed your window
        // into a larger transparent window and draw the frame shadow
        // yourself. As what we have said in WM_NCCALCSIZE, you can only
        // remove the top area of the window, this will let us be able to
        // resize outside of the window and don't need much process in this
        // message, it looks like a perfect plan, however, the top border is
        // missing due to the whole top area is removed, and it's very hard
        // to bring it back because we have to use a trick in WM_PAINT
        // (learned from Windows Terminal), but no matter what we do in
        // WM_PAINT, it will always break the backing store mechanism of Qt,
        // so actually we can't do it. And it's very difficult to do such
        // things in NativeEventFilters as well. What's worse, if we really
        // do this, the four window borders will become white and they look
        // horrible in dark mode. This solution only supports Windows 10
        // because the border width on Win10 is only one pixel, however it's
        // eight pixels on Windows 7 so preserving the three window borders
        // looks terrible on old systems.
        //
        // Unlike the 1.x code, we choose to preserve the three edges of the
        // window in 2.x, and get rid of the whole top part of the window.
        // There are quite some advantages such as the appearance looks much
        // better and due to we have the original system window frame, our
        // window can behave just like a normal Win32 window even if we now
        // doesn't have a title bar at all. Most importantly, the flicker and
        // jitter during window resizing is totally gone now. The disadvantage
        // is we have to draw a top frame border ourself. Previously I thought
        // we have to do the black magic in WM_PAINT just like what Windows
        // Terminal does, however, later I found that if we choose a proper
        // color, our homemade top border can almost have exactly the same
        // appearance with the system's one.

        const auto hitTestRecorder = qScopeGuard([&data, &result](){
            data->lastHitTestResult = getHittedWindowPart(*result);
        });

        const auto nativeGlobalPos = POINT{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        POINT nativeLocalPos = nativeGlobalPos;
        if (::ScreenToClient(hWnd, &nativeLocalPos) == FALSE) {
            WARNING << Utils::getSystemErrorMessage(kScreenToClient);
            break;
        }

        auto clientRect = RECT{ 0, 0, 0, 0 };
        if (::GetClientRect(hWnd, &clientRect) == FALSE) {
            WARNING << Utils::getSystemErrorMessage(kGetClientRect);
            break;
        }
        const auto clientWidth = RECT_WIDTH(clientRect);
        const auto clientHeight = RECT_HEIGHT(clientRect);

        const QPoint qtScenePos = Utils::fromNativeLocalPosition(qWindow, QPoint(nativeLocalPos.x, nativeLocalPos.y));
        SystemButtonType sysButtonType = SystemButtonType::Unknown;
        if (data->callbacks->isInsideSystemButtons(qtScenePos, &sysButtonType)) {
            // Even if the mouse is inside the chrome button area now, we should still allow the user
            // to be able to resize the window with the top or right window border, this is also the
            // normal behavior of a native Win32 window.
            static constexpr const int kBorderSize = 2;
            const bool isTop = (nativeLocalPos.y <= kBorderSize);
            const bool isRight = (nativeLocalPos.x >= (clientWidth - kBorderSize));
            if (isTop || isRight) {
                if (isTop && isRight) {
                    *result = HTTOPRIGHT;
                } else if (isTop) {
                    *result = HTTOP;
                } else {
                    *result = HTRIGHT;
                }
            } else {
                // OK, we are now really inside one of the chrome buttons, tell Windows the exact role of our button.
                // The Snap Layout feature introduced in Windows 11 won't work without this.
                switch (sysButtonType) {
                case SystemButtonType::WindowIcon:
                    *result = HTSYSMENU;
                    break;
                case SystemButtonType::Help:
                    *result = HTHELP;
                    break;
                case SystemButtonType::Minimize:
                    *result = HTREDUCE;
                    break;
                case SystemButtonType::Maximize:
                case SystemButtonType::Restore:
                    *result = HTZOOM;
                    break;
                case SystemButtonType::Close:
                    *result = HTCLOSE;
                    break;
                case SystemButtonType::Unknown:
                    *result = HTCLIENT; // Normally we'd never enter this branch.
                    break;
                }
            }
            return true;
        }
        // OK, we are not inside of any chrome buttons, try to find out which part of the window
        // are we hitting.

        const bool max = IsMaximized(hWnd);
        const bool full = Utils::isFullScreen(windowId);
        const int frameSizeY = Utils::getResizeBorderThickness(windowId, false, true);
        const bool isTop = (nativeLocalPos.y < frameSizeY);
        const bool isTitleBar = data->callbacks->isInsideTitleBarDraggableArea(qtScenePos);
        const bool isFixedSize = data->callbacks->isWindowFixedSize();
        const bool dontOverrideCursor = data->callbacks->getProperty(kDontOverrideCursorVar, false).toBool();
        const bool dontToggleMaximize = data->callbacks->getProperty(kDontToggleMaximizeVar, false).toBool();

        if (dontToggleMaximize) {
            static bool once = false;
            if (!once) {
                once = true;
                DEBUG << "To disable window maximization, you should remove the "
                         "WS_MAXIMIZEBOX style from the window instead. FramelessHelper "
                         "won't do that for you, so you'll have to do it manually yourself.";
            }
        }

        if (frameBorderVisible) {
            // This will handle the left, right and bottom parts of the frame
            // because we didn't change them.
            const LRESULT originalHitTestResult = ::DefWindowProcW(hWnd, WM_NCHITTEST, 0, lParam);
            if (originalHitTestResult != HTCLIENT) {
                *result = ((isFixedSize || dontOverrideCursor) ? HTBORDER : originalHitTestResult);
                return true;
            }
            if (full) {
                *result = HTCLIENT;
                return true;
            }
            if (max) {
                *result = (isTitleBar ? HTCAPTION : HTCLIENT);
                return true;
            }
            // At this point, we know that the cursor is inside the client area
            // so it has to be either the little border at the top of our custom
            // title bar or the drag bar. Apparently, it must be the drag bar or
            // the little border at the top which the user can use to move or
            // resize the window.
            if (isTop) {
                // Return HTCLIENT instead of HTBORDER here, because the mouse is
                // inside our homemade title bar now, return HTCLIENT to let our
                // title bar can still capture mouse events.
                *result = ((isFixedSize || dontOverrideCursor) ? HTCLIENT : HTTOP);
                return true;
            }
            if (isTitleBar) {
                *result = HTCAPTION;
                return true;
            }
            *result = HTCLIENT;
        } else {
            if (full) {
                *result = HTCLIENT;
                return true;
            }
            if (max) {
                *result = (isTitleBar ? HTCAPTION : HTCLIENT);
                return true;
            }
            if (!isFixedSize) {
                const bool isBottom = (nativeLocalPos.y >= (clientHeight - frameSizeY));
                // Make the border a little wider to let the user easy to resize on corners.
                const auto scaleFactor = ((isTop || isBottom) ? qreal(2) : qreal(1));
                const int frameSizeX = Utils::getResizeBorderThickness(windowId, true, true);
                const int scaledFrameSizeX = std::round(qreal(frameSizeX) * scaleFactor);
                const bool isLeft = (nativeLocalPos.x < scaledFrameSizeX);
                const bool isRight = (nativeLocalPos.x >= (clientWidth - scaledFrameSizeX));
                if (dontOverrideCursor && (isTop || isBottom || isLeft || isRight)) {
                    // Return HTCLIENT instead of HTBORDER here, because the mouse is
                    // inside the window now, return HTCLIENT to let the controls
                    // inside our window can still capture mouse events.
                    *result = HTCLIENT;
                    return true;
                }
                if (isTop) {
                    if (isLeft) {
                        *result = HTTOPLEFT;
                        return true;
                    }
                    if (isRight) {
                        *result = HTTOPRIGHT;
                        return true;
                    }
                    *result = HTTOP;
                    return true;
                }
                if (isBottom) {
                    if (isLeft) {
                        *result = HTBOTTOMLEFT;
                        return true;
                    }
                    if (isRight) {
                        *result = HTBOTTOMRIGHT;
                        return true;
                    }
                    *result = HTBOTTOM;
                    return true;
                }
                if (isLeft) {
                    *result = HTLEFT;
                    return true;
                }
                if (isRight) {
                    *result = HTRIGHT;
                    return true;
                }
            }
            if (isTitleBar) {
                *result = HTCAPTION;
                return true;
            }
            *result = HTCLIENT;
        }
        return true;
    }
    case WM_MOUSEMOVE:
        if ((data->lastHitTestResult != WindowPart::ChromeButton) && data->mouseLeaveBlocked) {
            data->mouseLeaveBlocked = false;
            std::ignore = requestForMouseLeaveMessage(hWnd, false);
        }
        break;
    case WM_NCMOUSEMOVE:
    case WM_NCLBUTTONDOWN:
    case WM_NCLBUTTONUP:
    case WM_NCLBUTTONDBLCLK:
    case WM_NCRBUTTONDOWN:
    case WM_NCRBUTTONUP:
    case WM_NCRBUTTONDBLCLK:
    case WM_NCMBUTTONDOWN:
    case WM_NCMBUTTONUP:
    case WM_NCMBUTTONDBLCLK:
    case WM_NCXBUTTONDOWN:
    case WM_NCXBUTTONUP:
    case WM_NCXBUTTONDBLCLK:
#if 0 // ### TODO: How to handle touch events?
    case WM_NCPOINTERUPDATE:
    case WM_NCPOINTERDOWN:
    case WM_NCPOINTERUP:
#endif
    case WM_NCMOUSEHOVER: {
        const WindowPart currentWindowPart = data->lastHitTestResult;
        if (uMsg == WM_NCMOUSEMOVE) {
            if (currentWindowPart != WindowPart::ChromeButton) {
                std::ignore = data->callbacks->resetQtGrabbedControl();
                if (data->mouseLeaveBlocked) {
                    emulateClientAreaMessage(WM_NCMOUSELEAVE);
                }
            }

            // We need to make sure we get the right hit-test result when a WM_NCMOUSELEAVE comes,
            // so we reset it when we receive a WM_NCMOUSEMOVE.

            // If the mouse is entering the client area, there must be a WM_NCHITTEST setting
            // it to `Client` before the WM_NCMOUSELEAVE comes;
            // If the mouse is leaving the window, current window part remains as `Outside`.
            data->lastHitTestResult = WindowPart::Outside;
        }

        if (currentWindowPart == WindowPart::ChromeButton) {
            emulateClientAreaMessage();
            if (uMsg == WM_NCMOUSEMOVE) {
                // ### FIXME FIXME FIXME
                // ### FIXME: Calling DefWindowProc() here is really dangerous, investigate how to avoid doing this.
                // ### FIXME FIXME FIXME
                *result = ::DefWindowProcW(hWnd, WM_NCMOUSEMOVE, wParam, lParam);
            } else {
                // According to MSDN, we should return non-zero for X button messages to indicate
                // we have handled these messages (due to historical reasons), for all other messages
                // we should return zero instead.
                *result = (((uMsg >= WM_NCXBUTTONDOWN) && (uMsg <= WM_NCXBUTTONDBLCLK)) ? TRUE : FALSE);
            }
            return true;
        }
    } break;
    case WM_NCMOUSELEAVE: {
        const WindowPart currentWindowPart = data->lastHitTestResult;
        if (currentWindowPart == WindowPart::ChromeButton) {
            // If we press on the chrome button and move mouse, Windows will take the pressing area
            // as HTCLIENT which maybe because of our former retransmission of WM_NCLBUTTONDOWN, as
            // a result, a WM_NCMOUSELEAVE will come immediately and a lot of WM_MOUSEMOVE will come
            // if we move the mouse, we should track the mouse in advance.
            if (data->mouseLeaveBlocked) {
                data->mouseLeaveBlocked = false;
                std::ignore = requestForMouseLeaveMessage(hWnd, false);
            }
        } else {
            if (data->mouseLeaveBlocked) {
                // The mouse is moving from the chrome button to other non-client area, we should
                // emulate a WM_MOUSELEAVE message to reset the button state.
                emulateClientAreaMessage(WM_NCMOUSELEAVE);
            }

            if (currentWindowPart == WindowPart::Outside) {
                // Notice: we're not going to clear window part cache when the mouse leaves window
                // from client area, which means we will get previous window part as HTCLIENT if
                // the mouse leaves window from client area and enters window from non-client area,
                // but it has no bad effect.

                std::ignore = data->callbacks->resetQtGrabbedControl();
            }
        }
    } break;
#if (QT_VERSION < QT_VERSION_CHECK(6, 2, 2)) // I contributed this small technique to upstream Qt since 6.2.2
    case WM_WINDOWPOSCHANGING: {
        // Tell Windows to discard the entire contents of the client area, as re-using
        // parts of the client area would lead to jitter during resize.
        // Check the suggestedGeometry against the current one to only discard during
        // resize, and not a plain move, otherwise this flag will cause many extra
        // repaints during window move, which will slow down the general performance
        // of the application a lot.
        const auto windowPos = reinterpret_cast<LPWINDOWPOS>(lParam);
        const QRect suggestedFrameGeometry{ windowPos->x, windowPos->y, windowPos->cx, windowPos->cy };
        const QMargins frameMargins = (Utils::getWindowSystemFrameMargins(windowId) + Utils::getWindowCustomFrameMargins(qWindow));
        const QRect suggestedGeometry = (suggestedFrameGeometry - frameMargins);
        if (Utils::toNativePixels(qWindow, qWindow->size()) != suggestedGeometry.size()) {
            windowPos->flags |= SWP_NOCOPYBITS;
        }
    } break;
#endif
#if (QT_VERSION <= QT_VERSION_CHECK(6, 4, 2))
    case WM_GETDPISCALEDSIZE: {
        // QtBase commit 2cfca7fd1911cc82a22763152c04c65bc05bc19a introduced a bug
        // which caused the custom margins is ignored during the handling of the
        // WM_GETDPISCALEDSIZE message, it was shipped with Qt 6.2.1 ~ 6.4.2.
        // We workaround it by overriding the wrong handling directly.
        RECT clientRect = {};
        if (::GetClientRect(hWnd, &clientRect) == FALSE) {
            WARNING << Utils::getSystemErrorMessage(kGetClientRect);
            *result = FALSE; // Use the default linear DPI scaling provided by Windows.
            return true; // Jump over Qt's wrong handling logic.
        }
        const auto newDpi = UINT(wParam);
        const QSize oldSize = {RECT_WIDTH(clientRect), RECT_HEIGHT(clientRect)};
        const QSize newSize = Utils::rescaleSize(oldSize, data->dpi.x, newDpi);
        const auto suggestedSize = reinterpret_cast<LPSIZE>(lParam);
        suggestedSize->cx = newSize.width();
        suggestedSize->cy = newSize.height();
        // If the window frame is visible, we need to expand the suggested size, currently
        // it's pure client size, we need to add the frame size to it. Windows expects a
        // full window size, including the window frame.
        // If the window frame is not visible, the window size equals to the client size,
        // the suggested size doesn't need further adjustments.
        if (frameBorderVisible) {
            const int frameSizeX = Utils::getResizeBorderThicknessForDpi(true, newDpi);
            const int frameSizeY = Utils::getResizeBorderThicknessForDpi(false, newDpi);
            suggestedSize->cx += (frameSizeX * 2); // The size of the two resize borders on the left and right edge.
            suggestedSize->cy += frameSizeY; // Only add the bottom resize border. We don't have anything on the top edge.
                                             // Both the top resize border and the title bar are in the client area.
        }
        *result = TRUE; // We have set our preferred window size, don't use the default linear DPI scaling.
        return true; // Jump over Qt's wrong handling logic.
    }
#endif // (QT_VERSION <= QT_VERSION_CHECK(6, 4, 2))
    case WM_DPICHANGED: {
        const Dpi oldDpi = data->dpi;
        const auto newDpi = Dpi{ UINT(LOWORD(wParam)), UINT(HIWORD(wParam)) };
        DEBUG.noquote() << "New DPI for window" << hwnd2str(hWnd)
                        << "is" << newDpi << "(was" << oldDpi << ").";
        data->dpi = newDpi;
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 1))
        if (Utils::isValidGeometry(data->restoreGeometry)) {
            // Update the window size only. The position should not be changed.
            data->restoreGeometry.setSize(Utils::rescaleSize(data->restoreGeometry.size(), oldDpi.x, newDpi.x));
        }
#endif // (QT_VERSION < QT_VERSION_CHECK(6, 5, 1))
        data->callbacks->forceChildrenRepaint();
    } break;
    case WM_DWMCOMPOSITIONCHANGED:
        // Re-apply the custom window frame if recovered from the basic theme.
        std::ignore = Utils::updateWindowFrameMargins(windowId, false);
        break;
#if (QT_VERSION < QT_VERSION_CHECK(6, 5, 1))
    case WM_ENTERSIZEMOVE: // Sent to a window when the user drags the title bar or the resize border.
    case WM_EXITSIZEMOVE: // Sent to a window when the user releases the mouse button (from dragging the title bar or the resize border).
        updateRestoreGeometry(false);
        break;
    case WM_SIZE: {
        if (wParam != SIZE_MAXIMIZED) {
            break;
        }
        if (!Utils::isValidGeometry(data->restoreGeometry)) {
            updateRestoreGeometry(true);
            break;
        }
        WINDOWPLACEMENT wp;
        SecureZeroMemory(&wp, sizeof(wp));
        wp.length = sizeof(wp);
        if (::GetWindowPlacement(hWnd, &wp) == FALSE) {
            WARNING << Utils::getSystemErrorMessage(kGetWindowPlacement);
            break;
        }
        // The restore geometry is correct, no need to bother.
        if (rect2qrect(wp.rcNormalPosition) == data->restoreGeometry) {
            break;
        }
        // OK, the restore geometry is wrong, let's correct it then :)
        wp.rcNormalPosition = qrect2rect(data->restoreGeometry);
        if (::SetWindowPlacement(hWnd, &wp) == FALSE) {
            WARNING << Utils::getSystemErrorMessage(kSetWindowPlacement);
        }
    } break;
#endif // (QT_VERSION < QT_VERSION_CHECK(6, 5, 1))
    case WM_MOVE: {
        const HMONITOR currentMonitor = ::MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);
        Q_ASSERT(currentMonitor);
        if (!currentMonitor) {
            WARNING << Utils::getSystemErrorMessage(kMonitorFromWindow);
            break;
        }
        if (currentMonitor == data->monitor) {
            break;
        }
        data->monitor = currentMonitor;
        data->callbacks->forceChildrenRepaint();
    } break;
    case WM_SYSCOMMAND: {
        const WPARAM filteredWParam = (wParam & 0xFFF0);
        // When the window is fullscreened, don't enter screen saver or power
        // down the monitor (only a suggestion to the OS, the OS can still ignore
        // our request).
        if ((filteredWParam == SC_SCREENSAVE) || (filteredWParam == SC_MONITORPOWER)) {
            if (Utils::isFullScreen(windowId)) {
                *result = FALSE;
                return true;
            }
        }
    } break;
    case WM_STYLECHANGED: {
        // The window style has been changed.
        if (wParam & GWL_STYLE) {
            // We need a little delay here because Qt will always change the window style
            // first and then change the window geometry, however we need to check the window
            // geometry to see if we need to adjust the window style or not, so we need to
            // wait until the window geometry has been changed.
            QTimer::singleShot(0, qWindow, [windowId, hWnd](){
                // There's nothing to do when the window is fullscreened.
                if (Utils::isFullScreen(windowId)) {
                    return;
                }
                // Check if the window style is "broken" when we are not fullscreened.
                ::SetLastError(ERROR_SUCCESS);
                auto dwStyle = static_cast<DWORD>(::GetWindowLongPtrW(hWnd, GWL_STYLE));
                if (dwStyle == 0) {
                    WARNING << Utils::getSystemErrorMessage(kGetWindowLongPtrW);
                    return;
                }
                // Avoid infinite recursions by not touching the window style if it's
                // appropriate.
                const bool hasPopup = (dwStyle & WS_POPUP);
                const bool thickFrameMissing = !(dwStyle & WS_THICKFRAME);
                if (!(hasPopup || thickFrameMissing)) {
                    return;
                }
                dwStyle &= ~WS_POPUP;
                dwStyle |= WS_THICKFRAME;
                ::SetLastError(ERROR_SUCCESS);
                if (::SetWindowLongPtrW(hWnd, GWL_STYLE, LONG_PTR(dwStyle)) == 0) {
                    WARNING << Utils::getSystemErrorMessage(kSetWindowLongPtrW);
                }
            });
        }
        // The extended window style has been changed.
        if (wParam & GWL_EXSTYLE) {
        }
    } break;
    default:
        break;
    }

    if (!frameBorderVisible) {
        switch (uMsg) {
        case WM_NCUAHDRAWCAPTION:
        case WM_NCUAHDRAWFRAME: {
            // These undocumented messages are sent to draw themed window
            // borders. Block them to prevent drawing borders over the client
            // area.
            *result = FALSE;
            return true;
        }
        case WM_NCPAINT: {
            // 边框阴影处于非客户区的范围，因此如果直接阻止非客户区的绘制，会导致边框阴影丢失

            if (!Utils::isDwmCompositionEnabled()) {
                // Only block WM_NCPAINT when DWM composition is disabled. If
                // it's blocked when DWM composition is enabled, the frame
                // shadow won't be drawn.
                *result = FALSE;
                return true;
            } else {
                break;
            }
        }
        case WM_NCACTIVATE: {
            if (Utils::isDwmCompositionEnabled()) {
                // DefWindowProc won't repaint the window border if lParam (normally a HRGN)
                // is -1. See the following link's "lParam" section:
                // https://docs.microsoft.com/en-us/windows/win32/winmsg/wm-ncactivate
                // Don't use "*result = 0" here, otherwise the window won't respond to the
                // window activation state change.
                *result = ::DefWindowProcW(hWnd, WM_NCACTIVATE, wParam, -1);
            } else {
                if (wParam == FALSE) {
                    *result = TRUE;
                } else {
                    *result = FALSE;
                }
            }
            return true;
        }
        case WM_SETICON:
        case WM_SETTEXT: {
            // Disable painting while these messages are handled to prevent them
            // from drawing a window caption over the client area.
            ::SetLastError(ERROR_SUCCESS);
            const auto oldStyle = static_cast<DWORD>(::GetWindowLongPtrW(hWnd, GWL_STYLE));
            if (oldStyle == 0) {
                WARNING << Utils::getSystemErrorMessage(kGetWindowLongPtrW);
                break;
            }
            // Prevent Windows from drawing the default title bar by temporarily
            // toggling the WS_VISIBLE style.
            const DWORD newStyle = (oldStyle & ~WS_VISIBLE);
            ::SetLastError(ERROR_SUCCESS);
            if (::SetWindowLongPtrW(hWnd, GWL_STYLE, static_cast<LONG_PTR>(newStyle)) == 0) {
                WARNING << Utils::getSystemErrorMessage(kSetWindowLongPtrW);
                break;
            }
            std::ignore = Utils::triggerFrameChange(windowId);
            const LRESULT originalResult = ::DefWindowProcW(hWnd, uMsg, wParam, lParam);
            ::SetLastError(ERROR_SUCCESS);
            if (::SetWindowLongPtrW(hWnd, GWL_STYLE, static_cast<LONG_PTR>(oldStyle)) == 0) {
                WARNING << Utils::getSystemErrorMessage(kSetWindowLongPtrW);
                break;
            }
            std::ignore = Utils::triggerFrameChange(windowId);
            *result = originalResult;
            return true;
        }
        default:
            break;
        }
    }

#if 0 // Conflicts with our blur mode setting.
    if ((uMsg == WM_DWMCOMPOSITIONCHANGED) || (uMsg == WM_DWMCOLORIZATIONCOLORCHANGED)) {
        if (Utils::isWindowAccelerated(qWindow) && Utils::isWindowTransparent(qWindow)) {
            std::ignore = Utils::updateFramebufferTransparency(windowId);
        }
    }
#endif

    const bool wallpaperChanged = ((uMsg == WM_SETTINGCHANGE) && (wParam == SPI_SETDESKWALLPAPER));
    bool systemThemeChanged = ((uMsg == WM_THEMECHANGED) || (uMsg == WM_SYSCOLORCHANGE)
                               || (uMsg == WM_DWMCOLORIZATIONCOLORCHANGED));
    if (WindowsVersionHelper::isWin10RS1OrGreater()) {
        if (uMsg == WM_SETTINGCHANGE) {
            if ((wParam == 0) && (lParam != 0) // lParam sometimes may be NULL.
                && (std::wcscmp(reinterpret_cast<LPCWSTR>(lParam), kThemeSettingChangeEventName) == 0)) {
                systemThemeChanged = true;
                if (WindowsVersionHelper::isWin10RS5OrGreater()) {
                    const bool dark = (FramelessManager::instance()->systemTheme() == SystemTheme::Dark);
                    const auto isWidget = [&data]() -> bool {
                        const auto widget = data->callbacks->getWidgetHandle();
                        return (widget && widget->isWidgetType());
                    }();
                    if (!isWidget) {
                        // Causes some QtWidgets paint incorrectly, so only apply to Qt Quick applications.
                        std::ignore = Utils::updateGlobalWin32ControlsTheme(windowId, dark);
                    }
                    std::ignore = Utils::refreshWin32ThemeResources(windowId, dark);
                }
            }
        }
    }
    if (systemThemeChanged || wallpaperChanged) {
        // Sometimes the FramelessManager instance may be destroyed already.
        if (FramelessManager * const manager = FramelessManager::instance()) {
            if (FramelessManagerPrivate * const managerPriv = FramelessManagerPrivate::get(manager)) {
                if (systemThemeChanged) {
                    managerPriv->notifySystemThemeHasChangedOrNot();
                }
                if (wallpaperChanged) {
                    managerPriv->notifyWallpaperHasChangedOrNot();
                }
            }
        }
    }

    return false;
}

FRAMELESSHELPER_END_NAMESPACE

#endif // native_impl

#endif // Q_OS_WINDOWS
