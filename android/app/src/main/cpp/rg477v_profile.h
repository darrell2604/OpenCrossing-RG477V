#pragma once

#include <cstdint>

namespace oc::rg477v {

// Android commonly exposes the RG477V's integrated controls through a
// standard Xbox-style gamepad mapping. Keep the physical mapping isolated so
// it can be adjusted without touching game logic.

enum AndroidKeyCode : std::int32_t {
    KEYCODE_A = 96,
    KEYCODE_B = 97,
    KEYCODE_X = 99,
    KEYCODE_Y = 100,
    KEYCODE_L1 = 102,
    KEYCODE_R1 = 103,
    KEYCODE_L2 = 104,
    KEYCODE_R2 = 105,
    KEYCODE_SELECT = 109,
    KEYCODE_START = 108,
    KEYCODE_DPAD_UP = 19,
    KEYCODE_DPAD_DOWN = 20,
    KEYCODE_DPAD_LEFT = 21,
    KEYCODE_DPAD_RIGHT = 22,
};

// Android motion axes for a conventional gamepad. The exact physical
// reporting is device/firmware dependent, so the event layer should treat
// these as defaults and retain remapping support.
enum AndroidAxis : std::int32_t {
    AXIS_X = 0,
    AXIS_Y = 1,
    AXIS_Z = 11,
    AXIS_RZ = 14,
    AXIS_LTRIGGER = 17,
    AXIS_RTRIGGER = 18,
};

} // namespace oc::rg477v
