#include "winDXGIScreencapturerImpl.h"

_START_SCREEN2WEB_NM_

WinDXGIScreenCapturer::~WinDXGIScreenCapturer()
{
    this->Release();
}

bool WinDXGIScreenCapturer::Open(const ::std::string &windowName) noexcept
{
    if (windowName.empty())
    {
		return false;
	}

	HWND hWnd = ::FindWindowA(NULL, windowName.c_str());
    if (!hWnd)
    {
		return false;
	}

	RECT rect;
	::GetWindowRect(hWnd, &rect);
	width_ = rect.right - rect.left;
	height_ = rect.bottom - rect.top;

	D3D_FEATURE_LEVEL featureLevel;
	HRESULT hr = ::D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, NULL, 0, D3D11_SDK_VERSION,
        		&device_, &featureLevel, &deviceContext_);
    if (FAILED(hr))
    {
		return false;
	}

	IDXGIDevice* dxgiDevice = nullptr;
	hr = device_->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(&dxgiDevice));
    if (FAILED(hr))
    {
		return false;
	}

	IDXGIAdapter* dxgiAdapter = nullptr;
	hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter), reinterpret_cast<void**>(&dxgiAdapter));
    if (FAILED(hr))
    {
		return false;
	}

	IDXGIFactory* dxgiFactory = nullptr;
	hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory), reinterpret_cast<void**>(&dxgiFactory));
	if (FAILED(hr))
	{
		return false;
	}

	hr = dxgiFactory->CreateSwapChain(device_, &swapChainDesc_, &swapChain_);
	if (FAILED(hr))
	{
		return false;
	}

	hr = dxgiDevice->QueryInterface(__uuidof(IDXGIOutputDuplication), reinterpret_cast<void**>(&duplication_));
	if (FAILED(hr))
	{
		return false;
	}

	return true;	
}

Frame WinDXGIScreenCapturer::CaptureOne() noexcept
{
	Frame frame(width_, height_, PixelFormat::RGBA);
	if (!duplication_)
	{
		return frame;
	}

	DXGI_OUTDUPL_FRAME_INFO frameInfo;
	IDXGIResource* resource = nullptr;
	HRESULT hr = duplication_->AcquireNextFrame(1000, &frameInfo, &resource);
	if (FAILED(hr))
	{
		return frame;
	}

	IDXGISurface* surface = nullptr;
	hr = resource->QueryInterface(__uuidof(IDXGISurface), reinterpret_cast<void**>(&surface));
	if (FAILED(hr))
	{
		return frame;
	}

	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width_;
	desc.Height = height_;
	desc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	desc.ArraySize = 1;
	desc.BindFlags = 0;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_STAGING;

	ID3D11Texture2D* texture = nullptr;
	hr = device_->CreateTexture2D(&desc, nullptr, &texture);
	if (FAILED(hr))
	{
		return frame;
	}

	deviceContext_->CopyResource(texture, (ID3D11Resource*)surface);

	D3D11_MAPPED_SUBRESOURCE mapped;
	hr = deviceContext_->Map(texture, 0, D3D11_MAP_READ, 0, &mapped);
	if (FAILED(hr))
	{
		return frame;
	}

	memcpy_s(frame.data, width_ * height_ * 4, mapped.pData, width_ * height_ * 4);

	deviceContext_->Unmap(texture, 0);

	duplication_->ReleaseFrame();
	resource->Release();
	surface->Release();
	texture->Release();

	return frame;
}

void WinDXGIScreenCapturer::Release() noexcept
{
    if (duplication_)
    {
        duplication_->Release();
    }
    if (device_)
    {
        device_->Release();
    }
    if (deviceContext_)
    {
        deviceContext_->Release();
    }
}

_END_SCREEN2WEB_NM_