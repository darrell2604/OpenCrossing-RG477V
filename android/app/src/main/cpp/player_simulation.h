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

class PlayerSimulation {
public:
    enum class MovementState : std::uint8_t {
        Idle,
        Walking,
    };

    enum class AnimationState : std::uint8_t {
        Idle,
        WalkStart,
        Walk,
        WalkStop,
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
    AnimationState animation_state() const { return animation_state_; }
    bool moving() const { return movement_state_ != MovementState::Idle; }
    bool blocked() const { return blocked_; }
    float animation_frame() const { return animation_frame_; }
    float animation_phase() const { return animation_phase_; }
    std::uint64_t steps() const { return steps_; }

private:
    bool collides(float x, float z) const;
    void try_move(float dx, float dz);
    void update_animation();

    float x_ = 0.0f;
    float z_ = 0.0f;
    float angle_ = 0.0f;
    float speed_ = 0.0f;
    float input_magnitude_ = 0.0f;
    MovementState movement_state_ = MovementState::Idle;
    AnimationState animation_state_ = AnimationState::Idle;
    bool blocked_ = false;
    float animation_frame_ = 0.0f;
    float animation_phase_ = 0.0f;
    std::uint64_t steps_ = 0;
    std::vector<CollisionRect> collision_rects_;
};

} // namespace open_crossing
