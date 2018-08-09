#ifndef BUTTONS_PANEL_H
#define BUTTONS_PANEL_H

#include <array>
#include <res.h>
#include "oxygine-framework.h"

class buttons_panel final
{
public:
	void init(std::uint32_t cols, std::uint32_t rows);
    void set_for_entity_for(std::uint32_t entity_id,
                            const std::string& entity_name,
                            const std::string& entity_health,
                            const std::string& entity_power,
							const std::array<std::string, 5>& button_bitmaps);

	void set_on_button(const std::function<void(std::uint32_t)>& fn) {
		on_button_fn = fn;
	}

	void set_get_button_description(const std::function<void(std::uint32_t)>& fn) {
		on_get_desc_fn = fn;
	}
	void set_on_entity(const std::function<void()>& fn) {
		on_entity_fn = fn;
	}
	void set_on_active_effect_fn(const std::function<void()>& fn) {
		on_active_effect_fn = fn;
	}

    void set_entity_info(const std::string& desc) {
		set_message(desc, description, description_ptr, desc_rect);
    }
	void set_active_effect(const std::string& desc) {
		set_message(desc, active_effects, active_effects_ptr, active_effects_rect);
	}
	void remove_entity_info() {
		remove_message(description, description_ptr, desc_rect);
	}
	void remove_active_effect() {
		remove_message(active_effects, active_effects_ptr, active_effects_rect);
	}

private:
	void set_message(const std::string& message,
					 std::string& message_field,
					 spTextField& text_field,
					 spColorRectSprite& rect_sprite) {

		message_field = message;
		if (!message_field.empty()) {

			if (text_field) {
				text_field->detach();
			}
			text_field = new oxygine::TextField;
			text_field->attachTo(oxygine::getStage());
			text_field->setFontSize(18);
			text_field->setColor(oxygine::Color::White);
			text_field->setFont(res::ui.getResFont("main"));
			text_field->setText(message_field);
			text_field->setPriority(10000);
			text_field->setTouchEnabled(false);
			text_field->setPosition(getStage()->getWidth() / 2, getStage()->getHeight() / 1.5);
			text_field->setHAlign(oxygine::TextStyle::HALIGN_MIDDLE);
			text_field->setVAlign(oxygine::TextStyle::VALIGN_MIDDLE);

			oxygine::RectF bounds;
			text_field->getBounds(bounds);
			bounds.expand(oxygine::Vector2(10, 10), oxygine::Vector2(10, 20));

			if (rect_sprite) {
				rect_sprite->detach();
			}
			rect_sprite = new oxygine::ColorRectSprite;
			rect_sprite->attachTo(oxygine::getStage());
			rect_sprite->setSize(bounds.getSize());
			rect_sprite->setColor(oxygine::Color::Black);
			rect_sprite->setPriority(9999);
			rect_sprite->setTouchEnabled(false);
			rect_sprite->setAnchor(0.5, 0.5);
			rect_sprite->setPosition(getStage()->getWidth() / 2, getStage()->getHeight() / 1.5);
		}
	}
	void remove_message(std::string& message_field,
						spTextField& text_field,
						spColorRectSprite& rect_sprite) {
		if (text_field) {
			message_field.clear();
            text_field->detach();
            text_field = nullptr;
			rect_sprite->detach();
			rect_sprite = nullptr;
		}
	}

    std::vector<oxygine::spSprite> buttons_;
	oxygine::spSprite end_turn;
	oxygine::spSprite entity_logo;
    oxygine::spTextField entity_name;
	std::function<void(std::uint32_t)> on_button_fn;
	std::function<void(std::uint32_t)> on_get_desc_fn;
	std::function<void()> on_entity_fn;
	std::function<void()> on_active_effect_fn;

	std::string description;
	spTextField description_ptr{nullptr};
	spColorRectSprite desc_rect{nullptr};

	std::string active_effects;
	spTextField active_effects_ptr{nullptr};
	spColorRectSprite active_effects_rect{nullptr};

    spTextField entity_health;
    spTextField entity_power;
	spColorRectSprite entity_power_rect{nullptr};
};

#endif