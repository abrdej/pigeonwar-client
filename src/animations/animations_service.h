//
// Created by abrdej on 10.01.18.
//

#ifndef PIGEONWAR_ANIMATIONS_SERVICE_H
#define PIGEONWAR_ANIMATIONS_SERVICE_H


#include <memory>
#include <unordered_map>
#include <oxygine-framework.h>
#include "game_state.h"

class game_state;

struct base_handler {
	virtual void handle(nlohmann::json& data, game_state& g_state) = 0;
};

class animations_service {

	std::unordered_map<std::string, std::unique_ptr<base_handler>> services;

	static std::unique_ptr<animations_service> impl;

	oxygine::spTween actual_tween;

	static animations_service& get_instance() {
		if (!impl) {
			impl = std::make_unique<animations_service>();
			impl->initialize();
		}
		return *impl;
	}

	template <typename T>
	void add(const std::string& animation_key) {
		services.emplace(animation_key, std::make_unique<T>());
	}

public:
	static void handle(nlohmann::json& data, game_state& g_state) {
		std::string animation_name = data["animation"][0];
		get_instance().services.at(animation_name)->handle(data["animation"], g_state);
	}
	static void set_tween(oxygine::spTween tween) {
		get_instance().actual_tween = tween;
	}
	static bool is_done() {
		return !get_instance().actual_tween || get_instance().actual_tween->isDone();
	}
	void initialize();
};

#endif //PIGEONWAR_ANIMATIONS_SERVICE_H
