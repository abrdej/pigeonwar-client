//
// Created by abrdej on 04.01.18.
//

#ifndef PIGEONWAR_REQUESTS_H
#define PIGEONWAR_REQUESTS_H

#include <string>

std::string make_on_button_message(std::int32_t client_id, std::uint32_t n);

std::string make_get_button_description_message(std::int32_t client_id, std::uint32_t n);

std::string make_on_board_message(std::int32_t client_id, std::uint32_t col, std::uint32_t row);

#endif //PIGEONWAR_REQUESTS_H
