#pragma once
#include "defines.h"

#include "screencapturer.h"

_START_SCREEN2WEB_NM_

class MacScreenCapturer : public ScreenCapturer
{
public:
    MacScreenCapturer() = default;
    ~MacScreenCapturer() = default;

public:
    virtual bool Open(const ::std::string &windowName) noexcept override;

    virtual ::cv::Mat CaptureOne() noexcept override;

    virtual void Release() noexcept override;
};

_END_SCREEN2WEB_NM_