#include "board_panel.h"
//#include "core/board.h"
#include "view_constants.h"
#include "../res.h"
#include "messages/message_makers.h"

using namespace oxygine;

void board_panel::init(std::int32_t rows_n, std::int32_t cols_n)
{
    for (std::uint32_t row = 0; row < rows_n; ++row)
    {
        for (std::uint32_t col = 0; col < cols_n; ++col)
        {
            std::int32_t x = col * constants::field_size;
            std::int32_t y = row * constants::field_size;

            spSprite sprite = new Sprite;
            sprite->setPosition(x, y);
            sprite->setSize(constants::field_size, constants::field_size);
            sprite->attachTo(getStage());
            sprite->setResAnim(res::ui.getResAnim("grass2"));

            sprite->addEventListener(TouchEvent::TOUCH_UP, [this, row, col](Event*){
                log::messageln(("board clicked: " + std::to_string(row) + " x " + std::to_string(col)).c_str());
                if (on_board_fn)
                    on_board_fn(col, row);
            });

            fields.emplace_back(sprite);
        }
    }
}

void board_panel::update_state(local_state& state) {

    for (auto&& field : fields) {
        field->setResAnim(res::ui.getResAnim("grass2"));
    }

    if (state.actual_target_type != target_types::non) {
        for (auto index : state.possible_movements) {

            if (state.actual_target_type == target_types::moving)
                fields[index]->setResAnim(res::ui.getResAnim("grass_move"));

            else if (state.actual_target_type == target_types::enemy) {
                if (state.valid_movements.find(index) != std::end(state.valid_movements))
                    fields[index]->setResAnim(res::ui.getResAnim("grass_damage"));
                else
                    fields[index]->setResAnim(res::ui.getResAnim("grass_enemy"));
            } else if (state.actual_target_type == target_types::friendly) {
                if (state.valid_movements.find(index) != std::end(state.valid_movements))
                    fields[index]->setResAnim(res::ui.getResAnim("grass_boost_bitmap"));
                else
                    fields[index]->setResAnim(res::ui.getResAnim("grass_friendly_bitmap"));
            } else {
                fields[index]->setResAnim(res::ui.getResAnim("grass_move"));
            }
        }
    }

    if (state.selected_index != states::no_selected_index) {
        fields[state.selected_index]->setResAnim(res::ui.getResAnim("grass_selected"));
    }
}


oxygine::Point board_indexes_to_point(std::uint32_t col, std::uint32_t row)
{
    std::int32_t x = 0 + col * constants::field_size;
    std::int32_t y = 0 + row * constants::field_size;
    return {x, y};
}

//board_panel::rect_type board_indexies_to_rectangle(std::uint32_t col, std::uint32_t row)
//{
//    auto point = board_indexies_to_point(col, row);
//    return {point.x, point.y, constants::field_size, constants::field_size};
//}
//
//board_panel::point_type board_indexies_to_point(double col, double row)
//{
//    std::int32_t x = constants::top_left_point.x + col * constants::field_size;
//    std::int32_t y = constants::top_left_point.y + row * constants::field_size;
//    return {x, y};
//}
//
//board_panel::rect_type board_indexes_to_rectangle(double col, double row)
//{
//    auto point = board_indexies_to_point(col, row);
//    return {point.x, point.y, constants::field_size, constants::field_size};
//}
//
Point board_index_to_point(std::uint32_t index)
{
    auto indexes = board_container::to_pos(index);
    return board_indexes_to_point(indexes.first, indexes.second);
}
//
//board_panel::rect_type board_index_to_rectangle(std::uint32_t index)
//{
//    auto indexes = board::to_pos(index);
//    return board_indexies_to_rectangle(indexes.first, indexes.second);
//}
//
//std::uint32_t indexes_to_index(std::uint32_t row, std::uint32_t col)
//{
//    return row * board::cols_n + col;
//}
//
//std::pair<std::uint32_t, std::uint32_t> index_to_indexes(std::uint32_t index)
//{
//    return std::make_pair(index % board::cols_n,
//                          index / board::cols_n);
//}
//}