#include "animations_handlers.h"

void magic_bullet_handler::handle(nlohmann::json& data, game_state& g_state) {

	std::uint32_t from_index, to_index;
	extract(data, from_index, to_index);

    auto monk_id = g_state.board.at(from_index);
    auto monk_sprite = sprites_manager::drawer_for(monk_id);

    spTweenQueue bullet_twin = new TweenQueue;

    bullet_twin->add(TweenAnim(res::ui.getResAnim("monk_use_1")), 100);
    bullet_twin->add(TweenAnim(res::ui.getResAnim("monk_use_2")), 100);
    bullet_twin->add(TweenAnim(res::ui.getResAnim("monk_use_3")), 100);
    bullet_twin->add(TweenAnim(res::ui.getResAnim("monk")), 0);

    bullet_twin->setDoneCallback([to_index](Event*) {
        auto bitmap = new oxygine::Sprite;
        bitmap->setPosition(board_index_to_point(to_index));
        bitmap->attachTo(oxygine::getStage());
        bitmap->setTouchEnabled(false);
        auto splash_tween = bitmap->addTween(TweenAnim(res::ui.getResAnim("magic_splash")), 150);
        splash_tween->detachWhenDone();

        animations_service::set_tween(splash_tween);
    });

    monk_sprite->sprite->addTween(bullet_twin);

    animations_service::set_tween(bullet_twin);
}

void teleport_handler::handle(nlohmann::json& data, game_state& g_state) {

	std::uint32_t from_index, to_index;
	extract(data, from_index, to_index);

	auto entity_id = g_state.board.take(from_index);

    auto sprite = sprites_manager::drawer_for(entity_id)->sprite;

    spTweenQueue teleport_twin = new TweenQueue;
    teleport_twin->add(TweenAnim(res::ui.getResAnim("teleport")), 100);
    teleport_twin->add(Actor::TweenPosition(board_index_to_point(to_index)), 0);
    teleport_twin->add(TweenAnim(res::ui.getResAnim("teleport")), 150);
    teleport_twin->add(TweenAnim(res::ui.getResAnim("droid")), 0);

    sprite->addTween(teleport_twin);
    animations_service::set_tween(teleport_twin);

	g_state.board.give_back(entity_id, to_index);
}

void drain_handler::handle(nlohmann::json& data, game_state& g_state) {

    extract(data, from_index, to_index);

    auto from_pos = board_index_to_point(from_index);
    auto to_pos = board_index_to_point(to_index);

    auto distance = std::sqrt(std::pow(from_pos.x - to_pos.x, 2) + std::pow(from_pos.y - to_pos.y, 2));

    auto entity_id = g_state.board.at(from_index);

    auto sprite = sprites_manager::drawer_for(entity_id)->sprite;
    sprite->setPriority(100);
    sprite->setFlippedX(from_pos.x - to_pos.x > 0);


    auto bitmap = sprite->getResAnim();

    spTweenQueue drain_tween = new TweenQueue;
    drain_tween->add(Actor::TweenPosition(to_pos),  distance * 5);

    if (!get_flush_bitmap().empty()) {
        drain_tween->add(TweenAnim(res::ui.getResAnim(get_flush_bitmap())), 150);
        drain_tween->add(TweenAnim(bitmap), 1);
    }

    drain_tween->add(Actor::TweenPosition(from_pos),  distance * 5);

    sprite->addTween(drain_tween)->setDoneCallback([&, sprite, entity_id, this](Event* ev) {
        sprite->setPriority(0);
        g_state.board.give_back(entity_id, from_index);

        auto pos = board_index_to_point(from_index);
        pos.x += 30;
        pos.y += 30;

        auto healing = new oxygine::Sprite;
        healing->setAnchor(0.5, 0.5);
        healing->setPosition(pos);
        healing->attachTo(oxygine::getStage());
        healing->setResAnim(res::ui.getResAnim("healthing"));
        healing->setTouchEnabled(false);
        healing->setVisible(true);
        healing->setPriority(100);

        auto anim = healing->addTween(TweenDummy(), 200);
        anim->detachWhenDone();

        animations_service::set_tween(anim);
    });

    animations_service::set_tween(drain_tween);
}

//void grenade_handler::handle(nlohmann::json& data, game_state& g_state) {
//
////	shot_base_handler::handle(data, g_state);
////
////	auto from_cr = board_container::to_pos(to_index);
////	from_cr.first -= 1;
////	from_cr.second -= 1;
////
////	animation::player<animation::flash_bitmap>::launch(animation::flash_bitmap(board_container::to_index(from_cr.first, from_cr.second),
////																			   std::chrono::milliseconds(150),
////																			   bitmap_key::detonation_anim));
//}

void giant_ram_handler::handle(nlohmann::json& data, game_state& g_state) {

	std::uint32_t from_index, to_index;
	extract(data, from_index, to_index);

    auto from_pos = board_index_to_point(from_index);
    auto to_pos = board_index_to_point(to_index);

    auto distance = std::sqrt(std::pow(from_pos.x - to_pos.x, 2) + std::pow(from_pos.y - to_pos.y, 2));

	auto entity_id = g_state.board.take(from_index);

    auto sprite = sprites_manager::drawer_for(entity_id)->sprite;

    sprite->setPriority(100);
    sprite->addTween(TweenAnim(res::ui.getResAnim("giant_ram")), distance * 3);

    auto move_tween = sprite->addTween(Actor::TweenPosition(to_pos), distance * 3);
    move_tween->setDoneCallback([sprite](Event* ev) {
        sprite->setPriority(0);
        sprite->setResAnim(res::ui.getResAnim("giant"));
    });

    g_state.board.give_back(entity_id, to_index);

    animations_service::set_tween(move_tween);
}

void kill_handler::handle(nlohmann::json& data, game_state& g_state) {
//	std::uint32_t from_index, to_index, enemy_index;
//	extract(data, from_index, to_index, enemy_index);
//
//	auto entity_id = g_state.board.take(from_index);
//
//	animation::player<animation::flash_bitmap>::launch(animation::flash_bitmap(to_index,
//																			   std::chrono::milliseconds(150),
//																			   bitmap_key::killer_jump));
//	animation::base_player::play();
//
//	animation::player<animation::move>::launch(animation::move(to_index, enemy_index, entity_id));
//	animation::base_player::play();
//
//	animation::player<animation::flash_bitmap>::launch(animation::flash_bitmap(enemy_index,
//																			   std::chrono::milliseconds(150),
//																			   bitmap_key::killer_attack));
//
//	animation::player<animation::move>::launch(animation::move(enemy_index, to_index, entity_id));
//	animation::base_player::play();
//
//	g_state.board.give_back(entity_id, to_index);
}

auto make_shoot_animation(std::uint32_t from_index,
                          std::uint32_t to_index,
                          const std::string& bullet_bitmap,
                          const std::string& explosion_bitmap = "") {

    auto from_pos = board_index_to_point(from_index);
    auto to_pos = board_index_to_point(to_index);
    from_pos.x += 30;
    from_pos.y += 30;
    to_pos.x += 30;
    to_pos.y += 30;

    auto distance = std::sqrt(std::pow(from_pos.x - to_pos.x, 2) + std::pow(from_pos.y - to_pos.y, 2));

    auto bullet = new oxygine::Sprite;
    bullet->setAnchor(0.5, 0.5);
    bullet->setPosition(from_pos);
    bullet->attachTo(oxygine::getStage());
    bullet->setResAnim(res::ui.getResAnim(bullet_bitmap));
    bullet->setTouchEnabled(false);

    spTweenQueue bullet_twin = new TweenQueue;
    bullet_twin->add(Actor::TweenPosition(to_pos),  distance * 2);
    if (!explosion_bitmap.empty()) {
        bullet_twin->add(TweenAnim(res::ui.getResAnim(explosion_bitmap)), 250);
    }
    bullet_twin->detachWhenDone();
    return std::make_pair(bullet, bullet_twin);
}

void spider_web_handler::handle(nlohmann::json& data, game_state& g_state) {

    std::uint32_t land_index;
    extract(data, from_index, to_index, land_index);

    auto enemy_id = g_state.board.at(to_index);

    auto shoot_animation_pack = make_shoot_animation(from_index, to_index, "spider_web");

    shoot_animation_pack.second->add(TweenDummy(), 1)->addDoneCallback([this, &g_state, enemy_id](Event*) {
        g_state.board.take(to_index);

        sprites_manager::drawer_for(enemy_id)->sprite->setVisible(false);
        getStage()->update();
    });

    if (to_index != land_index) {

        auto from_pos = board_index_to_point(from_index);
        auto to_pos = board_index_to_point(to_index);
        from_pos.x += 30;
        from_pos.y += 30;
        to_pos.x += 30;
        to_pos.y += 30;

        auto distance = std::sqrt(std::pow(from_pos.x - to_pos.x, 2) + std::pow(from_pos.y - to_pos.y, 2));

        shoot_animation_pack.second->add(Actor::TweenPosition(from_pos),  distance * 2);
	}

    shoot_animation_pack.second->addDoneCallback([this, enemy_id, land_index, &g_state](Event*) {
        g_state.board.give_back(enemy_id, land_index);

        sprites_manager::drawer_for(enemy_id)->set_pos(land_index);
        sprites_manager::drawer_for(enemy_id)->sprite->setVisible(true);
    });

    shoot_animation_pack.first->addTween(shoot_animation_pack.second);

    animations_service::set_tween(shoot_animation_pack.second);
}

void spiral_of_fire_handler::handle(nlohmann::json& data, game_state& g_state) {

//	std::uint32_t from_index, to_index;
//	extract(data, from_index, to_index);
//
//	auto entity_id = g_state.board.take(from_index);
//
//	sprites_manager::typed_drawer_for<entity_drawer>(entity_id)->change_bitmap(bitmap_key::spiral_of_fire);
//
//	animation::player<animation::move>::launch(animation::move(from_index, to_index, entity_id));
//	animation::base_player::play();
//
//	animation::player<animation::move>::launch(animation::move(to_index, from_index, entity_id));
//	animation::base_player::play();
//
//	sprites_manager::typed_drawer_for<entity_drawer>(entity_id)->change_bitmap(bitmap_key::creature);
//
//	g_state.board.give_back(entity_id, from_index);
}

void bludgeon_push_handler::handle(nlohmann::json& data, game_state& g_state) {

	std::uint32_t from_index, to_index, push_index;
	extract(data, from_index, to_index, push_index);

	auto caster_id = g_state.board.take(from_index);

    auto from_pos = board_index_to_point(from_index);
    auto to_pos = board_index_to_point(to_index);

    auto distance = std::sqrt(std::pow(from_pos.x - to_pos.x, 2) + std::pow(from_pos.y - to_pos.y, 2));

    auto caster = sprites_manager::drawer_for(caster_id)->sprite;
    caster->setPriority(100);
    caster->setFlippedX(from_pos.x - to_pos.x > 0);

    auto move_tween = caster->addTween(Actor::TweenPosition(to_pos), distance * 3);

    animations_service::set_tween(move_tween);

    move_tween->setDoneCallback([=, &g_state](Event* ev) mutable {
        caster->setPriority(0);

        g_state.board.give_back(caster_id, to_index);
        auto enemy_id = g_state.board.take_bottom(to_index);

        auto explosion_pos = to_pos;
        explosion_pos.x += 30;
        explosion_pos.y += 30;

        auto bitmap = new oxygine::Sprite;
        bitmap->setAnchor(0.5, 0.5);
        bitmap->setPosition(explosion_pos);
        bitmap->attachTo(oxygine::getStage());
        bitmap->setResAnim(res::ui.getResAnim("bum"));
        bitmap->setTouchEnabled(false);
        bitmap->setPriority(100);

        auto explosion_tween = bitmap->addTween(TweenDummy(), 150);

        animations_service::set_tween(explosion_tween);

        explosion_tween->setDoneCallback([=, &g_state](Event* ev) mutable {

            auto push_pos = board_index_to_point(push_index);

            auto push_distance = std::sqrt(std::pow(to_pos.x - push_pos.x, 2) + std::pow(to_pos.y - push_pos.y, 2));

            auto enemy = sprites_manager::drawer_for(enemy_id)->sprite;
            enemy->setPriority(100);
            enemy->setFlippedX(to_pos.x - push_pos.x > 0);

            auto push_tween = enemy->addTween(Actor::TweenPosition(push_pos), push_distance * 3);

            animations_service::set_tween(push_tween);

            push_tween->setDoneCallback([=, &g_state](Event* ev) mutable {
                g_state.board.give_back(enemy_id, push_index);
                enemy->setPriority(0);
            });
        });

        explosion_tween->detachWhenDone();
    });
}

void change_health_handler::handle(nlohmann::json& data, game_state& g_state) {

	std::int32_t to_index;
	std::int32_t change_health;

	extract(data, to_index, change_health);

	auto entity_id = g_state.board.at(to_index);

	oxygine::Color color = change_health < 0 ? oxygine::Color(210, 20, 15) : oxygine::Color(15, 210, 20);
	color = (change_health == 0) ? oxygine::Color(15, 20, 210) : color;

	auto health = new oxygine::TextField;
	health->setPosition(30, -15);
	health->attachTo(sprites_manager::drawer_for(entity_id)->sprite);
	health->setHAlign(TextStyle::HorizontalAlign::HALIGN_MIDDLE);
	health->setText(std::to_string(change_health));
    health->setFontSize(32);
    health->setFont(res::ui.getResFont("main"));
    health->setColor(color);
    health->setPriority(1500);
	health->setTouchEnabled(false);

	auto tween = health->addTween(Actor::TweenPosition(30, -40), 300);
    tween->setDoneCallback([entity_id, change_health](Event* ev) {
        sprites_manager::drawer_for(entity_id)->change_health(change_health);
    });
    tween->detachWhenDone();

    animations_service::set_tween(tween);
}

void change_power_handler::handle(nlohmann::json& data, game_state& g_state) {

	std::int32_t to_index;
	std::int32_t change_power;

	extract(data, to_index, change_power);

    auto entity_id = g_state.board.at(to_index);

    //oxygine::Color color = change_power < 0 ? oxygine::Color(210, 20, 15) : oxygine::Color(15, 210, 20);
    //color = (change_power == 0) ? oxygine::Color(15, 20, 210) : color;

    oxygine::Color color(25, 25, 200);

    auto health = new oxygine::TextField;
    health->setPosition(30, -15);
    health->attachTo(sprites_manager::drawer_for(entity_id)->sprite);
    health->setHAlign(TextStyle::HorizontalAlign::HALIGN_MIDDLE);
    health->setText(std::to_string(change_power));
    health->setFontSize(32);
    health->setFont(res::ui.getResFont("main"));
    health->setColor(color);
    health->setPriority(1500);
    health->setTouchEnabled(false);

    auto tween = health->addTween(Actor::TweenPosition(30, -40), 300);
    tween->setDoneCallback([entity_id, change_power](Event* ev) {
        //sprites_manager::drawer_for(entity_id)->change_health(change_power);
    });
    tween->detachWhenDone();

    animations_service::set_tween(tween);
}

void portal_handler::handle(nlohmann::json& data, game_state& g_state) {

	std::uint32_t from_index, to_index;
    std::vector<std::pair<std::uint32_t, std::uint32_t>> neighboring_moves;

	extract(data, from_index, to_index, neighboring_moves);

    auto entity_id = g_state.board.at(from_index);

    std::vector<std::pair<std::uint32_t, std::uint32_t>> taken;

    for (auto&& move_pack : neighboring_moves) {
        auto neighboring_id = g_state.board.at(move_pack.first);
        taken.emplace_back(neighboring_id, move_pack.second);
    };

    auto from_pos = board_index_to_point(from_index);
    from_pos.x += 30;
    from_pos.y += 30;

    auto to_pos = board_index_to_point(to_index);
    to_pos.x += 30;
    to_pos.y += 30;

    auto bitmap = new oxygine::Sprite;
    bitmap->setAnchor(0.5, 0.5);
    bitmap->setPosition(from_pos);
    bitmap->attachTo(oxygine::getStage());
    bitmap->setTouchEnabled(false);

    spTweenQueue portal_twin = new TweenQueue;

    auto tween_1 = portal_twin->add(TweenAnim(res::ui.getResAnim("portal_1")), 100);
    portal_twin->add(TweenAnim(res::ui.getResAnim("portal_2")), 100);
    auto tween_3 = portal_twin->add(TweenAnim(res::ui.getResAnim("portal_3")), 100);
    tween_3->setDoneCallback([bitmap, to_pos, &g_state, from_index, entity_id, neighboring_moves, taken](Event*) {

        g_state.board.take(from_index);
        for (auto&& move_pack : neighboring_moves) {
            g_state.board.take(move_pack.first);
        }

        sprites_manager::drawer_for(entity_id)->sprite->setVisible(false);
        for (auto&& taken_pack : taken) {
            sprites_manager::drawer_for(taken_pack.first)->sprite->setVisible(false);
        }

        bitmap->setPosition(to_pos);
    });

    portal_twin->add(TweenAnim(res::ui.getResAnim("portal_3")), 100);
    portal_twin->add(TweenAnim(res::ui.getResAnim("portal_2")), 100);
    auto tween_land = portal_twin->add(TweenAnim(res::ui.getResAnim("portal_1")), 100);
    tween_land->setDoneCallback([&g_state, entity_id, to_index, taken](Event*) {
        sprites_manager::drawer_for(entity_id)->sprite->setVisible(true);
        sprites_manager::drawer_for(entity_id)->set_pos(to_index);
        for (auto&& taken_pack : taken) {
            sprites_manager::drawer_for(taken_pack.first)->sprite->setVisible(true);
            sprites_manager::drawer_for(taken_pack.first)->set_pos(taken_pack.second);
        }

        g_state.board.give_back(entity_id, to_index);

        for (auto&& taken_pack : taken) {
            g_state.board.give_back(taken_pack.first, taken_pack.second);
        }
    });


    portal_twin->detachWhenDone();

    bitmap->addTween(portal_twin);

    animations_service::set_tween(portal_twin);
}

void destruction_handler::handle(nlohmann::json& data, game_state& g_state) {

    std::uint32_t index;
    extract(data, index);

    auto entity_id = g_state.board.at(index);
    auto sprite = sprites_manager::drawer_for(entity_id)->sprite;

    auto pos = board_index_to_point(index);
    pos.x += 30;
    pos.y += 30;

    auto bitmap = new oxygine::Sprite;
    bitmap->setAnchor(0.5, 0.5);
    bitmap->setPosition(pos);
    bitmap->attachTo(oxygine::getStage());
    bitmap->setResAnim(res::ui.getResAnim("destruction_1"));
    bitmap->setTouchEnabled(false);
    bitmap->setVisible(true);
    bitmap->setPriority(100);

    auto destruction_tween = new TweenQueue;

    destruction_tween->add(TweenAnim(res::ui.getResAnim("destruction_1")), 75);
    destruction_tween->add(TweenAnim(res::ui.getResAnim("destruction_2")), 75);
    destruction_tween->add(TweenAnim(res::ui.getResAnim("destruction_3")), 75);

    bitmap->addTween(destruction_tween);

    destruction_tween->detachWhenDone();

    animations_service::set_tween(destruction_tween);
}

void fist_of_doom_handler::handle(nlohmann::json& data, game_state& g_state) {

    std::uint32_t to_index;
    extract(data, to_index);

    auto to_pos = board_index_to_point(to_index);
    to_pos.x += 30;
    to_pos.y += 30;

    auto from_pos = to_pos;
    from_pos.y = 0;

    auto bullet = new oxygine::Sprite;
    bullet->setAnchor(0.5, 0.5);
    bullet->setPosition(from_pos);
    bullet->attachTo(oxygine::getStage());
    bullet->setResAnim(res::ui.getResAnim("meteorite"));
    bullet->setTouchEnabled(false);

    auto distance = std::sqrt(std::pow(from_pos.x - to_pos.x, 2) + std::pow(from_pos.y - to_pos.y, 2));

    spTweenQueue bullet_twin = new TweenQueue;
    bullet_twin->add(Actor::TweenPosition(to_pos),  distance * 2);
    bullet_twin->add(TweenAnim(res::ui.getResAnim("fist_of_doom_ex")), 100);
    bullet_twin->add(TweenAnim(res::ui.getResAnim("fist_of_doom_ex2")), 100);
    bullet_twin->detachWhenDone();
    bullet->addTween(bullet_twin);

    animations_service::set_tween(bullet_twin);
}

void meteorite_handler::handle(nlohmann::json& data, game_state& g_state) {

	std::uint32_t to_index;
	extract(data, to_index);

    auto to_pos = board_index_to_point(to_index);
    to_pos.x += 30;
    to_pos.y += 30;

    auto from_pos = to_pos;
    from_pos.y = 0;

    auto bullet = new oxygine::Sprite;
    bullet->setAnchor(0.5, 0.5);
    bullet->setPosition(from_pos);
    bullet->attachTo(oxygine::getStage());
    bullet->setResAnim(res::ui.getResAnim("meteorite"));
    bullet->setTouchEnabled(false);

    auto distance = std::sqrt(std::pow(from_pos.x - to_pos.x, 2) + std::pow(from_pos.y - to_pos.y, 2));

    spTweenQueue bullet_twin = new TweenQueue;
    bullet_twin->add(Actor::TweenPosition(to_pos),  distance * 2);
    bullet_twin->add(TweenAnim(res::ui.getResAnim("meteorite_explosion")), 250);
    bullet_twin->detachWhenDone();
    bullet->addTween(bullet_twin);

    animations_service::set_tween(bullet_twin);
}

void transmission_handler::handle(nlohmann::json& data, game_state& g_state) {

    std::uint32_t from_index, to_index;
    extract(data, from_index, to_index);

    auto entity_id = g_state.board.at(from_index);
    auto entity_holder = sprites_manager::drawer_for(entity_id);

    auto transmission_tween = entity_holder->sprite->addTween(Actor::TweenAlpha(0), 250);
    transmission_tween->setDoneCallback(
            [&g_state, entity_holder, from_index, to_index, transmission_tween](Event*) mutable {
        entity_holder->set_pos(to_index);
        g_state.board.move(from_index, to_index);
        transmission_tween = entity_holder->sprite->addTween(Actor::TweenAlpha(255), 250);
    });

    animations_service::set_tween(transmission_tween);
}