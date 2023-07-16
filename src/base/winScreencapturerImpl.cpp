#include "winScreencapturerImpl.h"

_START_SCREEN2WEB_NM_

namespace
{
    BITMAPINFOHEADER CreateBitmapInfoHeader(HBITMAP hBmp) noexcept
    {
        BITMAP bmp;
        BITMAPINFOHEADER bi;

        GetObject(hBmp, sizeof(BITMAP), &bmp);

        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = bmp.bmWidth;
        // negative height to ensure that the origin is at the top left corner of the returned bitmap
        bi.biHeight = -bmp.bmHeight;
        bi.biPlanes = 1;
        bi.biBitCount = bmp.bmBitsPixel;
        bi.biCompression = BI_RGB;
        bi.biSizeImage = 0;
        bi.biXPelsPerMeter = 0;
        bi.biYPelsPerMeter = 0;
        bi.biClrUsed = 0;
        bi.biClrImportant = 0;

        return bi;
    }
}

bool WinScreenCapturer::Open(const ::std::string &windowName) noexcept
{
    hwnd_ = FindWindow(nullptr, windowName.c_str());

    return hwnd_ != nullptr;
}

::cv::Mat WinScreenCapturer::CaptureOne() noexcept
{
    assert(hwnd);

    ::cv::Mat mat;

    // get handles to a DC
    HDC hwindowDC, hwindowCompatibleDC;
    hwindowDC = GetDC(hwnd_);
    hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

    // define scale, height and width
    int screenx = GetSystemMetrics(SM_XVIRTUALSCREEN);
    int screeny = GetSystemMetrics(SM_YVIRTUALSCREEN);
    int width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    int height = GetSystemMetrics(SM_CYVIRTUALSCREEN);

    mat.create(height, width, CV_8UC4);

    HBITMAP hbwindow;
    hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
    BITMAPINFOHEADER bi = CreateBitmapInfoHeader(hbwindow);

    // use the previously created device context with the bitmap
    SelectObject(hwindowCompatibleDC, hbwindow);

    // copy from the window device context to the bitmap device context
    StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, screenx, screeny, width, height, SRCCOPY); // change SRCCOPY to NOTSRCCOPY for wacky colors !
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, mat.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);          // copy from hwindowCompatibleDC to hbwindow

    // avoid memory leak
    DeleteObject(hbwindow);
    DeleteDC(hwindowCompatibleDC);
    ReleaseDC(hwnd_, hwindowDC);

    return mat;
}

void WinScreenCapturer::Release() noexcept
{
}

_END_SCREEN2WEB_NM_