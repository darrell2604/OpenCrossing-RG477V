#include "port_controller.h"

namespace {
oc::ControllerState g_state{};
}

namespace oc {

ControllerState make_rg477v_state() {
    return g_state;
}

void reset_controller_state() {
    g_state = ControllerState{};
}

void update_controller_button(Button button, bool pressed) {
    if (pressed) {
        g_state.buttons |= static_cast<std::uint16_t>(button);
    } else {
        g_state.buttons &= static_cast<std::uint16_t>(~button);
    }
}

void update_controller_axis(int axis, float value) {
    const float normalised = normalize_axis(value);
    switch (axis) {
    case 0: g_state.left_x = normalised; break;
    case 1: g_state.left_y = normalised; break;
    case 2: g_state.right_x = normalised; break;
    case 3: g_state.right_y = normalised; break;
    case 4: g_state.left_trigger = (normalised + 1.0f) * 0.5f; break;
    case 5: g_state.right_trigger = (normalised + 1.0f) * 0.5f; break;
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
