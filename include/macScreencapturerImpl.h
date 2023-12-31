#pragma once
#include "defines.h"

#include <string>
#include <vector>
#include <ApplicationServices/ApplicationServices.h>

#include "screencapturer.h"
#include "frame.h"

_START_SCREEN2WEB_NM_

class MacScreenCapturer : public ScreenCapturer
{
public:
    MacScreenCapturer() = default;
    ~MacScreenCapturer() = default;

public:
    ::std::vector<::std::string> GetAllWindowNames() noexcept override;

    virtual bool Open(const ::std::string &windowName) noexcept override;

    virtual Frame CaptureOne() noexcept override;

    virtual void Release() noexcept override;

private:
    CFDictionaryRef windowInfo_;
    CGRect windowRect_;
};

_END_SCREEN2WEB_NM_