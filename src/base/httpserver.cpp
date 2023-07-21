#include "httpserver.h"

#include "template.inc"

_START_SCREEN2WEB_NM_

namespace
{
	::std::string Frame2Base64Encoded(const Frame &frame)
	{
		std::string data(frame.data, frame.data + frame.size);
		return httplib::detail::base64_encode(data);
	}
}

int HttpServer::Init() noexcept
{
	server_.Get("/", [](const httplib::Request &req, httplib::Response &res)
				{ res.set_content(html_code_buf, "text/html"); });

	server_.Get("/GetOneFrame", [&](const httplib::Request &req, httplib::Response &res)
				{
		Frame frame;
		{
			::std::lock_guard<::std::mutex> lock(frame_queue_mutex_);
				frame = frame_queue_.front();
		}
		::std::string base64Data = Frame2Base64Encoded(frame);
		res.set_header("Content-Type", "image/png");
		res.set_content(base64Data, "image/png"); });

	return 0;
}

int HttpServer::Listen(const ::std::string &host, int port, int socket_flags) noexcept
{
	sv_t_ = ::std::thread([&]()
						  { 
			int result = server_.listen(host.c_str(), port, socket_flags);
		});

	sv_t_.detach();

	scanner_t_ = ::std::thread([this]() {
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
		}
		});

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
	if (frame_queue_.size() < 5)
	{
		frame_queue_.push(frame);
		cond_.notify_one();
	}
}

_END_SCREEN2WEB_NM_