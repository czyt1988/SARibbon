#ifndef WINDOWFRAME_H
#define WINDOWFRAME_H

#include <functional>

namespace FWC
{

class FramelessWindowConverter
{
public:
    FramelessWindowConverter();
    virtual ~FramelessWindowConverter();

    virtual bool filterNativeEvents(void* message, long *result) final;

    void convertWindowToFrameless(unsigned long long inWindowHandle, std::function<void(void)> releaseMouseGrab = [](){});

    void toggleWindowFrameAfterConversion();
    bool getIsFrameless();

    inline unsigned long long getWindowHandle() { return windowHandle; }
    inline std::function<bool(int,int)> getShouldPerformWindowDrag() { return shouldPerformWindowDrag; }

    // Decide if the window should be dragged when a mouse click was detected at current mouse Position
    // default inShouldPerformWindowDrag means you can click anywhere in the window to move it
    void setShouldPerformWindowDrag(std::function<bool(int,int)> inShouldPerformWindowDrag);

    // Release Mouse Grab to give the window manager control over it
    inline std::function<void(void)> getReleaseMouseGrab() { return releaseMouseGrab; }
    void setReleaseMouseGrab(std::function<void(void)> inReleaseMouseGrab);

    // Border Width for resizing (default is 8 pixels)
    void setBorderWidth(int inBorderWidth);
    int getBorderWidth();
    void setEnableResizing(bool inEnableResizing);
    bool getEnableResizing();

    // Minimum and maximum window sizes
    int getMinimumWindowWidth();
    int getMinimumWindowHeight();
    int getMaximumWindowWidth();
    int getMaximumWindowHeight();

    void setMinimumWindowWidth(int inWidth);
    void setMinimumWindowHeight(int inHeight);
    void setMaximumWindowWidth(int inWidth);
    void setMaximumWindowHeight(int inHeight);

    void setMinMaxWindowSizes(int inMinWidth, int inMinHeight, int inMaxWidth, int inMaxHeight);

    // Window control
    void hideForTranslucency();
    void showForTranslucency();
    void minimizeWindow();
    void maximizeWindow();
    void restoreWindow();
    void closeWindow();
    void toggleFullscreen();

    // Only works on macOS and Windows
    void setEnableShadow(bool inEnableShadow);
    bool getHasShadow();

    // macOS settings
    void useTrafficLightsOnMacOS(bool inUseTrafficLights);
    bool isUsingTrafficLightsOnMacOS();

    // Hide/Show or Enable/Disable individual traffic lights on macOS
    void setHiddenGreenTrafficLightOnMacOS(bool inHidden);
    void setHiddenRedTrafficLightOnMacOS(bool inHidden);
    void setHiddenYellowTrafficLightOnMacOS(bool inHidden);
    bool getHiddenGreenTrafficLightOnMacOS();
    bool getHiddenRedTrafficLightOnMacOS();
    bool getHiddenYellowTrafficLightOnMacOS();

    void setEnabledGreenTrafficLightOnMacOS(bool inEnabled);
    void setEnabledRedTrafficLightOnMacOS(bool inEnabled);
    void setEnabledYellowTrafficLightOnMacOS(bool inEnabled);
    bool getEnabledGreenTrafficLightOnMacOS();
    bool getEnabledRedTrafficLightOnMacOS();
    bool getEnabledYellowTrafficLightOnMacOS();

    // Positions are relative to upper left window corner
    void setHorizontalAlignmentOfTrafficLightsOnMacOS(bool inHorizontal);
    bool getHorizontalAlignmentOfTrafficLightsOnMacOS();
    void setUpperLeftXPositionOfTrafficLightsOnMacOS(int inXPos);
    void setUpperLeftYPositionOfTrafficLightsOnMacOS(int inYPos);
    int getUpperLeftXPositionOfTrafficLightsOnMacOS();
    int getUpperLeftYPositionOfTrafficLightsOnMacOS();

private:
    class FramelessWindowConverterPrivate* d_ptr;
    unsigned long long windowHandle;
    std::function<bool(int,int)> shouldPerformWindowDrag = [](int, int) { return true; };
    std::function<void(void)> releaseMouseGrab;
    int borderWidth = 8;
    int minimumWindowWidth;
    int minimumWindowHeight;
    int maximumWindowWidth;
    int maximumWindowHeight; 
    bool bUseTrafficLights = false;
    bool bIsFramless = false;
    bool bHasShadow = false;
    bool bEnableResizing = true;

    // macOS Settings
    bool bHiddenGreen = false;
    bool bHiddenRed = false;
    bool bHiddenYellow = false;

    bool bEnabledGreen = true;
    bool bEnabledRed = true;
    bool bEnabledYellow = true;

    // Default is horizontal
    bool bTrafficLightsAlignmentHorizontal = true;
    int xPosOfTrafficLights = 7, yPosOfTrafficLights = 3;
};

}

#endif // WINDOWFRAME_H
