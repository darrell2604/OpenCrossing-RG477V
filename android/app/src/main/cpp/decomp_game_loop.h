#pragma once

#include <cstdint>
#include <vector>

#include "platform_services.h"
#include "player_simulation.h"
#include "scene_state.h"
#include "game_state.h"
#include "inventory_system.h"

namespace open_crossing {

class DecompGameLoop {
public:
    bool initialise(const PlatformServices& services);
    void update(const PlatformServices& services);
    void set_collision_rects(const std::vector<CollisionRect>& rects) { player_.set_collision_rects(rects); }

    std::uint64_t tick() const { return tick_; }
    bool ready() const { return ready_; }
    float frame_scale() const { return frame_scale_; }
    GamePhase phase() const { return state_.state().phase; }
    std::uint32_t day() const { return state_.state().day; }
    std::uint64_t bells() const { return state_.state().bells; }
    std::uint64_t interaction_count() const { return state_.state().interactions; }
    const GameStateSystem& game_state() const { return state_; }
    const InventorySystem& inventory() const { return inventory_; }
    std::size_t selected_inventory_slot() const { return selected_slot_; }

    const PlayerSimulation& player() const { return player_; }
    SceneState scene_state() const {
        return SceneState{player_.x(), player_.z(), player_.angle(), tick_};
    }

private:
    void process_actions();
    void advance_game_state(std::uint64_t elapsed_frames);

    bool ready_ = false;
    std::uint64_t tick_ = 0;
    float frame_scale_ = 1.0f;
    GameStateSystem state_{};
    InventorySystem inventory_{};
    PlayerSimulation player_{};
    std::uint32_t previous_buttons_ = 0;
    std::uint8_t previous_dpad_ = 0;
    std::uint32_t interaction_target_id_ = 0;
    std::size_t selected_slot_ = 0;
};

} // namespace open_crossing
