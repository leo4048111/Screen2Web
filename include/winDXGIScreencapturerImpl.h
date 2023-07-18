#pragma once
#include "defines.h"

#include <d3d11.h>
#include <dxgi1_2.h>

#include "screencapturer.h"
#include "frame.h"

_START_SCREEN2WEB_NM_

class WinDXGIScreenCapturer : public ScreenCapturer
{
public:
    WinDXGIScreenCapturer() = default;
    ~WinDXGIScreenCapturer();

public:
    virtual bool Open(const ::std::string &windowName) noexcept override;

    virtual Frame CaptureOne() noexcept override;

    virtual void Release() noexcept override;

private:
    HWND hWnd_{ nullptr };
    ID3D11Device* device_{ nullptr };
    ID3D11DeviceContext* deviceContext_{ nullptr };
    IDXGIOutputDuplication* duplication_{ nullptr };

    size_t width_{ 0 }, height_{ 0 };
    DXGI_SWAP_CHAIN_DESC swapChainDesc_;
    IDXGISwapChain* swapChain_{ nullptr };
};

_END_SCREEN2WEB_NM_