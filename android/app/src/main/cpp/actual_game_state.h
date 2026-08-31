#pragma once

#include <cstdint>

namespace open_crossing {

enum class GamePhase : std::uint8_t {
    Boot,
    Playing,
    Paused,
};

struct GameState {
    GamePhase phase = GamePhase::Boot;
    std::uint64_t day = 1;
    std::uint32_t bells = 0;
    std::uint32_t interaction_count = 0;
    float player_x = 0.0f;
    float player_z = 0.0f;
    float player_angle = 0.0f;
};

} // namespace open_crossing
