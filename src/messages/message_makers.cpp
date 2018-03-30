#include "message_makers.h"

#include <iostream>
#include <external/json.hpp>

std::string make_on_button_message(std::int32_t client_id, std::uint32_t n) {
    using nlohmann::json;

    json inner;
    inner["client_id"] = client_id;
    inner["button"] = n;

    json data;
    data["on_button"] = inner;

    return "        " + data.dump() + "\n";
}

std::string make_get_button_description_message(std::int32_t client_id, std::uint32_t n) {
    using nlohmann::json;

    json inner;
    inner["client_id"] = client_id;
    inner["button"] = n;

    json data;
    data["get_button_description"] = inner;

    return "        " + data.dump() + "\n";
}

std::string make_on_board_message(std::int32_t client_id, std::uint32_t col, std::uint32_t row) {

    using nlohmann::json;

    json inner;
    inner["client_id"] = client_id;
    inner["col"] = col;
    inner["row"] = row;

    json data;
    data["on_board"] = inner;

    return "        " + data.dump() + "\n";
}
