#include "window.h"

#include "imgui.h"
#include "imgui_impl_sdl2.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#include <thread>
#include <optional>

#if defined(WIN32)
#include "winSDIScreencapturerImpl.h"
// #include "winDXGIScreencapturerImpl.h"
#elif defined(__APPLE__)
#include "macScreencapturerImpl.h"
#endif

#include "httpserver.h"

_START_SCREEN2WEB_NM_

namespace
{
template <typename T, typename... Args>
auto ImGui_HorizontallyCenterElement(T func, Args &&...args){
    const float input_width = ImGui::CalcItemWidth();
    ImGui::SetCursorPosX((ImGui::GetWindowContentRegionWidth() - input_width) / 2.f);
    return std::invoke(func, ::std::forward<Args>(args)...);
}
}

int Window::Init() noexcept
{
    // Setup SDL
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        printf("Error: %s\n", SDL_GetError());
        return -1;
    }

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100
    const char *glsl_version = "#version 100";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#elif defined(__APPLE__)
    // GL 3.2 Core + GLSL 150
    const char *glsl_version = "#version 150";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG); // Always required on Mac
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
#else
    // GL 3.0 + GLSL 130
    const char *glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
#endif

    // From 2.0.18: Enable native IME.
#ifdef SDL_HINT_IME_SHOW_UI
    SDL_SetHint(SDL_HINT_IME_SHOW_UI, "1");
#endif

    // Create window with graphics context
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
    window_ = SDL_CreateWindow(window_name_.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width_, window_height_, window_flags);
    gl_context_ = SDL_GL_CreateContext(window_);
    SDL_GL_MakeCurrent(window_, gl_context_);
    SDL_GL_SetSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    io.IniFilename = nullptr;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;  // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    // ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplSDL2_InitForOpenGL(window_, gl_context_);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    // io.Fonts->AddFontDefault();
    // io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    // io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    // ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    // IM_ASSERT(font != nullptr);

    // Init textures
    glGenTextures(1, &captured_window_texture_);
    glBindTexture(GL_TEXTURE_2D, captured_window_texture_);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR);
    // glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);

    done_ = false;

    get_windows_t_ = ::std::thread([&]()
                                   {
            ::std::this_thread::sleep_for(::std::chrono::milliseconds(200));
            while (!done_)
            {
                if (capturer_)
                {
                    auto x = capturer_->GetAllWindowNames();
                    {
                        ::std::lock_guard<::std::mutex> lock(window_mutex_);
                        windownames_ = x;
                    }
                }
                ::std::this_thread::sleep_for(::std::chrono::milliseconds(5000));
            } });

    get_windows_t_.detach();

    return 0;
}

int Window::Loop() noexcept
{
    ImGuiIO &io = ImGui::GetIO();
    (void)io;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    while (!done_)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                done_ = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(window_))
                done_ = true;
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED && event.window.windowID == SDL_GetWindowID(window_)) // handle resize
            {
                window_width_ = event.window.data1;
                window_height_ = event.window.data2;
            }
        }

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(window_width_, window_height_), 0);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::Begin("Screen2Web preview", nullptr, ImGuiWindowFlags_NoCollapse); // Create a window called "Hello, world!" and append into it.
        if (CheckCapturer())
        {
            ShowConfigWindow();
            ShowCapturedWindow();
        }
        ImGui::End();

        // Rendering
        ImGui::Render();
        glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window_);
    }

    return 0;
}

int Window::DeInit() noexcept
{
    // Destroy texture
    glDeleteTextures(1, &captured_window_texture_);

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context_);
    SDL_DestroyWindow(window_);
    SDL_Quit();

    return 0;
}

bool Window::CheckCapturer() noexcept
{
    if (capturer_ == nullptr)
    {
#if defined(WIN32)
        capturer_ = ::std::make_unique<WinSDIScreenCapturer>();
#elif defined(__APPLE__)
        capturer_ = ::std::make_unique<MacScreenCapturer>();
#endif
    }

    return true;
}

void Window::ShowConfigWindow() noexcept
{
    static ImGuiComboFlags flags = 0;
    {
        ::std::lock_guard<::std::mutex> lock(window_mutex_);
        auto combo_preview_value = captured_window_name_; // Pass in the preview value visible before opening the combo (it could be anything)
        if (ImGui_HorizontallyCenterElement(ImGui::BeginCombo, "Capture window", combo_preview_value.c_str(), flags))
        {
            for (int n = 0; n < windownames_.size(); n++)
            {
                if (ImGui::Selectable(windownames_[n].c_str(), false))
                {
                    if (windownames_[n] != captured_window_name_)
                    {
                        captured_window_name_ = windownames_[n];
                        capturer_->Open(windownames_[n]);
                    }
                }
            }
            ImGui::EndCombo();
        }
    }
}

void Window::ShowCapturedWindow() noexcept
{
    if (capturer_)
    {
        Frame frame = capturer_->CaptureOne();
        if (frame.fmt == PixelFormat::UNKNOWN)
            return;

        HttpServer::GetInstance().PushFrame(frame);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA,
                     frame.width,
                     frame.height,
                     0,
                     GL_BGRA,
                     GL_UNSIGNED_BYTE,
                     frame.data);
        if (frame.width > window_width_)
        {
            double ratio = (double)frame.width / frame.height;
            frame.width = window_width_;
            frame.height = frame.width / ratio;
        }

        if (frame.height > window_height_)
        {
            double ratio = (double)frame.height / frame.width;
            frame.height = window_height_ - 100;
            frame.width = frame.height / ratio;
        }
        ImGui::BeginChild("Captured window", ImVec2(window_width_, window_height_ - 100), true);
        // This centering has to be done manually because ImGui::CalcItemWidth() cannot get correct image width
        const float padding = (ImGui::GetWindowContentRegionWidth() - frame.width) / 2.0f;
        ImGui::SetCursorPosX(padding);
        ImGui::Image(
            (void*)(intptr_t)captured_window_texture_,
            ImVec2(frame.width, frame.height));
        ImGui::EndChild();
    }
}

_END_SCREEN2WEB_NM_
