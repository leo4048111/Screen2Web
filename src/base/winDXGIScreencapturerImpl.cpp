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

	HWND hWnd = FindWindow(NULL, windowName.c_str());
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

	IDXGIOutput* dxgiOutput;
	hr = dxgiAdapter->EnumOutputs(0, &dxgiOutput);
	if (FAILED(hr))
	{
		return false;
	}

	IDXGIOutput1* dxgiOutput1 = nullptr;
	hr = dxgiOutput->QueryInterface(
		__uuidof(IDXGIOutput1),
		reinterpret_cast<void**>(&dxgiOutput1));
	dxgiOutput->Release();
	if (FAILED(hr))
	{
		return false;
	}

	hr = dxgiOutput1->DuplicateOutput(device_, &duplication_);
	dxgiOutput1->Release();

	if (FAILED(hr))
	{
		return false;
	}

	return true;	
}

Frame WinDXGIScreenCapturer::CaptureOne() noexcept
{
	Frame frame(width_, height_, PixelFormat::RGBA);

	DXGI_OUTDUPL_FRAME_INFO frameInfo;
	IDXGIResource* resource = nullptr;
	ID3D11Texture2D* acquireFrame = nullptr;
	ID3D11Texture2D* texture = nullptr;

	HRESULT hr = duplication_->AcquireNextFrame(0, &frameInfo, &resource);
	if (FAILED(hr))
	{
		if (hr == DXGI_ERROR_WAIT_TIMEOUT)
		{
			return frame;
		}
		else
		{
			return frame;
		}
	}

	hr = resource->QueryInterface(__uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&acquireFrame));
	resource->Release();
	if (FAILED(hr))
	{
		return frame;
	}

	D3D11_TEXTURE2D_DESC desc;
	acquireFrame->GetDesc(&desc);
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	device_->CreateTexture2D(&desc, NULL, &texture);
	if (&texture && texture)
	{
		deviceContext_->CopyResource(texture, acquireFrame);
	}
	acquireFrame->Release();

	hr = duplication_->ReleaseFrame();
	if (FAILED(hr))
	{
		return frame;
	}

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	deviceContext_->Map(texture, 0, D3D11_MAP_READ, 0, &mappedResource);
	frame = Frame(desc.Width, desc.Height, PixelFormat::RGBA);
	for (size_t i = 0; i < desc.Height; i++)
		memcpy_s(frame.data + i * desc.Width * 4, desc.Width * 4, (uint8_t*)mappedResource.pData + i * mappedResource.RowPitch, desc.Width * 4);

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