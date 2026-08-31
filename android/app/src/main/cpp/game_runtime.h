#pragma once

#include <cstdint>
#include <vector>

#include "decomp_game_loop.h"
#include "decomp_runtime_adapter.h"
#include "platform_services.h"
#include "scene_state.h"
#include "player_camera.h"
#include "player_interaction.h"

namespace open_crossing {

class GameRuntime {
public:
    bool initialise();
    void resize(int width, int height);
    void frame();

    bool ready() const { return ready_; }
    const PlatformServices& platform() const { return platform_; }
    const DecompRuntimeAdapter& decomp() const { return decomp_; }
    const DecompGameLoop& game_loop() const { return game_loop_; }
    const SceneState& scene() const { return scene_; }
    const PlayerCamera& camera() const { return camera_; }
    const PlayerInteraction& interaction() const { return interaction_; }

private:
    bool ready_ = false;
    PlatformServices platform_{};
    DecompRuntimeAdapter decomp_{};
    DecompGameLoop game_loop_{};
    SceneState scene_{};
    PlayerCamera camera_{};
    PlayerInteraction interaction_{};
    std::vector<InteractionTarget> interaction_targets_;
    int width_ = 0;
    int height_ = 0;
    unsigned long long frame_counter_ = 0;
};

} // namespace open_crossing
