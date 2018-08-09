#include <iostream>
#include "entity_holder.h"
#include "gui/view_constants.h"
#include "gui/board_panel.h"
#include "res.h"
#include "drawer_factory.h"

using namespace oxygine;

entity_holder::entity_holder(std::uint32_t entity_id,
                             const std::string& entity_name,
                             std::int32_t entity_health,
                             std::int32_t entity_power,
                             oxygine::Point pos)
        : entity_id(entity_id),
          pos(pos),
          bmt_key(drawer_factory::get_bitmap(entity_name)),
          health(entity_health),
          power(entity_power) {

    sprite = new Sprite;
    sprite->setPosition(pos);
    sprite->setSize(constants::field_size, constants::field_size);
    sprite->attachTo(getStage());

    //ResAnim* resAnim = res::ui.getResAnim(bmt_key);
    //spTween tween = sprite->addTween(TweenAnim(resAnim), 500, -1);
    sprite->setResAnim(res::ui.getResAnim(bmt_key));

    sprite->setTouchEnabled(false);

    health_text = new TextField;
    health_text->setPosition(30, -8);

    if (entity_health != std::numeric_limits<decltype(entity_health)>::max())
        health_text->setText(std::to_string(entity_health));
    else
        health_text->setText("");

    health_text->setHAlign(TextStyle::HorizontalAlign::HALIGN_MIDDLE);
    health_text->setVAlign(TextStyle::VerticalAlign::VALIGN_MIDDLE);
    health_text->setFontSize(22);
    health_text->setColor(Color(32, 32, 32));
    health_text->setFont(res::ui.getResFont("main"));
    health_text->setPriority(1000);
    health_text->setTouchEnabled(false);
    health_text->attachTo(sprite);

    oxygine::RectF bounds;
    health_text->getBounds(bounds);
    bounds.expand(oxygine::Vector2(2, -1), oxygine::Vector2(2, -1));

    health_background = new oxygine::ColorRectSprite;
    health_background->setSize(bounds.getSize());
    health_background->setColor(oxygine::Color::White);
    health_background->setAlpha(150);
    health_background->setPriority(999);
    health_background->setTouchEnabled(false);
    health_background->setPosition(30, -5);
    health_background->setAnchor(0.5, 0.5);
    health_background->attachTo(sprite);

    if (entity_health == std::numeric_limits<decltype(entity_health)>::max()) {
        health_background->setVisible(false);
    }
}

void entity_holder::set_pos(const oxygine::Point& pos) {
    this->pos = pos;
    sprite->setPosition(pos);
}

void entity_holder::set_pos(std::uint32_t index) {
    this->pos = board_index_to_point(index);
    sprite->setPosition(pos);
}

void entity_holder::set_health(std::int32_t health) {
    this->health = health;
    if (health != std::numeric_limits<decltype(health)>::max()) {
        health_text->setVisible(true);
        health_text->setText(std::to_string(health));
        health_background->setVisible(true);
    } else {
        health_text->setVisible(false);
        health_background->setVisible(false);
    }
}

void entity_holder::change_health(std::int32_t amount) {
    this->health += amount;
    if (health != std::numeric_limits<decltype(health)>::max())
        health_text->setText(std::to_string(health));
    else
        health_text->setText("");
}

void entity_holder::set_power(std::int32_t power) {
    this->power = power;
}

void entity_holder::change_power(std::int32_t amount) {
    this->power += amount;
}

void entity_holder::remove() {
    sprite->detach();
}

void entity_holder::change_bitmap(const std::string& key) {
    sprite->setResAnim(res::ui.getResAnim(key));
    bmt_key = key;
}

std::string entity_holder::get_bitmap_key() {
    return bmt_key;
}