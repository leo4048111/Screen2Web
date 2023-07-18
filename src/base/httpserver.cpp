#include "httpserver.h"

#include "template.inc"

_START_SCREEN2WEB_NM_

namespace {
	::std::string Frame2Base64Encoded(const Frame& frame)
	{
		std::string data(frame.data, frame.data + frame.capacity);
		return httplib::detail::base64_encode(data);
	}
}

int HttpServer::Init() noexcept
{
	server_.Get("/", [](const httplib::Request& req, httplib::Response& res) {
		res.set_content(html_code_buf, "text/html");
	});

	server_.Get("/GetOneFrame", [&](const httplib::Request& req, httplib::Response& res) {
		Frame frame;
		{
			::std::lock_guard<::std::mutex> lock(frame_queue_mutex_);
			if(frame_queue_.size())
				frame = frame_queue_.front();
			if (frame_queue_.size() > 1) frame_queue_.pop();
		}
		::std::string base64Data = Frame2Base64Encoded(frame);
		res.set_header("Content-Type", "image/png");
		res.set_content(base64Data, "image/png");
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

void HttpServer::PushFrame(const Frame& frame) noexcept
{
	::std::lock_guard<::std::mutex> lock(frame_queue_mutex_);
	if(frame_queue_.size() < 5)
		frame_queue_.push(frame);
}

_END_SCREEN2WEB_NM_