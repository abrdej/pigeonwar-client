#include "oxygine-framework.h"
#include "res.h"
#include "client.h"
#include "game.h"

using namespace oxygine;

std::unique_ptr<client> client_;
std::unique_ptr<game> game_;

void preinitialize() {

//    std::string address = "194.182.72.172";
    std::string address = "127.0.0.1";
    int port = 5555;
    client_ = std::make_unique<client>(address, port);
}

void initizalize() {

    res::load();

    game_ = std::make_unique<game>(client_->get_message_queue(),
                                   [&client_](const std::string& msg) {
                                       client_->send_message(msg);
                                   });

    client_->connect();
    game_->setup();
}

void update() {
    game_->process_messages();
}

void destroy() {
    res::free();
}