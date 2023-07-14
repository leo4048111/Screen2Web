#include "macScreencapturerImpl.h"

_START_SCREEN2WEB_NM_

bool MacScreenCapturer::Open(const ::std::string &windowName) noexcept
{
    CFStringRef titleRef = CFStringCreateWithCString(kCFAllocatorDefault, windowName.c_str(), kCFStringEncodingUTF8);
    CFArrayRef windows = CGWindowListCopyWindowInfo(kCGWindowListOptionOnScreenOnly, kCGNullWindowID);
    CGRect windowRect = CGRectNull;

    for (CFIndex i = 0; i < CFArrayGetCount(windows); i++)
    {
        windowInfo_ = (CFDictionaryRef)CFArrayGetValueAtIndex(windows, i);
        CFStringRef name = (CFStringRef)CFDictionaryGetValue(windowInfo_, kCGWindowName);

        if (name && CFStringCompare(name, titleRef, kCFCompareCaseInsensitive) == kCFCompareEqualTo)
        {
            CFStringRef boundsString = (CFStringRef)CFDictionaryGetValue(windowInfo_, kCGWindowBounds);
            CFDataRef boundsData = CFStringCreateExternalRepresentation(kCFAllocatorDefault, boundsString, kCFStringEncodingUTF8, '?');
            CGRectMakeWithDictionaryRepresentation((CFDictionaryRef)CFPropertyListCreateWithData(kCFAllocatorDefault, boundsData, kCFPropertyListImmutable, NULL, NULL), &windowRect);
            CFRelease(boundsData);
            break;
        }
    }

    CFRelease(windows);
    CFRelease(titleRef);
    return true;
}

::cv::Mat MacScreenCapturer::CaptureOne() noexcept
{
    return ::cv::Mat();
}

void MacScreenCapturer::Release() noexcept
{
}

_END_SCREEN2WEB_NM_