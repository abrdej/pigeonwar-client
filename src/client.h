#ifndef GUI_H
#define GUI_H

#include <thread>

#define ASIO_STANDALONE
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

#include "asio.hpp"

#include <utils/message_joiner.h>
#include <utils/threadsafe_queue.h>

class client final
{
public:
	client(const std::string& address, unsigned short port);
	void connect();
	threadsafe_queue<std::string>& get_message_queue();
	void send_message(const std::string& message);

private:
	void read_async();

	const std::string address;
	const unsigned short port;

    asio::io_service io_service;
    std::unique_ptr<asio::ip::tcp::socket> socket;
	asio::streambuf buffer;
    std::thread io_service_thread;

    message_joiner joiner;
	threadsafe_queue<std::string> message_queue;
};

#endif