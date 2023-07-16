#include "macScreencapturerImpl.h"

_START_SCREEN2WEB_NM_

bool MacScreenCapturer::Open(const ::std::string &windowName) noexcept
{
    auto windowList = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly | kCGWindowListExcludeDesktopElements, kCGNullWindowID);

    CFIndex numWindows = CFArrayGetCount(windowList);
    bool found = false;

    for (int i = 0; i < (int)numWindows; i++)
    {
        windowInfo_ = (CFDictionaryRef)CFArrayGetValueAtIndex(windowList, i);
        CFStringRef windowOwnerNameRef = (CFStringRef)CFDictionaryGetValue(windowInfo_, kCGWindowOwnerName);

        if (windowOwnerNameRef == nullptr)
            continue;

        CFIndex bufferSize = CFStringGetLength(windowOwnerNameRef) + 1; // The +1 is for having space for the string to be NUL terminated
        char buffer[bufferSize];
        ::std::string windowOwnerNameStr;
        if (CFStringGetCString(windowOwnerNameRef, buffer, bufferSize, kCFStringEncodingUTF8))
            windowOwnerNameStr = ::std::string(buffer);
        if (windowOwnerNameStr == windowName)
        {
            CFDictionaryRef boundsString = (CFDictionaryRef)CFDictionaryGetValue(windowInfo_, kCGWindowBounds);
            CGRectMakeWithDictionaryRepresentation(boundsString, &windowRect_);
            found = true;
            break;
        }
    }

    return found;
}

Frame MacScreenCapturer::CaptureOne() noexcept
{
    assert(!CGRectIsNull(windowRect_));

    // create a bitmap with the window size
    CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
    CGContextRef ctx = CGBitmapContextCreate(NULL, windowRect_.size.width, windowRect_.size.height, 8, 4 * windowRect_.size.width, colorSpace, kCGImageAlphaPremultipliedFirst | kCGBitmapByteOrder32Little);
    CGColorSpaceRelease(colorSpace);

    // capture the window and draw it into the bitmap ctx
    auto typeRef = (CFTypeRef)CFDictionaryGetValue(windowInfo_, kCGWindowNumber);
    CGWindowID wndId = 0;

    if (CFNumberRef numberRef = CFNumberCreate(NULL, kCFNumberSInt32Type, &wndId))
    {
        if (CFGetTypeID(typeRef) == CFNumberGetTypeID())
        {
            CFNumberGetValue(static_cast<CFNumberRef>(typeRef), kCFNumberSInt32Type, &wndId);
        }
        CFRelease(numberRef);
    }

    CGImageRef imageRef = CGWindowListCreateImage(CGRectNull, kCGWindowListOptionIncludingWindow, wndId, kCGWindowImageBoundsIgnoreFraming);
    CGContextDrawImage(ctx, windowRect_, imageRef);
    CGImageRelease(imageRef);

    // bitmap ctx to ::cv::Mat
    ::cv::Mat mat(windowRect_.size.height, windowRect_.size.width, CV_8UC4, CGBitmapContextGetData(ctx), 4 * windowRect_.size.width);
    CGContextRelease(ctx);

    return mat;
}

void MacScreenCapturer::Release() noexcept
{
}

_END_SCREEN2WEB_NM_