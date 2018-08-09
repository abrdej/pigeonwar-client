#include "../board_container.h"
#include "buttons_panel.h"
#include "view_constants.h"
#include "../res.h"
#include "sprites/drawer_factory.h"

using namespace oxygine;

void buttons_panel::init(std::uint32_t cols, std::uint32_t rows)
{
    auto y_pos = 0 + rows * constants::field_size;

    auto x_pos = 0 + 2 * constants::field_size + constants::field_size / 2;

	for (std::uint32_t n = 0; n < 5; ++n)
	{
		std::int32_t x = x_pos + n * constants::field_size;
		std::int32_t y = y_pos;

		spSprite border = new Sprite;
		border->setPosition(x + constants::field_size / 2, y + constants::field_size / 2);
		border->setSize(constants::field_size, constants::field_size);
		border->attachTo(getStage());
		border->setResAnim(res::ui.getResAnim("border"));
		border->setTouchEnabled(true);
		border->setAnchor(0.5, 0.5);
        border->setPriority(45);

		border->addEventListener(TouchEvent::CLICK, [n, this](Event* ev){

			log::messageln(("button clicked: " + std::to_string(n)).c_str());

			auto touch = static_cast<oxygine::TouchEvent*>(ev);

			if (touch->mouseButton == MouseButton::MouseButton_Left) {
				on_button_fn(n);
			} else {
				on_get_desc_fn(n);
			}
		});

		border->addEventListener(TouchEvent::OVER, [border, n, this](Event* ev){

            on_get_desc_fn(n);

            border->setScale(1.2);
			border->setPriority(55);
            buttons_[n]->setScale(1.2);
			buttons_[n]->setPriority(60);
		});
        border->addEventListener(TouchEvent::OUT, [border, n, this](Event* ev){
            border->setScale(1);
            border->setPriority(45);
            buttons_[n]->setScale(1);
            buttons_[n]->setPriority(50);

            if (description_ptr) {
                description_ptr->detach();
                description_ptr = nullptr;
            }
            if (desc_rect) {
                desc_rect->detach();
                desc_rect = nullptr;
            }
        });

        spSprite button = new Sprite;
        button->setPosition(x + constants::field_size / 2, y + constants::field_size / 2);
        button->setSize(constants::field_size, constants::field_size);
        button->attachTo(getStage());
        button->setTouchEnabled(false);
        button->setAnchor(0.5, 0.5);
		button->setPriority(50);

		buttons_.emplace_back(button);
	}

	auto x = constants::field_size * (cols - 1);
	auto y = constants::field_size * rows;

	end_turn = new Sprite;
	end_turn->setPosition(x, y);
	end_turn->setSize(constants::field_size, constants::field_size);
	end_turn->attachTo(getStage());
	end_turn->setResAnim(res::ui.getResAnim("end_turn"));

	end_turn->addEventListener(TouchEvent::CLICK, [this](Event*){
		log::messageln(("end turn clicked"));
		on_button_fn(5);
	});

	entity_logo = new Sprite;
	entity_logo->setPosition(constants::field_size / 4, (rows) * constants::field_size + constants::field_size / 8);
	entity_logo->attachTo(getStage());
	entity_logo->setScale(1.2f);
	entity_logo->addEventListener(TouchEvent::CLICK, [this](Event*) {
		on_entity_fn();
	});

    entity_logo->addEventListener(TouchEvent::OVER, [this](Event* ev){
        entity_logo->setScale(1.4);
        on_entity_fn();
    });
    entity_logo->addEventListener(TouchEvent::OUT, [this](Event* ev){
        entity_logo->setScale(1.2);
        remove_entity_info();
    });

    entity_name = new TextField;
    entity_name->setPosition(constants::field_size / 4, (rows + 1) * constants::field_size + constants::field_size / 8);
    entity_name->attachTo(getStage());
    entity_name->setFontSize(22);
    entity_name->setColor(Color::White);
    entity_name->setFont(res::ui.getResFont("main"));

    entity_health = new TextField;
    entity_health->setPosition(constants::field_size + constants::field_size / 1.8,
                               rows * constants::field_size + constants::field_size / 4);
    entity_health->attachTo(getStage());
    entity_health->setFontSize(28);
    entity_health->setColor(Color::LimeGreen);
    entity_health->setFont(res::ui.getResFont("main"));
    entity_health->setAlign(TextStyle::VerticalAlign::VALIGN_MIDDLE, TextStyle::HorizontalAlign::HALIGN_MIDDLE);

    entity_power = new TextField;
    entity_power->setPosition(constants::field_size + constants::field_size / 1.2,
                              rows * constants::field_size + constants::field_size / 1.5);
    entity_power->attachTo(getStage());
    entity_power->setFontSize(28);
    entity_power->setColor(Color::DodgerBlue);
    entity_power->setFont(res::ui.getResFont("main"));
    entity_power->setAlign(TextStyle::VerticalAlign::VALIGN_MIDDLE, TextStyle::HorizontalAlign::HALIGN_MIDDLE);

//    oxygine::RectF bounds;
//    entity_power->getBounds(bounds);
//    bounds.expand(oxygine::Vector2(10, 10), oxygine::Vector2(10, 20));
//
//    entity_power_rect = new oxygine::ColorRectSprite;
//    entity_power_rect->attachTo(oxygine::getStage());
//    entity_power_rect->setSize(bounds.getSize());
//    entity_power_rect->setColor(oxygine::Color::Black);
//    entity_power_rect->setPriority(9999);
//    entity_power_rect->setTouchEnabled(false);
//    entity_power_rect->setAnchor(0.5, 0.5);
//    entity_power_rect->setPosition(constants::field_size + constants::field_size / 1.2,
//                                   board_container::rows_n * constants::field_size + constants::field_size / 1.5);

    // effects stuff
    for (std::uint32_t n = 0; n < 5; ++n) {

        auto x_pos2 = 0 + 8 * constants::field_size + constants::field_size / 2;

        std::int32_t x2 = x_pos2 + n * constants::field_size;
        std::int32_t y2 = y_pos;

        spSprite border = new Sprite;
        border->setPosition(x2 + constants::field_size / 2, y2 + constants::field_size / 2);
        border->setSize(constants::field_size, constants::field_size);
        border->attachTo(getStage());
        border->setResAnim(res::ui.getResAnim("border"));
        border->setTouchEnabled(true);
        border->setAnchor(0.5, 0.5);
        border->setPriority(45);

        border->addEventListener(TouchEvent::CLICK, [n, this](Event *ev) {

            auto touch = static_cast<oxygine::TouchEvent *>(ev);
            if (touch->mouseButton == MouseButton::MouseButton_Left) {

            }
        });

        border->addEventListener(TouchEvent::OVER, [border, n, this](Event *ev) {
            border->setScale(1.2);
            border->setPriority(55);

            on_active_effect_fn();
        });
        border->addEventListener(TouchEvent::OUT, [border, n, this](Event *ev) {
            border->setScale(1);
            border->setPriority(45);
            remove_active_effect();
        });
    }
}

void buttons_panel::set_for_entity_for(std::uint32_t entity_id,
									   const std::string& entity_name,
                                       const std::string& entity_health,
                                       const std::string& entity_power,
									   const std::array<std::string, 5>& button_bitmaps) {

	if (entity_id != std::numeric_limits<decltype(entity_id)>::max()) {
        auto bitmap = drawer_factory::get_bitmap(entity_name);
		entity_logo->setResAnim(res::ui.getResAnim(bitmap));
	}

    this->entity_name->setText(entity_name);
    this->entity_health->setText(entity_health);
    this->entity_power->setText(entity_power);

	for (std::int32_t i = 0; i < 5; ++i) {
		if (!button_bitmaps[i].empty()) {
			buttons_[i]->setResAnim(res::ui.getResAnim(button_bitmaps[i]));
		} else {
			buttons_[i]->setResAnim(nullptr);
		}
	}
}