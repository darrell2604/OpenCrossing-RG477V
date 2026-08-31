#pragma once

#include <cstdint>

#include "platform_services.h"
#include "player_simulation.h"
#include "scene_state.h"

namespace open_crossing {

// Portable representation of the first frame-level contract in the upstream
// game layer. The original game advances its simulation once per game frame;
// platform-specific timing and rendering remain outside this class.
class DecompGameLoop {
public:
    bool initialise(const PlatformServices& services);
    void update(const PlatformServices& services);
    uint64_t tick() const { return tick_; }
    bool ready() const { return ready_; }
    float frame_scale() const { return frame_scale_; }
    const PlayerSimulation& player() const { return player_; }
    SceneState scene_state() const {
        return SceneState{player_.x(), player_.z(), player_.angle(), tick_};
    }

private:
    bool ready_ = false;
    uint64_t tick_ = 0;
    float frame_scale_ = 1.0f;
    PlayerSimulation player_{};
};

} // namespace open_crossing
