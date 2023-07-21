#include "httpserver.h"

#include "template.inc"

#include <vector>

#define SVPNG_PUT(u) buffer.push_back(u)
#define SVPNG_OUTPUT ::std::vector<std::uint8_t>& buffer
#include "svpng.inc"

_START_SCREEN2WEB_NM_

namespace
{

}

int HttpServer::Init() noexcept
{
	server_.Get("/", [](const httplib::Request &req, httplib::Response &res)
				{ res.set_content(html_code_buf, "text/html"); });

	server_.Get("/GetOneFrame", [&](const httplib::Request &req, httplib::Response &res)
				{
			Frame frame;
			{
				bool isEmpty = false;
				{
					::std::lock_guard<::std::mutex> lg(frame_queue_mutex_);
					isEmpty = frame_queue_.empty();
				}
				if (isEmpty) {
					::std::unique_lock<::std::mutex> lock(cond_mutex_);
					cond_.wait(lock);
				}

				::std::lock_guard<::std::mutex> lg(frame_queue_mutex_);
				frame = frame_queue_.front();
			}
			::std::vector<std::uint8_t> buffer;
			frame.ToFormat(PixelFormat::BGRA);
			svpng(buffer, frame.width, frame.height, (const unsigned char *)frame.data, 1);
			res.set_header("Content-Type", "image/png"); // Change "image/jpeg" to the appropriate content type if it's not an image.
			res.body = httplib::detail::base64_encode(::std::string(buffer.begin(), buffer.end()));
			res.status = 200; });

	return 0;
}

int HttpServer::Listen(const ::std::string &host, int port, int socket_flags) noexcept
{
	sv_t_ = ::std::thread([&]()
						  { int result = server_.listen(host.c_str(), port, socket_flags); });

	sv_t_.detach();

	scanner_t_ = ::std::thread([this]()
							   {
		for (;;)
		{
			if (frame_queue_mutex_.try_lock())
			{
				bool isEmpty = frame_queue_.empty();
				frame_queue_mutex_.unlock();
				if (isEmpty) {
					::std::unique_lock<::std::mutex> lock(cond_mutex_);
					cond_.wait(lock);
				}

				{
					::std::lock_guard<::std::mutex> lock(frame_queue_mutex_);
					auto now = ::std::chrono::high_resolution_clock::now();
					while (!frame_queue_.empty())
					{
						auto frame = frame_queue_.front();
						if (now - frame.timestamp > ::std::chrono::milliseconds(1000 / 25))
						{
							frame_queue_.pop();
						}
						else break;
					}
				}
			}
			::std::this_thread::sleep_for(::std::chrono::milliseconds(1000 / 25));
		} });

	scanner_t_.detach();

	return 0;
}

int HttpServer::CleanUp() noexcept
{
	done_ = true;
	return 0;
}

void HttpServer::PushFrame(Frame &frame) noexcept
{
	frame.timestamp = ::std::chrono::high_resolution_clock::now();
	::std::lock_guard<::std::mutex> lock(frame_queue_mutex_);
	if (frame_queue_.size() < 10)
	{
		frame_queue_.push(frame);
		cond_.notify_all();
	}
}

_END_SCREEN2WEB_NM_