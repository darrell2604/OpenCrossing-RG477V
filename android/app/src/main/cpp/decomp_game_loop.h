#pragma once

#include <cstdint>

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

    std::uint64_t tick() const { return tick_; }
    bool ready() const { return ready_; }
    float frame_scale() const { return frame_scale_; }
    GameStateSystem::GamePhase phase() const { return state_.state().phase; }
    std::uint32_t day() const { return state_.state().day; }
    std::uint64_t bells() const { return state_.state().bells; }
    std::uint64_t interaction_count() const { return state_.state().interactions; }
    const GameStateSystem& game_state() const { return state_; }
    const InventorySystem& inventory() const { return inventory_; }

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
};

} // namespace open_crossing
