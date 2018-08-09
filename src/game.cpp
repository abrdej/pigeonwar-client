#include "game.h"
#include "res.h"

#include <iostream>

#include <animations/animations_service.h>
#include <messages/message_makers.h>
#include <messages/json_message_conversions.h>
#include <sprites/sprites_manager.h>

game::game(threadsafe_queue<std::string>& message_queue,
           const std::function<void(const std::string&)>& send_fn)
        : incoming_messages(message_queue),
          sender(send_fn) {
}

void game::setup() {
    board_panel_.set_on_board([this](std::uint32_t col, std::uint32_t row) {
        sender(make_on_board_message(player_id, col, row));
    });
    board_panel_.init(board_container::rows_n, board_container::cols_n);

    buttons_panel_.set_on_button([this](std::uint32_t n) {
        sender(make_on_button_message(player_id, n));
    });
    buttons_panel_.set_get_button_description([this](std::uint32_t n) {
        sender(make_get_button_description_message(player_id, n));
    });
    buttons_panel_.set_on_entity([this]() {

        auto selected_entity_id = state.board.at(lstate.selected_index);

        std::string entity_info = lstate.entity_name;

        std::string active_effects;
        for (auto&& effect : state.entities_additional_effects[selected_entity_id]) {
            active_effects += effect;
            active_effects += "\n";
        }

        if (active_effects.empty()) {
            active_effects = "None.\n";
        }
        entity_info += "\nActive effects:\n" + active_effects;

        buttons_panel_.set_entity_info(entity_info);
    });
    buttons_panel_.set_on_active_effect_fn([this]() {
        auto selected_entity_id = state.board.at(lstate.selected_index);


        std::string active_effects;
        for (auto&& effect : state.entities_additional_effects[selected_entity_id]) {
            active_effects += effect;
            active_effects += "\n";
        }

        if (active_effects.empty()) {
            active_effects = "None.\n";
        }
        std::string active_effects_msg = "\nActive effects:\n" + active_effects;

        buttons_panel_.set_active_effect(active_effects_msg);
    });

    buttons_panel_.init(board_container::cols_n, board_container::rows_n);
}

void game::process_messages() {

    if (!animations_service::is_done())
        return;

    std::string message;
    if (incoming_messages.try_pop(message)) {

        try {

            using nlohmann::json;
            json data = json::parse(message);

            if (data.count("map_size")) {
//                map_size = data["map_size"];

            } else if (data.count("client_id")) {
                std::cout << "client_id:\n";
                player_id = data["client_id"];

            } else if (data.count("map_name")) {
                std::string map_name;
                map_name = data["map_name"];
                //window_.setTitle(map_name);

//			} else if (data.count("entities_names")) {
//				json_to_unordered_map(data["entities_names"], state.entities_names);
//
//			} else if (data.count("entities_healths")) {
//				json_to_unordered_map(data["entities_healths"], state.healths);

            } else if (data.count("entities_pack")) {

                std::unordered_map<std::uint32_t,
                        std::tuple<std::string, std::int32_t, std::uint32_t, std::uint32_t>> entities_pack;
                json_to_unordered_map(data["entities_pack"], entities_pack);

                for (auto&& entity_pack : entities_pack) {
                    sprites_manager::update_sprite(entity_pack.first,
                                                   std::get<0>(entity_pack.second),
                                                   std::get<1>(entity_pack.second),
                                                   std::get<2>(entity_pack.second),
                                                   std::get<3>(entity_pack.second));
                }

            } else if (data.count("remove_entity")) {
                std::uint32_t entity_id_to_remove = data["remove_entity"];
                sprites_manager::remove_sprite(entity_id_to_remove);

            } else if (data.count("create_entity")) {

                std::uint32_t entity_id = data["create_entity"];
                std::string name = data["name"];
                std::int32_t health = data["health"];
                std::int32_t power = data["power"];
                std::uint32_t index = data["index"];

                sprites_manager::update_sprite(entity_id,
                                               name,
                                               health,
                                               power,
                                               index);


            } else if (data.count("end_turn")) {
                std::uint32_t active_player_id = data["end_turn"];
                std::cout << "end_turn: " << active_player_id << "\n";

            } else if (data.count("move_entity")) {

                std::uint32_t entity_id = data["move_entity"];
                std::uint32_t to_index = data["to_index"];

                sprites_manager::drawer_for(entity_id)->set_pos(to_index);

            } else if (data.count("game_state")) {
                state = data["game_state"];
                update_for_entity();

            } else if (data.count("local_state")) {
                from_json(data["local_state"], lstate);
                std::cout << "update state\n";
                board_panel_.update_state(lstate);
                update_for_entity();

            } else if (data.count("description")) {
                std::string description = data["description"];
                buttons_panel_.set_entity_info(description);

            } else if (data.count("animation")) {
                animations_service::handle(data, state);
            } else if (data.count("battle_end")) {
                std::int32_t win_player_id = data["battle_end"];
                if (player_id == win_player_id) {
                    std::cout << "You win!!!\n";
                    std::exit(0);
                } else {
                    std::cout << "You defeat!!!\n";
                    std::exit(0);
                }
            }

        } catch (nlohmann::detail::parse_error&) {
            std::cout << "Parse error in: " << message << "\n";

        } catch (nlohmann::detail::type_error&) {
            std::cout << "Type error in: " << message << "\n";
        }
    }
}

void game::update_for_entity()
{
    if (lstate.selected_index == -1) {
        return;
    }

    std::cout << "selected_index: " << lstate.selected_index << "\n";

    auto entity_id = state.board.at(lstate.selected_index);

    std::cout << "entity_id: " << entity_id << "\n";

    if (entity_id != std::numeric_limits<decltype(entity_id)>::max()) {

        std::string entity_health;
        std::string entity_power;

        auto health_value = sprites_manager::drawer_for(entity_id)->health;
        if (health_value != std::numeric_limits<std::int32_t>::max()) {
            entity_health = std::to_string(health_value);
        }
        auto power_value = sprites_manager::drawer_for(entity_id)->power;
        if (power_value != std::numeric_limits<std::int32_t>::max()) {
            entity_power = std::to_string(power_value);
        }

        buttons_panel_.set_for_entity_for(entity_id,
                                          lstate.entity_name,
                                          entity_health,
                                          entity_power,
                                          lstate.button_bitmaps);

        auto this_entity_id = state.board.at(lstate.selected_index);

        std::string effects;

        for (auto&& effect : state.entities_additional_effects[this_entity_id]) {
            effects += effect;
            effects += "\n";
        }
    }
}