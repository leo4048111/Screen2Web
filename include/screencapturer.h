#pragma once
#include "defines.h"

#include <vector>
#include <string>

#include "frame.h"

_START_SCREEN2WEB_NM_

class ScreenCapturer
{
public:
    ScreenCapturer() = default;
    ~ScreenCapturer() = default;

public:
    virtual ::std::vector<::std::string> GetAllWindowNames() noexcept = 0;

    virtual bool Open(const ::std::string &windowName) noexcept = 0;

    virtual Frame CaptureOne() noexcept = 0;

    virtual void Release() noexcept = 0;
};

_END_SCREEN2WEB_NM_