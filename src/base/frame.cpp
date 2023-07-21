#include "frame.h"

#include <cstring>

_START_SCREEN2WEB_NM_

namespace 
{
    size_t PixelFormatSize(const PixelFormat fmt)
    {
        switch (fmt)
        {
        case PixelFormat::RGB:
        case PixelFormat::BGR:
            return 3;

        case PixelFormat::BGRA:
        case PixelFormat::RGBA:
            return 4;
        default:
            return 0;
        }
    }
}

Frame::Frame(const size_t width, const size_t height, const PixelFormat fmt)
{
    this->size = width * height * PixelFormatSize(fmt);
    this->data = new uint8_t[size];
    this->width = width;
    this->height = height;
    this->fmt = fmt;
}

Frame::~Frame()
{
    if (data)
    {
        delete data;
        data = nullptr;
    }
}

Frame::Frame(const Frame& other)
{
    this->size = other.size;
    data = new uint8_t[size];
    ::std::memcpy(data, other.data, size);

    width = other.width;
    height = other.height;
    fmt = other.fmt;
    timestamp = other.timestamp;
}

Frame::Frame(Frame&& other)
{
    data = other.data;
    width = other.width;
    height = other.height;
    fmt = other.fmt;
    size = other.size;
    timestamp = other.timestamp;

    other.data = nullptr;
    other.width = 0;
    other.height = 0;
    other.fmt = PixelFormat::UNKNOWN;
    other.size = 0;
}

Frame& Frame::operator=(const Frame& other)
{
    if (this != &other)
    {
        delete[] data;

        size = other.size;
        data = new uint8_t[size];
        ::std::memcpy(data, other.data, size);

        width = other.width;
        height = other.height;
        fmt = other.fmt;
        timestamp = other.timestamp;
    }
    return *this;
}

Frame& Frame::operator=(Frame&& other)
{
    if (this != &other)
    {
        if(this->data)
        {
            delete[] data;
            data = nullptr;
        }

        data = other.data;
        width = other.width;
        height = other.height;
        fmt = other.fmt;
        size = other.size;
        timestamp = other.timestamp;

        other.data = nullptr;
        other.width = 0;
        other.height = 0;
        other.fmt = PixelFormat::UNKNOWN;
        other.size = 0;
    }
    return *this;
}

_END_SCREEN2WEB_NM_