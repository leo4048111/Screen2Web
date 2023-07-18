#pragma once
#include "defines.h"

#include <thread>
#include <httplib.h>

_START_SCREEN2WEB_NM_

class HttpServer
{
    SCREEN2WEB_SINGLETON(HttpServer)

public:
    int Init() noexcept;

    int Listen(const ::std::string& host, int port, int socket_flags = 0) noexcept;

    int CleanUp() noexcept;

private:
    ::httplib::Server server_;
    ::std::thread sv_t_;
};

_END_SCREEN2WEB_NM_