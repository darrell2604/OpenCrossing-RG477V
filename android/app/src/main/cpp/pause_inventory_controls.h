#pragma once

#include "game_state.h"

namespace open_crossing {

void process_pause_inventory_controls(GameStateSystem& state,
                                      bool just_pressed_start,
                                      bool just_pressed_y);

} // namespace open_crossing
