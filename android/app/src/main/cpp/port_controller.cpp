#include "port_controller.h"

#include <cmath>

namespace oc {
namespace {
ControllerState g_state;
}

ControllerState make_rg477v_state() {
    ControllerState state = g_state;
    state.left_x = normalize_axis(state.left_x);
    state.left_y = normalize_axis(state.left_y);
    state.right_x = normalize_axis(state.right_x);
    state.right_y = normalize_axis(state.right_y);
    state.left_trigger = std::clamp(state.left_trigger, 0.0f, 1.0f);
    state.right_trigger = std::clamp(state.right_trigger, 0.0f, 1.0f);
    return state;
}

void reset_controller_state() {
    g_state = {};
}

void update_controller_button(Button button, bool pressed) {
    if (pressed) {
        g_state.buttons |= static_cast<std::uint16_t>(button);
    } else {
        g_state.buttons &= static_cast<std::uint16_t>(~button);
    }
}

void update_controller_axis(int axis, float value) {
    switch (axis) {
        case 0: g_state.left_x = value; break;
        case 1: g_state.left_y = value; break;
        case 2: g_state.right_x = value; break;
        case 3: g_state.right_y = value; break;
        case 4: g_state.left_trigger = (value + 1.0f) * 0.5f; break;
        case 5: g_state.right_trigger = (value + 1.0f) * 0.5f; break;
        default: break;
    }
}

void update_controller_dpad(int hat_x, int hat_y) {
    g_state.dpad = 0;
    if (hat_y < 0) g_state.dpad |= DPAD_UP;
    if (hat_y > 0) g_state.dpad |= DPAD_DOWN;
    if (hat_x < 0) g_state.dpad |= DPAD_LEFT;
    if (hat_x > 0) g_state.dpad |= DPAD_RIGHT;
}

const ControllerState& controller_state() {
    return g_state;
}

} // namespace oc
