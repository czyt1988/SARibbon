#ifndef NATIVEWINDOWEVENTFILTER_P_H
#define NATIVEWINDOWEVENTFILTER_P_H

namespace FWC
{

class FramelessWindowConverter;

// Same structure as _NET_WM_MOVERESIZ_*
//#define _NET_WM_MOVERESIZE_SIZE_TOPLEFT      0
//#define _NET_WM_MOVERESIZE_SIZE_TOP          1
//#define _NET_WM_MOVERESIZE_SIZE_TOPRIGHT     2
//#define _NET_WM_MOVERESIZE_SIZE_RIGHT        3
//#define _NET_WM_MOVERESIZE_SIZE_BOTTOMRIGHT  4
//#define _NET_WM_MOVERESIZE_SIZE_BOTTOM       5
//#define _NET_WM_MOVERESIZE_SIZE_BOTTOMLEFT   6
//#define _NET_WM_MOVERESIZE_SIZE_LEFT         7
//#define _NET_WM_MOVERESIZE_MOVE              8   /* movement only */
enum class FWCBorderHitTestResult
{
    TOP_LEFT,
    TOP,
    TOP_RIGHT,
    RIGHT,
    BOTTOM_RIGHT,
    BOTTOM,
    BOTTOM_LEFT,
    LEFT,
    CLIENT,
    NONE
};

struct FWCRect
{
    int x1;
    int y1;
    int x2;
    int y2;

    FWCRect(int inX1, int inY1, int inX2, int inY2) : x1(inX1), y1(inY1), x2(inX2), y2(inY2) {}
    inline int left()  { return x1; }
    inline int right()  { return x2; }
    inline int top()  { return y1; }
    inline int bottom()  { return y2; }
};

struct FWCPoint
{
    int x;
    int y;

    FWCPoint(int inX, int inY) : x(inX), y(inY) {}
    FWCPoint() : x(0), y(0) {}
};

struct FWCFloatingPoint
{
    double x;
    double y;

    FWCFloatingPoint(double inX, double inY) : x(inX), y(inY) {}
    FWCFloatingPoint() : x(0), y(0) {}
};

class FramelessWindowConverterPrivate
{
public:
    static FramelessWindowConverterPrivate* create(FramelessWindowConverter* q);
    FramelessWindowConverterPrivate() = delete;
    FramelessWindowConverterPrivate(FramelessWindowConverter* q) : q_ptr(q) {}
    virtual ~FramelessWindowConverterPrivate() {}
    virtual bool filterNativeEvent(void *message, long *result) = 0;

    virtual void convertToFrameless() = 0;
    virtual void convertToWindowWithFrame() = 0;

    virtual void hideForTranslucency() {}
    virtual void showForTranslucency() {}
    virtual void minimizeWindow() = 0;
    virtual void maximizeWindow() = 0;
    virtual void closeWindow() = 0;
    virtual void restoreWindow() = 0;
    virtual void toggleFullscreen() = 0;
    virtual void setEnableShadow() {}

    // MacOS
    virtual void setHiddenGreenTrafficLightOnMacOS(bool inHidden) { (void)inHidden; }
    virtual void setHiddenRedTrafficLightOnMacOS(bool inHidden)  { (void)inHidden; }
    virtual void setHiddenYellowTrafficLightOnMacOS(bool inHidden)  { (void)inHidden; }

    virtual void setEnabledGreenTrafficLightOnMacOS(bool inEnabled)  { (void)inEnabled; }
    virtual void setEnabledRedTrafficLightOnMacOS(bool inEnabled)  { (void)inEnabled; }
    virtual void setEnabledYellowTrafficLightOnMacOS(bool inEnabled) { (void)inEnabled; }

    virtual void setHorizontalAlignmentOfTrafficLightsOnMacOS() {}
    virtual void setUpperLeftXPositionOfTrafficLightsOnMacOS() {}
    virtual void setUpperLeftYPositionOfTrafficLightsOnMacOS() {}

protected:
    FramelessWindowConverter* q_ptr;
    virtual FWCBorderHitTestResult doBorderHitTest(FWCRect inWindowRect, FWCPoint inMousePosition, int inBorderWidth);
    bool bFullscreen = false;
};

}

#endif // NATIVEWINDOWEVENTFILTER_P_H
