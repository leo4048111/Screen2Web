#include "winScreencapturerImpl.h"

_START_SCREEN2WEB_NM_

bool WinScreenCapturer::Open(const ::std::string &windowName) noexcept
{
    hwnd_ = FindWindow(nullptr, windowName.c_str());

    return true;
}

::cv::Mat WinScreenCapturer::CaptureOne() noexcept
{
    return ::cv::Mat();
}

void WinScreenCapturer::Release() noexcept
{
}

_END_SCREEN2WEB_NM_