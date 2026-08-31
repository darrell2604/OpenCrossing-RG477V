#include "player_simulation.h"

#include <algorithm>
#include <cmath>

namespace open_crossing {

namespace {
constexpr float kAcceleration = 0.0812f;
constexpr float kDeceleration = 0.0435f;
constexpr float kWalkSpeed = 0.65f;
constexpr float kInputEpsilon = 0.05f;
constexpr float kSpeedEpsilon = 0.01f;
constexpr float kPi = 3.14159265f;
constexpr float kTwoPi = 6.28318531f;

float shortest_angle_delta(float target, float current) {
    float delta = target - current;
    while (delta > kPi) delta -= kTwoPi;
    while (delta < -kPi) delta += kTwoPi;
    return delta;
}
}

void PlayerSimulation::reset() {
    x_ = 0.0f;
    z_ = 0.0f;
    angle_ = 0.0f;
    speed_ = 0.0f;
    input_magnitude_ = 0.0f;
    movement_state_ = MovementState::Idle;
    steps_ = 0;
}

void PlayerSimulation::update(const oc::ControllerState& controller) {
    const float input_x = oc::normalize_axis(controller.left_x);
    const float input_y = oc::normalize_axis(controller.left_y);
    input_magnitude_ = std::min(1.0f, std::sqrt(input_x * input_x + input_y * input_y));

    if (input_magnitude_ > kInputEpsilon) {
        const float target_angle = std::atan2(input_x, -input_y);
        angle_ += shortest_angle_delta(target_angle, angle_) * 0.18f;

        const float target_speed = kWalkSpeed * input_magnitude_;
        if (speed_ < target_speed) {
            speed_ = std::min(target_speed, speed_ + kAcceleration);
        } else {
            speed_ = std::max(target_speed, speed_ - kDeceleration);
        }
    } else {
        input_magnitude_ = 0.0f;
        speed_ = std::max(0.0f, speed_ - kDeceleration);
    }

    x_ += std::sin(angle_) * speed_;
    z_ += -std::cos(angle_) * speed_;
    movement_state_ = speed_ > kSpeedEpsilon ? MovementState::Walking : MovementState::Idle;
    ++steps_;
}

} // namespace open_crossing
