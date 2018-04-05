//
// Created by abrdej on 10.01.18.
//

#ifndef PIGEONWAR_ANIMATIONS_HANDLERS_H
#define PIGEONWAR_ANIMATIONS_HANDLERS_H

#include <iostream>
#include "animations_service.h"
#include "gui/board_panel.h"
#include "oxygine-framework.h"
#include "res.h"
#include "sprites/sprites_manager.h"

template <typename... DataTypes>
void extract(nlohmann::json& data, DataTypes&... x) {

	std::size_t index = 1;

	auto extractor = [&data, &index](auto& y) {
			y = data[index++].get<std::decay_t<decltype(y)>>();
	};
	std::int32_t tab[] = {(extractor(x), 0)...};
}

template <typename DataType>
void extract_from_index(nlohmann::json& data, DataType& x, std::size_t i) {
    x = data[i].get<std::decay_t<decltype(x)>>();
}

#define BULLET_BITMAP(str) \
    std::string get_bullet_bitmap() const override { \
        return str; \
    } \

#define EXPLOSION_BITMAP(str) \
    std::string get_explosion_bitmap() const override { \
        return str; \
    } \


struct shoot_base_handler : base_handler {
	std::uint32_t from_index, to_index;

	virtual std::string get_bullet_bitmap() const = 0;
	virtual std::string get_explosion_bitmap() const {
        return "";
    }

	void handle(nlohmann::json& data, game_state& g_state) override {

		extract(data, from_index, to_index);

		auto from_pos = board_index_to_point(from_index);
		auto to_pos = board_index_to_point(to_index);
        from_pos.x += 30;
        from_pos.y += 30;
        to_pos.x += 30;
        to_pos.y += 30;

		auto bullet = new oxygine::Sprite;
        bullet->setAnchor(0.5, 0.5);
		bullet->setPosition(from_pos);
		bullet->attachTo(oxygine::getStage());
		bullet->setResAnim(res::ui.getResAnim(get_bullet_bitmap()));
		bullet->setTouchEnabled(false);
        bullet->setFlippedX(from_pos.x - to_pos.x > 0);

		auto distance = std::sqrt(std::pow(from_pos.x - to_pos.x, 2) + std::pow(from_pos.y - to_pos.y, 2));

        spTweenQueue bullet_twin = new TweenQueue;
        bullet_twin->add(Actor::TweenPosition(to_pos),  distance * 2.2);
		if (!get_explosion_bitmap().empty()) {
			bullet_twin->add(TweenAnim(res::ui.getResAnim(get_explosion_bitmap())), 200);
		}
		bullet_twin->detachWhenDone();
        bullet->addTween(bullet_twin);

		animations_service::set_tween(bullet_twin);
	}
};

struct move_base_handler : base_handler {
	std::uint32_t from_index, to_index;

	void handle(nlohmann::json& data, game_state& g_state) override {
		extract(data, from_index, to_index);

        auto from_pos = board_index_to_point(from_index);
        auto to_pos = board_index_to_point(to_index);

		auto distance = std::sqrt(std::pow(from_pos.x - to_pos.x, 2) + std::pow(from_pos.y - to_pos.y, 2));

		auto entity_id = g_state.board.take(from_index);

        auto sprite = sprites_manager::drawer_for(entity_id)->sprite;
        sprite->setPriority(100);

		sprite->setFlippedX(from_pos.x - to_pos.x > 0);

		auto move_tween = sprite->addTween(Actor::TweenPosition(to_pos), distance * 3);
		move_tween->setDoneCallback([sprite](Event* ev) {
            sprite->setPriority(0);
        });

		g_state.board.give_back(entity_id, to_index);

		animations_service::set_tween(move_tween);
	}
};

#define FLUSH_BITMAP(str) \
    std::string get_flush_bitmap() const override { \
        return str; \
    } \

#define MOVE_BITMAP(str) \
    std::string get_move_bitmap() const override { \
        return str; \
    } \

template <bool index_to_return = false>
struct move_with_return_base_handler : base_handler {
	std::uint32_t from_index, to_index;

	virtual std::string get_flush_bitmap() const {
		return "";
	}

    virtual std::string get_move_bitmap() const {
        return "";
    }

	void handle(nlohmann::json& data, game_state& g_state) override {
		extract(data, from_index, to_index);

		auto from_pos = board_index_to_point(from_index);
		auto to_pos = board_index_to_point(to_index);

		auto distance = std::sqrt(std::pow(from_pos.x - to_pos.x, 2) + std::pow(from_pos.y - to_pos.y, 2));

		auto entity_id = g_state.board.at(from_index);

		auto sprite = sprites_manager::drawer_for(entity_id)->sprite;
        sprite->setPriority(100);
        sprite->setFlippedX(from_pos.x - to_pos.x > 0);

		auto bitmap = sprite->getResAnim();

		spTweenQueue move_twin = new TweenQueue;

		if (!get_move_bitmap().empty()) {
            sprite->setResAnim(res::ui.getResAnim(get_move_bitmap()));
		}

		move_twin->add(Actor::TweenPosition(to_pos),  distance * 5);


		if (!get_flush_bitmap().empty()) {
			move_twin->add(TweenAnim(res::ui.getResAnim(get_flush_bitmap())), 150);
            if (!get_move_bitmap().empty()) {
                move_twin->add(TweenAnim(res::ui.getResAnim(get_move_bitmap())), 1);
            } else {
                move_twin->add(TweenAnim(bitmap), 1);
            }
		}

		if (index_to_return) {
            extract_from_index(data, from_index, 3);
			from_pos = board_index_to_point(from_index);
		}

		move_twin->add(Actor::TweenPosition(from_pos),  distance * 5);

        sprite->addTween(move_twin)->setDoneCallback([this, sprite, bitmap](Event* ev) {
            sprite->setPriority(0);

            if (!get_move_bitmap().empty()) {
                sprite->setResAnim(bitmap);
            }
        });

		g_state.board.give_back(entity_id, from_index);

		animations_service::set_tween(move_twin);
	}
};

template <bool hide_index = false, long time = 150>
struct bitmap_flush_base_handler : base_handler {
	std::uint32_t index;

	virtual std::string get_flush_bitmap() const {
		return "";
	}

	void handle(nlohmann::json& data, game_state& g_state) override {
		extract(data, index);

		std::uint32_t entity_id{0};
        oxygine::spSprite sprite;

        if (hide_index) {
            entity_id = g_state.board.at(index);
            sprite = sprites_manager::drawer_for(entity_id)->sprite;
        }

		auto pos = board_index_to_point(index);
		pos.x += 30;
		pos.y += 30;

		auto bitmap = new oxygine::Sprite;
		bitmap->setAnchor(0.5, 0.5);
		bitmap->setPosition(pos);
		bitmap->attachTo(oxygine::getStage());
        bitmap->setResAnim(res::ui.getResAnim(get_flush_bitmap()));
		bitmap->setTouchEnabled(false);
        bitmap->setVisible(true);
        bitmap->setPriority(100);
		bitmap->setSize(res::ui.getResAnim(get_flush_bitmap())->getSize());

		if (hide_index) {
			sprite->setVisible(false);
		}

		auto anim = bitmap->addTween(TweenDummy(), time);

		if (hide_index) {
			anim->setDoneCallback([sprite](Event* ev) {
				sprite->setVisible(true);
			});
		}

		anim->detachWhenDone();

		animations_service::set_tween(anim);
	}
};

#define CHANGE_BITMAP(str) \
    std::string get_bitmap() const override { \
        return str; \
    } \

struct change_bitmap_base_handler : base_handler {
	std::uint32_t entity_id;

	virtual std::string get_bitmap() const = 0;

	void handle(nlohmann::json& data, game_state& g_state) override {
		extract(data, entity_id);
		sprites_manager::drawer_for(entity_id)->change_bitmap(get_bitmap());
	}
};

struct shoot_handler : shoot_base_handler {
	BULLET_BITMAP("bullet")
	EXPLOSION_BITMAP("bum")
};

struct blow_the_ax_handler : move_with_return_base_handler<> {

};

struct dodge_handler : bitmap_flush_base_handler<true> {
	FLUSH_BITMAP("dodge")
};

struct magic_bullet_handler : base_handler {
	void handle(nlohmann::json& data, game_state& g_state) override;
};

struct magic_suck_handler : bitmap_flush_base_handler<> {
	FLUSH_BITMAP("magic_suck")
};

struct sniper_shot_handler : shoot_base_handler {
	BULLET_BITMAP("sniper_bullet")
	EXPLOSION_BITMAP("sniper_bullet_explosion")
};

struct teleport_handler : base_handler {
	void handle(nlohmann::json& data, game_state& g_state) override;
};

struct poisoned_missile_handler : shoot_base_handler {
	BULLET_BITMAP("poisoned_missile")
	EXPLOSION_BITMAP("poisoned_missile_explosion")
};

struct laser_handler : shoot_base_handler {
	BULLET_BITMAP("laser")
};

struct drain_handler : move_with_return_base_handler<> {
	void handle(nlohmann::json& data, game_state& g_state) override;
	FLUSH_BITMAP("native_attack")
};

struct grenade_handler : shoot_base_handler {
	BULLET_BITMAP("grenade")
	EXPLOSION_BITMAP("detonation")
};

struct power_bullet_handler : shoot_base_handler {
	BULLET_BITMAP("power_bullet")
	EXPLOSION_BITMAP("power_bullet_bum")
};

struct protection_field_handler : shoot_base_handler {
	BULLET_BITMAP("protection_field")
	EXPLOSION_BITMAP("field_shoot")
};

struct sabers_handler : move_with_return_base_handler<> {
	FLUSH_BITMAP("sabers_attack")
};

struct move_handler : move_base_handler {

};

struct aura_of_immunity_handler : bitmap_flush_base_handler<> {
	FLUSH_BITMAP("defender")
};

struct aura_of_immunity_break_handler : bitmap_flush_base_handler<> {
	FLUSH_BITMAP("v")
};

struct basic_melee_attack_handler : move_with_return_base_handler<> {
	FLUSH_BITMAP("claws")
};

struct chopper_handler : move_with_return_base_handler<> {
	FLUSH_BITMAP("creature_fired")
};

struct counterattack_handler : move_with_return_base_handler<> {
	FLUSH_BITMAP("native_attack")
};

struct flame_thrower_handler : shoot_base_handler {
	BULLET_BITMAP("flame_thrower")
    EXPLOSION_BITMAP("detonation")
};

struct giant_blow_handler : move_with_return_base_handler<> {
	FLUSH_BITMAP("giant_do_blow")
};

struct giant_ram_handler : base_handler {
	void handle(nlohmann::json& data, game_state& g_state) override;
};


struct handcannon_handler : shoot_base_handler {
	BULLET_BITMAP("handcannon")
	EXPLOSION_BITMAP("handcannon_explosion")
};

struct hypnosis_handler : bitmap_flush_base_handler<> {
	FLUSH_BITMAP("hypnosis")
};

struct jaw_spider_handler : bitmap_flush_base_handler<> {
	FLUSH_BITMAP("jaw_spider")
};

struct kill_handler : move_with_return_base_handler<> {
	void handle(nlohmann::json& data, game_state& g_state) override;
	FLUSH_BITMAP("killer_attack")
};

struct set_killer_instinct_handler : change_bitmap_base_handler {
	CHANGE_BITMAP("killer_jump")
};

struct remove_killer_instinct_handler : change_bitmap_base_handler {
	CHANGE_BITMAP("killer")
};

struct prison_connection_handler : bitmap_flush_base_handler<> {
	FLUSH_BITMAP("protection_field")
};

struct spear_handler : move_with_return_base_handler<> {
	FLUSH_BITMAP("guardian_attack")
};

struct spider_web_handler : shoot_base_handler {
	void handle(nlohmann::json& data, game_state& g_state) override;
	BULLET_BITMAP("spider_web")
};

struct spiral_of_fire_handler : base_handler {
	void handle(nlohmann::json& data, game_state& g_state) override;
};

struct sword_blow_handler : bitmap_flush_base_handler<true> {
    FLUSH_BITMAP("samurai_sword_blow")
};

struct tongue_of_fire_handler : shoot_base_handler {
	BULLET_BITMAP("tongue_of_fire")
};

struct vicious_circle_handler : bitmap_flush_base_handler<false, 200> {
	FLUSH_BITMAP("magic_energy")
};

struct warrior_blow_handler : move_with_return_base_handler<> {
	FLUSH_BITMAP("warrior_attack")
};

struct set_immortality_handler : change_bitmap_base_handler {
	CHANGE_BITMAP("warrior_immortal")
};

struct bludgeon_push_handler : base_handler {
	void handle(nlohmann::json& data, game_state& g_state) override;
};

struct bludgeon_handler : move_with_return_base_handler<> {
	FLUSH_BITMAP("bum")
};

struct rage_handler : bitmap_flush_base_handler<true> {
    FLUSH_BITMAP("rage")
};

struct shiruken_handler : shoot_base_handler {
	BULLET_BITMAP("shiruken")
	EXPLOSION_BITMAP("shiruken_splash")
};

struct death_mark_handler : bitmap_flush_base_handler<> {
	FLUSH_BITMAP("assassin_slash")
};

struct set_invisibility_handler : change_bitmap_base_handler {
	CHANGE_BITMAP("saberhand_transparency")
};

struct remove_invisibility_handler : change_bitmap_base_handler {
	CHANGE_BITMAP("saberhand")
};

struct poison_handler : bitmap_flush_base_handler<> {
	FLUSH_BITMAP("poison_effect")
};

struct start_sorcerer_attack_handler : change_bitmap_base_handler {
	CHANGE_BITMAP("sorcerer_attack")
};

struct end_sorcerer_attack_handler : change_bitmap_base_handler {
	CHANGE_BITMAP("sorcerer")
};

struct change_health_handler : base_handler {
	void handle(nlohmann::json& data, game_state& g_state) override;
};

struct change_power_handler : base_handler {
	void handle(nlohmann::json& data, game_state& g_state) override;
};

struct rocket_launcher_handler : shoot_base_handler {
	BULLET_BITMAP("rocket")
	EXPLOSION_BITMAP("bum")
};

struct portal_handler : base_handler {
	void handle(nlohmann::json& data, game_state& g_state) override;
};

struct cure_handler : bitmap_flush_base_handler<> {
	FLUSH_BITMAP("cure_anim")
};

struct lightning_prepare_handler : bitmap_flush_base_handler<> {
	FLUSH_BITMAP("lightning_prepare")
};

struct lightning_handler : bitmap_flush_base_handler<> {
	FLUSH_BITMAP("lightning")
};

struct soul_out_handler : bitmap_flush_base_handler<> {
	FLUSH_BITMAP("soul")
};

struct saw_passing_handler : move_base_handler {
};

struct arrow_handler : shoot_base_handler {
	BULLET_BITMAP("arrow_anim")
};

struct destruction_handler : base_handler {
	void handle(nlohmann::json& data, game_state& g_state) override;
};

struct fist_of_doom_handler : base_handler {
	void handle(nlohmann::json& data, game_state& g_state) override;
};

struct uselessness_handler : bitmap_flush_base_handler<> {
	FLUSH_BITMAP("uselessness")
};

struct meteorite_before_handler : bitmap_flush_base_handler<> {
	FLUSH_BITMAP("uselessness")
};

struct meteorite_handler : base_handler {
	void handle(nlohmann::json& data, game_state& g_state) override;
};

struct purification_handler : bitmap_flush_base_handler<> {
	FLUSH_BITMAP("purification")
};

struct flame_burning_handler : shoot_base_handler {
	BULLET_BITMAP("power_bullet")
	EXPLOSION_BITMAP("power_bullet_bum")
};

struct ball_and_chain_handler : bitmap_flush_base_handler<false, 200> {
	FLUSH_BITMAP("ball_and_chain")
};

struct charge_handler : move_with_return_base_handler<true> {
    MOVE_BITMAP("move_charge")
    FLUSH_BITMAP("reaper_attack")
};

struct set_insensitivity_handler : change_bitmap_base_handler {
	CHANGE_BITMAP("reaper_insensitive")
};

struct set_discharged_handler : change_bitmap_base_handler {
	CHANGE_BITMAP("reaper_discharged")
};

struct reaper_no_effect_handler : change_bitmap_base_handler {
	CHANGE_BITMAP("reaper")
};

struct detonation_handler : bitmap_flush_base_handler<> {
    FLUSH_BITMAP("detonation");
};

struct repair_handler : bitmap_flush_base_handler<> {
    FLUSH_BITMAP("repair");
};

struct transmission_handler : base_handler {
	void handle(nlohmann::json& data, game_state& g_state) override;
};

#endif //PIGEONWAR_ANIMATIONS_HANDLERS_H
