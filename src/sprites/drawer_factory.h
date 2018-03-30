//
// Created by abrdej on 18.01.18.
//

#ifndef PIGEONWAR_DRAWER_FACOTRY_H
#define PIGEONWAR_DRAWER_FACOTRY_H

#include "entity_holder.h"

struct drawer_factory {
	static std::shared_ptr<entity_holder> create(std::uint32_t entity_id, const std::string& entity_name);
	static std::string get_bitmap(const std::string& entity_name);
};


#endif //PIGEONWAR_DRAWER_FACOTRY_H
