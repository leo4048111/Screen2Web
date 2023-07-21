#pragma once
#include "defines.h"

#include <thread>
#include <queue>
#include <mutex>

#include "httplib.h"

#include "frame.h"

_START_SCREEN2WEB_NM_

class HttpServer
{
    SCREEN2WEB_SINGLETON(HttpServer)

public:
    int Init() noexcept;

    int Listen(const ::std::string &host, int port, int socket_flags = 0) noexcept;

    int CleanUp() noexcept;

    void PushFrame(Frame &frame) noexcept;

private:
    bool done_{false};

    ::httplib::Server server_;
    ::std::thread sv_t_;
    ::std::thread scanner_t_;
    ::std::mutex cond_mutex_;
    ::std::condition_variable cond_;

    ::std::queue<Frame> frame_queue_;
    ::std::mutex frame_queue_mutex_;
};

_END_SCREEN2WEB_NM_