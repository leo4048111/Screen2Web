#include "winSDIScreencapturerImpl.h"

_START_SCREEN2WEB_NM_

namespace
{
}

WinSDIScreenCapturer::~WinSDIScreenCapturer()
{
    this->Release();
}

::std::vector<::std::string> WinSDIScreenCapturer::GetAllWindowNames() noexcept
{
    ::std::vector<std::string> windowNames;

    HWND hwnd = ::GetTopWindow(NULL);
    while (hwnd)
    {
        constexpr int MAX_CLASS_NAME_LENGTH = 256;
        constexpr int MAX_WINDOW_NAME_LENGTH = 256;

        char className[MAX_CLASS_NAME_LENGTH];
        ::GetClassName(hwnd, className, MAX_CLASS_NAME_LENGTH);

        if (::strcmp(className, "ConsoleWindowClass") == 0)
        {
            hwnd = ::GetNextWindow(hwnd, GW_HWNDNEXT);
            continue;
        }

        char windowName[MAX_WINDOW_NAME_LENGTH];
        ::GetWindowText(hwnd, windowName, MAX_WINDOW_NAME_LENGTH);

        if (::IsWindowVisible(hwnd) && windowName[0] != '\0')
        {
            windowNames.emplace_back(windowName);
        }

        hwnd = ::GetNextWindow(hwnd, GW_HWNDNEXT);
    }

    return windowNames;
}


bool WinSDIScreenCapturer::Open(const ::std::string &windowName) noexcept
{
    hwnd_ = FindWindow(nullptr, windowName.c_str());

    return hwnd_ != nullptr;
}

Frame WinSDIScreenCapturer::CaptureOne() noexcept
{
    if(hwnd_ == nullptr) return Frame(0, 0, PixelFormat::UNKNOWN);

    RECT rect;
    ::GetWindowRect(hwnd_, &rect);
    width_ = rect.right - rect.left;
    height_ = rect.bottom - rect.top;

    HDC hdcScreen = GetDC(NULL);
    HDC hdc = CreateCompatibleDC(hdcScreen);
    HBITMAP hbmp = CreateCompatibleBitmap(hdcScreen, width_, height_);
    SelectObject(hdc, hbmp);
    PrintWindow(hwnd_, hdc,  PW_CLIENTONLY | PW_RENDERFULLCONTENT);

    Frame frame(width_, height_, PixelFormat::RGBA);
    BITMAPINFO bmi;
    memset(&bmi, 0, sizeof(BITMAPINFO));
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width_;
    bmi.bmiHeader.biHeight = -height_;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32; // Assuming 32-bit RGBA format
    bmi.bmiHeader.biCompression = BI_RGB;

    // Get the window pixel data
    GetDIBits(hdc, hbmp, 0, height_, frame.data, &bmi, DIB_RGB_COLORS);

    // Clean up resources
    DeleteObject(hbmp);
    DeleteDC(hdc);
    ReleaseDC(NULL, hdcScreen);
    return frame;
}

void WinSDIScreenCapturer::Release() noexcept
{
	hwnd_ = nullptr;
}

_END_SCREEN2WEB_NM_