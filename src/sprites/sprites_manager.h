//
// Created by abrdej on 18.11.17.
//

#ifndef PIGEONWAR_DRAWING_MANAGER_H
#define PIGEONWAR_DRAWING_MANAGER_H

#include "entity_holder.h"
#include "gui/board_panel.h"

class sprites_manager  {
public:
    static std::shared_ptr<entity_holder> drawer_for(std::uint32_t entity_id) {
        return entities.at(entity_id);
    }

//    static std::shared_ptr<entity_holder> entity_at_index(std::uint32_t index) {
//        for (auto&& entity_pack : entities) {
//            if (entity_pack.second->get_board_index() == index) {
//                return entity_pack.second;
//            }
//        }
//        return nullptr;
//    }

    static void update_sprite(std::uint32_t entity_id,
                              const std::string& entity_name,
                              std::int32_t entity_health,
                              std::int32_t entity_power,
                              std::uint32_t index_pos) {

        auto pos = board_index_to_point(index_pos);

        auto it = entities.find(entity_id);
        if (it != std::end(entities)) {
            it->second->set_health(entity_health);
            it->second->set_power(entity_power);
            it->second->set_pos(index_pos);
        } else {
            entities.emplace(entity_id, std::make_shared<entity_holder>(entity_id,
                                                                        entity_name,
                                                                        entity_health,
                                                                        entity_power,
                                                                        pos));
        }
    }

    static void remove_sprite(std::uint32_t entity_id) {
        entities.at(entity_id)->remove();
        entities.erase(entity_id);
    }

    template <typename Drawer>
    static std::shared_ptr<Drawer> typed_drawer_for(std::uint32_t entity_id) {
        return std::static_pointer_cast<Drawer>(entities.at(entity_id));
    }

    static std::string get_bitmap_key_for(std::uint32_t entity_id) {
        return entities.at(entity_id)->get_bitmap_key();
    }
private:
    static std::unordered_map<std::uint32_t, std::shared_ptr<entity_holder>> entities;
};


#endif //PIGEONWAR_DRAWING_MANAGER_H
