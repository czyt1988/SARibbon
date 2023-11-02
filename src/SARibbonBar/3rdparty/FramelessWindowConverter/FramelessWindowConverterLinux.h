#ifndef FRAMELESSWINDOWCONVERTERLINUX_H
#define FRAMELESSWINDOWCONVERTERLINUX_H

#ifdef __linux__

#include "FramelessWindowConverter_p.h"

struct _XDisplay;
typedef _XDisplay Display;

struct xcb_connection_t;
typedef xcb_connection_t xcb_connection_t;

typedef unsigned int xcb_window_t;

namespace FWC
{

class FramelessWindowConverterLinux : public FramelessWindowConverterPrivate
{
public:
    FramelessWindowConverterLinux(FramelessWindowConverter* q);
    void convertToFrameless() override;
    void convertToWindowWithFrame() override;
    bool filterNativeEvent(void *message, long *result) override;
    void minimizeWindow() override;
    void maximizeWindow() override;
    void restoreWindow() override;
    void closeWindow() override;
    void toggleFullscreen() override;

private:
    xcb_connection_t *connection;
    xcb_window_t rootWindow;
    Display* display;
    xcb_window_t windowHandle;

    void changeCursorShape(unsigned int shape);
    int xiOpCode; // XInput
    unsigned int lastButtonPressTime;   
     bool isSystemOpRunning = false; // resize or move
    FWCRect getCurrentWindowFrame();
    unsigned int getAtom(const char* name);
};

}

#endif
#endif // FRAMELESSWINDOWCONVERTERLINUX_H
