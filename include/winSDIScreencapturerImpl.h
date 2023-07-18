#pragma once
#include "defines.h"

#include <Windows.h>

#include "screencapturer.h"
#include "frame.h"

_START_SCREEN2WEB_NM_

class WinSDIScreenCapturer : public ScreenCapturer
{
public:
    WinSDIScreenCapturer() = default;
    ~WinSDIScreenCapturer();

public:
    virtual ::std::vector<::std::string> GetAllWindowNames() noexcept override;

    virtual bool Open(const ::std::string &windowName) noexcept override;

    virtual Frame CaptureOne() noexcept override;

    virtual void Release() noexcept override;

private:
    HWND hwnd_{nullptr};
};

_END_SCREEN2WEB_NM_