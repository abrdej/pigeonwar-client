#ifndef BUTTONS_PANEL_H
#define BUTTONS_PANEL_H

#include <array>
#include <res.h>
#include "oxygine-framework.h"

class buttons_panel final
{
public:
	void init();
    void set_for_entity_for(std::uint32_t entity_id,
                            const std::string& entity_name,
                            const std::string& entity_health,
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

    void set_entity_info(const std::string& desc) {
        set_desc(desc);
    }

	void set_desc(const std::string& desc) {
		description = desc;

		if (!description.empty()) {

			if (description_ptr) {
				description_ptr->detach();
			}
			description_ptr = new oxygine::TextField;
			description_ptr->attachTo(oxygine::getStage());
			description_ptr->setFontSize(18);
			description_ptr->setColor(oxygine::Color::White);
			description_ptr->setFont(res::ui.getResFont("main"));
			description_ptr->setText(description);
			description_ptr->setPriority(10000);
			description_ptr->setTouchEnabled(false);
			description_ptr->setPosition(getStage()->getWidth() / 2, getStage()->getHeight() / 1.5);
			description_ptr->setHAlign(oxygine::TextStyle::HALIGN_MIDDLE);
			description_ptr->setVAlign(oxygine::TextStyle::VALIGN_MIDDLE);

			oxygine::RectF bounds;
			description_ptr->getBounds(bounds);
			bounds.expand(oxygine::Vector2(10, 10), oxygine::Vector2(10, 20));

			if (desc_rect) {
				desc_rect->detach();
			}
			desc_rect = new oxygine::ColorRectSprite;
			desc_rect->attachTo(oxygine::getStage());
			desc_rect->setSize(bounds.getSize());
			desc_rect->setColor(oxygine::Color::Black);
			desc_rect->setPriority(9999);
			desc_rect->setTouchEnabled(false);
			desc_rect->setAnchor(0.5, 0.5);
			desc_rect->setPosition(getStage()->getWidth() / 2, getStage()->getHeight() / 1.5);

		}
	}

private:
    std::vector<oxygine::spSprite> buttons_;
	oxygine::spSprite end_turn;
	oxygine::spSprite entity_logo;
    oxygine::spTextField entity_name;
	std::function<void(std::uint32_t)> on_button_fn;
	std::function<void(std::uint32_t)> on_get_desc_fn;
	std::function<void()> on_entity_fn;

	std::string description;
	spTextField description_ptr{nullptr};
	spColorRectSprite desc_rect{nullptr};

    spTextField entity_health;
    spTextField entity_power;
	spColorRectSprite entity_power_rect{nullptr};
};

#endif