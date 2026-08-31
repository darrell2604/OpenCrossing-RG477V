#pragma once

#include <cstdint>

#include "port_controller.h"

namespace open_crossing {

// Portable player simulation. This is intentionally platform-neutral so the
// same movement/state contract can later be fed by the real game systems.
class PlayerSimulation {
public:
    enum class MovementState : std::uint8_t {
        Idle,
        Walking,
    };

    void reset();
    void update(const oc::ControllerState& controller);

    float x() const { return x_; }
    float z() const { return z_; }
    float angle() const { return angle_; }
    float speed() const { return speed_; }
    float input_magnitude() const { return input_magnitude_; }
    MovementState movement_state() const { return movement_state_; }
    bool moving() const { return movement_state_ != MovementState::Idle; }
    std::uint64_t steps() const { return steps_; }

private:
    float x_ = 0.0f;
    float z_ = 0.0f;
    float angle_ = 0.0f;
    float speed_ = 0.0f;
    float input_magnitude_ = 0.0f;
    MovementState movement_state_ = MovementState::Idle;
    std::uint64_t steps_ = 0;
};

} // namespace open_crossing
