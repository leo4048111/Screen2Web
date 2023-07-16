#pragma once
#include "defines.h"

#include <Windows.h>

#include "screencapturer.h"
#include "frame.h"

_START_SCREEN2WEB_NM_

class WinScreenCapturer : public ScreenCapturer
{
public:
    WinScreenCapturer() = default;
    ~WinScreenCapturer() = default;

public:
    virtual bool Open(const ::std::string &windowName) noexcept override;

    virtual Frame CaptureOne() noexcept override;

    virtual void Release() noexcept override;

private:
    HWND hwnd_{nullptr};
};

_END_SCREEN2WEB_NM_