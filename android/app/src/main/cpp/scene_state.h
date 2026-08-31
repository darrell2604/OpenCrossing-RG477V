#pragma once

#include <cstdint>

namespace open_crossing {

struct SceneState {
    float player_x = 0.0f;
    float player_z = 0.0f;
    float player_angle = 0.0f;
    std::uint64_t frame = 0;
};

} // namespace open_crossing
