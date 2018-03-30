#ifndef BOARD_PANEL_H
#define BOARD_PANEL_H

#include <oxygine-framework.h>
#include <game_state.h>

class board_panel final
{
public:
	void init(std::int32_t rows_n, std::int32_t cols_n);
	void update_state(local_state& state);

	void set_on_board(const std::function<void(std::uint32_t, std::uint32_t)>& fn) {
        on_board_fn = fn;
    }

private:
	std::vector<oxygine::spSprite> fields;
    std::function<void(std::uint32_t, std::uint32_t)> on_board_fn;
};

oxygine::Point board_indexes_to_point(std::uint32_t col, std::uint32_t row);
oxygine::Point board_index_to_point(std::uint32_t index);
oxygine::Point board_index_to_rectangle(std::uint32_t index);

#endif