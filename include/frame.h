#pragma once

#include "defines.h"

#include <cstdint>
#include <chrono>

_START_SCREEN2WEB_NM_

enum class PixelFormat
{
    UNKNOWN,
    RGB,
    RGBA,
    BGR,
    BGRA
};

class Frame
{
public:
    Frame() = default;
    Frame(const size_t width, const size_t height, const PixelFormat fmt);
    ~Frame();
    Frame(const Frame &);
    Frame(Frame &&);
    Frame &operator=(const Frame &);
    Frame &operator=(Frame &&);

    void ToFormat(const PixelFormat fmt);

public:
    PixelFormat fmt{PixelFormat::UNKNOWN};
    uint8_t *data{nullptr};
    size_t size{0};
    size_t width{0};
    size_t height{0};
    ::std::chrono::time_point<std::chrono::steady_clock, ::std::chrono::duration<double, ::std::milli>> timestamp;
};

_END_SCREEN2WEB_NM_