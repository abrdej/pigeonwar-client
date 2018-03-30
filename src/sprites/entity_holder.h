//
// Created by abrdej on 18.11.17.
//

#ifndef PIGEONWAR_ENTITY_DRAWER_H
#define PIGEONWAR_ENTITY_DRAWER_H

#include "oxygine-framework.h"

struct entity_holder {

    std::uint32_t entity_id;
    std::string bmt_key;
    oxygine::spSprite sprite;
    oxygine::spTextField health_text;
    oxygine::spColorRectSprite health_background;
    oxygine::Point pos;
    std::int32_t health;

    entity_holder(std::uint32_t entity_id,
                  const std::string& entity_name,
                  std::int32_t entity_health,
                  oxygine::Point pos);

    void remove();

    void set_pos(const oxygine::Point& pos);
    void set_pos(std::uint32_t index);
    void set_health(std::int32_t health);
    void change_health(std::int32_t amount);

    void change_bitmap(const std::string& key);
    std::string get_bitmap_key();
};

#endif //PIGEONWAR_ENTITY_DRAWER_H
