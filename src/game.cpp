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
    board_panel_.init(10, 15);

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

//        if (description_ptr) {
//            description_ptr->detach();
//            description_ptr = nullptr;
//            desc_rect->detach();
//            desc_rect = nullptr;
//            desc_init_pos = oxygine::Vector2(0, 0);
//        }
//
//        description_ptr = new oxygine::TextField;
//        description_ptr->attachTo(oxygine::getStage());
//        description_ptr->setFontSize(18);
//        description_ptr->setColor(oxygine::Color::White);
//        description_ptr->setFont(res::ui.getResFont("main"));
//        description_ptr->setText(effects);
//        description_ptr->setPriority(10000);
//        description_ptr->setTouchEnabled(false);
//        description_ptr->setPosition(getStage()->getWidth() / 2, getStage()->getHeight() / 1.5);
//        description_ptr->setHAlign(oxygine::TextStyle::HALIGN_MIDDLE);
//        description_ptr->setVAlign(oxygine::TextStyle::VALIGN_MIDDLE);
//
//        oxygine::RectF bounds;
//        description_ptr->getBounds(bounds);
//        bounds.expand(oxygine::Vector2(10, 10), oxygine::Vector2(10, 20));
//
//        desc_rect = new oxygine::ColorRectSprite;
//        desc_rect->attachTo(oxygine::getStage());
//        desc_rect->setSize(bounds.getSize());
//        desc_rect->setColor(oxygine::Color::Black);
//        desc_rect->setPriority(9999);
//        desc_rect->setTouchEnabled(false);
//        desc_rect->setAnchor(0.5, 0.5);
//        desc_rect->setPosition(getStage()->getWidth() / 2, getStage()->getHeight() / 1.5);
//
//        oxygine::getStage()->addEventListener(oxygine::TouchEvent::MOVE, [=](oxygine::Event* ev) {
//
//            oxygine::TouchEvent *touch = (oxygine::TouchEvent *) ev;
//            auto pos = touch->localPosition;
//
//            if (desc_init_pos == oxygine::Vector2(0, 0)) {
//                desc_init_pos = pos;
//            } else if (desc_init_pos.distance(pos) > 50) {
//                if (description_ptr) {
//                    description_ptr->detach();
//                    description_ptr = nullptr;
//                    desc_rect->detach();
//                    desc_rect = nullptr;
//                    desc_init_pos = oxygine::Vector2(0, 0);
//                }
//                oxygine::getStage()->removeEventListener(ev->listenerID);
//            }
//        });

    });
    buttons_panel_.init();
}

void game::process_messages() {

    if (!animations_service::is_done())
        return;

    std::string message;
    if (incoming_messages.try_pop(message)) {

        try {

            using nlohmann::json;
            json data = json::parse(message);

            if (data.count("client_id")) {
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
                        std::tuple<std::string, std::int32_t, std::uint32_t>> entities_pack;
                json_to_unordered_map(data["entities_pack"], entities_pack);

                for (auto&& entity_pack : entities_pack) {
                    sprites_manager::update_sprite(entity_pack.first,
                                                   std::get<0>(entity_pack.second),
                                                   std::get<1>(entity_pack.second),
                                                   std::get<2>(entity_pack.second));
                }

            } else if (data.count("remove_entity")) {
                std::uint32_t entity_id_to_remove = data["remove_entity"];
                sprites_manager::remove_sprite(entity_id_to_remove);

            } else if (data.count("create_entity")) {

                std::uint32_t entity_id = data["create_entity"];
                std::string name = data["name"];
                std::int32_t health = data["health"];
                std::uint32_t index = data["index"];

                sprites_manager::update_sprite(entity_id,
                                               name,
                                               health,
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

//                for (auto&& health_pack : state.healths) {
//                    sprites_manager::drawer_for(health_pack.first)->set_health(health_pack.second);
//                }
//
//                for (std::uint32_t index = 0; index < state.board.fields_.size(); ++index) {
//                    for (auto&& entity_id : state.board.fields_[index]) {
//                        sprites_manager::drawer_for(entity_id)->set_pos(board_index_to_point(index));
//                    }
//                }
//
//                update_for_entity();

            } else if (data.count("local_state")) {
                from_json(data["local_state"], lstate);
                update_for_entity();
                std::cout << "update state\n";
                board_panel_.update_state(lstate);

            } else if (data.count("description")) {
                std::string description = data["description"];

                buttons_panel_.set_desc(description);

//                if (description_ptr) {
//                    description_ptr->detach();
//                    description_ptr = nullptr;
//                    desc_rect->detach();
//                    desc_rect = nullptr;
//                    desc_init_pos = oxygine::Vector2(0, 0);
//                }
//
//                description_ptr = new oxygine::TextField;
//                description_ptr->attachTo(oxygine::getStage());
//                description_ptr->setFontSize(18);
//                description_ptr->setColor(oxygine::Color::White);
//                description_ptr->setFont(res::ui.getResFont("main"));
//                description_ptr->setText(description);
//                description_ptr->setPriority(10000);
//                description_ptr->setTouchEnabled(false);
//                description_ptr->setPosition(getStage()->getWidth() / 2, getStage()->getHeight() / 1.5);
//                description_ptr->setHAlign(oxygine::TextStyle::HALIGN_MIDDLE);
//                description_ptr->setVAlign(oxygine::TextStyle::VALIGN_MIDDLE);
//
//                oxygine::RectF bounds;
//                description_ptr->getBounds(bounds);
//                bounds.expand(oxygine::Vector2(10, 10), oxygine::Vector2(10, 20));
//
//                desc_rect = new oxygine::ColorRectSprite;
//                desc_rect->attachTo(oxygine::getStage());
//                desc_rect->setSize(bounds.getSize());
//                desc_rect->setColor(oxygine::Color::Black);
//                desc_rect->setPriority(9999);
//                desc_rect->setTouchEnabled(false);
//                desc_rect->setAnchor(0.5, 0.5);
//                desc_rect->setPosition(getStage()->getWidth() / 2, getStage()->getHeight() / 1.5);
//
//                oxygine::getStage()->addEventListener(oxygine::TouchEvent::MOVE, [=](oxygine::Event* ev) {
//
//                    oxygine::TouchEvent *touch = (oxygine::TouchEvent *) ev;
//                    auto pos = touch->localPosition;
//
//                    if (desc_init_pos == oxygine::Vector2(0, 0)) {
//                        desc_init_pos = pos;
//                    } else if (desc_init_pos.distance(pos) > 50) {
//                        if (description_ptr) {
//                            description_ptr->detach();
//                            description_ptr = nullptr;
//                            desc_rect->detach();
//                            desc_rect = nullptr;
//                            desc_init_pos = oxygine::Vector2(0, 0);
//                        }
//                        oxygine::getStage()->removeEventListener(ev->listenerID);
//                    }
//                });

            } else if (data.count("animation")) {
                animations_service::handle(data, state);
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
    if (lstate.selected_index == -1)
        return;

    auto entity_id = state.board.at(lstate.selected_index);

    if (entity_id != std::numeric_limits<decltype(entity_id)>::max()) {
        std::string entity_health = std::to_string(sprites_manager::drawer_for(entity_id)->health);

        buttons_panel_.set_for_entity_for(entity_id, lstate.entity_name, entity_health, lstate.button_bitmaps);

        auto this_entity_id = state.board.at(lstate.selected_index);

        std::string effects;

        for (auto&& effect : state.entities_additional_effects[this_entity_id]) {
            effects += effect;
            effects += "\n";
        }
    }
}