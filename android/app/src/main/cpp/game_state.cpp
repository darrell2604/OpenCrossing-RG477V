#include "game_state.h"

#include <algorithm>
#include <limits>

namespace open_crossing {

void GameStateSystem::reset() {
    state_ = {};
    state_.phase = GamePhase::Boot;
    state_.day = 1;
    frame_remainder_ = 0;
}

void GameStateSystem::start_new_day() {
    if (state_.phase != GamePhase::Boot) {
        state_.day = std::max<std::uint32_t>(1, state_.day + 1);
    } else {
        state_.day = 1;
    }
    state_.game_minutes = 0;
    frame_remainder_ = 0;
    state_.phase = GamePhase::Playing;
}

void GameStateSystem::update(std::uint64_t elapsed_frames) {
    if (state_.phase != GamePhase::Playing || elapsed_frames == 0) return;

    constexpr std::uint64_t kFramesPerGameMinute = 60;
    constexpr std::uint64_t kMinutesPerDay = 24u * 60u;

    const std::uint64_t frames = frame_remainder_ + elapsed_frames;
    const std::uint64_t minutes = frames / kFramesPerGameMinute;
    frame_remainder_ = frames % kFramesPerGameMinute;
    if (minutes == 0) return;

    const std::uint64_t total_minutes = state_.game_minutes + minutes;
    const std::uint64_t completed_days = total_minutes / kMinutesPerDay;
    state_.game_minutes = total_minutes % kMinutesPerDay;

    for (std::uint64_t i = 0; i < completed_days; ++i) {
        if (state_.day < std::numeric_limits<std::uint32_t>::max()) {
            ++state_.day;
        }
    }
}

void GameStateSystem::toggle_pause() {
    if (state_.phase == GamePhase::Playing) {
        state_.phase = GamePhase::Paused;
    } else if (state_.phase == GamePhase::Paused) {
        state_.phase = GamePhase::Playing;
    }
}

void GameStateSystem::add_bells(std::uint32_t amount) {
    const std::uint64_t max_bells = std::numeric_limits<std::uint64_t>::max();
    state_.bells = (max_bells - state_.bells < amount) ? max_bells : state_.bells + amount;
}

bool GameStateSystem::spend_bells(std::uint32_t amount) {
    if (state_.bells < amount) return false;
    state_.bells -= amount;
    return true;
}

void GameStateSystem::record_interaction() {
    ++state_.interactions;
}

void GameStateSystem::set_inventory_open(bool open) {
    state_.inventory_open = open;
}

} // namespace open_crossing
