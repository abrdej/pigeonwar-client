#include "client.h"
#include "gui/view_constants.h"
#include <algorithm>
#include <chrono>
#include "sprites/drawer_factory.h"
#include "messages/json_message_conversions.h"
#include "messages/message_makers.h"
#include "animations/animations_service.h"
#include "sprites/sprites_manager.h"

client::client(const std::string& address, unsigned short port)
		: address(address),
		  port(port),
		  joiner("\n") {
}

void client::connect()
{
	std::cout << "Waiting for server...\n";

    asio::ip::tcp::endpoint endpoint(asio::ip::address::from_string(address), port);
    socket = std::make_unique<asio::ip::tcp::socket>(io_service);

    try {
        socket->connect(endpoint);
    } catch (...) {
        std::cout << "connect exception\n";
    }

    std::cout << "Server connected:)\n";

    read_async();

    try {
        io_service_thread = std::thread([this]() {
            io_service.run();
        });
    } catch (asio::system_error& err) {
        std::cout << "connect exception: " << err.what() << "\n";
    }
}

threadsafe_queue<std::string>& client::get_message_queue() {
	return message_queue;
}

void client::send_message(const std::string& message) {
	asio::write(*socket, asio::buffer(message), asio::transfer_all());
}

void client::read_async() {
    asio::async_read_until(*socket, buffer, "\n",
                           [this](const asio::error_code& error,
                                  std::size_t bytes_transferred) {

                               if (error) {
                                   std::cout << "errorfsdf" << error.message() << "\n";
                               }

                               std::cout << "Get message\n";

                               std::istream istream(&buffer);
                               std::string message_data(std::istreambuf_iterator<char>(istream), {});

                               auto messages = joiner.add_message_data(message_data);

                               for (auto&& message : messages) {
                                   std::cout << message << "\n";
								   message_queue.push(message);
                               }
                               buffer.consume(buffer.size() + 1);

                               read_async();
                           });
}




