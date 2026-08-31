#include "player_camera.h"

#include <algorithm>
#include <cmath>

namespace open_crossing {

namespace {
constexpr float kCameraFollow = 0.18f;
constexpr float kCameraTurn = 0.12f;
constexpr float kCameraDistance = 6.0f;
constexpr float kCameraHeight = 4.0f;
constexpr float kCameraPitch = 0.58f;
constexpr float kCameraTurnRate = 0.035f;
constexpr float kPi = 3.14159265f;
constexpr float kTwoPi = 6.28318531f;

float wrap_angle(float value) {
    while (value > kPi) value -= kTwoPi;
    while (value < -kPi) value += kTwoPi;
    return value;
}
}

float PlayerCamera::approach(float current, float target, float amount) {
    return current + (target - current) * amount;
}

void PlayerCamera::reset(const PlayerSimulation& player) {
    state_.target_x = player.x();
    state_.target_z = player.z();
    state_.yaw = player.angle();
    state_.distance = kCameraDistance;
    state_.height = kCameraHeight;
    state_.pitch = kCameraPitch;
    state_.x = state_.target_x - std::sin(state_.yaw) * state_.distance;
    state_.z = state_.target_z + std::cos(state_.yaw) * state_.distance;
}

void PlayerCamera::update(const PlayerSimulation& player, const oc::ControllerState& controller) {
    state_.target_x = approach(state_.target_x, player.x(), kCameraFollow);
    state_.target_z = approach(state_.target_z, player.z(), kCameraFollow);

    if (std::fabs(controller.right_x) > 0.08f) {
        state_.yaw = wrap_angle(state_.yaw + controller.right_x * kCameraTurnRate);
    } else {
        state_.yaw = wrap_angle(approach(state_.yaw, player.angle(), kCameraTurn));
    }

    state_.x = approach(
        state_.x,
        state_.target_x - std::sin(state_.yaw) * state_.distance,
        kCameraFollow);
    state_.z = approach(
        state_.z,
        state_.target_z + std::cos(state_.yaw) * state_.distance,
        kCameraFollow);
}

} // namespace open_crossing
