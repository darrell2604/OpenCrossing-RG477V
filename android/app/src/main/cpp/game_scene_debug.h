#pragma once

#include <cstdint>

namespace open_crossing {

// Temporary bridge between the portable player simulation and the eventual
// world/actor renderer. This keeps debug world geometry isolated while we port
// the real scene systems from the decompilation.
struct GameSceneDebug {
    std::uint32_t frame = 0;
    float player_x = 0.0f;
    float player_z = 0.0f;
    float player_angle = 0.0f;
};

} // namespace open_crossing
