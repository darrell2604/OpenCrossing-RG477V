#include "game_state.h"

#include "port_controller.h"

namespace open_crossing {

// Centralised handling for the simple control transitions already represented
// by GameStateSystem. Kept separate so the native game loop can delegate these
// state changes without duplicating button-edge logic.
void process_pause_inventory_controls(GameStateSystem& state,
                                      bool just_pressed_start,
                                      bool just_pressed_y) {
    if (just_pressed_start) {
        state.toggle_pause();
    }

    if (just_pressed_y && state.state().phase == GamePhase::Playing) {
        state.set_inventory_open(!state.state().inventory_open);
    }
}

} // namespace open_crossing
