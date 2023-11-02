#include "FramelessWindowConverter_p.h"

#ifdef WIN32
#include "FramelessWindowConverterWindows.h"
#elif __linux__
#include "FramelessWindowConverterLinux.h"
#elif __APPLE__
#include "FramelessWindowConverterMacos.h"
#endif

using namespace FWC;

FramelessWindowConverterPrivate* FramelessWindowConverterPrivate::create(FramelessWindowConverter* q)
{
#ifdef WIN32
    return new FramelessWindowConverterWindows(q);
#elif __linux__
    return new FramelessWindowConverterLinux(q);
#elif __APPLE__
    return new FramelessWindowConverterMacos(q);
#endif
}

FWCBorderHitTestResult FramelessWindowConverterPrivate::doBorderHitTest(FWCRect inWindowRect, FWCPoint inMousePosition, int borderWidth)
{
    FWCBorderHitTestResult bhtResult = FWCBorderHitTestResult::NONE; // outside window

    if(inMousePosition.x > inWindowRect.left() + borderWidth && inMousePosition.x < inWindowRect.right() - borderWidth &&
            inMousePosition.y < inWindowRect.bottom() - borderWidth && inMousePosition.y > inWindowRect.top() + borderWidth)
    {
        return FWCBorderHitTestResult::CLIENT;
    }
    //left border
    if (inMousePosition.x >= inWindowRect.left() && inMousePosition.x <= inWindowRect.left() + borderWidth)
    {
        bhtResult = FWCBorderHitTestResult::LEFT;
    }
    //right border
    else if (inMousePosition.x <= inWindowRect.right() && inMousePosition.x >= inWindowRect.right() - borderWidth)
    {
        bhtResult = FWCBorderHitTestResult::RIGHT;
    }
    //bottom border
    else if (inMousePosition.y <= inWindowRect.bottom() && inMousePosition.y >= inWindowRect.bottom() - borderWidth)
    {
        bhtResult = FWCBorderHitTestResult::BOTTOM;
    }
    //top border
    else if (inMousePosition.y >= inWindowRect.top() && inMousePosition.y <= inWindowRect.top() + borderWidth)
    {
        bhtResult = FWCBorderHitTestResult::TOP;
    }

    //bottom left corner
    if (bhtResult == FWCBorderHitTestResult::LEFT &&
            inMousePosition.y <= inWindowRect.bottom() && inMousePosition.y >= inWindowRect.bottom() - borderWidth)
    {
        bhtResult = FWCBorderHitTestResult::BOTTOM_LEFT;
    }
    //bottom right corner
    else if (bhtResult == FWCBorderHitTestResult::RIGHT &&
             inMousePosition.y <= inWindowRect.bottom() && inMousePosition.y >= inWindowRect.bottom() - borderWidth)
    {
        bhtResult = FWCBorderHitTestResult::BOTTOM_RIGHT;
    }
    //top left corner
    else if (bhtResult == FWCBorderHitTestResult::LEFT &&
             inMousePosition.y >= inWindowRect.top() && inMousePosition.y <= inWindowRect.top() + borderWidth)
    {
        bhtResult = FWCBorderHitTestResult::TOP_LEFT;
    }
    //top right corner
    else if (bhtResult == FWCBorderHitTestResult::RIGHT &&
             inMousePosition.y >= inWindowRect.top() && inMousePosition.y <= inWindowRect.top() + borderWidth)
    {
        bhtResult = FWCBorderHitTestResult::TOP_RIGHT;
    }

    return bhtResult;
}
