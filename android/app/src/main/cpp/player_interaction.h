#pragma once

#include <cstdint>
#include <vector>

#include "port_controller.h"
#include "player_simulation.h"

namespace open_crossing {

struct InteractionTarget {
    float x = 0.0f;
    float z = 0.0f;
    float radius = 0.6f;
    std::uint32_t id = 0;
};

class PlayerInteraction {
public:
    void reset();
    void set_targets(const std::vector<InteractionTarget>& targets);
    void update(const PlayerSimulation& player, const oc::ControllerState& controller);

    bool in_range() const { return in_range_; }
    bool pressed() const { return pressed_; }
    bool interacting() const { return interacting_; }
    std::uint32_t target_id() const { return target_id_; }
    std::uint64_t interaction_count() const { return interaction_count_; }

private:
    std::vector<InteractionTarget> targets_;
    bool in_range_ = false;
    bool pressed_ = false;
    bool interacting_ = false;
    std::uint32_t target_id_ = 0;
    std::uint64_t interaction_count_ = 0;
    bool previous_a_ = false;
};

} // namespace open_crossing
