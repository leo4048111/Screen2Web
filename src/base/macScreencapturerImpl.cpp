#include "macScreencapturerImpl.h"

_START_SCREEN2WEB_NM_

bool MacScreenCapturer::Open(const ::std::string &windowName) noexcept
{
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