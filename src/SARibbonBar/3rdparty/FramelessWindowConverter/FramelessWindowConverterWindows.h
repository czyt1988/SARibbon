#ifndef WINDOWSNATIVEWINDOWEVENTFILTER_P_H
#define WINDOWSNATIVEWINDOWEVENTFILTER_P_H

#ifdef WIN32

#include "FramelessWindowConverter_p.h"
#include <qt_windows.h>

namespace FWC
{

class FramelessWindowConverterWindows : public FramelessWindowConverterPrivate
{
public:
    FramelessWindowConverterWindows(FramelessWindowConverter* q);
    bool filterNativeEvent(void *message, long *result) override;
    void convertToFrameless() override;
    void convertToWindowWithFrame() override;
    void minimizeWindow() override;
    void maximizeWindow() override;
    void restoreWindow() override;
    void closeWindow() override;
    void toggleFullscreen() override;
    void setEnableShadow() override;

private:
    FWCRect getCurrentClientRect();
    FWCRect getCurrentWindowRect();
    FWCPoint getCurrentMousePos(LPARAM lParam);
    HWND handle;

    // Not using WS_CAPTION in borderless, since it messes with translucent Qt-Windows.
    enum class Style : DWORD {
        windowed         = WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        // WS_CAPTION is needed for minimize/maximize animation -> it gets added to the window "on demand",
        // to not get rounded corners with translucent windows (no idea why windows does this)
        // The WM_SYSMENU etc. messes with maximized windows when they get reactivated -> windows positions them outside the screen
        aero_borderless  = WS_POPUP | WS_THICKFRAME/* | WS_CAPTION*/ | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
        maximized_borderless = WS_POPUP | WS_THICKFRAME
    };
    void setFrameless(bool enabled);
    class FramelessWindowConverterPrivate* d_ptr;
};

}

#endif
#endif // WINDOWSNATIVEWINDOWEVENTFILTER_P_H
