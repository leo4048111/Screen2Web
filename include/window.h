#pragma once

#include "defines.h"

#include <SDL2/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif

_START_SCREEN2WEB_NM_

class Window
{
    SCREEN2WEB_SINGLETON(Window)

public:
    int Init() noexcept;

    int Loop() noexcept;

    int DeInit() noexcept;

private:
    bool done_{false};
    SDL_Window *window_{nullptr};
    SDL_GLContext gl_context_{nullptr};
};

_END_SCREEN2WEB_NM_
