#include "player_simulation.h"

#include <algorithm>
#include <cmath>

namespace open_crossing {

namespace {
constexpr float kAcceleration = 0.0812f;
constexpr float kDeceleration = 0.0435f;
constexpr float kWalkSpeed = 0.65f;
}

void PlayerSimulation::reset() {
    x_ = 0.0f;
    z_ = 0.0f;
    angle_ = 0.0f;
    speed_ = 0.0f;
    steps_ = 0;
}

void PlayerSimulation::update(const oc::ControllerState& controller) {
    const float input_x = oc::normalize_axis(controller.left_x);
    const float input_y = oc::normalize_axis(controller.left_y);
    const float magnitude = std::min(1.0f, std::sqrt(input_x * input_x + input_y * input_y));

    if (magnitude > 0.0f) {
        // Match the upstream walk path's gradual facing/speed response rather
        // than snapping directly to the stick vector.
        const float target_angle = std::atan2(input_x, -input_y);
        float delta = target_angle - angle_;
        while (delta > 3.14159265f) delta -= 6.28318531f;
        while (delta < -3.14159265f) delta += 6.28318531f;
        angle_ += delta * 0.18f;

        const float target_speed = kWalkSpeed * magnitude;
        if (speed_ < target_speed) {
            speed_ = std::min(target_speed, speed_ + kAcceleration);
        } else {
            speed_ = std::max(target_speed, speed_ - kDeceleration);
        }
    } else {
        speed_ = std::max(0.0f, speed_ - kDeceleration);
    }

    x_ += std::sin(angle_) * speed_;
    z_ += -std::cos(angle_) * speed_;
    ++steps_;
}

} // namespace open_crossing
