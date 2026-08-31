#pragma once

#include <cstdint>

#include "platform_services.h"
#include "player_simulation.h"
#include "scene_state.h"

namespace open_crossing {

class DecompGameLoop {
public:
    enum class GamePhase : std::uint8_t {
        Boot,
        Playing,
        Paused,
        Transition,
    };

    bool initialise(const PlatformServices& services);
    void update(const PlatformServices& services);

    uint64_t tick() const { return tick_; }
    bool ready() const { return ready_; }
    float frame_scale() const { return frame_scale_; }
    GamePhase phase() const { return phase_; }
    std::uint32_t day() const { return day_; }
    std::uint32_t bells() const { return bells_; }
    std::uint32_t interaction_count() const { return interaction_count_; }

    const PlayerSimulation& player() const { return player_; }
    SceneState scene_state() const {
        return SceneState{player_.x(), player_.z(), player_.angle(), tick_};
    }

private:
    void advance_game_time();
    void process_gameplay_state();

    bool ready_ = false;
    uint64_t tick_ = 0;
    float frame_scale_ = 1.0f;
    GamePhase phase_ = GamePhase::Boot;
    std::uint32_t day_ = 1;
    std::uint32_t bells_ = 0;
    std::uint32_t interaction_count_ = 0;
    std::uint32_t frames_since_day_start_ = 0;
    PlayerSimulation player_{};
};

} // namespace open_crossing
