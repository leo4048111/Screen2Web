#include "screenshotter.h"

_START_SCREEN2WEB_NM_

SDL_Surface *ScreenCapturer::GetWindowSurface(const std::string &windowName) noexcept
{
    SDL_Window *window = SDL_GetWindowFromID(SDL_GetWindowID(SDL_GetWindows()->id));
    if (window == nullptr)
    {
        return nullptr;
    }
    return SDL_GetWindowSurface(window);
}

SDL_Window *ScreenCapturer::FindWindowByName(const ::std::string &windowName) noexcept
{
    SDL_Window *wnd = nullptr;

    for (int i = 0; i < SDL_GetNumVideoDrivers(); i++)
    {
        ::std::string title = SDL_GetVideoDriver(i);
        SDL_GetWindowTitle() if (title == windowName)
        {
            wnd =
        }
    }
}

_END_SCREEN2WEB_NM_
