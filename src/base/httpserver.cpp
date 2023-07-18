#include "httpserver.h"

#include "template.inc"

_START_SCREEN2WEB_NM_

int HttpServer::Init() noexcept
{
	server_.Get("/", [](const httplib::Request& req, httplib::Response& res) {
		res.set_content(html_code_buf, "text/html");
	});

	return 0;
}

int HttpServer::Listen(const ::std::string& host, int port, int socket_flags) noexcept
{
	sv_t_ = ::std::thread([&]() {
		server_.listen("0.0.0.0", 1899);
		});

	sv_t_.detach();

	return 0;
}

int HttpServer::CleanUp() noexcept
{
	sv_t_.join();

	return 0;
}

_END_SCREEN2WEB_NM_