#pragma once

#include <cstdint>
#include <vector>

#include "port_controller.h"

namespace open_crossing {

struct CollisionRect {
    float min_x = 0.0f;
    float min_z = 0.0f;
    float max_x = 0.0f;
    float max_z = 0.0f;
};

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
    void set_collision_rects(const std::vector<CollisionRect>& rects);

    float x() const { return x_; }
    float z() const { return z_; }
    float angle() const { return angle_; }
    float speed() const { return speed_; }
    float input_magnitude() const { return input_magnitude_; }
    MovementState movement_state() const { return movement_state_; }
    bool moving() const { return movement_state_ != MovementState::Idle; }
    bool blocked() const { return blocked_; }
    std::uint64_t steps() const { return steps_; }

private:
    bool collides(float x, float z) const;
    void try_move(float dx, float dz);

    float x_ = 0.0f;
    float z_ = 0.0f;
    float angle_ = 0.0f;
    float speed_ = 0.0f;
    float input_magnitude_ = 0.0f;
    MovementState movement_state_ = MovementState::Idle;
    bool blocked_ = false;
    std::uint64_t steps_ = 0;
    std::vector<CollisionRect> collision_rects_;
};

} // namespace open_crossing
