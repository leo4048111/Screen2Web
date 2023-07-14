#pragma once
#include "defines.h"

#include <SDL2/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif

#include <string>

_START_SCREEN2WEB_NM_

class ScreenCapturer
{
    SCREEN2WEB_SINGLETON(ScreenCapturer)

public:
    SDL_Surface *GetWindowSurface(const ::std::string &windowName) noexcept;

    SDL_Window *FindWindowByName(const ::std::string &windowName) noexcept;
};

_END_SCREEN2WEB_NM_