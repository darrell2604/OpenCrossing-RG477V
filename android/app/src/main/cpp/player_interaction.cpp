#include "player_interaction.h"

#include <cmath>
#include <limits>

namespace open_crossing {

void PlayerInteraction::reset() {
    in_range_ = false;
    pressed_ = false;
    interacting_ = false;
    target_id_ = 0;
    interaction_count_ = 0;
    previous_a_ = false;
}

void PlayerInteraction::set_targets(const std::vector<InteractionTarget>& targets) {
    targets_ = targets;
}

void PlayerInteraction::update(const PlayerSimulation& player, const oc::ControllerState& controller) {
    const bool a_pressed = (controller.buttons & oc::A) != 0;
    pressed_ = a_pressed && !previous_a_;
    previous_a_ = a_pressed;

    const float px = player.x();
    const float pz = player.z();
    float closest = std::numeric_limits<float>::max();
    const InteractionTarget* best = nullptr;

    for (const auto& target : targets_) {
        const float dx = target.x - px;
        const float dz = target.z - pz;
        const float distance_sq = dx * dx + dz * dz;
        const float range = target.radius;
        if (distance_sq <= range * range && distance_sq < closest) {
            closest = distance_sq;
            best = &target;
        }
    }

    in_range_ = best != nullptr;
    target_id_ = best ? best->id : 0;
    interacting_ = in_range_ && pressed_;
    if (interacting_) {
        ++interaction_count_;
    }
}

} // namespace open_crossing
