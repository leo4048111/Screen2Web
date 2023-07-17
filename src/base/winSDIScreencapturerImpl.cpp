#include "winSDIScreencapturerImpl.h"

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
        bi.biHeight = bmp.bmHeight;
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

bool WinSDIScreenCapturer::Open(const ::std::string &windowName) noexcept
{
    hwnd_ = FindWindow(nullptr, windowName.c_str());

    return hwnd_ != nullptr;
}

Frame WinSDIScreenCapturer::CaptureOne() noexcept
{
    assert(hwnd_);

    HDC hdcScreen;
    HDC hdcWindow;
    HDC hdcMemDC = NULL;
    HBITMAP hbmScreen = NULL;
    BITMAP bmpScreen;
    DWORD dwBytesWritten = 0;
    DWORD dwSizeofDIB = 0;
    HANDLE hFile = NULL;
    char *lpbitmap = NULL;
    HANDLE hDIB = NULL;
    DWORD dwBmpSize = 0;
    Frame frame(0, 0, PixelFormat::UNKNOWN);

    // Retrieve the handle to a display device context for the client
    // area of the window.
    // hdcScreen = GetDC(NULL);
    hdcWindow = GetDC(hwnd_);

    // Create a compatible DC, which is used in a BitBlt from the window DC.
    hdcMemDC = CreateCompatibleDC(hdcWindow);

    if (!hdcMemDC)
    {
        goto done;
    }

    // Get the client area for size calculation.
    RECT rcClient;
    GetClientRect(hwnd_, &rcClient);

    //// This is the best stretch mode.
    // SetStretchBltMode(hdcWindow, HALFTONE);

    //// The source DC is the entire screen, and the destination DC is the current window (HWND).
    // if (!StretchBlt(hdcWindow,
    //     0, 0,
    //     rcClient.right, rcClient.bottom,
    //     hdcScreen,
    //     0, 0,
    //     GetSystemMetrics(SM_CXSCREEN),
    //     GetSystemMetrics(SM_CYSCREEN),
    //     SRCCOPY))
    //{
    //     LOG("StretchBlt has failed");
    //     goto done;
    // }

    // Create a compatible bitmap from the Window DC.
    int width = rcClient.right - rcClient.left;
    int height = rcClient.bottom - rcClient.top;

    hbmScreen = CreateCompatibleBitmap(hdcWindow, width, height);

    if (!hbmScreen)
    {
        goto done;
    }

    // Select the compatible bitmap into the compatible memory DC.
    SelectObject(hdcMemDC, hbmScreen);

    // Bit block transfer into our compatible memory DC.
    if (!StretchBlt(hdcMemDC,
                    0, 0,
                    width, height,
                    hdcWindow,
                    0, 0,
                    width, height,
                    SRCCOPY))
    {
        goto done;
    }

    // Get the BITMAP from the HBITMAP.
    GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

    BITMAPFILEHEADER bmfHeader;
    BITMAPINFOHEADER bi;

    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bmpScreen.bmWidth;
    bi.biHeight = bmpScreen.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 32;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

    // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that
    // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc
    // have greater overhead than HeapAlloc.
    hDIB = GlobalAlloc(GHND, dwBmpSize);
    lpbitmap = (char *)GlobalLock(hDIB);

    // Gets the "bits" from the bitmap, and copies them into a buffer
    // that's pointed to by lpbitmap.
    // frame = Frame(bi.biWidth, bi.biHeight, PixelFormat::RGBA);
    frame = Frame(width, height, PixelFormat::RGBA);

    GetDIBits(hdcWindow, hbmScreen, 0,
              (UINT)bmpScreen.bmHeight,
              frame.data,
              (BITMAPINFO *)&bi, DIB_RGB_COLORS);

    // Unlock and Free the DIB from the heap.
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);

    // Clean up.
done:
    DeleteObject(hbmScreen);
    DeleteObject(hdcMemDC);
    // ReleaseDC(NULL, hdcScreen);
    ReleaseDC(hwnd_, hdcWindow);

    return frame;
}

void WinSDIScreenCapturer::Release() noexcept
{
}

_END_SCREEN2WEB_NM_