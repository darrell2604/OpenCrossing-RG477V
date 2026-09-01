#pragma once

#include <cstdint>

namespace open_crossing {

enum class GamePhase : std::uint8_t {
    Boot,
    Playing,
    Paused,
    Transition,
};

struct GameState {
    GamePhase phase = GamePhase::Boot;
    std::uint32_t day = 1;
    std::uint64_t game_minutes = 0;
    std::uint64_t bells = 0;
    std::uint64_t interactions = 0;
    bool inventory_open = false;
};

class GameStateSystem {
public:
    void reset();
    void start_new_day();
    void update(std::uint64_t elapsed_frames);
    void toggle_pause();
    void add_bells(std::uint32_t amount);
    bool spend_bells(std::uint32_t amount);
    void record_interaction();
    void set_inventory_open(bool open);

    const GameState& state() const { return state_; }
    std::uint32_t hour() const { return static_cast<std::uint32_t>((state_.game_minutes / 60u) % 24u); }
    std::uint32_t minute() const { return static_cast<std::uint32_t>(state_.game_minutes % 60u); }
    std::uint32_t time_of_day_minutes() const { return static_cast<std::uint32_t>(state_.game_minutes % 1440u); }

private:
    GameState state_{};
    std::uint64_t frame_remainder_ = 0;
};

} // namespace open_crossing
