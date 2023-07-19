#pragma once

#include "defines.h"

#include <cstdint>

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

public:
    PixelFormat fmt{PixelFormat::UNKNOWN};
    uint8_t *data{nullptr};
    size_t capacity{0};
    size_t width{0};
    size_t height{0};
};

_END_SCREEN2WEB_NM_