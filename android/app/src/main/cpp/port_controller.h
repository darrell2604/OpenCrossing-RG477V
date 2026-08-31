#pragma once

#include <algorithm>
#include <cstdint>

namespace oc {

struct ControllerState {
    std::uint16_t buttons = 0;
    std::uint8_t dpad = 0;
    float left_x = 0.0f;
    float left_y = 0.0f;
    float right_x = 0.0f;
    float right_y = 0.0f;
    float left_trigger = 0.0f;
    float right_trigger = 0.0f;
};

enum Button : std::uint16_t {
    A = 1u << 0,
    B = 1u << 1,
    X = 1u << 2,
    Y = 1u << 3,
    START = 1u << 4,
    SELECT = 1u << 5,
    L = 1u << 6,
    R = 1u << 7,
    Z = 1u << 8,
};

enum Dpad : std::uint8_t {
    DPAD_UP = 1u << 0,
    DPAD_DOWN = 1u << 1,
    DPAD_LEFT = 1u << 2,
    DPAD_RIGHT = 1u << 3,
};

inline float normalize_axis(float value, float dead_zone = 0.10f) {
    const float magnitude = std::abs(value);
    if (magnitude <= dead_zone) return 0.0f;
    const float scaled = (magnitude - dead_zone) / (1.0f - dead_zone);
    return std::clamp(value < 0.0f ? -scaled : scaled, -1.0f, 1.0f);
}

// RG477V default profile: physical Android gamepad events are translated
// into a neutral GameCube-style state here; game code never sees Android APIs.
ControllerState make_rg477v_state();

void reset_controller_state();
void update_controller_button(Button button, bool pressed);
void update_controller_axis(int axis, float value);
void update_controller_dpad(int hat_x, int hat_y);

const ControllerState& controller_state();

} // namespace oc
