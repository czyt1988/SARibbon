#ifdef __APPLE__

#include "FramelessWindowConverterMacos.h"
#include "FramelessWindowConverter.h"

#import <AppKit/AppKit.h>
#import <objc/runtime.h>

#include <QDebug>

using namespace FWC;

static id exitFullscreenObserver;
static id resizeObserver;

// TrafficLightsHelper objective-c class
@interface TrafficLightsHelper : NSObject
                                 @property (assign, nonatomic) NSWindow* window;
@property (assign, nonatomic) FramelessWindowConverterMacos* fwcM;
- (id) initWithFWCMAndWindow:(FramelessWindowConverterMacos*)fwcM :(NSWindow*)inWin;
- (BOOL)_mouseInGroup:(NSButton *)button;
- (void) minimizeButtonAction:(id)sender;
- (void) closeButtonAction:(id)sender;
- (void) zoomButtonAction:(id)sender;
- (void) fakeResizeAction:(id)sender;
@end

@implementation TrafficLightsHelper
@synthesize fwcM;
@synthesize window;
- (id) initWithFWCMAndWindow:(FramelessWindowConverterMacos*)inFWCM :(NSWindow*)inWin
{
    if((self = [super init]) != nil)
    {
        fwcM = inFWCM;
        window = inWin;
        return self;
    }
    else return nil;
}

// Hack to get proper traffig light highlighting
static bool isMouseInGroup = false;
- (BOOL)_mouseInGroup:(NSButton *)button
{
    (void)button;
    return isMouseInGroup;
}

- (void) minimizeButtonAction:(id)sender
{
    (void)sender;
    self.fwcM->minimizeWindow();
}

- (void) closeButtonAction:(id)sender
{
    (void)sender;
    self.fwcM->closeWindow();
}

- (void) zoomButtonAction:(id)sender
{
    (void)sender;
    NSEventModifierFlags currentFlags = [NSEvent modifierFlags];
    if(currentFlags & NSEventModifierFlagOption)
    {
        self.fwcM->maximizeWindow(); // also minimizes
    }
    else
    {
        self.fwcM->toggleFullscreen();
    }
}

- (void) fakeResizeAction:(id)sender
{
    (void)sender;
    NSRect noFrame = self.window.frame;
    noFrame.size.width += 1;
    [self.window setFrame:noFrame display:YES];
    noFrame.size.width -= 1;
    [self.window setFrame:noFrame display:YES];
}

@end
static TrafficLightsHelper* tlHelper;
// END of TrafficLightsHelper objective-c class


FramelessWindowConverterMacos::FramelessWindowConverterMacos(FramelessWindowConverter* q) : FramelessWindowConverterPrivate(q)
{
    nativeWidgetView = nil;
    window = nil;
    fullScreenButton = nil;
    closeButton = nil;
    minimizeButton = nil;
}

void FramelessWindowConverterMacos::minimizeWindow()
{
    [window miniaturize:window];
}

void FramelessWindowConverterMacos::maximizeWindow()
{  
    hideForTranslucency();
    isResizing = true;

    // The NSWindow needs to be resizable, otherwise the window will
    // not be possible to zoom back to non-zoomed state.
    const bool wasResizable = window.styleMask & NSWindowStyleMaskResizable;
    window.styleMask |= NSWindowStyleMaskResizable;

    [window zoom:window];

    if (!wasResizable)
        window.styleMask &= ~NSWindowStyleMaskResizable;

    if(q_ptr->isUsingTrafficLightsOnMacOS())
    {
        [fullScreenButton removeFromSuperview];
        [window.contentView addSubview:fullScreenButton];
        [closeButton removeFromSuperview];
        [window.contentView addSubview:closeButton];
        [minimizeButton removeFromSuperview];
        [window.contentView addSubview:minimizeButton];

        repositionTrafficLights();
    }

    isResizing = false;
    showForTranslucency();
}

void FramelessWindowConverterMacos::restoreWindow()
{
    // Zoom will restore window
    maximizeWindow();
}

void FramelessWindowConverterMacos::closeWindow()
{
    if([[window delegate] respondsToSelector:@selector(windowShouldClose:)])
    {
        if(![[window delegate] windowShouldClose:window]) return;
    }

    [window close];
}

void FramelessWindowConverterMacos::toggleFullscreen()
{
    isResizing = true;
    [window toggleFullScreen:window];
    isResizing = false;
}

void FramelessWindowConverterMacos::showTrafficLights()
{
    if(fullScreenButton && !q_ptr->getHiddenGreenTrafficLightOnMacOS()) [fullScreenButton setHidden:NO];
    if(closeButton && !q_ptr->getHiddenRedTrafficLightOnMacOS()) [closeButton setHidden:NO];
    if(minimizeButton && !q_ptr->getHiddenYellowTrafficLightOnMacOS()) [minimizeButton setHidden:NO];
}

void FramelessWindowConverterMacos::hideForTranslucency()
{
    // Even if user wants buttons to be shown (!q_ptr->getHiddenGreenTrafficLightOnMacOS())
    // hide the buttons for translucency
    if(fullScreenButton) [fullScreenButton setHidden:YES];
    if(closeButton) [closeButton setHidden:YES];
    if(minimizeButton) [minimizeButton setHidden:YES];
}

void FramelessWindowConverterMacos::showForTranslucency()
{
    if(q_ptr->isUsingTrafficLightsOnMacOS())
    {
        showTrafficLights();
    }
}

void FramelessWindowConverterMacos::setHiddenGreenTrafficLightOnMacOS(bool inHidden)
{
    if(inHidden) [fullScreenButton setHidden:YES];
    else [fullScreenButton setHidden:NO];
}

void FramelessWindowConverterMacos::setHiddenRedTrafficLightOnMacOS(bool inHidden)
{
    if(inHidden) [closeButton setHidden:YES];
    else [closeButton setHidden:NO];
}

void FramelessWindowConverterMacos::setHiddenYellowTrafficLightOnMacOS(bool inHidden)
{
    if(inHidden) [minimizeButton setHidden:YES];
    else [minimizeButton setHidden:NO];
}

void FramelessWindowConverterMacos::setEnabledGreenTrafficLightOnMacOS(bool inEnabled)
{
    if(inEnabled) [fullScreenButton setEnabled:YES];
    else [fullScreenButton setEnabled:NO];
}

void FramelessWindowConverterMacos::setEnabledRedTrafficLightOnMacOS(bool inEnabled)
{
    if(inEnabled) [closeButton setEnabled:YES];
    else [closeButton setEnabled:NO];
}

void FramelessWindowConverterMacos::setEnabledYellowTrafficLightOnMacOS(bool inEnabled)
{
    if(inEnabled) [minimizeButton setEnabled:YES];
    else [minimizeButton setEnabled:NO];
}

void FramelessWindowConverterMacos::setHorizontalAlignmentOfTrafficLightsOnMacOS()
{
    repositionTrafficLights();
}

void FramelessWindowConverterMacos::setUpperLeftXPositionOfTrafficLightsOnMacOS()
{
    repositionTrafficLights();
}

void FramelessWindowConverterMacos::setUpperLeftYPositionOfTrafficLightsOnMacOS()
{
    repositionTrafficLights();
}

void FramelessWindowConverterMacos::repositionTrafficLights()
{
    // Default macOS positions
    // int xPosOfGreen = 48, PosOfGreen = 3;
    // int xPosOfRed = 7, yPosOfRed = 3;
    // int xPosOfYellow = 28, yPosOfYellow = 3;
    int xPos = q_ptr->getUpperLeftXPositionOfTrafficLightsOnMacOS();
    int yPos = q_ptr->getUpperLeftYPositionOfTrafficLightsOnMacOS();
    if(q_ptr->getHorizontalAlignmentOfTrafficLightsOnMacOS())
    {
        [fullScreenButton setFrameOrigin:NSMakePoint(xPos + 41, yPos)];
        [closeButton setFrameOrigin:NSMakePoint(xPos + 0, yPos)];
        [minimizeButton setFrameOrigin:NSMakePoint(xPos + 21, yPos)];
    }
    else
    {
        [fullScreenButton setFrameOrigin:NSMakePoint(xPos, yPos + 41)];
        [closeButton setFrameOrigin:NSMakePoint(xPos, yPos + 0)];
        [minimizeButton setFrameOrigin:NSMakePoint(xPos, yPos + 21)];
    }
}

void FramelessWindowConverterMacos::convertToFrameless()
{
    nativeWidgetView = reinterpret_cast<NSView *>(q_ptr->getWindowHandle());
    window = [nativeWidgetView window];

    // Title Bar invisible
    window.titleVisibility = NSWindowTitleHidden;
    window.titlebarAppearsTransparent = YES;
    window.movable = NO; // Custom Move and Resize

    nativeWidgetView.layer.borderWidth = 0;

    [nativeWidgetView.layer setBorderColor:[NSColor clearColor].CGColor];

    // Reset Style Mask
    window.styleMask &= ~NSWindowStyleMaskBorderless;
    window.styleMask |= NSWindowStyleMaskFullSizeContentView;
    //window.styleMask |= NSWindowStyleMaskTitled;
    window.styleMask |= NSWindowStyleMaskClosable;
    window.styleMask |= NSWindowStyleMaskMiniaturizable;
    window.styleMask &= ~NSWindowStyleMaskResizable; // Custom Resize

    [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
    // Enable Layer backing
    // Fixes issues with WA_TranslucentBackground and paintEvent.
    // Repainting the window would not update the translucent pixels correctly.
    // The Part of the window where the system buttons would be located are updating correctly for some reason
    // The rest of the window would be more opaque with every repaint.
    nativeWidgetView.wantsLayer = YES;

    if(q_ptr->isUsingTrafficLightsOnMacOS())
    {
        // Traffic lights
        tlHelper = [[TrafficLightsHelper alloc] initWithFWCMAndWindow:this :window];

        //fullScreenButton = [window standardWindowButton:NSWindowZoomButton];
        fullScreenButton = [NSWindow standardWindowButton:NSWindowZoomButton
                forStyleMask:NSWindowStyleMaskFullSizeContentView];
        // [fullScreenButton.superview willRemoveSubview:fullScreenButton];
        //[fullScreenButton removeFromSuperview];
        //[fullScreenButton viewWillMoveToSuperview:[window contentView]];
        [window.contentView addSubview:fullScreenButton];
        //[fullScreenButton viewDidMoveToSuperview];
        [fullScreenButton setTarget:tlHelper];
        [fullScreenButton setAction:@selector(zoomButtonAction:)];
        [fullScreenButton setEnabled:YES];
        [fullScreenButton setNeedsDisplay:YES];

        //closeButton = [window standardWindowButton:NSWindowCloseButton];
        closeButton = [NSWindow standardWindowButton:NSWindowCloseButton
                forStyleMask:NSWindowStyleMaskFullSizeContentView];
        [closeButton.superview willRemoveSubview:closeButton];
        [closeButton removeFromSuperview];
        [closeButton viewWillMoveToSuperview:[window contentView]];
        [window.contentView addSubview:closeButton];
        [closeButton viewDidMoveToSuperview];
        [closeButton setTarget:tlHelper];
        [closeButton setAction:@selector(closeButtonAction:)];

        //minimizeButton = [window standardWindowButton:NSWindowMiniaturizeButton];
        minimizeButton = [NSWindow standardWindowButton:NSWindowMiniaturizeButton
                forStyleMask:NSWindowStyleMaskFullSizeContentView];
        [minimizeButton.superview willRemoveSubview:minimizeButton];
        [minimizeButton removeFromSuperview];
        [minimizeButton viewWillMoveToSuperview:[window contentView]];
        [window.contentView addSubview:minimizeButton];
        [minimizeButton viewDidMoveToSuperview];
        [minimizeButton setTarget:tlHelper];
        [minimizeButton setAction:@selector(minimizeButtonAction:)];

        repositionTrafficLights();

        // Replace _mouseInGroup method in contentView to enable proper highlighting for traffic lights
        SEL swizzledSelector = @selector(_mouseInGroup:);
        Method swizzledMethod = class_getInstanceMethod([tlHelper class], swizzledSelector);
        class_replaceMethod([nativeWidgetView class],
                swizzledSelector,
                method_getImplementation(swizzledMethod),
                method_getTypeEncoding(swizzledMethod));

        [closeButton setHidden:YES];
        [minimizeButton setHidden:YES];
        [fullScreenButton setHidden:YES];
        showTrafficLights();

        if(!q_ptr->getEnabledGreenTrafficLightOnMacOS()) [fullScreenButton setEnabled:NO];
        if(!q_ptr->getEnabledRedTrafficLightOnMacOS()) [closeButton setEnabled:NO];
        if(!q_ptr->getEnabledYellowTrafficLightOnMacOS()) [minimizeButton setEnabled:NO];
        isMouseInGroup = true;
    }
    else
    {
        fullScreenButton = [window standardWindowButton:NSWindowZoomButton];
        closeButton = [window standardWindowButton:NSWindowCloseButton];
        minimizeButton = [window standardWindowButton:NSWindowMiniaturizeButton];

        [closeButton setHidden:YES];
        [minimizeButton setHidden:YES];
        [fullScreenButton setHidden:YES];
    }

    // Exiting fullscreen mode messes up everything, so fix it here
    exitFullscreenObserver = [[NSNotificationCenter defaultCenter]
            addObserverForName:NSWindowDidExitFullScreenNotification object:window queue:nil usingBlock:^(NSNotification *){
        if(q_ptr->getIsFrameless())
            convertToFrameless();
    }];

    // Exiting fullscreen mode messes up everything, so fix it here
    resizeObserver = [[NSNotificationCenter defaultCenter]
            addObserverForName:NSWindowDidResizeNotification object:window queue:nil usingBlock:^(NSNotification *){
        // Catch resizing events not caught by this class directly
        if(!isResizing)
            repositionTrafficLights();
    }];

    [[NSNotificationCenter defaultCenter]
            addObserverForName:NSWindowDidResignKeyNotification object:window queue:nil usingBlock:^(NSNotification *){
        [fullScreenButton setEnabled:NO];
        [closeButton setEnabled:NO];
        [minimizeButton setEnabled:NO];
    }];

    [[NSNotificationCenter defaultCenter]
            addObserverForName:NSWindowDidBecomeKeyNotification object:window queue:nil usingBlock:^(NSNotification *){
        if(q_ptr->getEnabledGreenTrafficLightOnMacOS()) [fullScreenButton setEnabled:YES];
        if(q_ptr->getEnabledRedTrafficLightOnMacOS()) [closeButton setEnabled:YES];
        if(q_ptr->getEnabledYellowTrafficLightOnMacOS()) [minimizeButton setEnabled:YES];
    }];


    // Control Cursor shape ourselves
    [window disableCursorRects];
    if(q_ptr->getHasShadow())
        [window setHasShadow:YES];
    else [window setHasShadow:NO];
    [window setOpaque:NO];
    [window setBackgroundColor:[NSColor clearColor]];

    // Do a fake resize to update drop shadow of window
    [NSTimer scheduledTimerWithTimeInterval:0.5
        target:tlHelper
        selector:@selector(fakeResizeAction:)
      userInfo:nil
      repeats:NO];
}

void FramelessWindowConverterMacos::convertToWindowWithFrame()
{
    nativeWidgetView = reinterpret_cast<NSView *>(q_ptr->getWindowHandle());
    window = [nativeWidgetView window];

    // Title Bar invisible
    window.titleVisibility = NSWindowTitleVisible;
    window.titlebarAppearsTransparent = NO;
    window.movable = YES;

    // Reset Style Mask
    window.styleMask &= ~NSWindowStyleMaskBorderless;
    window.styleMask |= NSWindowStyleMaskTitled;
    window.styleMask |= NSWindowStyleMaskClosable;
    window.styleMask |= NSWindowStyleMaskMiniaturizable;
    window.styleMask |= NSWindowStyleMaskResizable;
    window.styleMask &= ~NSWindowStyleMaskFullSizeContentView;

    [window setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
    // Enable Layer backing
    // Fixes issues with WA_TranslucentBackground and paintEvent.
    // Repainting the window would not update the translucent pixels correctly.
    // The Part of the window where the system buttons would be located are updating correctly for some reason
    // The rest of the window would be more opaque with every repaint.
    nativeWidgetView.wantsLayer = YES;

    // Traffic lights
    if(!q_ptr->getHiddenGreenTrafficLightOnMacOS()) [[window standardWindowButton:NSWindowCloseButton] setHidden:NO];
    if(!q_ptr->getHiddenGreenTrafficLightOnMacOS()) [[window standardWindowButton:NSWindowMiniaturizeButton] setHidden:NO];
    if(!q_ptr->getHiddenGreenTrafficLightOnMacOS()) [[window standardWindowButton:NSWindowZoomButton] setHidden:NO];

    [[NSNotificationCenter defaultCenter] removeObserver:exitFullscreenObserver];
    [[NSNotificationCenter defaultCenter] removeObserver:resizeObserver];

    // Control Cursor shape ourselves
    [window enableCursorRects];
    if(q_ptr->getHasShadow())
        [window setHasShadow:YES];
    else [window setHasShadow:NO];
    [window setOpaque:NO];
    [window setBackgroundColor:[NSColor clearColor]];
}

void FramelessWindowConverterMacos::showCursorByHitResult(FWCBorderHitTestResult inBorderHitResult)
{
    switch(inBorderHitResult)
    {
    case FWCBorderHitTestResult::LEFT:
        // Fallthrough
    case FWCBorderHitTestResult::RIGHT:
    {
        SEL selector = NSSelectorFromString(@"_windowResizeEastWestCursor");
        NSCursor* newCursor = [NSCursor performSelector:selector];
        [newCursor set];
        break;
    }
    case FWCBorderHitTestResult::TOP:
        // Fallthrough
    case FWCBorderHitTestResult::BOTTOM:
    {
        SEL selector = NSSelectorFromString(@"_windowResizeNorthSouthCursor");
        NSCursor* newCursor = [NSCursor performSelector:selector];
        [newCursor set];
        break;
    }
    case FWCBorderHitTestResult::TOP_RIGHT:
        // Fallthrough
    case FWCBorderHitTestResult::BOTTOM_LEFT:
    {
        SEL selector = NSSelectorFromString(@"_windowResizeNorthEastSouthWestCursor");
        NSCursor* newCursor = [NSCursor performSelector:selector];
        [newCursor set];
        break;
    }
    case FWCBorderHitTestResult::BOTTOM_RIGHT:
        // Fallthrough
    case FWCBorderHitTestResult::TOP_LEFT:
    {
        SEL selector = NSSelectorFromString(@"_windowResizeNorthWestSouthEastCursor");
        NSCursor* newCursor = [NSCursor performSelector:selector];
        [newCursor set];
        break;
    }
    case FWCBorderHitTestResult::CLIENT:
        if(isResizing == false)
        {
            [[NSCursor arrowCursor] set];
        }
        break;
    default:
        // do nothing
        break;
    }
}

void FramelessWindowConverterMacos::showAppropriateCursor()
{
    // Convert NSPoint to QPoint (different origin of window)
    NSPoint localPos = [[window contentView] convertPoint: [window mouseLocationOutsideOfEventStream] fromView: nil];
    FWCPoint mousePos(static_cast<int>(localPos.x), static_cast<int>(localPos.y));
    FWCRect rect(0, 0, static_cast<int>(window.frame.size.width), static_cast<int>(window.frame.size.height));

    showCursorByHitResult(doBorderHitTest(rect, mousePos, q_ptr->getBorderWidth()));
}

void FramelessWindowConverterMacos::resizeWindow(FWCFloatingPoint mouseLocationInWindow)
{
    NSRect oldFrame = [window frame];
    NSRect newFrame = oldFrame;

    // New Structure for the Hit Result
    bool bHitTop = false;
    bool bHitBottom = false;
    bool bHitRight = false;
    bool bHitLeft = false;

    switch(currentBHTR)
    {
    case FWCBorderHitTestResult::RIGHT:
        bHitRight = true;
        break;
    case FWCBorderHitTestResult::LEFT:
        bHitLeft = true;
        break;
    case FWCBorderHitTestResult::TOP:
        bHitTop = true;
        break;
    case FWCBorderHitTestResult::BOTTOM:
        bHitBottom = true;
        break;
    case FWCBorderHitTestResult::TOP_LEFT:
        bHitTop = true;
        bHitLeft = true;
        break;
    case FWCBorderHitTestResult::TOP_RIGHT:
        bHitTop = true;
        bHitRight = true;
        break;
    case FWCBorderHitTestResult::BOTTOM_LEFT:
        bHitBottom = true;
        bHitLeft = true;
        break;
    case FWCBorderHitTestResult::BOTTOM_RIGHT:
        bHitBottom = true;
        bHitRight = true;
        break;
    default:
        break;
    }

    if(bHitTop)
    {
        newFrame.size.height = mouseLocationInWindow.y + startDiffCursorFrameLocs.y;
    }
    else if(bHitBottom)
    {
        newFrame.origin.y += mouseLocationInWindow.y - startDiffCursorFrameLocs.y;
        newFrame.size.height = newFrame.size.height - mouseLocationInWindow.y + startDiffCursorFrameLocs.y;
    }

    if(bHitRight)
    {
        newFrame.size.width = mouseLocationInWindow.x + startDiffCursorFrameLocs.x;
    }
    else if(bHitLeft)
    {
        newFrame.origin.x += mouseLocationInWindow.x - startDiffCursorFrameLocs.x;
        newFrame.size.width = newFrame.size.width - mouseLocationInWindow.x + startDiffCursorFrameLocs.x;
    }

    // Limit the resizing of the window
    int limit = 0;
    if(((limit = q_ptr->getMinimumWindowWidth()) > newFrame.size.width) ||
            (limit = q_ptr->getMaximumWindowWidth()) < newFrame.size.width)
    {
        if(bHitLeft)
            newFrame.origin.x += (newFrame.size.width - limit);
        newFrame.size.width = limit;
    }
    if(((limit = q_ptr->getMinimumWindowHeight()) > newFrame.size.height) ||
            (limit = q_ptr->getMaximumWindowHeight()) < newFrame.size.height)
    {
        if(bHitBottom)
            newFrame.origin.y += (newFrame.size.height - limit);
        newFrame.size.height = limit;
    }

    [window setFrame:newFrame display:YES];
}

bool FramelessWindowConverterMacos::filterNativeEvent(void *message, long *result)
{
    result = nullptr; // not used on mac

    NSEvent *event = static_cast<NSEvent *>(message);

    if ([event type] == NSEventTypeLeftMouseDown)
    {
        // Convert NSPoint to QPoint (different origin of window)
        NSPoint localPos = [[window contentView] convertPoint: [event locationInWindow] fromView: nil];
        FWCPoint mousePos(static_cast<int>(localPos.x), static_cast<int>(localPos.y));

        // Only this widget is used for dragging.
        bool shouldDrag = q_ptr->getShouldPerformWindowDrag()(mousePos.x, mousePos.y);

        if(shouldDrag && q_ptr->isUsingTrafficLightsOnMacOS())
        {
            // Determine if Mouse Cursor is inside of traffic light area
            bool isInside = NSPointInRect(localPos, NSMakeRect(closeButton.frame.origin.x, closeButton.frame.origin.y,
                                                               (fullScreenButton.frame.origin.x + fullScreenButton.frame.size.width) - closeButton.frame.origin.x,
                                                               (fullScreenButton.frame.origin.y + fullScreenButton.frame.size.height) - closeButton.frame.origin.y));
            if(isInside)
                shouldDrag = false;
        }

        // Double Click
        if([event clickCount] == 2)
        {
            if(shouldDrag)
                maximizeWindow();
            return false;
        }

        FWCRect rect(0, 0, static_cast<int>(window.frame.size.width), static_cast<int>(window.frame.size.height));
        int frameWidth = rect.right() - rect.left();
        int frameHeight = rect.bottom() - rect.top();

        if((currentBHTR = doBorderHitTest(rect, mousePos, q_ptr->getBorderWidth())) != FWCBorderHitTestResult::CLIENT) // Resize
        {
            // Save the difference between the frame border and the current mouse location
            if(currentBHTR == FWCBorderHitTestResult::LEFT || currentBHTR == FWCBorderHitTestResult::BOTTOM_LEFT ||
                    currentBHTR == FWCBorderHitTestResult::BOTTOM)
            {
                startDiffCursorFrameLocs.x = mousePos.x;
                startDiffCursorFrameLocs.y = frameHeight - mousePos.y;
            }
            else if(currentBHTR == FWCBorderHitTestResult::TOP_LEFT)
            {
                startDiffCursorFrameLocs.x = mousePos.x;
                startDiffCursorFrameLocs.y = mousePos.y;
            }
            else if(currentBHTR == FWCBorderHitTestResult::RIGHT || currentBHTR == FWCBorderHitTestResult::BOTTOM_RIGHT)
            {
                startDiffCursorFrameLocs.x = frameWidth - mousePos.x;
                startDiffCursorFrameLocs.y = frameHeight - mousePos.y;
            }
            else //if(currentBHTR == FWCBorderHitTestResult::TOP || currentBHTR == FWCBorderHitTestResult::TOP_RIGHT)
            {
                startDiffCursorFrameLocs.x = frameWidth - mousePos.x;
                startDiffCursorFrameLocs.y = mousePos.y;
            }

            isResizing = true;
            // Every resize traffic lights get reset to "preferred" position
            // just hide them while resizing
            [closeButton setHidden:YES];
            [minimizeButton setHidden:YES];
            [fullScreenButton setHidden:YES];
            return false;
        }
        else if(shouldDrag) // Move
        {
            startDiffCursorFrameLocs.x = mousePos.x;
            startDiffCursorFrameLocs.y = frameHeight - mousePos.y;
            isMoving = true;
            return false;
        }
    }

    if([event type] == NSEventTypeLeftMouseUp)
    {
        if(isResizing)
        {
            isResizing = !isResizing;
            showCursorByHitResult(FWCBorderHitTestResult::CLIENT);

            if(q_ptr->isUsingTrafficLightsOnMacOS())
            {
                // Every resize macos sets the position of the original buttons to the "preferred" state,
                // so set the position manually here
                repositionTrafficLights();

                showTrafficLights();
            }
            return false;
        }
        else if(isMoving)
        {
            isMoving = !isMoving;
        }
    }
    if ([event type] == NSEventTypeLeftMouseDragged)
    {
        if(isResizing == true)
        {
            // The cursor can go outside of the application and thus changes shape.
            // As soon as the cursor is inside of the application again, the appropriate cursor is set.
            //showAppropriateCursor();
            showCursorByHitResult(currentBHTR);

            NSPoint mouseLocInWindow = [event locationInWindow];
            resizeWindow(FWCFloatingPoint(mouseLocInWindow.x, mouseLocInWindow.y));
        }
        else if(isMoving)
        {
            NSRect newFrame = [window frame];
            newFrame.origin.x = [NSEvent mouseLocation].x - startDiffCursorFrameLocs.x;
            newFrame.origin.y = [NSEvent mouseLocation].y - startDiffCursorFrameLocs.y;

            [window setFrame:newFrame display:YES];
        }
        return false;
    }
    if([event type] == NSEventTypeFlagsChanged)
    {
        if(q_ptr->isUsingTrafficLightsOnMacOS())
        {
            if(event.keyCode == 58) // option key pressed or released
                [fullScreenButton setNeedsDisplay:YES]; // Properly show fullscreen or zoom button
        }
    }
    if([event type] == NSEventTypeMouseMoved)
    {
        showAppropriateCursor();

        if(q_ptr->isUsingTrafficLightsOnMacOS())
        {
            // Determine if Mouse Cursor is inside of traffic light area
            NSPoint localPos = [[window contentView] convertPoint: [window mouseLocationOutsideOfEventStream] fromView: nil];
            bool isInside = NSPointInRect(localPos, NSMakeRect(closeButton.frame.origin.x, closeButton.frame.origin.y,
                                                               (fullScreenButton.frame.origin.x + fullScreenButton.frame.size.width) - closeButton.frame.origin.x,
                                                               (fullScreenButton.frame.origin.y + fullScreenButton.frame.size.height) - closeButton.frame.origin.y));
            if(isInside != isMouseInGroup)
            {
                isMouseInGroup = isInside;
                [fullScreenButton setNeedsDisplay:YES];
                [closeButton setNeedsDisplay:YES];
                [minimizeButton setNeedsDisplay:YES];
            }
        }

        return false;
    }

    return false;
}

#endif

