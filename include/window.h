#pragma once

#include "defines.h"

#include <string>
#include <memory>
#include <mutex>
#include <thread>

#include <SDL2/SDL.h>
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL2/SDL_opengles2.h>
#else
#include <SDL2/SDL_opengl.h>
#endif

#include "screencapturer.h"

_START_SCREEN2WEB_NM_

class Window
{
    SCREEN2WEB_SINGLETON(Window)

public:
    int Init() noexcept;

    int Loop() noexcept;

    int DeInit() noexcept;

private:
    bool CheckCapturer() noexcept;

    void ShowConfigWindow() noexcept;

    void ShowCapturedWindow() noexcept;

private:
    bool done_{false};
    SDL_Window *window_{nullptr};
    ::std::unique_ptr<ScreenCapturer> capturer_{nullptr};
    SDL_GLContext gl_context_{nullptr};
    ::std::string window_name_{"Screen2Web"};
    ::std::string captured_window_name_{window_name_};
    size_t window_width_{1280}, window_height_{720};
    ::std::vector<::std::string> windownames_;
    ::std::mutex window_mutex_;
    GLuint captured_window_texture_;
    ::std::thread get_windows_t_;

    uint32_t fps_{25};
};

_END_SCREEN2WEB_NM_
