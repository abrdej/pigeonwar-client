//
// Created by abrdej on 21.03.18.
//

#ifndef PIGEONWAR_GAME_H
#define PIGEONWAR_GAME_H

#include <gui/buttons_panel.h>
#include <gui/board_panel.h>
#include <game_state.h>
#include <utils/threadsafe_queue.h>

class game {
public:
    explicit game(threadsafe_queue<std::string>& message_queue,
                  const std::function<void(const std::string&)>& send_fn);
    void setup();
    void process_messages();

private:
    void update_for_entity();

    threadsafe_queue<std::string>& incoming_messages;
    std::function<void(const std::string&)> sender;

    static const std::int32_t no_player_id = std::numeric_limits<std::int32_t>::max();
    std::int32_t player_id{no_player_id};

    board_panel board_panel_;
    buttons_panel buttons_panel_;

    oxygine::spTextField description_ptr;
    oxygine::Vector2 desc_init_pos;
    oxygine::spColorRectSprite desc_rect;

    game_state state;
    local_state lstate;
};


#endif //PIGEONWAR_GAME_H
