#pragma once
#include "defines.h"

#include <string>

#include <opencv2/opencv.hpp>

_START_SCREEN2WEB_NM_

class ScreenCapturer
{
public:
    ScreenCapturer() = default;
    ~ScreenCapturer() = default;

public:
    virtual bool Open(const ::std::string &windowName) noexcept = 0;

    virtual ::cv::Mat CaptureOne() noexcept = 0;

    virtual void Release() noexcept = 0;
};

_END_SCREEN2WEB_NM_