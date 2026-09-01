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
constexpr float kCollisionRadius = 0.20f;
constexpr float kAnimationStartFrames = 2.0f;
constexpr float kWalkCycleFrames = 8.0f;
constexpr float kWalkRate = 0.35f;
constexpr float kTurnRate = 0.22f;
constexpr float kStopFrames = 2.0f;
constexpr float kPi = 3.14159265f;
constexpr float kTwoPi = 6.28318531f;
constexpr float kWorldMinX = -8.0f;
constexpr float kWorldMaxX = 8.0f;
constexpr float kWorldMinZ = -8.0f;
constexpr float kWorldMaxZ = 8.0f;

float shortest_angle_delta(float target, float current) {
    float delta = target - current;
    while (delta > kPi) delta -= kTwoPi;
    while (delta < -kPi) delta += kTwoPi;
    return delta;
}

bool point_in_expanded_rect(float x, float z, const CollisionRect& rect, float radius) {
    return x >= rect.min_x - radius && x <= rect.max_x + radius &&
           z >= rect.min_z - radius && z <= rect.max_z + radius;
}

float clamp_unit(float value) {
    return std::clamp(value, 0.0f, 1.0f);
}

} // namespace

void PlayerSimulation::reset() {
    x_ = 0.0f;
    z_ = 0.0f;
    angle_ = 0.0f;
    previous_angle_ = 0.0f;
    speed_ = 0.0f;
    input_magnitude_ = 0.0f;
    turn_amount_ = 0.0f;
    movement_state_ = MovementState::Idle;
    animation_state_ = AnimationState::Idle;
    blocked_ = false;
    animation_frame_ = 0.0f;
    animation_phase_ = 0.0f;
    animation_blend_ = 0.0f;
    steps_ = 0;
}

void PlayerSimulation::set_collision_rects(const std::vector<CollisionRect>& rects) {
    collision_rects_ = rects;
    if (collides(x_, z_)) {
        blocked_ = true;
    }
}

bool PlayerSimulation::collides(float x, float z) const {
    if (x < kWorldMinX + kCollisionRadius || x > kWorldMaxX - kCollisionRadius ||
        z < kWorldMinZ + kCollisionRadius || z > kWorldMaxZ - kCollisionRadius) {
        return true;
    }

    for (const CollisionRect& rect : collision_rects_) {
        if (point_in_expanded_rect(x, z, rect, kCollisionRadius)) {
            return true;
        }
    }
    return false;
}

void PlayerSimulation::try_move(float dx, float dz) {
    blocked_ = false;
    const float next_x = x_ + dx;
    const float next_z = z_ + dz;

    if (!collides(next_x, next_z)) {
        x_ = next_x;
        z_ = next_z;
        return;
    }

    bool moved = false;
    if (std::fabs(dx) > 0.0f && !collides(next_x, z_)) {
        x_ = next_x;
        moved = true;
    }
    if (std::fabs(dz) > 0.0f && !collides(x_, next_z)) {
        z_ = next_z;
        moved = true;
    }

    blocked_ = !moved;
}

void PlayerSimulation::update_animation() {
    const float turn_delta = shortest_angle_delta(angle_, previous_angle_);
    turn_amount_ = std::min(1.0f, std::fabs(turn_delta) * 2.0f);
    previous_angle_ = angle_;

    switch (animation_state_) {
        case AnimationState::Idle:
            animation_frame_ = 0.0f;
            animation_blend_ = 0.0f;
            if (turn_amount_ > 0.08f && speed_ <= kSpeedEpsilon) {
                animation_state_ = AnimationState::Turn;
                animation_frame_ = 0.0f;
                animation_blend_ = turn_amount_;
            } else if (speed_ > kSpeedEpsilon) {
                animation_state_ = AnimationState::WalkStart;
                animation_frame_ = 0.0f;
                animation_blend_ = 0.0f;
            }
            break;

        case AnimationState::Turn:
            animation_frame_ += kWalkRate;
            animation_blend_ = clamp_unit(1.0f - animation_frame_ / kStopFrames);
            if (speed_ > kSpeedEpsilon) {
                animation_state_ = AnimationState::WalkStart;
                animation_frame_ = 0.0f;
                animation_blend_ = 0.0f;
            } else if (animation_frame_ >= kStopFrames) {
                animation_state_ = AnimationState::Idle;
                animation_frame_ = 0.0f;
                animation_blend_ = 0.0f;
            }
            break;

        case AnimationState::WalkStart:
            animation_frame_ += kWalkRate + speed_ * 0.5f;
            animation_blend_ = clamp_unit(animation_frame_ / kAnimationStartFrames);
            if (animation_frame_ >= kAnimationStartFrames) {
                animation_state_ = AnimationState::Walk;
                animation_frame_ = std::fmod(animation_frame_, kWalkCycleFrames);
                animation_blend_ = 1.0f;
            }
            if (speed_ <= kSpeedEpsilon) {
                animation_state_ = AnimationState::WalkStop;
                animation_frame_ = 0.0f;
                animation_blend_ = 0.0f;
            }
            break;

        case AnimationState::Walk:
            animation_frame_ = std::fmod(
                animation_frame_ + kWalkRate + speed_ * 0.8f, kWalkCycleFrames);
            animation_blend_ = clamp_unit(speed_ / kWalkSpeed);
            if (turn_amount_ > 0.18f && speed_ < kWalkSpeed * 0.35f) {
                animation_state_ = AnimationState::Turn;
                animation_frame_ = 0.0f;
                animation_blend_ = turn_amount_;
            } else if (speed_ <= kSpeedEpsilon) {
                animation_state_ = AnimationState::WalkStop;
                animation_frame_ = 0.0f;
                animation_blend_ = 0.0f;
            }
            break;

        case AnimationState::WalkStop:
            animation_frame_ += kWalkRate;
            animation_blend_ = clamp_unit(animation_frame_ / kStopFrames);
            if (speed_ > kSpeedEpsilon) {
                animation_state_ = AnimationState::WalkStart;
                animation_frame_ = 0.0f;
                animation_blend_ = 0.0f;
            } else if (animation_frame_ >= kStopFrames) {
                animation_state_ = AnimationState::Idle;
                animation_frame_ = 0.0f;
                animation_blend_ = 0.0f;
            }
            break;
    }

    animation_phase_ = animation_state_ == AnimationState::Turn
        ? clamp_unit(animation_frame_ / kStopFrames)
        : std::fmod(animation_frame_ / kWalkCycleFrames, 1.0f);
}

void PlayerSimulation::update(const oc::ControllerState& controller) {
    const float input_x = oc::normalize_axis(controller.left_x);
    const float input_y = oc::normalize_axis(controller.left_y);
    input_magnitude_ = std::min(1.0f, std::sqrt(input_x * input_x + input_y * input_y));

    if (input_magnitude_ > kInputEpsilon) {
        const float target_angle = std::atan2(input_x, -input_y);
        angle_ += shortest_angle_delta(target_angle, angle_) * kTurnRate;

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

    const float dx = std::sin(angle_) * speed_;
    const float dz = -std::cos(angle_) * speed_;
    try_move(dx, dz);
    movement_state_ = speed_ > kSpeedEpsilon ? MovementState::Walking : MovementState::Idle;
    update_animation();
    ++steps_;
}

} // namespace open_crossing