#ifdef __linux__

#include "FramelessWindowConverter.h"
#include "FramelessWindowConverterLinux.h"

#include <X11/cursorfont.h>
#include <X11/Xlib-xcb.h>
#include <X11/extensions/XI2proto.h>
#include <string.h>

using namespace FWC;

FramelessWindowConverterLinux::FramelessWindowConverterLinux(FramelessWindowConverter* q) : FramelessWindowConverterPrivate(q), lastButtonPressTime(0)
{

}

void FramelessWindowConverterLinux::convertToFrameless()
{
    display =  XOpenDisplay(nullptr);
    connection = XGetXCBConnection(display);
    rootWindow = xcb_setup_roots_iterator (xcb_get_setup (connection)).data->root;
    windowHandle = static_cast<xcb_window_t>(q_ptr->getWindowHandle());
}

void FramelessWindowConverterLinux::convertToWindowWithFrame()
{
    // Do nothing, it just works!
}

void FramelessWindowConverterLinux::changeCursorShape(unsigned int shape)
{
    Cursor cursor = XCreateFontCursor(display, shape);
    XDefineCursor(display, windowHandle, cursor);
    XFlush(display);
}

unsigned int FramelessWindowConverterLinux::getAtom(const char* name)
{
    xcb_intern_atom_cookie_t cookie;
    xcb_intern_atom_reply_t *reply;

    cookie = xcb_intern_atom(connection, 0, static_cast<uint16_t>(strlen(name)), name);
    if ((reply = xcb_intern_atom_reply(connection, cookie, nullptr)))
    {
        unsigned int replyAtom = reply->atom;
        free(reply);
        return replyAtom;
    }
    else return 0;
}

void FramelessWindowConverterLinux::minimizeWindow()
{
    xcb_client_message_event_t event;
    event.response_type = XCB_CLIENT_MESSAGE;
    event.format = 32;
    event.sequence = 0;
    event.window = windowHandle;
    event.type = getAtom("WM_CHANGE_STATE");
    event.data.data32[0] = 3; // IconicState
    event.data.data32[1] = 0;
    event.data.data32[2] = 0;
    event.data.data32[3] = 0;
    event.data.data32[4] = 0;

    xcb_send_event(connection, 0, rootWindow,
                   XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                   reinterpret_cast<char*>(&event));
    xcb_flush(connection);
}

void FramelessWindowConverterLinux::maximizeWindow()
{
    xcb_client_message_event_t event;
    event.response_type = XCB_CLIENT_MESSAGE;
    event.format = 32;
    event.sequence = 0;
    event.window = windowHandle;
    event.type = getAtom("_NET_WM_STATE");
    event.data.data32[0] = 1; //_NET_WM_STATE_ADD
    event.data.data32[1] = getAtom("_NET_WM_STATE_MAXIMIZED_HORZ");
    event.data.data32[2] = getAtom("_NET_WM_STATE_MAXIMIZED_VERT");
    event.data.data32[3] = 0;
    event.data.data32[4] = 0;

    xcb_send_event(connection, 0, rootWindow,
                   XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                   reinterpret_cast<char*>(&event));
    xcb_flush(connection);
}

void FramelessWindowConverterLinux::restoreWindow()
{
    xcb_client_message_event_t event;
    event.response_type = XCB_CLIENT_MESSAGE;
    event.format = 32;
    event.sequence = 0;
    event.window = windowHandle;
    event.type = getAtom("_NET_WM_STATE");
    event.data.data32[0] = 0; //_NET_WM_STATE_REMOVE
    event.data.data32[1] = getAtom("_NET_WM_STATE_MAXIMIZED_HORZ");
    event.data.data32[2] = getAtom("_NET_WM_STATE_MAXIMIZED_VERT");
    event.data.data32[3] = 0;
    event.data.data32[4] = 0;

    xcb_send_event(connection, 0, rootWindow,
                   XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                   reinterpret_cast<char*>(&event));
    xcb_flush(connection);
}

void FramelessWindowConverterLinux::closeWindow()
{
    xcb_client_message_event_t event;
    event.response_type = XCB_CLIENT_MESSAGE;
    event.format = 32;
    event.sequence = 0;
    event.window = windowHandle;
    event.type = getAtom("WM_PROTOCOLS");
    event.data.data32[0] = getAtom("WM_DELETE_WINDOW");
    event.data.data32[1] = CurrentTime;
    event.data.data32[2] = 0;
    event.data.data32[3] = 0;
    event.data.data32[4] = 0;

    xcb_send_event(connection, 0, rootWindow,
                   XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                   reinterpret_cast<char*>(&event));
    xcb_flush(connection);
}

void FramelessWindowConverterLinux::toggleFullscreen()
{
    bFullscreen = !bFullscreen;

    xcb_client_message_event_t event;
    event.response_type = XCB_CLIENT_MESSAGE;
    event.format = 32;
    event.sequence = 0;
    event.window = windowHandle;
    event.type = getAtom("_NET_WM_STATE");
    event.data.data32[0] = 2; //_NET_WM_STATE_TOGGLE
    event.data.data32[1] = getAtom("_NET_WM_STATE_FULLSCREEN");
    event.data.data32[2] = 0;
    event.data.data32[3] = 0;
    event.data.data32[4] = 0;

    xcb_send_event(connection, 0, rootWindow,
                   XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                   reinterpret_cast<char*>(&event));
    xcb_flush(connection);
}

bool isXIEvent(xcb_generic_event_t *event, int opCode)
{
    xcb_ge_event_t *e = reinterpret_cast<xcb_ge_event_t*>(event);
    return e->pad0 == opCode;
}

bool isXIType(xcb_generic_event_t *event, int opCode, uint16_t type)
{
    if (!isXIEvent(event, opCode))
        return false;

    xXIGenericDeviceEvent *xiEvent = reinterpret_cast<xXIGenericDeviceEvent *>(event);
    return xiEvent->evtype == type;

}

int fixed1616ToInt(FP1616 val)
{
    return int(val) / 0x10000;
}

FWCRect FramelessWindowConverterLinux::getCurrentWindowFrame()
{
    xcb_get_geometry_cookie_t geom_cookie = xcb_get_geometry (connection, static_cast<xcb_drawable_t>(windowHandle));
    xcb_get_geometry_reply_t* frame_geometry = xcb_get_geometry_reply(connection, geom_cookie, nullptr);
    FWCRect rect(0, 0, frame_geometry->width, frame_geometry->height);
    free(frame_geometry);
    return rect;
}

bool FramelessWindowConverterLinux::filterNativeEvent(void *message, long*)
{
    xcb_generic_event_t* ev = static_cast<xcb_generic_event_t *>(message);

    if ((ev->response_type & ~0x80) == XCB_GE_GENERIC)
    {
        int xiEventBase, xiErrorBase;
        XQueryExtension(display, "XInputExtension", &xiOpCode, &xiEventBase, &xiErrorBase);

        if (isXIType(ev, xiOpCode, XI_Motion))
        {
            if(isSystemOpRunning)
            {
                // Change event to button release event, fixes some bugs with Qt and _NET_WM_MOVERESIZE
                // After _NET_WM_MOVERESIZE ends and the mouse is moved this gets called and Qt gets a button release event.
                // This is very hacky, but it works :)
                xXIGenericDeviceEvent *xiEvent = reinterpret_cast<xXIGenericDeviceEvent *>(ev);
                xiEvent->evtype = XI_ButtonRelease;

                isSystemOpRunning = false;
            }

            xXIDeviceEvent* xev = reinterpret_cast<xXIDeviceEvent*>(ev);
            const int x = fixed1616ToInt(xev->event_x);
            const int y = fixed1616ToInt(xev->event_y);
            FWCPoint mousePos(x, y);

            switch (doBorderHitTest(getCurrentWindowFrame(), mousePos, q_ptr->getBorderWidth()))
            {
            case FWCBorderHitTestResult::LEFT:
                changeCursorShape( XC_left_side);
                break;
            case FWCBorderHitTestResult::RIGHT:
                changeCursorShape( XC_right_side);
                break;
            case FWCBorderHitTestResult::TOP:
                changeCursorShape( XC_top_side);
                break;
            case FWCBorderHitTestResult::BOTTOM:
                changeCursorShape( XC_bottom_side);
                break;
            case FWCBorderHitTestResult::BOTTOM_LEFT:
                changeCursorShape( XC_bottom_left_corner);
                break;
            case FWCBorderHitTestResult::BOTTOM_RIGHT:
                changeCursorShape( XC_bottom_right_corner);
                break;
            case FWCBorderHitTestResult::TOP_LEFT:
                changeCursorShape( XC_top_left_corner);
                break;
            case FWCBorderHitTestResult::TOP_RIGHT:
                changeCursorShape( XC_top_right_corner);
                break;
            default:
                changeCursorShape( XC_arrow);
                break;
            }

            return false;
        }
        if (isXIType(ev, xiOpCode, XI_ButtonPress))
        {
            xXIDeviceEvent* deviceEvent = reinterpret_cast<xXIDeviceEvent*>(ev);

            if(deviceEvent->detail != 1) return false;

            const int x = fixed1616ToInt(deviceEvent->event_x);
            const int y = fixed1616ToInt(deviceEvent->event_y);
            FWCPoint mousePos(x, y);

            // Only this widget is used for dragging.
            bool shouldDrag = q_ptr->getShouldPerformWindowDrag()(mousePos.x, mousePos.y);

            // Double Click
            if(lastButtonPressTime != 0 && (deviceEvent->time - lastButtonPressTime) < 400 && shouldDrag)
            {
                xcb_get_property_cookie_t cookie = xcb_get_property(connection, false, windowHandle, getAtom("_NET_WM_STATE"), XCB_ATOM_ATOM, 0, 32);
                xcb_get_property_reply_t* reply = xcb_get_property_reply(connection, cookie, nullptr);
                xcb_atom_t* isMaximized = reinterpret_cast<xcb_atom_t*>(xcb_get_property_value(reply));
                free(reply);

                xcb_client_message_event_t event;
                event.response_type = XCB_CLIENT_MESSAGE;
                event.format = 32;
                event.sequence = 0;
                event.window = windowHandle;
                event.type = getAtom("_NET_WM_STATE");
                event.data.data32[1] = getAtom("_NET_WM_STATE_MAXIMIZED_HORZ");
                event.data.data32[2] = getAtom("_NET_WM_STATE_MAXIMIZED_VERT");
                event.data.data32[3] = 0;
                event.data.data32[4] = 0;

                if((*isMaximized == getAtom("_NET_WM_STATE_MAXIMIZED_VERT")) ||
                        (*isMaximized == getAtom("_NET_WM_STATE_MAXIMIZED_HORZ")))
                {
                    event.data.data32[0] = 0; //_NET_WM_STATE_REMOVE
                }
                else
                {
                    event.data.data32[0] = 1; //_NET_WM_STATE_ADD
                }

                xcb_send_event(connection, 0, rootWindow,
                               XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                               reinterpret_cast<char*>(&event));
                xcb_flush(connection);

                return false;
            }
            lastButtonPressTime = deviceEvent->time;

            //                        xcb_client_message_event_t event;
            //                        event.response_type = XCB_CLIENT_MESSAGE;
            //                        event.format = 32;
            //                        event.sequence = 0;
            //                        event.window = windowHandle;
            //                        event.type = getAtom("_NET_WM_MOVERESIZE");
            //                        event.data.data32[0] = fixed1616ToInt(deviceEvent->root_x);
            //                        event.data.data32[1] = fixed1616ToInt(deviceEvent->root_y);
            //                        // Either one of the different resize events or a Move event
            //                        event.data.data32[2] = static_cast<int>(doBorderHitTest(getCurrentWindowFrame(), mousePos, q_ptr->getBorderWidth()));
            //                        event.data.data32[3] = XCB_BUTTON_INDEX_1;
            //                        event.data.data32[4] = 0;

            //                         q_ptr->getReleaseMouseGrab()();
            //                        xcb_send_event(connection, 0, rootWindow,
            //                                       XCB_EVENT_MASK_STRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
            //                                       (const char *)&event);
            //                        XFlush(display);

            // Test mouse cursor position in window and check if the window should be dragged
            FWCBorderHitTestResult bhResult = doBorderHitTest(getCurrentWindowFrame(), mousePos, q_ptr->getBorderWidth());
            if(bhResult == FWCBorderHitTestResult::CLIENT && shouldDrag == false) return false;

            // Qt gives a warning of a unhandled client message with xcb events -> use xlib event instead :)
            XEvent xev;
            // Set type of event
            // Either one of the different resize events or a Move event
            xev.xclient.data.l[2] = static_cast<int>(bhResult);

            Atom netMoveResize = getAtom("_NET_WM_MOVERESIZE");
            xev.xclient.type = ClientMessage;
            xev.xclient.message_type = netMoveResize;
            xev.xclient.display = display;
            xev.xclient.window = windowHandle;
            xev.xclient.format = 32;
            xev.xclient.data.l[0] = fixed1616ToInt(deviceEvent->root_x);
            xev.xclient.data.l[1] = fixed1616ToInt(deviceEvent->root_y);

            xev.xclient.data.l[3] = Button1;
            xev.xclient.data.l[4] = 0;

            q_ptr->getReleaseMouseGrab()();
            XSendEvent(display, rootWindow, False,
                       SubstructureRedirectMask | SubstructureNotifyMask, &xev);
            XFlush(display);

            isSystemOpRunning = true;

            return false;
        }
    }

    return false;
}

#endif
